#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <windows.h>
#include <WinSock2.h>
#include <iostream>

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
		int cmdrequestlen = recv(clientSocket, buffer, sizeof(buffer), 0);
		if (cmdrequestlen <= 0) {
			printf("�ͻ������˳����������\n");
			break;
		}
		// 8 ��������
		if (0 == strcmp(buffer, "getName")) {
			printf("���յ���������: %s \n", buffer);
			// 9 send��ͻ��˷���һ������
			char response[] = "qiang ge";
			send(clientSocket, response, sizeof(response), 0);
		}
		else if (0 == strcmp(buffer, "getAge")) {
			printf("���յ���������: %s \n", buffer);
			// 9 send��ͻ��˷���һ������
			char response[] = "80";
			send(clientSocket, response, sizeof(response), 0);
		}
		else {
			printf("���յ���������: %s \n", buffer);
			// 9 send��ͻ��˷���һ������
			char response[] = "???";
			send(clientSocket, response, sizeof(response), 0);
		}
	}
	// 10 �ر��׽���closesocket
	closesocket(serverSocket);
	WSACleanup();

	return 0;
}
 