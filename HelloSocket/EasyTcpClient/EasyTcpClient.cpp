/*
 Copyright menghuan. All rights reserved.

 @Author: mengHuan
 @Date: 2023/07/19 15:45:28

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

	  https://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/

#include "EasyTcpClient.h"


EasyTcpClient::EasyTcpClient(){
	clientSocket = INVALID_SOCKET;
}

EasyTcpClient::~EasyTcpClient(){
	Close();
}

void EasyTcpClient::InitSocket(){
#ifdef _WIN32
	//����Windows socket 2.x����
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif
	if (INVALID_SOCKET != clientSocket) {
		printf("<socket = %d> �رվ�����...\n", clientSocket);
		Close();
	}
	// ����һ��socket
	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == clientSocket) {
		printf("���󣬽���socketʧ��...\n");
	}
	else {
		//printf("����socket�ɹ�...\n");
	}
}

int EasyTcpClient::Connect(const char* ip, unsigned short port){
	if (INVALID_SOCKET == clientSocket) {
		InitSocket();
	}
	// ���÷�������ַ�Ͷ˿�
	struct sockaddr_in serverAddress = {};
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	serverAddress.sin_addr.s_addr = inet_addr(ip);
	//printf("<socket=%d>�������ӷ�����<%s:%d>...\n", clientSocket, ip, port);
	// ���ӷ����� connect
	int ret = connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(struct sockaddr_in));
	if (SOCKET_ERROR == ret) {
		printf("<socket=%d>�������ӷ�����<%s:%d>ʧ��...\n",clientSocket, ip, port);
	}
	else {
		//printf("<socket=%d>���ӷ�����<%s:%d>�ɹ�...\n", clientSocket, ip, port);
	}
	return ret;
}

void EasyTcpClient::Close(){
	if (clientSocket != INVALID_SOCKET) {
#ifdef _WIN32
		// �ر��׽���closesocket
		closesocket(clientSocket);
		//���Windows socket����
		WSACleanup();
#else
		close(clientSocket);
#endif	
		clientSocket = INVALID_SOCKET;
	}
}

bool EasyTcpClient::OnRun(){
	if (isRun()) {
		fd_set fdReads;
		FD_ZERO(&fdReads);
		FD_SET(clientSocket, &fdReads);
		timeval t = { 0,0 };
		int ret = select(clientSocket + 1, &fdReads, 0, 0, &t);
		if (ret < 0)
		{
			printf("<socket=%d>select�������1\n",clientSocket);
			Close();
			return false;
		}
		if (FD_ISSET(clientSocket, &fdReads))
		{
			FD_CLR(clientSocket, &fdReads);

			if (-1 == RecvData(clientSocket))
			{
				printf("<socket=%d>select�������2\n",clientSocket);
				Close();
				return false;
			}
		}
		return true;
	}
	return false;
}

bool EasyTcpClient::isRun(){
	return clientSocket != INVALID_SOCKET;
}

// ��������С��Ԫ��С
#define  RECV_BUFF_SIZE 102400
// ���ջ�����
char _szRecv[RECV_BUFF_SIZE] = {};
// ��Ϣ���������ڶ���������
char _szMsgBuf[RECV_BUFF_SIZE * 10] = {};
// ��Ϣ������������β��λ��
int _lastPos = 0;
// �������� ����ճ�� ��ְ�
int EasyTcpClient::RecvData(SOCKET clientSocket)
{
	// ���տͻ�������
	int nLen = recv(clientSocket, _szRecv, RECV_BUFF_SIZE, 0);
	if (nLen <= 0)
	{
		printf("<socket=%d>��������Ͽ����ӣ����������\n", clientSocket);
		return -1;
	}
	//����ȡ�������ݿ�������Ϣ������
	memcpy(_szMsgBuf + _lastPos, _szRecv, nLen);
	//��Ϣ������������β��λ�ú���
	_lastPos += nLen;
	//�ж���Ϣ�����������ݳ��ȴ�����ϢͷDataHeader����
	while (_lastPos >= sizeof(DataHeader)) {
		//��ʱ�Ϳ���֪����ǰ��Ϣ�ĳ���
		DataHeader* header = (DataHeader*)_szMsgBuf;
		//�ж���Ϣ�����������ݳ��ȴ�����Ϣ����
		if (_lastPos >= header->dataLength) {
			//��Ϣ������ʣ��δ�������ݵĳ���
			int nSize = _lastPos - header->dataLength;
			//����������Ϣ
			OnNetMsg(header);
			//����Ϣ������ʣ��δ��������ǰ��
			memcpy(_szMsgBuf, _szMsgBuf + header->dataLength,nSize);
			//��Ϣ������������β��ǰ��
			_lastPos = nSize;
		}
		else {
			//��Ϣ������ʣ�����ݲ���һ��������Ϣ
			break;
		}
	}	
	return 0;
}

void EasyTcpClient::OnNetMsg(DataHeader* header){
	switch (header->cmd){
		case CMD_LOGIN_RESULT:{
			LoginResult* login = (LoginResult*)header;
			//printf("<socket=%d>�յ��������Ϣ��CMD_LOGIN_RESULT,���ݳ��ȣ�%d\n",clientSocket, login->dataLength);
		}
		break;
		case CMD_LOGOUT_RESULT:{
			LogoutResult* logout = (LogoutResult*)header;
			//printf("<socket=%d>�յ��������Ϣ��CMD_LOGOUT_RESULT,���ݳ��ȣ�%d\n",clientSocket, logout->dataLength);
		}
		break;
		case CMD_NEW_USER_JOIN:{
			NewUserJoin* userJoin = (NewUserJoin*)header;
			//printf("<socket=%d>�յ��������Ϣ��CMD_NEW_USER_JOIN,���ݳ��ȣ�%d\n",clientSocket, userJoin->dataLength);
		}
		break;
		case CMD_ERROR: {
			printf("<socket=%d>�յ��������Ϣ��CMD_ERROR,���ݳ��ȣ�%d\n", clientSocket, header->dataLength);
		}
		break;
		default: {
			printf("<socket=%d>�յ�δ������Ϣ,���ݳ��ȣ�%d\n", clientSocket, header->dataLength);
		}
		break;
	}
}

int EasyTcpClient::SendData(DataHeader* header)
{
	if (isRun() && header)
	{
		return send(clientSocket, (const char*)header, header->dataLength, 0);
	}
	return SOCKET_ERROR;
}
