#ifndef __WIN_SOCKET_H__
#define  __WIN_SOCKET_H__ 

#ifdef COMPILER_MSVC
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include "socket.hpp"

#pragma comment (lib, "Ws2_32.lib")

struct platform_socket
{
	bool connected;
	SOCKET socket;
};

struct platform_socket CreateSocket(char* hostname, int port);
int SendToSocket(struct platform_socket* platormSocket, char* message, int length);
int RecvFromSocket(struct platform_socket* platformSocket, char* dest, int len);

#endif
#endif
