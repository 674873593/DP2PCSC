#ifndef __client_H__
	#define __client_H__
	
	
	#include <pthread.h>
	
	#include <stdio.h>
	#include <stdlib.h>
	
	

	
	#include "socket.h"
	#include "Queue.h"
	#include "friend.h"
	#include "listen_thread.h"
	#include "talk_thread.h"
	#include "show_thread.h"
	
	

	#define TRUE 0
	#define FALSE 1
	
	
	extern socket_fd listen_socket_fd;
	extern Queue name_address;
	//extern pthread_t *listen_thread_id;
	
	int client_shutdown;
	
	void init_socket();
	int input();
	void send_message(char *friend_name, char *message);
	
#endif /* __client_H__ */ 
	
