#ifndef __friend_H__
	#define __friend_H__
	
	#include "Queue.h"
	#include "socket.h"
	#include <pthread.h>
	#include <stdio.h>
	#include <unistd.h>
	
	
	#define FRIEND_ADDRESS_FILE "friend_address"
	#define LINE_LENGTH 100
	
	
	#define TALK_RUNNING 0
	#define TALK_SHUTDOWN 1 
	
	struct friend{
		char *friend_name;
		pthread_t friend_thread_id;
		socket_fd friend_socket_fd;
		int type;//MESSAGE_CONNECT FILE_CONNECT
		int state;};//TALK_RUNNING TALK_SHUTDOWN
	
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
	int dequeue_connector_length(LinkQueue *friend_queue);
	int dequeue_connector(LinkQueue *friend_queue, struct friend *friend_val);
//	int get_connector_socket_fd_by_name(LinkQueue *friend_queue, char *friend_name);
	int find_connector_by_name(LinkQueue *friend_queue, char *friend_name, struct friend *friend_val);
	int find_connector_by_threadid(LinkQueue *friend_queue, pthread_t friend_thread_id, struct friend *friend_val);
	int remove_connector(LinkQueue *friend_queue, socket_fd talk_socket_fd);
	void close_connector(socket_fd talk_socket_fd);
	void close_all_connector(LinkQueue *friend_queue);
	void destory_connector(LinkQueue *friend_queue);
	void print_connector(LinkQueue *queue);
	//int get_friend_addr(char *friend_name,char *friend_ip);
#endif /* __friend_H__ */ 


