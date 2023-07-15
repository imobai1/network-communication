#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <windows.h>
#include <WinSock2.h>
#include <stdio.h>
#include <iostream>
#pragma comment(lib,"ws2_32.lib")


int main() {
	char buffer[1024] = {};
	std::string request = "Hello, server!";
	
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
	// 4�������ݸ�������
	send(clientSocket,request.c_str(),request.size(),0);
	
	// 5���շ�������Ϣrecv
	int bytesRead = recv(clientSocket,buffer, sizeof(buffer), 0);
	if (bytesRead > 0) {
		printf("���յ�����: %s \n", buffer);
	}
	// 4�ر��׽���closesocket
	closesocket(clientSocket);
	WSACleanup();
	getchar();
	return 0;
}