#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <windows.h>
#include <WinSock2.h>
#include <iostream>
#include <vector>
#include "DataPackage.h"
#pragma comment(lib,"ws2_32.lib")

std::vector<SOCKET> g_clients;

int processor(SOCKET clientSocket) {
	//������
	char szRecv[4096] = {};
	//���տͻ�������
	int cLength = recv(clientSocket, szRecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (cLength <= 0) {
		printf("�ͻ���<Socket = %d>���˳������������\n",clientSocket);
		return -1;
	}
	switch (header->cmd) {
		case CMD_LOGIN: {
			recv(clientSocket, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			Login* login = (Login*)szRecv;
			printf("�յ��ͻ���<Socket=%d>����CMD_LOGIN,���ݳ��ȣ�%d,userName=%s PassWord=%s\n", clientSocket, login->dataLength, login->userName, login->passWord);
			//�����ж��û������Ƿ���ȷ�Ĺ���
			LoginResult res;
			send(clientSocket, (char*)&res, sizeof(LoginResult), 0);
			break;
		}
		case CMD_LOGOUT: {
			recv(clientSocket, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			Login* login = (Login*)szRecv;
			printf("�յ��ͻ���<Socket=%d>����CMD_LOGOUT,���ݳ��ȣ�%d,userName=%s\n", clientSocket, login->dataLength, login->userName);
			//�����ж��û������Ƿ���ȷ�Ĺ���
			LoginResult res;
			send(clientSocket, (char*)&res, sizeof(LoginResult), 0);
			break;
		}
		default:
		{
			DataHeader header = { 0,CMD_ERROR };
			send(clientSocket, (char*)&header, sizeof(header), 0);
			break;
		}
	}
	return 0;
}

int main() {
	//����Windows socket 2.x����
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	//--��socket API��������TCP�����
	// 1 ����һ��socket
	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM,0);
	// 2 ���÷�������ַ�Ͷ˿�
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(4567);
	serverAddress.sin_addr.s_addr = INADDR_ANY; 
	// 3 bind�����ڽ��ܿͻ������ӵ�����˿�
	if (SOCKET_ERROR == bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress))) {
		printf("ERROR,�����ڽ��ܿͻ������ӵ�����˿�ʧ��\n");
	}
	else {
		printf("������˿ڳɹ�...\n");
	}
	// 4 listen ��������˿�
	if (SOCKET_ERROR == listen(serverSocket, 5)) {
		printf("����,��������˿�ʧ��...\n");
	}
	else {
		printf("��������˿ڳɹ�...\n");
	}
	// 6 ���տͻ��˷��͵�����
	while (true) {
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

		for (int n = g_clients.size() - 1; n >= 0; n--) {
			FD_SET(g_clients[n], &fdRead);
		}
		timeval t = { 1,0 };
		int ret = select(serverSocket + 1, &fdRead, &fdWrite, &fdExp, &t);
		if (ret < 0) {
			printf("select ���������\n");
			break;
		}
		//�ж���������socket���Ƿ��ڼ�����  
		if (FD_ISSET(serverSocket, &fdRead))
		{
			FD_CLR(serverSocket, &fdRead);
		
			// 5 accept�ȴ����ܿͻ�������
			struct sockaddr_in clientAddr;
			int clientAddressLenth = sizeof(clientAddr);
			SOCKET clientSocket = INVALID_SOCKET;
			clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddressLenth);
			if (INVALID_SOCKET == clientSocket)
			{
				printf("����,���ܵ���Ч�ͻ���SOCKET...\n");
			}
			else{
				for (int n = (int)g_clients.size() - 1; n >= 0; n--)
				{
					NewUserJoin userJoin;
					send(g_clients[n], (const char*)&userJoin, sizeof(NewUserJoin), 0);
				}
				g_clients.push_back(clientSocket);
				printf("�¿ͻ��˼��룺socket = %d,IP = %s \n", (int)clientSocket, inet_ntoa(clientAddr.sin_addr));
			}
		}
		for (size_t n = 0; n < fdRead.fd_count; n++)
		{
			if (-1 == processor(fdRead.fd_array[n]))
			{
				auto iter = find(g_clients.begin(), g_clients.end(), fdRead.fd_array[n]);
				if (iter != g_clients.end())
				{
					g_clients.erase(iter);
				}
			}
		}

		//printf("����ʱ�䴦������ҵ��..\n");
	}

	for (size_t n = g_clients.size() - 1; n >= 0; n--)
	{
		closesocket(g_clients[n]);
	}
	// 10 �ر��׽���closesocket
	closesocket(serverSocket);
	WSACleanup();
	std::cout << "���˳�";
	getchar();
	return 0;
}
 