#ifndef _EASYTCPSERVER_H_
#define _EASYTCPSERVER_H_

#ifdef _WIN32
	#define FD_SETSIZE   1024
	#define WIN32_LEAN_AND_MEAN
	#define _WINSOCK_DEPRECATED_NO_WARNINGS
	#include <windows.h>
	#include <WinSock2.h>
	#pragma comment(lib,"ws2_32.lib")	
#else
	#include <unistd.h>
	#include <arpa/inet.h>
	#include <string.h>
	#define SOCKET int
	#define INVALID_SOCKET  (SOCKET)(~0)
	#define SOCKET_ERROR            (-1)
#endif
#include <stdio.h>
#include <iostream>
#include <vector>
#include "MessageHeader.h"
#include "ClientSocket.h"
#include "CELLTimestamp.h"

class EasyTcpServer
{
public:
	EasyTcpServer();
	~EasyTcpServer();
	//初始化Socket
	SOCKET InitSocket();
	//绑定IP和端口号
	int Bind(const char* ip, unsigned short port);
	//监听端口号
	int Listen(int n);
	//接受客户端连接
	SOCKET Accept();
	//关闭Socket
	void Close();
	//select处理网络消息
	bool OnRun();
	//是否工作中
	bool isRun();
	//接收数据 处理粘包 拆分包
	int RecvData(ClientSocket* pClient);
	//响应网络消息
	void OnNetMsg(SOCKET clientSocket, DataHeader* header);
	//发送指定Socket数据
	int SendData(SOCKET clientSocket, DataHeader* header);
	void SendDataToAll(DataHeader* header);

private:
	SOCKET serverSocket;
	std::vector<ClientSocket *> _clients;
	CELLTimetamp _tTime;
	int _recvCount;
};


#endif // !_EASYTCPSERVER_H_
