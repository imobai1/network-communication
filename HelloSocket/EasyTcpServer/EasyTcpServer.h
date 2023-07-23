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
	//��ʼ��Socket
	SOCKET InitSocket();
	//��IP�Ͷ˿ں�
	int Bind(const char* ip, unsigned short port);
	//�����˿ں�
	int Listen(int n);
	//���ܿͻ�������
	SOCKET Accept();
	//�ر�Socket
	void Close();
	//select����������Ϣ
	bool OnRun();
	//�Ƿ�����
	bool isRun();
	//�������� ����ճ�� ��ְ�
	int RecvData(ClientSocket* pClient);
	//��Ӧ������Ϣ
	void OnNetMsg(SOCKET clientSocket, DataHeader* header);
	//����ָ��Socket����
	int SendData(SOCKET clientSocket, DataHeader* header);
	void SendDataToAll(DataHeader* header);

private:
	SOCKET serverSocket;
	std::vector<ClientSocket *> _clients;
	CELLTimetamp _tTime;
	int _recvCount;
};


#endif // !_EASYTCPSERVER_H_
