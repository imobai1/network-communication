#ifndef _EASYTCPCLIENT_H_
#define _EASYTCPCLIENT_H_

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
#include "MessageHeader.h"

class EasyTcpClient
{
	SOCKET clientSocket;
public:
	EasyTcpClient();
	~EasyTcpClient();
	//初始化socket
	void InitSocket();
	//连接服务器
	int Connect(const char* ip, unsigned short port);
	//关闭套节字closesocket
	void Close();
	//处理网络消息
	bool OnRun();
	//是否工作中
	bool isRun();
	//接收数据 处理粘包 拆分包
	int RecvData(SOCKET _cSock);
	//响应网络消息
	virtual void OnNetMsg(DataHeader* header);
	//发送数据
	int SendData(DataHeader* header);
};


#endif // !_EASYTCPCLIENT_H_
