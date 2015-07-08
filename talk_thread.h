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
#endif /* __talk_thread_H__ */ 

