#include"dll.h"
#define MAXBUFFER 1024*1024
AFX_EXT_FUNC int lginit (int port) {


	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		//printf("来自C++ WinSocketDLL无法加载Winsock\n");
		return -1;
	}

	sockSrv = socket(AF_INET, SOCK_STREAM, 0);

	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(port);
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (bind(sockSrv, (LPSOCKADDR)& addrSrv, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) {
		//printf("来自C++ WinSocketDLL绑定端口失败:%d\n", WSAGetLastError());
		return -1;
	}


	if (listen(sockSrv, 10) == SOCKET_ERROR) {
		//printf("来自C++ WinSocketDLL监听失败:%d\n", WSAGetLastError());
		return -1;
	}
	//printf("来自C++ WinSocketDLL初始化成功\n");

	return 0;
}

AFX_EXT_FUNC SOCKET lgaccept () {
	SOCKADDR_IN addrClient;
	int len = sizeof(SOCKADDR);
	SOCKET sockConn = accept(sockSrv, (SOCKADDR*)& addrClient, &len);
	if (sockConn == SOCKET_ERROR) {
		//printf("来自C++ TCP连接接受失败:%d\n", WSAGetLastError());
		return -1;
	}
	//cout << "TCP连接接受成功!\n";

	char* recvBuf=(new char[MAXBUFFER]);
	memset(recvBuf, 0, sizeof(recvBuf));

	if (recv(sockConn, recvBuf, MAXBUFFER, 0) <= 0) {
		//printf("来自C++ TCP信息接收失败\n");
		return NULL;
	}

	WebSocket tools = *(new  WebSocket());
	if (tools.parseHandshake((unsigned char*)recvBuf, MAXBUFFER) != OPENING_FRAME) {
		//cout << "收到错误的websocket请求头!\n";
		return -1;
	}

	//cout << "收到正确的websocket请求头!\n";

	char* sendbuf=new char[MAXBUFFER];
	string ans = tools.answerHandshake();

	size_t length = ans.copy(sendbuf, string::npos, 0);//需要注意,websocket发送的包长度必须刚好是信息的长度,不能发送整个缓冲区,而且结尾不能有"\0"

	if (send(sockConn, sendbuf, length, 0) == SOCKET_ERROR) {
		//printf("来自C++ websocket接受头发送失败\n");
		return -1;
	}

	conlist[sockConn] = &tools;

	//printf("来自C++ WEBSOCKET连接接受成功 IP:[%s]  PORT:[%d]\n", inet_ntoa(addrClient.sin_addr), addrClient.sin_port);
	return sockConn;
}

AFX_EXT_FUNC int lgsend(char* msg, SOCKET sockConn) {
	////printf("来自C++ 信息发送开始,进行封包\n");

	WebSocket* tool = conlist[sockConn];
	//printf("来自C++ 封包工具加载完成\n");
	int msglenn = 0;
	char a;
	do {
		a = msg[msglenn];
		msglenn++;
	} while (a != '\0');

	char* buffer=new char[MAXBUFFER];
	int len = tool->makeFrame(TEXT_FRAME, (unsigned char*)msg, msglenn - 1, (unsigned char*)buffer, MAXBUFFER);
	//printf("来自C++ 信息封包完成\n");

	if (send(sockConn, buffer, len, 0) == SOCKET_ERROR) {
		//printf("来自C++ 信息发送失败\n");
		return -1;
	}
	//printf("来自C++ 信息发送成功,包长:%d\n", len);
	return 0;
}

AFX_EXT_FUNC char* lgrecv(SOCKET sockConn) {
	//printf("来自C++ TCP信息开始接收\n");
	char* recvBuf=new char[MAXBUFFER];
	if (recv(sockConn, recvBuf, MAXBUFFER, 0) <= 0) {
		//printf("来自C++ TCP信息接受失败:%d\n", WSAGetLastError());
		return NULL;
	}
	//printf("来自C++ TCP信息接收成功\n");

	char* out=new char[MAXBUFFER];
	int outlength = 0;
	WebSocket* tool = conlist[sockConn];
	WebSocketFrameType type=tool->getFrame((unsigned char*)recvBuf, MAXBUFFER, (unsigned char*)out, MAXBUFFER, &outlength);
	if (type != TEXT_FRAME) {
		printf("来自C++ 错误的websocket包\n");
		return NULL;
	}
	//printf("来自C++ WebSocket信息解析成功,长:%d,类:%x\n", outlength, type);
	out[outlength + 1] = '\0';
	return out;
}

AFX_EXT_FUNC int closecon (SOCKET sockConn) {
	closesocket(sockConn);
	return 0;
}






