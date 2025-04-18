
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string.h>

#include <Windows.h>

#pragma comment(lib,"Ws2_32.lib")

#define BUF_SIZE 1024

char sdMsg[BUF_SIZE];

unsigned SendMsg(void* arg) {
	SOCKET sock = *((SOCKET*)arg);
	while (1) {
		if (fgets(sdMsg, BUF_SIZE, stdin) == NULL) {
			closesocket(sock);
			exit(-1);
		}
		size_t len = strlen(sdMsg);
		if (len > 0 && sdMsg[len - 1] == '\n') {
			sdMsg[len - 1] = '\0';
		}
		if (!strcmp(sdMsg, "QUIT") || !strcmp(sdMsg, "quit")) {
			closesocket(sock);
			exit(-1);
		}
		else {
			send(sock, sdMsg, strlen(sdMsg), 0);
		}
	}
	return 0;
}


unsigned RecvMsg(void* arg) {
	SOCKET sock = *((SOCKET*)arg);
	char recvMsg[BUF_SIZE];
	while (1) {
		int len = recv(sock, recvMsg, sizeof(recvMsg)-1, 0);
		if (len < 0) {
			closesocket(sock);
			exit(-1);
		}
		recvMsg[len] = '\0';
		printf("%s\n", recvMsg);
	}
	return 0;
}

int main() {
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		return -1;
	}

	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2) {
		WSACleanup();
		return -1; 
	}

	SOCKET hsock = socket(AF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(9999);
	inet_pton(AF_INET, "192.168.113.128", &addr.sin_addr);


	if (connect(hsock, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
		printf("connect error: %d", GetLastError());
		exit(-1);
	}
	else {
		printf("Welcome, input your name: ");

	}

	HANDLE hSendHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SendMsg, (void*)&hsock, 0, NULL);

	HANDLE hRecvHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecvMsg, (void*)&hsock, 0, NULL);

	WaitForSingleObject(hSendHandle, INFINITE);
	WaitForSingleObject(hRecvHandle, INFINITE);

	closesocket(hsock);
	WSACleanup();
}