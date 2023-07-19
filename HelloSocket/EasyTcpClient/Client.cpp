#include "EasyTcpClient.h"

void cmdThread(EasyTcpClient* client)
{
	while (true)
	{
		char cmdBuf[256] = {};
		scanf("%s", cmdBuf);
		if (0 == strcmp(cmdBuf, "exit"))
		{
			client->Close();
			printf("退出cmdThread线程\n");
			break;
		}
		else if (0 == strcmp(cmdBuf, "login"))
		{
			Login login;
			strcpy(login.userName, "lyd");
			strcpy(login.passWord, "lydmm");
			client->SendData(&login);
		}
		else if (0 == strcmp(cmdBuf, "logout"))
		{
			Logout logout;
			strcpy(logout.userName, "lyd");
			client->SendData(&logout);
		}
		else {
			printf("不支持的命令。\n");
		}
	}
}


int main() {
	EasyTcpClient client;
	client.Connect("127.0.0.1", 4567);
	//启动线程
	std::thread t1(cmdThread, &client);
	t1.detach();
	while (client.isRun()) {
		client.OnRun();
		//printf("空闲时间处理其它业务..\n");
	}
	client.Close();
	printf("已退出。");
	getchar();
	return 0;
}