#include "win_socket.hpp"

#ifdef COMPILER_MSVC
struct platform_socket CreateSocket(char* hostname, int port)
{
	//TODO(): SET UP FOR IPV6
	//https://docs.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-getaddrinfow
	int addrIndex = 0;
	WSADATA wsData = {};
	int iResult = 0;
	struct in_addr addr = {};
	struct hostent* remoteHost = NULL;
	ADDRINFO hints = {};
	ADDRINFO* results = NULL;
	DWORD addrInfoRet = 0;
	SOCKET sock = 0;
	sockaddr_in server = {};
	struct platform_socket result = {};
	char* host = NULL;
	char* portStr = NULL;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsData);

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;


	host = StripToHostName(hostname);

	portStr = IToS(port);


	addrInfoRet = GetAddrInfo(host, portStr, &hints, &results);
	if (addrInfoRet != 0)
	{
		MessageBox(NULL, "error getting address info", "error getting address info", MB_OK);
		WSACleanup();
	}
	else {
		if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
		{
			MessageBox(NULL, "can't not open socket", "can't not open socket", MB_OK);
		}
		else {

			server.sin_family = results->ai_family;
			server.sin_port = htons(port);

			switch (results->ai_family)
			{
			case AF_INET:
			{
				sockaddr_in* sockaddrin = (sockaddr_in*)results->ai_addr;
				server.sin_addr = sockaddrin->sin_addr;
				break;
			}
			case AF_INET6:
			{
				//server.sin_addr = (LPSOCKADDR)results->ai_addr;
				break;
			}
			}

			if (connect(sock, (struct sockaddr*) & server, sizeof(server)) < 0)
			{
				MessageBox(NULL, "connected error!", "connected error!", MB_OK);
			}
			else {
				result.connected=true;
				//MessageBox(NULL, "connected", "connected!", MB_OK);
				result.socket = sock;
			}
		}
	}

	if (host)
	{
		Free(host);
		host = NULL;
	}

	return result;
}

int SendToSocket(struct platform_socket* platormSocket, char* message, int length)
{
	int result = 0;
	result = send(platormSocket->socket, message, length, 0);
	return result;
}

int RecvFromSocket(struct platform_socket* platformSocket, char* dest, int len)
{
	int result = 0;

	result = recv(platformSocket->socket, dest, len, 0);

	return result;
}
#endif
