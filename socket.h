#ifndef __socket_H__
	#define __socket_H__
	
	#include <netinet/in.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>
	
	typedef int socket_fd;
	socket_fd listen_socket_fd;
	
	#define SERVER_PORT 12345
	#define RECV_BUFSIZE 10
	#define SEND_BUFSIZE RECV_BUFSIZE
	#define INPUT_BUFSIZE 1024*1024*8
	#define LISTEN_LIST_LENGTH 20
	
#endif /* __socket_H__ */ 
