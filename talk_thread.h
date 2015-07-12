#ifndef __talk_thread_H__
	#define __talk_thread_H__
	
	#include <pthread.h>
	#include <errno.h>
	#include <unistd.h>
	#include <stdlib.h>
	
	#include "Queue.h"
	#include "friend.h"
	#include "socket.h"
	#include "show_thread.h"
	
	
	
	extern Queue name_address;
	extern int client_shutdown;
	extern Queue connectors;
	
	struct talk_thread_arg{
		socket_fd connect_socket_fd;
		int connect_launcher;//TRUE is launcher;FALSE is the accpeter
		int connect_type;//MESSAGE_CONNECT FILE_CONNECT
		void *append;//points to filename or other args may be need in furture
	};
	
//	struct talk_control_arg{
//		socket_fd connect_socket_fd;
//		void *pointer;
//	};
	
//	typedef int (*connect_ptr)(void *arg);
	
//	struct message_connect_arg{
//		socket_fd connect_socket_fd;
//		char *friend_name;
//	};
	
	struct connect_info{
		socket_fd connect_socket_fd;
		char *friend_name;
		char *file_name;
		int connect_launcher;
		Queue *data_recv;
		FILE *file_ptr;
	};
	
	void *talk_thread(void *arg);
	//void recombine_message(LinkQueue *recv_queue,char *message);
	
	int recv_equal_char(socket_fd recv_socket_fd,char ch);
	
	struct connect_info *init_message(socket_fd talk_socket_fd, char *friend_name, Queue *data_recv);
	void show_message(struct connect_info *cinfo);
	void destroy_message(struct connect_info *cinfo);
	
	struct connect_info *init_download(socket_fd talk_socket_fd, char *friend_name, Queue *data_recv, char *file_name, int connect_launcher);
	void download_file(struct connect_info *cinfo);
	void destroy_download(struct connect_info *cinfo);
	
	Queue *init_split_data_recv();
	int recv_unwrap_split_data(socket_fd recv_socket_fd, Queue *data_recv, char *tail);//FALSE when socket closed;TRUE when recv end
	void destroy_split_data_recv(Queue *data_recv);
	
	char *init_data_recombine(Queue *data_recv);
	void recombine_data(LinkQueue *data_recv,char *data);
	void destroy_data_recombine(char *data);
#endif /* __talk_thread_H__ */ 

