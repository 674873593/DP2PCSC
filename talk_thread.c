#include "talk_thread.h"

void *talk_thread(void *arg)
{
	
	pthread_detach(pthread_self());
	pthread_t friend_thread_id = pthread_self();
	//socket_fd ==> address ==> friend_name =is_in_connectors?No=>	enqueue_connector
	//		TODO	TODO		=is_in_connectors?Yes=>	close thread and socket_fd
	//get socket_fd
	socket_fd talk_socket_fd = *(socket_fd *)arg;
	int state = TALK_RUNNING;
	struct sockaddr addr;
	socklen_t addr_len = sizeof(struct sockaddr);
	getpeername(talk_socket_fd, (struct sockaddr *)&addr, &addr_len);
	
	//get address
	char ip[16] = {0};
	strcpy(ip, inet_ntoa(((struct sockaddr_in *)&addr)->sin_addr));
	int friend_name_length = (get_friend_name_length(&name_address, ip) + 1) * sizeof(char);
	char *friend_name = (char *)malloc_safe(friend_name, friend_name_length);
	
	get_friend_name(&name_address, ip, friend_name);
	printf("[friendname]%s\n",friend_name);
	//get friend_name
	
	printf("[enqueue_connector threadid]%d\n",friend_thread_id);
	enqueue_connector(&connectors, friend_name, friend_thread_id, talk_socket_fd);
	//enqueue_connector
	print_connector(&connectors);
/*	struct friend *this;*/
/*	this  = (struct friend *)malloc(sizeof(struct friend));*/
/*	memset(this, 0, sizeof(struct friend));*/
/*	find_connector_by_threadid(&connectors, friend_thread_id, this);*/
/*	if (this == NULL) {*/
/*		printf("[ERROR THIS]\n\n--------------\n\n-------------\n\n");*/
/*	}*/
	printf("[this state]%d\n",state);
	//socket_fd friend_socket_fd = this->friend_socket_fd;
	if (talk_socket_fd == 0) {
		state = TALK_SHUTDOWN;
	}
	
	
	//need a new function wrap and un_wrap
	//int wrap(const char *from,const char head,const char tail,char *dst)
	//int un_wrap(const char *from,char head,char tail,char *dst)
	//recv head control message
	//	type + ETB
	//send ACK + ETB

	//head
	//recv data + ETB
	//send ACK + ETB
	//
	//	message:
	//	while{recv SOH + message + ETB}
	//	
	//	file:
	//	file_trans->accept_state FILE_UNSURE_ACCEPT wait	
	//	file_trans->accept_state FILE_REFUSED CAN + ETB exit
	//	file_trans->accept_state FILE_ACCEPT ACK + ETB
	//	recv data + ETB
	//	send ACK + ETB
	//	recv EOT
	
	//judge connect type
		
	
/*	//if type == MESSAGE_CONNECT*/
/*	//while {recv data ;error goto end*/
/*	//	recombine*/
/*	//	un_wrap*/
/*	//	show message}*/
/*	*/
/*	//if type == FILE_CONNECT*/
/*	//open file*/
/*	//while {recv data ;error close and goto end*/
/*	//	recombine*/
/*	//	un_wrap*/
/*	//		write*/
/*	//		close*/
/*	//		send ACK	*/
/*	//	}*/
/*	//*/
/*	//*/
	
	//while {recv data;if error set state = ONRUN/ONINIT
	//	if(!error){
	//		un_wrap
	//	}	
	//	callback(talk_listener_list[type,state],arg)
	//}
	
	
	//TALKFUNC talk_listener_list[][]{
	//	{init_message,show_message,destory_message},
	//	{init_file,download_file,close_file}
	//}
	
	//init_data_queue()--malloc
	//new function recv_queue(Queue)
	//destory_data_queue()--free
	//struct talk_listener_arg{
	//	char *un_wrap_message;
	//	void *pointer;
	//}arg;
	//
	//recv type = atoi(un_wrap(data))
	//send wrap(ACK)
	//state = ONINIT
	//if type == FILE_CONNECT;arg->pointer = FILE *
	//if type == MESSAGE_CONNECT;arg->pointer = NULL
	//callback(talk_listener_list[type,state],arg)
	//while {recv data;
	//	if error {set state = ONRUN/ONDESTROY}
	//	if(!error){
	//		un_wrap
	//	}	
	//	callback(talk_listener_list[type,state],arg)
	//}
	
	Queue *data_recv = init_data_recv();
	
	//destory_data_recv(data_recv);
	
/*	Queue message_recv;*/
/*	InitQueue(&message_recv, sizeof(char *));*/
/*	*/
/*	*/
	while (!state && !client_shutdown) {
/*		int recv_result;*/
/*		int recv_end = 0;*/
/*			*/
/*		do {	*/
/*			printf("[---------malloc---------]\n");*/
/*			char *recvbuf = (char *)malloc_safe(recvbuf, (RECV_BUFSIZE + 1) * sizeof(char));*/
/*			//memset(recvbuf, 0, (RECV_BUFSIZE + 1) * sizeof(char));*/
/*			printf("[begin recv]\n");*/
/*			recv_result = recv(talk_socket_fd, recvbuf, RECV_BUFSIZE - 1, 0);*/
/*			printf("[recv result]%d\n",recv_result);*/
/*			printf("[recv buff]%s\n",recvbuf);*/
/*			//printf("[get wrap]%c\n",get_wrap(recvbuf));*/
/*			if (!compare_wrap(recvbuf,ETB)) {*/
/*				recv_end = 1;*/
/*				un_wrap(recvbuf, NULL);*/
/*			}*/
/*//			if (strcspn(recvbuf,"\x17") == (recv_result - 1)) {*/
/*//				memset(recvbuf + recv_result - 1, 0, 1);*/
/*//				recv_end = 1;*/
/*//			}*/
/*			*/


/*			EnQueue(&message_recv, &recvbuf);*/
/*			if (recv_result == 0) */
/*				goto end;*/
/*			if (recv_result < 0 && !(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN))*/
/*				goto end;*/
/*			*/
/*			*/
/*		} while (!recv_end &&( recv_result > 0 || (recv_result < 0 && (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN))));//receive all data*/
/*		printf("[get out from while!!!!]\n");*/
/*	*/
/*	*/
/*	*/

/*	*/

	//recombine_data
/*		int queue_length_max = QueueLength(&message_recv);*/
/*		char *message = (char *)malloc_safe(message, RECV_BUFSIZE * sizeof(char) * (queue_length_max + 1));*/

/*		recombine_message(&message_recv, message);*/
		if(recv_data(talk_socket_fd, data_recv) == FALSE)
			goto end;
		char *data = init_data_recombine(data_recv);
		recombine_data(data_recv, data);
		
	
		//show message
		if (strlen(data)){
			printf("[message]%s\n[length]%d\n",data,strlen(data));
			show(friend_name, data, SHOW_DIRECTION_IN);
		}
/*		if (strlen(message)) {*/
/*			printf("[message]%s\n[length]%d\n",message,strlen(message));*/
/*			show(friend_name, message, SHOW_DIRECTION_IN);*/
/*		}*/
/*		free_safe(message);*/
		destory_data_recombine(data);
		
		
		
		usleep(500);
	}//end while
	end:
	printf("[End come need to free num]%d\n",QueueLength(data_recv));
/*	while (QueueLength(&message_recv)) {//prevent malloc without free*/
/*		char *recvbuf;*/
/*		DeQueue(&message_recv, &recvbuf);*/
/*		free_safe(recvbuf);*/
/*		//printf("[---------freeend---------]\n");*/
/*	}*/
/*	DestoryQueue(&message_recv);*/
	destory_data_recv(data_recv);	
	
	
	
	
	
/*	shutdown(talk_socket_fd, SHUT_RDWR);*/
/*	close(talk_socket_fd);	*/
/*	printf("[need to be remove]%s\n",friend_name);*/
	
/*	remove_connector(&connectors, friend_name);*/
	printf("[begin find_connector by threadid]%d\n",friend_thread_id);
	printf("[connectors next]%p %p\n",connectors,(&connectors)->front);
	if (!find_connector_by_threadid(&connectors, friend_thread_id, NULL)) {
		printf("[need to be remove]%s sockfd=%d\n",friend_name,talk_socket_fd);
		remove_connector(&connectors, talk_socket_fd);
	}
	close_connector(talk_socket_fd);
/*	free_safe(this);*/
	free_safe(friend_name);
	pthread_exit((void *)NULL);
	//return (void *)NULL;
}



Queue *init_data_recv()
{
	Queue *data_recv = (Queue *)malloc_safe(data_recv, sizeof(Queue));
	InitQueue(data_recv, sizeof(char *));
	return data_recv;
}

int recv_data(socket_fd recv_socket_fd, Queue *data_recv)
{
	int recv_result;
	int recv_end = 0;
		
	do {	
		printf("[---------malloc---------]\n");
		char *recvbuf = (char *)malloc_safe(recvbuf, (RECV_BUFSIZE + 1) * sizeof(char));
		//memset(recvbuf, 0, (RECV_BUFSIZE + 1) * sizeof(char));
		printf("[begin recv]\n");
		recv_result = recv(recv_socket_fd, recvbuf, RECV_BUFSIZE - 1, 0);
		printf("[recv result]%d\n",recv_result);
		printf("[recv buff]%s\n",recvbuf);
		//printf("[get wrap]%c\n",get_wrap(recvbuf));
		if (!compare_wrap(recvbuf,ETB)) {
			recv_end = 1;
			un_wrap(recvbuf, NULL);
		}
/*			if (strcspn(recvbuf,"\x17") == (recv_result - 1)) {*/
/*				memset(recvbuf + recv_result - 1, 0, 1);*/
/*				recv_end = 1;*/
/*			}*/
		


		EnQueue(data_recv, &recvbuf);
		if (recv_result == 0) 
			return FALSE;
		if (recv_result < 0 && !(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN))
			return FALSE;
		
		
	} while (!recv_end &&( recv_result > 0 || (recv_result < 0 && (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN))));
	return TRUE;
}

void destory_data_recv(Queue *data_recv)
{
	while (QueueLength(data_recv)) {//prevent malloc without free
		char *recvbuf;
		DeQueue(data_recv, &recvbuf);
		free_safe(recvbuf);
		//printf("[---------freeend---------]\n");
	}
	DestoryQueue(data_recv);
	free_safe(data_recv);
}

char *init_data_recombine(Queue *data_recv)
{
	int queue_length_max = QueueLength(data_recv);
	char *data = (char *)malloc_safe(data, RECV_BUFSIZE * sizeof(char) * (queue_length_max + 1));
	return data;	
}

void recombine_data(LinkQueue *data_recv,char *data)
{
	int queue_length_max = QueueLength(data_recv);
	int queue_length = queue_length_max;
	int data_length = 0;
	while (queue_length > 0) {
		char *recvbuf;
		DeQueue(data_recv, &recvbuf);
/*		printf("[combine recvbuf]%s\n",recvbuf);*/
/*		printf("[combine recvbuflength]%d\n",strlen(recvbuf));*/
/*		printf("[combine message before]%s\n",data);*/
/*		printf("[combine message start length]%d\n",(RECV_BUFSIZE - 1) * (queue_length_max - queue_length));*/
		if (data != NULL){
			memcpy(data + data_length, recvbuf, strlen(recvbuf));
			data_length += strlen(recvbuf);
		}
/*		printf("[combine message]%s\n",data);*/
/*		*/
/*		printf("[combine message_length]%d\n",strlen(data));*/
		free_safe(recvbuf);
		//printf("[---------freein---------]\n");
		queue_length = QueueLength(data_recv);
	} //recombine all data to message
	printf("[combine message finally]%s\n",data);
}

void destory_data_recombine(char *data)
{
	free_safe(data);
}
