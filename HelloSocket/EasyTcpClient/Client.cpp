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
		else if (0 == strcmp(cmdRequest, "login")) {
			Login login = { "lyd","lydmm" };
			DataHeader dh = {sizeof(login),CMD_LOGIN };
			//5 向服务器发送请求命令
			send(clientSocket, (const char*)&dh, sizeof(dh), 0);
			send(clientSocket, (const char*)&login, sizeof(login), 0);
			// 接收服务器返回的数据
			DataHeader retHeader = {};
			LoginResult res = {};
			recv(clientSocket, (char*)&retHeader, sizeof(DataHeader), 0);
			recv(clientSocket, (char*)&res, sizeof(LoginResult), 0);
			printf("LoginResult: %d \n", res.result);
		}
		else if (0 == strcmp(cmdRequest, "logout")) {
			Logout logout = { "lyd" };
			DataHeader dh = { sizeof(logout), CMD_LOGOUT };
			//5 向服务器发送请求命令
			send(clientSocket, (const char*)&dh, sizeof(DataHeader), 0);
			send(clientSocket, (const char*)&logout, sizeof(Logout), 0);
			// 接收服务器返回的数据
			DataHeader retHeader = {};
			LogoutResult logoutRet = {};
			recv(clientSocket, (char*)&retHeader, sizeof(DataHeader), 0);
			recv(clientSocket, (char*)&logoutRet, sizeof(LogoutResult), 0);
			printf("LogoutResult: %d \n", logoutRet.result);
		}
		else {
			printf("不支持的命令，请重新输入。\n");
		}
	}
	// 8 关闭套节字closesocket
	closesocket(clientSocket);
	//清除Windows socket环境
	WSACleanup();
	printf("已退出。");
	getchar();
	return 0;
}