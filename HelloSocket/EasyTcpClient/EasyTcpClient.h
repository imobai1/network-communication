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
	//��ʼ��socket
	void InitSocket();
	//���ӷ�����
	int Connect(const char* ip, unsigned short port);
	//�ر��׽���closesocket
	void Close();
	//����������Ϣ
	bool OnRun();
	//�Ƿ�����
	bool isRun();
	//�������� ����ճ�� ��ְ�
	int RecvData(SOCKET _cSock);
	//��Ӧ������Ϣ
	virtual void OnNetMsg(DataHeader* header);
	//��������
	int SendData(DataHeader* header);
};


#endif // !_EASYTCPCLIENT_H_
