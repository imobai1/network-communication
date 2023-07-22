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
	//����Windows socket 2.x����
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif 
	if (INVALID_SOCKET != serverSocket)
	{
		printf("<socket=%d>�رվ�����...\n", (int)serverSocket);
		Close();
	}
	// �����������˵�socke
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == serverSocket)
	{
		printf("���󣬽���socketʧ��...\n");
	}
	else {
		printf("����socket=<%d>�ɹ�...\n", (int)serverSocket);
	}
	return serverSocket;
}

int EasyTcpServer::Bind(const char* ip, unsigned short port){
	// ���÷�������ַ�Ͷ˿�
	sockaddr_in serverAddress = {};
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	if (ip) {
		serverAddress.sin_addr.s_addr = inet_addr(ip);
	}
	else {
		serverAddress.sin_addr.s_addr = INADDR_ANY;
	}
	// bind�����ڽ��ܿͻ������ӵ�����˿�
	int ret = bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(sockaddr_in));
	if (SOCKET_ERROR == ret) {
		printf("�󶨴���,������˿�<%d>ʧ��\n",port);
	}
	else {
		printf("������˿�<%d>�ɹ�...\n",port);
	}
	return ret;
}

int EasyTcpServer::Listen(int n){
	// listen ��������˿�
	int ret = listen(serverSocket, n);
	if (SOCKET_ERROR == ret) {
		printf("socket=<%d>����,��������˿�ʧ��...\n", serverSocket);
	}
	else {
		printf("socket=<%d>��������˿ڳɹ�...\n", serverSocket);
	}
	return ret;
}

SOCKET EasyTcpServer::Accept()
{
	//  accept�ȴ����ܿͻ�������
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
		printf("socket=<%d>����,���ܵ���Ч�ͻ���SOCKET...\n", (int)serverSocket);
	}
	else {
		NewUserJoin userJion;
		SendDataToAll(&userJion);
		_clients.push_back(new ClientSocket(clientSocket));
		printf("socket=<%d>�¿ͻ��˼���<socket = %d>,IP = %s \n",(int)serverSocket ,(int)clientSocket, inet_ntoa(clientAddr.sin_addr));
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
		// �ر��׽���closesocket
		closesocket(serverSocket);
		//���Windows socket����
		WSACleanup();

#else
		for (int n = (int)g_clients.size() - 1; n >= 0; n--) {
			close(_clients[n]);
			delete _clients[n];
		}
		// �ر��׽���closesocket
		close(serverSocket);
#endif // _WIN32
	}
}

int n_Count = 0;
bool EasyTcpServer::OnRun(){
	if (isRun()) {
		//�������׽��� BSD socket
		fd_set fdRead;//��������socket�� ����  
		fd_set fdWrite;
		fd_set fdExp;
		//������
		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExp);
		//����������socket�����뼯��  
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
		// ����nfds��ָ�����������ļ���������������
		// ͨ��������Ϊselect�����������ļ��������е����ֵ��1����Ϊ�ļ��������Ǵ�0��ʼ������
		int ret = select(maxSock + 1, &fdRead, &fdWrite, &fdExp, &t);
		if (ret < 0) {
			printf("select ���������\n");
			Close();
			return false;
		}
		//�ж���������socket���Ƿ��ڼ�����  
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

// ������
char _szRecv[RECV_BUFF_SZIE] = {};

int EasyTcpServer::RecvData(ClientSocket* pClient)
{
	// ���տͻ�������
	int cLength = (int)recv(pClient->sockfd(), _szRecv, RECV_BUFF_SZIE, 0);
	DataHeader* header = (DataHeader*)_szRecv;
	if (cLength <= 0) {
		printf("�ͻ���<Socket = %d>���˳������������\n", pClient->sockfd());
		return -1;
	}
	//����ȡ�������ݿ�������Ϣ������
	memcpy(pClient->msgBuf() + pClient->getLastPos(), _szRecv, cLength);
	//��Ϣ������������β��λ�ú���
	pClient->setLastPos(pClient->getLastPos() + cLength);
	//�ж���Ϣ�����������ݳ��ȴ�����ϢͷDataHeader����
	while (pClient->getLastPos() >= sizeof(DataHeader)) {
		//��ʱ�Ϳ���֪����ǰ��Ϣ�ĳ���
		DataHeader* header = (DataHeader*)pClient->msgBuf();
		//�ж���Ϣ�����������ݳ��ȴ�����Ϣ����
		if (pClient->getLastPos() >= header->dataLength)
		{
			//��Ϣ������ʣ��δ�������ݵĳ���
			int nSize = pClient->getLastPos() - header->dataLength;
			//����������Ϣ
			OnNetMsg(pClient->sockfd(), header);
			//����Ϣ������ʣ��δ��������ǰ��
			memcpy(pClient->msgBuf(), pClient->msgBuf() + header->dataLength, nSize);
			//��Ϣ������������β��λ��ǰ��
			pClient->setLastPos(nSize);
		}
		else {
			//��Ϣ������ʣ�����ݲ���һ��������Ϣ
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
			//printf("�յ��ͻ���<Socket=%d>����CMD_LOGIN,���ݳ��ȣ�%d,userName=%s PassWord=%s\n", clientSocket, login->dataLength, login->userName, login->passWord);
			//�����ж��û������Ƿ���ȷ�Ĺ���;
			LoginResult res;
			//send(clientSocket, (char*)&res, sizeof(LoginResult), 0);
			SendData(clientSocket, &res);
			break;
		}
		case CMD_LOGOUT: {
			Logout* logout = (Logout*)header;
			//printf("�յ��ͻ���<Socket=%d>����CMD_LOGOUT,���ݳ��ȣ�%d,userName=%s\n", clientSocket, logout->dataLength, logout->userName);
			//�����ж��û������Ƿ���ȷ�Ĺ���
			LoginResult res;
			//send(clientSocket, (char*)&res, sizeof(LoginResult), 0);
			SendData(clientSocket, &res);
			break;
		}
		default:{
			printf("<socket=%d>�յ�δ������Ϣ,���ݳ��ȣ�%d\n", clientSocket, header->dataLength);
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

