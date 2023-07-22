/*
 Copyright Zero One Star. All rights reserved.

 @Author: mengHuan
 @Date: 2023/07/19 23:18:38

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

	  https://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/

#include "EasyTcpServer.h"



EasyTcpServer::EasyTcpServer()
{
	serverSocket = INVALID_SOCKET;
}

EasyTcpServer::~EasyTcpServer()
{
	Close();
}

SOCKET EasyTcpServer::InitSocket(){
#ifdef _WIN32
	//启动Windows socket 2.x环境
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif 
	if (INVALID_SOCKET != serverSocket)
	{
		printf("<socket=%d>关闭旧连接...\n", (int)serverSocket);
		Close();
	}
	// 创建服务器端的socke
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == serverSocket)
	{
		printf("错误，建立socket失败...\n");
	}
	else {
		printf("建立socket=<%d>成功...\n", (int)serverSocket);
	}
	return serverSocket;
}

int EasyTcpServer::Bind(const char* ip, unsigned short port){
	// 设置服务器地址和端口
	sockaddr_in serverAddress = {};
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	if (ip) {
		serverAddress.sin_addr.s_addr = inet_addr(ip);
	}
	else {
		serverAddress.sin_addr.s_addr = INADDR_ANY;
	}
	// bind绑定用于接受客户端连接的网络端口
	int ret = bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(sockaddr_in));
	if (SOCKET_ERROR == ret) {
		printf("绑定错误,绑定网络端口<%d>失败\n",port);
	}
	else {
		printf("绑定网络端口<%d>成功...\n",port);
	}
	return ret;
}

int EasyTcpServer::Listen(int n){
	// listen 监听网络端口
	int ret = listen(serverSocket, n);
	if (SOCKET_ERROR == ret) {
		printf("socket=<%d>错误,监听网络端口失败...\n", serverSocket);
	}
	else {
		printf("socket=<%d>监听网络端口成功...\n", serverSocket);
	}
	return ret;
}

SOCKET EasyTcpServer::Accept()
{
	//  accept等待接受客户端连接
	struct sockaddr_in clientAddr = {};
	int clientAddressLenth = sizeof(sockaddr_in);
	SOCKET clientSocket = INVALID_SOCKET;
#ifdef _WIN32
	clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddressLenth);
#else
	clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, (socklen_t*)&clientAddressLenth);
#endif
	if (INVALID_SOCKET == clientSocket)
	{
		printf("socket=<%d>错误,接受到无效客户端SOCKET...\n", (int)serverSocket);
	}
	else {
		NewUserJoin userJion;
		SendDataToAll(&userJion);
		_clients.push_back(new ClientSocket(clientSocket));
		printf("socket=<%d>新客户端加入<socket = %d>,IP = %s \n",(int)serverSocket ,(int)clientSocket, inet_ntoa(clientAddr.sin_addr));
	}
	return clientSocket;
}

void EasyTcpServer::Close()
{
	if (serverSocket != INVALID_SOCKET){
#ifdef _WIN32
		for (int n = (int)_clients.size() - 1; n >= 0; n--) {
			closesocket(_clients[n]->sockfd());
			delete _clients[n];
		}
		// 关闭套节字closesocket
		closesocket(serverSocket);
		//清除Windows socket环境
		WSACleanup();

#else
		for (int n = (int)g_clients.size() - 1; n >= 0; n--) {
			close(_clients[n]);
			delete _clients[n];
		}
		// 关闭套接字closesocket
		close(serverSocket);
#endif // _WIN32
	}
}

int n_Count = 0;
bool EasyTcpServer::OnRun(){
	if (isRun()) {
		//伯克利套接字 BSD socket
		fd_set fdRead;//描述符（socket） 集合  
		fd_set fdWrite;
		fd_set fdExp;
		//清理集合
		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExp);
		//将描述符（socket）加入集合  
		FD_SET(serverSocket, &fdRead);
		FD_SET(serverSocket, &fdWrite);
		FD_SET(serverSocket, &fdExp);
		SOCKET maxSock = serverSocket;
		for (int n = (int)_clients.size() - 1; n >= 0; n--) {
			FD_SET(_clients[n]->sockfd(), &fdRead);
			if (maxSock < _clients[n]->sockfd()) {
				maxSock = _clients[n]->sockfd();
			}
		}

		timeval t = { 1,0 };
		// 参数nfds：指定被监听的文件描述符的总数，
		// 通常被设置为select监听的所有文件描述符中的最大值加1，因为文件描述符是从0开始计数的
		int ret = select(maxSock + 1, &fdRead, &fdWrite, &fdExp, &t);
		if (ret < 0) {
			printf("select 任务结束。\n");
			Close();
			return false;
		}
		//判断描述符（socket）是否在集合中  
		if (FD_ISSET(serverSocket, &fdRead))
		{
			FD_CLR(serverSocket, &fdRead);
			Accept();
		}
		for (int n = (int)_clients.size() - 1; n >= 0; n--)
		{
			if (FD_ISSET(_clients[n]->sockfd(), &fdRead)) {

				if (RecvData(_clients[n]) ==  -1) {
					auto iter = _clients.begin() + n;
					if (iter != _clients.end())
					{
						delete _clients[n];
						_clients.erase(iter);
					}
				}
			}
		}
		return true;
	}
	return false;
}

bool EasyTcpServer::isRun()
{
	return serverSocket != INVALID_SOCKET;
}

// 缓冲区
char _szRecv[RECV_BUFF_SZIE] = {};

int EasyTcpServer::RecvData(ClientSocket* pClient)
{
	// 接收客户端数据
	int cLength = (int)recv(pClient->sockfd(), _szRecv, RECV_BUFF_SZIE, 0);
	DataHeader* header = (DataHeader*)_szRecv;
	if (cLength <= 0) {
		printf("客户端<Socket = %d>已退出，任务结束。\n", pClient->sockfd());
		return -1;
	}
	//将收取到的数据拷贝到消息缓冲区
	memcpy(pClient->msgBuf() + pClient->getLastPos(), _szRecv, cLength);
	//消息缓冲区的数据尾部位置后移
	pClient->setLastPos(pClient->getLastPos() + cLength);
	//判断消息缓冲区的数据长度大于消息头DataHeader长度
	while (pClient->getLastPos() >= sizeof(DataHeader)) {
		//这时就可以知道当前消息的长度
		DataHeader* header = (DataHeader*)pClient->msgBuf();
		//判断消息缓冲区的数据长度大于消息长度
		if (pClient->getLastPos() >= header->dataLength)
		{
			//消息缓冲区剩余未处理数据的长度
			int nSize = pClient->getLastPos() - header->dataLength;
			//处理网络消息
			OnNetMsg(pClient->sockfd(), header);
			//将消息缓冲区剩余未处理数据前移
			memcpy(pClient->msgBuf(), pClient->msgBuf() + header->dataLength, nSize);
			//消息缓冲区的数据尾部位置前移
			pClient->setLastPos(nSize);
		}
		else {
			//消息缓冲区剩余数据不够一条完整消息
			break;
		}
	}
	return 0;
}

void EasyTcpServer::OnNetMsg(SOCKET clientSocket, DataHeader* header)
{
	switch (header->cmd) {
		case CMD_LOGIN: {
			Login* login = (Login*)header;
			//printf("收到客户端<Socket=%d>请求：CMD_LOGIN,数据长度：%d,userName=%s PassWord=%s\n", clientSocket, login->dataLength, login->userName, login->passWord);
			//忽略判断用户密码是否正确的过程;
			LoginResult res;
			//send(clientSocket, (char*)&res, sizeof(LoginResult), 0);
			SendData(clientSocket, &res);
			break;
		}
		case CMD_LOGOUT: {
			Logout* logout = (Logout*)header;
			//printf("收到客户端<Socket=%d>请求：CMD_LOGOUT,数据长度：%d,userName=%s\n", clientSocket, logout->dataLength, logout->userName);
			//忽略判断用户密码是否正确的过程
			LoginResult res;
			//send(clientSocket, (char*)&res, sizeof(LoginResult), 0);
			SendData(clientSocket, &res);
			break;
		}
		default:{
			printf("<socket=%d>收到未定义消息,数据长度：%d\n", clientSocket, header->dataLength);
			//DataHeader header = { 0,CMD_ERROR };
			//send(clientSocket, (char*)&header, sizeof(header), 0);
			//SendData(clientSocket, &res);
			break;
		}
	}
}

int EasyTcpServer::SendData(SOCKET clientSocket, DataHeader* header){
	if (isRun() && header)
	{
		return send(clientSocket, (const char*)header, header->dataLength, 0);
	}
	return SOCKET_ERROR;
}

void EasyTcpServer::SendDataToAll(DataHeader* header){
	for (int n = (int)_clients.size() - 1; n >= 0; n--)
	{
		SendData(_clients[n]->sockfd(), header);
	}
}

