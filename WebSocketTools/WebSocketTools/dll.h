#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define AFX_EXT_FUNC extern "C" __declspec(dllimport)
#ifdef _USRDLL
#define AFX_EXT_FUNC extern "C" __declspec(dllexport)
#endif

#ifdef NODLL
#define AFX_EXT_FUNC 
#endif 



#include<cstdio>
#include<iostream>
using namespace std;
#include "WebSocket.h"
#include<map>
#include<WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

WSADATA wsaData;//winsock信息
SOCKET sockSrv;//套接字句柄
SOCKADDR_IN addrSrv;//套接字初始化信息
map<SOCKET, WebSocket*> conlist;

AFX_EXT_FUNC int lginit(int port);
AFX_EXT_FUNC SOCKET lgaccept();
AFX_EXT_FUNC int lgsend(char* msg, SOCKET sockConn);
AFX_EXT_FUNC char* lgrecv(SOCKET sockConn);
AFX_EXT_FUNC int closecon(SOCKET sockConn);