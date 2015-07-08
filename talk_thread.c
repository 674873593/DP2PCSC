#include "talk_thread.h"

void *talk_thread(void *arg)
{
	
	pthread_detach(pthread_self());
	pthread_t friend_thread_id = pthread_self();
	//get socket_fd ok
	socket_fd talk_socket_fd = *(socket_fd *)arg;
	struct sockaddr addr;
	socklen_t addr_len = sizeof(struct sockaddr);
	getpeername(talk_socket_fd, (struct sockaddr *)&addr, &addr_len);
	char ip[16] = {0};
	strcpy(ip, inet_ntoa(((struct sockaddr_in *)&addr)->sin_addr));
	int friend_name_length = get_friend_name_length(&name_address, ip) * sizeof(char);
	char *friend_name = (char *)malloc(friend_name_length);
	memset(friend_name, 0, friend_name_length);
	//get address
	get_friend_name(&name_address, ip, friend_name);
	printf("[friendname]%s\n",friend_name);
	//get friend_name
	enqueue_connector(&connectors, friend_name, friend_thread_id, talk_socket_fd);
	//enqueue_connector
	
	struct friend *this;
	this  = (struct friend *)malloc(sizeof(struct friend));
	memset(this, 0, sizeof(struct friend));
	find_connector_by_threadid(&connectors, friend_thread_id, this);
	if (this == NULL) {
		printf("[ERROR THIS]\n\n--------------\n\n-------------\n\n");
	}
	socket_fd friend_socket_fd = this->friend_socket_fd;
/*	if (friend_socket_fd == 0) {*/
/*		this->state = TALK_SHUTDOWN;*/
/*	}*/
//TODO need to be ctrl-shift-M
	Queue message_recv;
	InitQueue(&message_recv,sizeof(char **),sizeof(char *));
	
	int bufsize = RECV_BUFSIZE;
	
	while (!this->state && !client_shutdown) {
		int recv_result;
		do {	
			char *recvbuf = (char *)malloc(bufsize * sizeof(char));
			memset(recvbuf, 0, bufsize * sizeof(char));
			//TODO can't get the right socket fd
			//printf("[sockfd]%d\n",friend_socket_fd);
			recv_result = recv(friend_socket_fd, recvbuf, bufsize-1, 0);
/*			if (strlen(recvbuf)) {*/
			//	printf("[recv_packge]%s\n[recv_length]%d\n",recvbuf,strlen(recvbuf));
/*			}*/
			EnQueue(&message_recv,recvbuf);
			free(recvbuf);
		} while (recv_result > 0 || (recv_result < 0 && (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)));//receive all data
		int queue_length_max = QueueLength(&message_recv);
		int queue_length = queue_length_max;
		char *message = (char *)malloc(bufsize * sizeof(char) * (queue_length_max + 1));
		memset(message, 0, bufsize * sizeof(char) * (queue_length_max + 1));
		do {
			char *recvbuf = (char *)malloc(bufsize * sizeof(char));
			memset(recvbuf, 0,bufsize * sizeof(char));
			DeQueue(&message_recv,recvbuf);
			memcpy(message + sizeof(*recvbuf) * queue_length_max - queue_length, recvbuf, bufsize);
			free(recvbuf);
			queue_length = QueueLength(&message_recv);
		} while (queue_length > 0);//recombinant all data to message
		
		if (strlen(message)) {
			printf("[message]%s\n[length]%d\n",message,strlen(message));
			show(this->friend_name, message, SHOW_DIRECTION_IN);
		}
		free(message);
		usleep(500);
	}//end while
	free(this);
	shutdown(friend_socket_fd, SHUT_RDWR);
	close(friend_socket_fd);	
	remove_connector(&connectors, this->friend_name);
	pthread_exit((void *)NULL);
	
	//return (void *)NULL;
}


