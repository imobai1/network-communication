/*
 Copyright menghuan. All rights reserved.

 @Author: mengHuan
 @Date: 2023/07/19 15:45:28

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

#include "EasyTcpClient.h"


EasyTcpClient::EasyTcpClient(){
	clientSocket = INVALID_SOCKET;
}

EasyTcpClient::~EasyTcpClient(){
	Close();
}

void EasyTcpClient::InitSocket(){
#ifdef _WIN32
	//启动Windows socket 2.x环境
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif
	if (INVALID_SOCKET != clientSocket) {
		printf("<socket = %d> 关闭旧连接...\n", clientSocket);
		Close();
	}
	// 建立一个socket
	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == clientSocket) {
		printf("错误，建立socket失败...\n");
	}
	else {
		//printf("建立socket成功...\n");
	}
}

int EasyTcpClient::Connect(const char* ip, unsigned short port){
	if (INVALID_SOCKET == clientSocket) {
		InitSocket();
	}
	// 设置服务器地址和端口
	struct sockaddr_in serverAddress = {};
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	serverAddress.sin_addr.s_addr = inet_addr(ip);
	//printf("<socket=%d>正在连接服务器<%s:%d>...\n", clientSocket, ip, port);
	// 连接服务器 connect
	int ret = connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(struct sockaddr_in));
	if (SOCKET_ERROR == ret) {
		printf("<socket=%d>错误，连接服务器<%s:%d>失败...\n",clientSocket, ip, port);
	}
	else {
		//printf("<socket=%d>连接服务器<%s:%d>成功...\n", clientSocket, ip, port);
	}
	return ret;
}

void EasyTcpClient::Close(){
	if (clientSocket != INVALID_SOCKET) {
#ifdef _WIN32
		// 关闭套节字closesocket
		closesocket(clientSocket);
		//清除Windows socket环境
		WSACleanup();
#else
		close(clientSocket);
#endif	
		clientSocket = INVALID_SOCKET;
	}
}

bool EasyTcpClient::OnRun(){
	if (isRun()) {
		fd_set fdReads;
		FD_ZERO(&fdReads);
		FD_SET(clientSocket, &fdReads);
		timeval t = { 0,0 };
		int ret = select(clientSocket + 1, &fdReads, 0, 0, &t);
		if (ret < 0)
		{
			printf("<socket=%d>select任务结束1\n",clientSocket);
			Close();
			return false;
		}
		if (FD_ISSET(clientSocket, &fdReads))
		{
			FD_CLR(clientSocket, &fdReads);

			if (-1 == RecvData(clientSocket))
			{
				printf("<socket=%d>select任务结束2\n",clientSocket);
				Close();
				return false;
			}
		}
		return true;
	}
	return false;
}

bool EasyTcpClient::isRun(){
	return clientSocket != INVALID_SOCKET;
}

// 缓冲区最小单元大小
#define  RECV_BUFF_SIZE 102400
// 接收缓冲区
char _szRecv[RECV_BUFF_SIZE] = {};
// 消息缓冲区（第二缓冲区）
char _szMsgBuf[RECV_BUFF_SIZE * 10] = {};
// 消息缓冲区的数量尾部位置
int _lastPos = 0;
// 接收数据 处理粘包 拆分包
int EasyTcpClient::RecvData(SOCKET clientSocket)
{
	// 接收客户端数据
	int nLen = recv(clientSocket, _szRecv, RECV_BUFF_SIZE, 0);
	if (nLen <= 0)
	{
		printf("<socket=%d>与服务器断开连接，任务结束。\n", clientSocket);
		return -1;
	}
	//将收取到的数据拷贝到消息缓冲区
	memcpy(_szMsgBuf + _lastPos, _szRecv, nLen);
	//消息缓冲区的数据尾部位置后移
	_lastPos += nLen;
	//判断消息缓冲区的数据长度大于消息头DataHeader长度
	while (_lastPos >= sizeof(DataHeader)) {
		//这时就可以知道当前消息的长度
		DataHeader* header = (DataHeader*)_szMsgBuf;
		//判断消息缓冲区的数据长度大于消息长度
		if (_lastPos >= header->dataLength) {
			//消息缓冲区剩余未处理数据的长度
			int nSize = _lastPos - header->dataLength;
			//处理网络消息
			OnNetMsg(header);
			//将消息缓冲区剩余未处理数据前移
			memcpy(_szMsgBuf, _szMsgBuf + header->dataLength,nSize);
			//消息缓冲区的数据尾部前移
			_lastPos = nSize;
		}
		else {
			//消息缓冲区剩余数据不够一条完整消息
			break;
		}
	}	
	return 0;
}

void EasyTcpClient::OnNetMsg(DataHeader* header){
	switch (header->cmd){
		case CMD_LOGIN_RESULT:{
			LoginResult* login = (LoginResult*)header;
			//printf("<socket=%d>收到服务端消息：CMD_LOGIN_RESULT,数据长度：%d\n",clientSocket, login->dataLength);
		}
		break;
		case CMD_LOGOUT_RESULT:{
			LogoutResult* logout = (LogoutResult*)header;
			//printf("<socket=%d>收到服务端消息：CMD_LOGOUT_RESULT,数据长度：%d\n",clientSocket, logout->dataLength);
		}
		break;
		case CMD_NEW_USER_JOIN:{
			NewUserJoin* userJoin = (NewUserJoin*)header;
			//printf("<socket=%d>收到服务端消息：CMD_NEW_USER_JOIN,数据长度：%d\n",clientSocket, userJoin->dataLength);
		}
		break;
		case CMD_ERROR: {
			printf("<socket=%d>收到服务端消息：CMD_ERROR,数据长度：%d\n", clientSocket, header->dataLength);
		}
		break;
		default: {
			printf("<socket=%d>收到未定义消息,数据长度：%d\n", clientSocket, header->dataLength);
		}
		break;
	}
}

int EasyTcpClient::SendData(DataHeader* header)
{
	if (isRun() && header)
	{
		return send(clientSocket, (const char*)header, header->dataLength, 0);
	}
	return SOCKET_ERROR;
}
