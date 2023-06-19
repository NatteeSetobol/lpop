#include <pthread.h>
#include "nix_socket.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include "memory_arena.hpp"
#include <sys/wait.h>

struct platform_socket CreateSocket(memory_arena *memoryArena,char* hostname, int port)
{
	//TODO(): Implement IPv6 -- Not sure how to do this on nix machines yet. Need to do more research on this because gethostbyname doesn't support ipv6!!.
	//NOTE(): IPv6 Does not work yet.
	struct platform_socket result = {};
	addrinfo hints = {};
	addrinfo* addrinfoResults = NULL;
	int addrInfoRet = 0;
	char * host = NULL;
	struct sockaddr_in dest_addr={};
	struct hostent *hostEnt = NULL;
	/*
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol |= AI_CANONNAME;
	*/

	result.connected = true;
	host = StripToHostName(memoryArena,hostname);

	hostEnt = gethostbyname(host);

	if (hostEnt == NULL)
	{
		printf( "error getting address info\n");
		result.connected = false;
	} else {
		
		result.socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		if (result.socket != -1) 
		{
			struct sockaddr_in *sockin = NULL;

			dest_addr.sin_family=AF_INET;
			dest_addr.sin_port = htons(port);
			sockin = (struct sockaddr_in*) &addrinfoResults->ai_addr;

			memcpy(&dest_addr.sin_addr, hostEnt->h_addr_list[0], hostEnt->h_length);

			memset(&(dest_addr.sin_zero),'\0',8);

			if (connect(result.socket, (struct sockaddr *) &dest_addr, sizeof(struct sockaddr)) == -1)
			{
				result.connected = false;
				fprintf(stderr, "Value of errno: %d\n", errno);
				printf("could not connect to %s\n", host);
			}
		} else {

			result.connected = false;
			printf("socket failed\n");
		}

	}

	return result;
}



struct platform_socket CreateSocket(char* hostname, int port)
{
	//TODO(): Implement IPv6 -- Not sure how to do this on nix machines yet. Need to do more research on this because gethostbyname doesn't support ipv6!!.
	//NOTE(): IPv6 Does not work yet.
	struct platform_socket result = {};
	addrinfo hints = {};
	addrinfo* addrinfoResults = NULL;
	int addrInfoRet = 0;
	char * host = NULL;
	struct sockaddr_in dest_addr={};
	struct hostent *hostEnt = NULL;
	/*
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol |= AI_CANONNAME;
	*/

	result.connected = true;
	host = StripToHostName(hostname);

	hostEnt = gethostbyname(host);

	if (hostEnt == NULL)
	{
		printf( "error getting address info\n");

		result.connected = false;

	} else {
		
		result.socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		if (result.socket != -1) 
		{
			struct sockaddr_in *sockin = NULL;

			dest_addr.sin_family=AF_INET;
			dest_addr.sin_port = htons(port);
			sockin = (struct sockaddr_in*) &addrinfoResults->ai_addr;

			memcpy(&dest_addr.sin_addr, hostEnt->h_addr_list[0], hostEnt->h_length);

			memset(&(dest_addr.sin_zero),'\0',8);

			if (connect(result.socket, (struct sockaddr *) &dest_addr, sizeof(struct sockaddr)) == -1)
			{
				result.connected = false;
				fprintf(stderr, "Value of errno: %d\n", errno);
				printf("could not connect to %s\n", host);
			}
		} else {

			result.connected = false;
			printf("socket failed\n");
		}

		if (host)
		{
			Free(host);
			host=NULL;
		}
		
	}

	return result;
}

i32 ListenSocket(i32 port, i32 maxClients,on_accept_users *accept_user_function)
{
     i32 sock=0;
     i32 newsockfd=0; 
     i32 portno=0; 
     i32 clilen;
     struct sockaddr_in serv_addr={};
     struct sockaddr_in cli_addr={};

	sock = socket(AF_INET,SOCK_STREAM,0);

	Assert(sock);
	bzero((char*) &serv_addr, sizeof(serv_addr));
	portno = port;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if (bind(sock,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("can not bind socket!\n");
	} else {
		listen(sock,maxClients);

		newsockfd = accept(sock,(struct sockaddr *) &cli_addr, (unsigned int*) &clilen);

		if (newsockfd < 0)
		{
			printf("An error has occured!\n");
		} else {
			
			int pid ;
			pid = fork();

			if (pid == 0)
			{
				accept_user_function(newsockfd);
			} 
			
    		int returnStatus;    
    		wait(&returnStatus);

			/*
			pthread_attr_t attr;
			pthread_t threadID;
			pthread_attr_init(&attr);
			int result = pthread_create(&threadID, &attr, accept_user_function, NULL);

			*/
			/*
			if (accept_user_function)
			{
				accept_user_function(newsockfd);
			}
			*/
			
		}
	}

	return sock;
}


int SendToSocket(struct platform_socket* platformSocket, char* message, int length)
{
	int result = 0;

	result = send(platformSocket->socket,message,length,0);

	return result;
}

int RecvFromSocket(struct platform_socket* platformSocket, char* dest, int len)
{
	int result = 0;

	result = recv(platformSocket->socket,dest,len,0);

	return result;
}

int CloseSocket(struct platform_socket* platformSocket)
{
	close(platformSocket->socket);
}
