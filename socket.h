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
	
	#define SOH '\x1'	//start of headline
	#define STX '\x02'	//start of text
	#define EOT '\x04'	//end of transmission
	#define ENQ '\x05'	//enquiry
	#define ACK '\x06'	//acknowledge
	#define NAK '\x15'	//negative acknowledge
	#define ETB '\x17'	//end of trans. block
	#define CAN '\x18'	//cancel
	
#endif /* __socket_H__ */ 
