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
	//缓冲区
	char szRecv[4096] = {};
	//接收客户端数据
	int cLength = recv(clientSocket, szRecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (cLength <= 0) {
		printf("客户端<Socket = %d>已退出，任务结束。\n",clientSocket);
		return -1;
	}
	switch (header->cmd) {
		case CMD_LOGIN: {
			recv(clientSocket, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			Login* login = (Login*)szRecv;
			printf("收到客户端<Socket=%d>请求：CMD_LOGIN,数据长度：%d,userName=%s PassWord=%s\n", clientSocket, login->dataLength, login->userName, login->passWord);
			//忽略判断用户密码是否正确的过程
			LoginResult res;
			send(clientSocket, (char*)&res, sizeof(LoginResult), 0);
			break;
		}
		case CMD_LOGOUT: {
			recv(clientSocket, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			Login* login = (Login*)szRecv;
			printf("收到客户端<Socket=%d>请求：CMD_LOGOUT,数据长度：%d,userName=%s\n", clientSocket, login->dataLength, login->userName);
			//忽略判断用户密码是否正确的过程
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
	//启动Windows socket 2.x环境
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	//--用socket API建立简易TCP服务端
	// 1 建立一个socket
	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM,0);
	// 2 设置服务器地址和端口
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(4567);
	serverAddress.sin_addr.s_addr = INADDR_ANY; 
	// 3 bind绑定用于接受客户端连接的网络端口
	if (SOCKET_ERROR == bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress))) {
		printf("ERROR,绑定用于接受客户端连接的网络端口失败\n");
	}
	else {
		printf("绑定网络端口成功...\n");
	}
	// 4 listen 监听网络端口
	if (SOCKET_ERROR == listen(serverSocket, 5)) {
		printf("错误,监听网络端口失败...\n");
	}
	else {
		printf("监听网络端口成功...\n");
	}
	// 6 接收客户端发送的数据
	while (true) {
		//伯克利套接字 BSD socket
		fd_set fdRead;//描述符（socket） 集合  
		fd_set fdWrite;
		fd_set fdExp;
		//清理集合
		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExp);
		//将描述符（socket）加入集合  
		FD_SET(serverSocket, &fdRead);
		FD_SET(serverSocket, &fdWrite);
		FD_SET(serverSocket, &fdExp);

		for (int n = g_clients.size() - 1; n >= 0; n--) {
			FD_SET(g_clients[n], &fdRead);
		}
		timeval t = { 1,0 };
		int ret = select(serverSocket + 1, &fdRead, &fdWrite, &fdExp, &t);
		if (ret < 0) {
			printf("select 任务结束。\n");
			break;
		}
		//判断描述符（socket）是否在集合中  
		if (FD_ISSET(serverSocket, &fdRead))
		{
			FD_CLR(serverSocket, &fdRead);
		
			// 5 accept等待接受客户端连接
			struct sockaddr_in clientAddr;
			int clientAddressLenth = sizeof(clientAddr);
			SOCKET clientSocket = INVALID_SOCKET;
			clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddressLenth);
			if (INVALID_SOCKET == clientSocket)
			{
				printf("错误,接受到无效客户端SOCKET...\n");
			}
			else{
				for (int n = (int)g_clients.size() - 1; n >= 0; n--)
				{
					NewUserJoin userJoin;
					send(g_clients[n], (const char*)&userJoin, sizeof(NewUserJoin), 0);
				}
				g_clients.push_back(clientSocket);
				printf("新客户端加入：socket = %d,IP = %s \n", (int)clientSocket, inet_ntoa(clientAddr.sin_addr));
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

		//printf("空闲时间处理其它业务..\n");
	}

	for (size_t n = g_clients.size() - 1; n >= 0; n--)
	{
		closesocket(g_clients[n]);
	}
	// 10 关闭套节字closesocket
	closesocket(serverSocket);
	WSACleanup();
	std::cout << "已退出";
	getchar();
	return 0;
}
 