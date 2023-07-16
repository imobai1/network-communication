#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <windows.h>
#include <WinSock2.h>
#include <stdio.h>
#include <iostream>
#include "DataPackage.h"
#pragma comment(lib,"ws2_32.lib")



int main() {
	char buffer[1024] = {};
	//����Windows socket 2.x����
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	//--��socket API��������TCP�ͻ���
	// 1����һ��socket
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == clientSocket) {
		printf("���󣬽���socketʧ��...\n");
	}
	else {
		printf("����socket�ɹ�...\n");
	}
	// 2���÷�������ַ�Ͷ˿�
	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(4567);
	serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	// 3���ӷ����� connect
	int ret = connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
	if (SOCKET_ERROR == ret) {
		printf("����,����ʧ��...\n");
	}
	else {
		printf("���ӳɹ�...\n");
	}
	while (true){
		// 4 ������������
		char cmdRequest[1024] = {};
		scanf("%s", cmdRequest);
		// 5 ������������ 
		if (0 == strcmp(cmdRequest, "exit")) {
			break;
		}
		else if (0 == strcmp(cmdRequest, "login")) {
			Login login = { "lyd","lydmm" };
			DataHeader dh = {sizeof(login),CMD_LOGIN };
			//5 �������������������
			send(clientSocket, (const char*)&dh, sizeof(dh), 0);
			send(clientSocket, (const char*)&login, sizeof(login), 0);
			// ���շ��������ص�����
			DataHeader retHeader = {};
			LoginResult res = {};
			recv(clientSocket, (char*)&retHeader, sizeof(DataHeader), 0);
			recv(clientSocket, (char*)&res, sizeof(LoginResult), 0);
			printf("LoginResult: %d \n", res.result);
		}
		else if (0 == strcmp(cmdRequest, "logout")) {
			Logout logout = { "lyd" };
			DataHeader dh = { sizeof(logout), CMD_LOGOUT };
			//5 �������������������
			send(clientSocket, (const char*)&dh, sizeof(DataHeader), 0);
			send(clientSocket, (const char*)&logout, sizeof(Logout), 0);
			// ���շ��������ص�����
			DataHeader retHeader = {};
			LogoutResult logoutRet = {};
			recv(clientSocket, (char*)&retHeader, sizeof(DataHeader), 0);
			recv(clientSocket, (char*)&logoutRet, sizeof(LogoutResult), 0);
			printf("LogoutResult: %d \n", logoutRet.result);
		}
		else {
			printf("��֧�ֵ�������������롣\n");
		}
	}
	// 8 �ر��׽���closesocket
	closesocket(clientSocket);
	//���Windows socket����
	WSACleanup();
	printf("���˳���");
	getchar();
	return 0;
}