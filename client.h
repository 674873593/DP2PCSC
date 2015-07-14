#ifndef __client_H__
	#define __client_H__
	
	
	#include <pthread.h>
	
	#include <stdio.h>
	#include <stdlib.h>
	#include <sys/types.h>
	#include <dirent.h>
	
	

	
	#include "socket.h"
	#include "Queue.h"
	#include "friend.h"
	#include "listen_thread.h"
	#include "talk_thread.h"
	#include "show_thread.h"
	#include "ulib.h"
	#include "file_trans.h"
	
	
	extern socket_fd listen_socket_fd;
	extern Queue name_address;
	extern Queue connectors;
	extern Queue *file_trans_control;
	//extern pthread_t *listen_thread_id;
	
	int client_shutdown;//TRUE or FALSE
	
	void init_socket();
	int input();
	int file_mode();
	void send_file(char *friend_name, char *file_location);
	void send_message(char *friend_name, char *message);
	void send_wrap_split_data(socket_fd send_socket_fd, char *data, char tail);
	void send_split_data(socket_fd send_socket_fd, char *send_data);
	int connect_TCP_by_name(char *friend_name);
#endif /* __client_H__ */ 
	
