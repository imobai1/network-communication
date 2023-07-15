#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <windows.h>
#include <WinSock2.h>
#include <iostream>

#pragma comment(lib,"ws2_32.lib")


int main() {
	
	char buffer[1024] = {};
	
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
	
	// 5 accept等待接受客户端连接
	struct sockaddr_in clientAddr;
	int clientAddressLenth = sizeof(clientAddr); 
	SOCKET clientSocket = INVALID_SOCKET;
	clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddressLenth);
	if (INVALID_SOCKET == clientSocket) {
		printf("错误, 接受到无效客户端SOCKET . . . \n");
	}
	printf("新客户端加入：IP = %s \n", inet_ntoa(clientAddr.sin_addr));
	// 6 接收客户端发送的数据
	while (true) {
		// 7 接收客户端的请求命令
		int cmdrequestlen = recv(clientSocket, buffer, sizeof(buffer), 0);
		if (cmdrequestlen <= 0) {
			printf("客户端已退出，任务结束\n");
			break;
		}
		// 8 处理请求
		if (0 == strcmp(buffer, "getName")) {
			printf("接收到请求数据: %s \n", buffer);
			// 9 send向客户端发送一条数据
			char response[] = "qiang ge";
			send(clientSocket, response, sizeof(response), 0);
		}
		else if (0 == strcmp(buffer, "getAge")) {
			printf("接收到请求数据: %s \n", buffer);
			// 9 send向客户端发送一条数据
			char response[] = "80";
			send(clientSocket, response, sizeof(response), 0);
		}
		else {
			printf("接收到请求数据: %s \n", buffer);
			// 9 send向客户端发送一条数据
			char response[] = "???";
			send(clientSocket, response, sizeof(response), 0);
		}
	}
	// 10 关闭套节字closesocket
	closesocket(serverSocket);
	WSACleanup();

	return 0;
}
 