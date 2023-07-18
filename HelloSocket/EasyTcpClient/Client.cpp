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
	//缓冲区
	char szRecv[4096] = {};
	// 5 接收客户端数据
	int nLen = recv(clientSocket, szRecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (nLen <= 0)
	{
		printf("与服务器断开连接，任务结束。\n");
		return -1;
	}
	switch (header->cmd)
	{
	case CMD_LOGIN_RESULT:
	{
		recv(clientSocket, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		LoginResult* login = (LoginResult*)szRecv;
		printf("收到服务端消息：CMD_LOGIN_RESULT,数据长度：%d\n", login->dataLength);
	}
	break;
	case CMD_LOGOUT_RESULT:
	{
		recv(clientSocket, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		LogoutResult* logout = (LogoutResult*)szRecv;
		printf("收到服务端消息：CMD_LOGOUT_RESULT,数据长度：%d\n", logout->dataLength);
	}
	break;
	case CMD_NEW_USER_JOIN:
	{
		recv(clientSocket, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		NewUserJoin* userJoin = (NewUserJoin*)szRecv;
		printf("收到服务端消息：CMD_NEW_USER_JOIN,数据长度：%d\n", userJoin->dataLength);
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
			printf("退出cmdThread线程\n");
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
			printf("不支持的命令。\n");
		}
	}
}


int main() {
#ifdef _WIN32
	//启动Windows socket 2.x环境
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif

	//用socket API建立简易TCP客户端
	// 1建立一个socket
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == clientSocket) {
		printf("错误，建立socket失败...\n");
	}
	else {
		printf("建立socket成功...\n");
	}
	// 2设置服务器地址和端口
	struct sockaddr_in serverAddress = {};
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(4567);
#ifdef _WIN32
	serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
#else
	serverAddress.sin_addr.s_addr = inet_addr("192.168.4.1");
#endif
	// 3连接服务器 connect
	int ret = connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(struct sockaddr_in));
	if (SOCKET_ERROR == ret) {
		printf("错误,连接失败...\n");
	}
	else {
		printf("连接成功...\n");
	}
	//启动线程
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
			printf("select任务结束1\n");
			break;
		}
		if (FD_ISSET(clientSocket, &fdReads))
		{
			FD_CLR(clientSocket, &fdReads);

			if (-1 == processor(clientSocket))
			{
				printf("select任务结束2\n");
				break;
			}
		}
		//printf("空闲时间处理其它业务..\n");
	}
#ifdef _WIN32
	// 8 关闭套节字closesocket
	closesocket(clientSocket);
	//清除Windows socket环境
	WSACleanup();
#else
	close(clientSocket);
#endif	
	printf("已退出。");
	getchar();
	return 0;
}