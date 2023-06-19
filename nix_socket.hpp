#ifndef _NIX_SOCKET_H_
#define _NIX_SOCKET_H_
#include <string.h>
#include <stdio.h>
#include "socket.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "memory_arena.hpp"
#include "strings.hpp"

#define ON_ACCEPT_USERS(name) void name(i32 newSocket)
typedef ON_ACCEPT_USERS(on_accept_users);

struct platform_socket
{
	int socket;
	bool connected;
};


i32 ListenSocket(i32 port, i32 maxClients,on_accept_users *accept_user_function);
int CloseSocket(struct platform_socket* platformSocket);
struct platform_socket CreateSocket(memory_arena *memoryArena,char* hostname, int port);
#endif
