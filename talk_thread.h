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
	//void recombine_message(LinkQueue *recv_queue,char *message);
	Queue *init_data_recv();
	int recv_data(socket_fd recv_socket_fd, Queue *data_recv);
	void destory_data_recv(Queue *data_recv);
	
	char *init_data_recombine(Queue *data_recv);
	void recombine_data(LinkQueue *data_recv,char *data);
	void destory_data_recombine(char *data);
#endif /* __talk_thread_H__ */ 

