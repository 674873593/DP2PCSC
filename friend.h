#ifndef __friend_H__
	#define __friend_H__
	
	#include "Queue.h"
	#include "socket.h"
	#include <pthread.h>
	#include <stdio.h>
	
	
	#define FRIEND_ADDRESS_FILE "friend_address"
	#define LINE_LENGTH 100
	
	struct friend{
		char *friend_name;
		pthread_t friend_thread_id;
		socket_fd friend_socket_fd;
		int state;};
	
	Queue connectors;
	
	struct friend_name_addr{
		char *friend_name;
		char *friend_address;};
	Queue name_address;

	void init_friend_name_addr();
	void enqueue_friend_name_addr(LinkQueue *queue, char *friend_name,char *friend_address);
	void dequeue_friend_name_addr(LinkQueue *queue);
	void destory_friend_name_addr(LinkQueue *queue);
	int get_friend_address(LinkQueue *name_address_queue, char *friend_name, char *friend_ip);
	int get_friend_name(LinkQueue *name_address_queue, char *friend_ip, char *friend_name);
	int get_friend_name_length(LinkQueue *name_address_queue, char *friend_ip);
//	void print_name_addr(LinkQueue *queue);//TODO remove debug print
	
	int init_connector();
//	int add_connector(LinkQueue *friend_queue,struct friend *friendinfo);
	int enqueue_connector(LinkQueue *friend_queue, char *friend_name, pthread_t friend_thread_id, socket_fd friend_socket_fd);
	int dequeue_connector(LinkQueue *friend_queue);
//	int get_connector_socket_fd_by_name(LinkQueue *friend_queue, char *friend_name);
	int find_connector_by_name(LinkQueue *friend_queue, char *friend_name, struct friend *friend_find);
	int find_connector_by_threadid(LinkQueue *friend_queue, pthread_t friend_thread_id, struct friend *friend_val);
	int remove_connector(LinkQueue *friend_queue, char *friend_name);
	void destory_connector(LinkQueue *friend_queue);
	void print_connector(LinkQueue *queue);
	//int get_friend_addr(char *friend_name,char *friend_ip);
#endif /* __friend_H__ */ 


