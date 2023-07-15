#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <windows.h>
#include <WinSock2.h>
#include <stdio.h>
#include <iostream>
#pragma comment(lib,"ws2_32.lib")


int main() {
	char buffer[1024] = {};
	//启动Windows socket 2.x环境
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	//--用socket API建立简易TCP客户端
	// 1建立一个socket
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == clientSocket) {
		printf("错误，建立socket失败...\n");
	}
	else {
		printf("建立socket成功...\n");
	}
	// 2设置服务器地址和端口
	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(4567);
	serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	// 3连接服务器 connect
	int ret = connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
	if (SOCKET_ERROR == ret) {
		printf("错误,连接失败...\n");
	}
	else {
		printf("连接成功...\n");
	}
	while (true){
		// 4 输入请求命令
		char cmdRequest[1024] = {};
		scanf("%s", cmdRequest);
		// 5 处理请求命令 
		if (0 == strcmp(cmdRequest, "exit")) {
			break;
		}
		else{
			// 6 想服务器发送命令
			send(clientSocket, cmdRequest, sizeof(cmdRequest), 0);
		}
		// 7 接收服务器信息recv
		int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
		if (bytesRead > 0) {
			printf("接收到数据: %s \n", buffer);
		}
	}
	// 8 关闭套节字closesocket
	closesocket(clientSocket);
	WSACleanup();
	getchar();
	return 0;
}