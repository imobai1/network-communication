#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#define _WINSOCK_DEPRECATED_NO_WARNINGS
	#include <windows.h>
	#include <WinSock2.h>
	#pragma comment(lib,"ws2_32.lib")
#else
	#include<unistd.h> //uni std
	#include<arpa/inet.h>
	#include<string.h>

	#define SOCKET int
	#define INVALID_SOCKET  (SOCKET)(~0)
	#define SOCKET_ERROR            (-1)
#endif

#include <stdio.h>
#include <iostream>
#include <thread>
#include "DataPackage.h"

int processor(SOCKET clientSocket)
{
	//������
	char szRecv[4096] = {};
	// 5 ���տͻ�������
	int nLen = recv(clientSocket, szRecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (nLen <= 0)
	{
		printf("��������Ͽ����ӣ����������\n");
		return -1;
	}
	switch (header->cmd)
	{
	case CMD_LOGIN_RESULT:
	{
		recv(clientSocket, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		LoginResult* login = (LoginResult*)szRecv;
		printf("�յ��������Ϣ��CMD_LOGIN_RESULT,���ݳ��ȣ�%d\n", login->dataLength);
	}
	break;
	case CMD_LOGOUT_RESULT:
	{
		recv(clientSocket, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		LogoutResult* logout = (LogoutResult*)szRecv;
		printf("�յ��������Ϣ��CMD_LOGOUT_RESULT,���ݳ��ȣ�%d\n", logout->dataLength);
	}
	break;
	case CMD_NEW_USER_JOIN:
	{
		recv(clientSocket, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		NewUserJoin* userJoin = (NewUserJoin*)szRecv;
		printf("�յ��������Ϣ��CMD_NEW_USER_JOIN,���ݳ��ȣ�%d\n", userJoin->dataLength);
	}
	break;
	}
	return 0;
}

bool g_bRun = true;
void cmdThread(SOCKET sock)
{
	while (true)
	{
		char cmdBuf[256] = {};
		scanf("%s", cmdBuf);
		if (0 == strcmp(cmdBuf, "exit"))
		{
			g_bRun = false;
			printf("�˳�cmdThread�߳�\n");
			break;
		}
		else if (0 == strcmp(cmdBuf, "login"))
		{
			Login login;
			strcpy(login.userName, "lyd");
			strcpy(login.passWord, "lydmm");
			send(sock, (const char*)&login, sizeof(Login), 0);
		}
		else if (0 == strcmp(cmdBuf, "logout"))
		{
			Logout logout;
			strcpy(logout.userName, "lyd");
			send(sock, (const char*)&logout, sizeof(Logout), 0);
		}
		else {
			printf("��֧�ֵ����\n");
		}
	}
}


int main() {
#ifdef _WIN32
	//����Windows socket 2.x����
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif

	//��socket API��������TCP�ͻ���
	// 1����һ��socket
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == clientSocket) {
		printf("���󣬽���socketʧ��...\n");
	}
	else {
		printf("����socket�ɹ�...\n");
	}
	// 2���÷�������ַ�Ͷ˿�
	struct sockaddr_in serverAddress = {};
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(4567);
#ifdef _WIN32
	serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
#else
	serverAddress.sin_addr.s_addr = inet_addr("192.168.4.1");
#endif
	// 3���ӷ����� connect
	int ret = connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(struct sockaddr_in));
	if (SOCKET_ERROR == ret) {
		printf("����,����ʧ��...\n");
	}
	else {
		printf("���ӳɹ�...\n");
	}
	//�����߳�
	std::thread t1(cmdThread, clientSocket);
	t1.detach();
	while (g_bRun) {
		fd_set fdReads;
		FD_ZERO(&fdReads);
		FD_SET(clientSocket, &fdReads);
		timeval t = { 1,0 };
		int ret = select(clientSocket + 1, &fdReads, 0, 0, &t);
		if (ret < 0)
		{
			printf("select�������1\n");
			break;
		}
		if (FD_ISSET(clientSocket, &fdReads))
		{
			FD_CLR(clientSocket, &fdReads);

			if (-1 == processor(clientSocket))
			{
				printf("select�������2\n");
				break;
			}
		}
		//printf("����ʱ�䴦������ҵ��..\n");
	}
#ifdef _WIN32
	// 8 �ر��׽���closesocket
	closesocket(clientSocket);
	//���Windows socket����
	WSACleanup();
#else
	close(clientSocket);
#endif	
	printf("���˳���");
	getchar();
	return 0;
}