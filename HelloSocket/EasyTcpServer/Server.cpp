#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <windows.h>
#include <WinSock2.h>
#include <iostream>

#pragma comment(lib,"ws2_32.lib")


int main() {
	
	char buffer[1024] = {};
	char response[] = "Hello,I'am server!";
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
	while (true) {
		clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr,&clientAddressLenth);
		if (INVALID_SOCKET == clientSocket) {
			printf("����, ���ܵ���Ч�ͻ���SOCKET . . . \n");
		}
		printf("�¿ͻ��˼��룺IP = %s \n",inet_ntoa(clientAddr.sin_addr));
		// 6 ���տͻ��˷��͵�����
		int bytesRead = recv(clientSocket,buffer,sizeof(buffer),0);
		if (bytesRead > 0) {
			printf("���յ�����: %s \n", buffer);
		}
		// 7 send��ͻ��˷���һ������
		send(clientSocket, response, sizeof(response),0);
	}
	// 6 �ر��׽���closesocket
	closesocket(serverSocket);
	WSACleanup();

	return 0;
}
 