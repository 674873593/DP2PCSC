#ifndef __talk_thread_H__
	#define __talk_thread_H__
	
	#include <pthread.h>
	#include <errno.h>
	#include <unistd.h>
	
	#include "Queue.h"
	#include "friend.h"
	#include "socket.h"
	#include "show_thread.h"
	
	
	extern Queue name_address;
	extern int client_shutdown;
	extern Queue connectors;
	
	void *talk_thread(void *arg);
	void recombine_message(LinkQueue *recv_queue,char *message);
	void close_talk_thread(socket_fd talk_socket_fd, char *friend_name);
	void close_all_talk_thread(LinkQueue *connectors);
#endif /* __talk_thread_H__ */ 

