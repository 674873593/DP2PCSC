#ifndef __listen_thread_H__
	#define __listen_thread_H__
	
	#include <pthread.h>
	#include <unistd.h>
	#include "socket.h"
	#include "talk_thread.h"
	#include "client.h"
	
	extern socket_fd listen_socket_fd;
	extern int client_shutdown;
	//pthread_t *listen_thread_id;
	void *listen_thread(void *arg);
	
#endif /* __listen_thread_H__ */ 
