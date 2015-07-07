#ifndef __friend_H__
	#define __friend_H__
	
	#include "Queue.h"
	#include "socket.h"
	#include <pthread.h>
	
	
	
	struct friend{
		char *friend_name;
		pthread_t friend_thread_id;
		socket_fd friend_socket_fd;
		int state;};
	
	Queue connectors;

	int init_connector();
	int add_connector(LinkQueue *friend_queue,struct friend *friendinfo);
	int find_connector_by_name(LinkQueue *friend_queue, char *friend_name, struct friend *friend_find);
	int find_connector_by_threadid(LinkQueue *friend_queue, pthread_t friend_thread_id, struct friend *friend_val);
	int remove_connector(LinkQueue *friend_queue, char *friend_name);
	int destory_connector();

	int get_friend_addr(char *friend_name,char *friend_ip);
#endif /* __friend_H__ */ 


