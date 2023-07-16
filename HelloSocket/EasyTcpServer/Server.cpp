#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <windows.h>
#include <WinSock2.h>
#include <iostream>
#include "DataPackage.h"
#pragma comment(lib,"ws2_32.lib")



int main() {
	
	char buffer[1024] = {};
	
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
	// 5 accept�ȴ����ܿͻ�������
	struct sockaddr_in clientAddr;
	int clientAddressLenth = sizeof(clientAddr); 
	SOCKET clientSocket = INVALID_SOCKET;
	clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddressLenth);
	if (INVALID_SOCKET == clientSocket) {
		printf("����, ���ܵ���Ч�ͻ���SOCKET . . . \n");
	}
	printf("�¿ͻ��˼��룺IP = %s \n", inet_ntoa(clientAddr.sin_addr));
	// 6 ���տͻ��˷��͵�����
	while (true) {
		// 7 ���տͻ��˵���������
		DataHeader header = {};
		int cLength = recv(clientSocket, (char*)&header, sizeof(DataHeader), 0);
		if (cLength <= 0) {
			std::cout << "�ͻ������˳����������";
			break;
		}
		switch (header.cmd) {
			case CMD_LOGIN: {
				Login login = {};
				recv(clientSocket, (char*)&login + sizeof(DataHeader) , sizeof(Login) - sizeof(DataHeader), 0);
				printf("�յ�����:CMD_LOGIN ,���ݳ��ȣ�%d, �û�����%s,�û����룺%s\n",login.dataLength,login.userName,login.passWord);
				//�����ж��û������Ƿ���ȷ�Ĺ���
				LoginResult res;
				send(clientSocket, (char*)&res, sizeof(LoginResult), 0);
				break;
			}
			case CMD_LOGOUT: {
				Logout logout = {};
				recv(clientSocket, (char*)&logout + sizeof(DataHeader), sizeof(Logout) - sizeof(DataHeader), 0);
				printf("�յ�����:CMD_LOGOUT ,���ݳ��ȣ�%d, �û�����%s\n", logout.dataLength, logout.userName);
				//�����ж��û������Ƿ���ȷ�Ĺ���
				LogoutResult res;
				send(clientSocket, (char*)&res, sizeof(LogoutResult), 0);
				break;
			}
			default:
				header.cmd = CMD_ERROR;
				header.dataLength = 0;
				send(clientSocket, (char*)&header, sizeof(header), 0);
				break;
		}
	}
	// 10 �ر��׽���closesocket
	closesocket(serverSocket);
	WSACleanup();
	std::cout << "���˳�";
	getchar();
	return 0;
}
 