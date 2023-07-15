#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <WinSock2.h>

#pragma comment(lib,"ws2_32.lib")


int main() {
	//启动Windows socket 2.x环境
	WORD ver = MAKEWORD(2,2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	//--用socket API建立简易TCP客户端
	// 1建立一个socket
	// 2连接服务器 connect
	// 3接收服务器信息recv
	// 4关闭套节字closesocket
	//--用socket API建立简易TCP服务端
	// 1 建立一个socket
	// 2 bind绑定用于接受客户端连接的网络端口
	// 3 listen 监听网络端口
	// 4 accept等待接受客户端连接
	// 5 send向客户端发送一条数据
	// 6 关闭套节字closesocket
	WSACleanup();
	return 0;
}