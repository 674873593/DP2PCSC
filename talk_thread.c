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
	char *friend_name = (char *)malloc(friend_name_length);
	memset(friend_name, 0, friend_name_length);
	
	get_friend_name(&name_address, ip, friend_name);
	printf("[friendname]%s\n",friend_name);
	//get friend_name
	
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
	Queue message_recv;
	InitQueue(&message_recv, sizeof(char **), sizeof(char *));
	
	//int bufsize = RECV_BUFSIZE;
	
	while (!state && !client_shutdown) {
		int recv_result;
		int recv_end = 0;
		do {	
			char *recvbuf = (char *)malloc((RECV_BUFSIZE + 1) * sizeof(char));
			printf("[---------malloc---------]\n");
			memset(recvbuf, 0, (RECV_BUFSIZE + 1) * sizeof(char));
			//TODO can't get the right socket fd
			//printf("[sockfd]%d\n",friend_socket_fd);
			printf("[begin recv]\n");
			recv_result = recv(talk_socket_fd, recvbuf, RECV_BUFSIZE - 1, 0);
			printf("[recv result]%d\n",recv_result);
			printf("[recv buff]%s\n",recvbuf);
			
			if (strcspn(recvbuf,"\x4") == (recv_result - 1)) {
				memset(recvbuf + recv_result - 1, 0, 1);
				recv_end = 1;
			}
			EnQueue(&message_recv, &recvbuf);
			if (recv_result == 0) 
				goto end;
			if (recv_result < 0 && !(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN))
				goto end;
			
			
		} while (!recv_end &&( recv_result > 0 || (recv_result < 0 && (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN))));//receive all data
		printf("[get out from while!!!!]\n");
		
/*		int queue_length_max = QueueLength(&message_recv);*/
/*		int queue_length = queue_length_max;*/
/*		char *message = (char *)malloc(bufsize * sizeof(char) * (queue_length_max + 1));*/
/*		memset(message, 0, bufsize * sizeof(char) * (queue_length_max + 1));*/
/*		do {*/
//			char *recvbuf/*= (char *)malloc(bufsize * sizeof(char))*/;
/*			//memset(recvbuf, 0,bufsize * sizeof(char));*/
/*			DeQueue(&message_recv, &recvbuf);*/
/*			memcpy(message + bufsize * (queue_length_max - queue_length), recvbuf, bufsize);*/
/*			free(recvbuf);*/
/*			queue_length = QueueLength(&message_recv);*/
/*		} while (queue_length > 0);//recombinant all data to message*/
/*		*/
/*		if (strlen(message)) {*/
/*			printf("[message]%s\n[length]%d\n",message,strlen(message));*/
/*			show(this->friend_name, message, SHOW_DIRECTION_IN);*/
/*		}*/
/*		free(message);*/
		int queue_length_max = QueueLength(&message_recv);
		char *message = (char *)malloc(RECV_BUFSIZE * sizeof(char) * (queue_length_max + 1));
		memset(message, 0, RECV_BUFSIZE * sizeof(char) * (queue_length_max + 1));
		recombine_message(&message_recv, message);
		if (strlen(message)) {
			printf("[message]%s\n[length]%d\n",message,strlen(message));
			show(friend_name, message, SHOW_DIRECTION_IN);
		}
		free(message);
		
		
		
		
		usleep(500);
	}//end while
	end:
	printf("[End come need to free num]%d\n",QueueLength(&message_recv));
	while (QueueLength(&message_recv)) {//prevent malloc without free
		char *recvbuf;
		DeQueue(&message_recv, &recvbuf);
		free(recvbuf);
		//printf("[---------freeend---------]\n");
	}
	DestoryQueue(&message_recv);
	
/*	shutdown(talk_socket_fd, SHUT_RDWR);*/
/*	close(talk_socket_fd);	*/
/*	printf("[need to be remove]%s\n",friend_name);*/
	
/*	remove_connector(&connectors, friend_name);*/
	printf("[begin find_connector]\n");
	if (!find_connector_by_threadid(&connectors, friend_thread_id, NULL)) {
		printf("[need to be remove]%s\n",friend_name);
		remove_connector(&connectors, friend_name);
	}
	close_talk_thread(talk_socket_fd, friend_name);
/*	free(this);*/
	free(friend_name);
	pthread_exit((void *)NULL);
	//return (void *)NULL;
}

void recombine_message(LinkQueue *recv_queue,char *message)
{
	int queue_length_max = QueueLength(recv_queue);
	int queue_length = queue_length_max;
	int message_length = 0;
	while (queue_length > 0) {
		char *recvbuf;
		DeQueue(recv_queue, &recvbuf);
		printf("[combine recvbuf]%s\n",recvbuf);
		printf("[combine recvbuflength]%d\n",strlen(recvbuf));
		printf("[combine message before]%s\n",message);
		printf("[combine message start length]%d\n",(RECV_BUFSIZE - 1) * (queue_length_max - queue_length));
		if (message != NULL){
			memcpy(message + message_length, recvbuf, strlen(recvbuf));
			message_length += strlen(recvbuf);
		}
		printf("[combine message]%s\n",message);
		
		printf("[combine message_length]%d\n",strlen(message));
		free(recvbuf);
		//printf("[---------freein---------]\n");
		queue_length = QueueLength(recv_queue);
	} //recombinant all data to message
}

void close_talk_thread(socket_fd talk_socket_fd, char *friend_name)
{
	struct friend *this;
	this  = (struct friend *)malloc(sizeof(struct friend));
	memset(this, 0, sizeof(struct friend));

	
		shutdown(talk_socket_fd, SHUT_RDWR);
		close(talk_socket_fd);	
		
	free(this);
}

void close_all_talk_thread(LinkQueue *connectors)
{
/*	while(QueueLength(connectors)){*/
/*		struct friend *this = (struct friend *)malloc(sizeof(struct friend));*/
/*		memset(this, 0, sizeof(struct friend));*/
/*		int friend_name_length = dequeue_connector_length(connectors) + 1;*/
/*		this->friend_name = (char *)malloc(friend_name_length * sizeof(char));*/
/*		memset(this->friend_name, 0, friend_name_length * sizeof(char));*/
/*		printf("[begin dequeue one connector]!!\n");*/
/*		dequeue_connector(connectors, this);*/
/*		close_talk_thread(this->friend_socket_fd, this->friend_name);*/
/*		free(this->friend_name);*/
/*		free(this);*/
/*	}*/
	QNode *p = connectors->front;
	
	//printf("[PTR]%p\n",p);
	//printf("[print connectors]\n");
	while((p = p->next)){
		close_talk_thread(((struct friend *)p->pointer)->friend_socket_fd, ((struct friend *)p->pointer)->friend_name);
	}
}
