#include "EasyTcpClient.h"
#include <thread>

bool g_bRun = true;
void cmdThread()
{
	while (true)
	{
		char cmdBuf[256] = {};
		scanf("%s", cmdBuf);
		if (0 == strcmp(cmdBuf, "exit"))
		{
			g_bRun = false;
			printf("�˳�cmdThread�߳�\n");
			break;
		}
		else {
			printf("��֧�ֵ����\n");
		}
	}
}



int main() {

	const int cCount = FD_SETSIZE - 1;
	EasyTcpClient* client[cCount];

	for (int n = 0; n < cCount; n++)
	{
		client[n] = new EasyTcpClient();
	}
	for (int n = 0; n < cCount; n++)
	{
		client[n]->Connect("127.0.0.1", 4567);
	}

	//�����߳�
	std::thread t1(cmdThread);
	t1.detach();

	Login login;
	strcpy(login.userName, "lyd");
	strcpy(login.passWord, "lydmm");

	while (g_bRun) {
		for (int n = 0; n < cCount; n++)
		{
			client[n]->OnRun();
			client[n]->SendData(&login);
		}
	}
	for (int n = 0; n < cCount; n++)
	{
		client[n]->Close();
	}
	printf("���˳���");
	getchar();
	return 0;
}