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
	// 4发送数据给服务器
	send(clientSocket,request.c_str(),request.size(),0);
	
	// 5接收服务器信息recv
	int bytesRead = recv(clientSocket,buffer, sizeof(buffer), 0);
	if (bytesRead > 0) {
		printf("接收到数据: %s \n", buffer);
	}
	// 4关闭套节字closesocket
	closesocket(clientSocket);
	WSACleanup();
	getchar();
	return 0;
}