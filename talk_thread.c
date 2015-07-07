#include "talk_thread.h"

void *talk_thread(void *arg)
{
	pthread_detach(pthread_self());
	pthread_t friend_thread_id = pthread_self();
	struct friend *this;
	this  = (struct friend *)malloc(sizeof(struct friend));
	find_connector_by_threadid(&connectors, friend_thread_id, this);
	
	socket_fd friend_socket_fd = this->friend_socket_fd;
	Queue message_recv;
	InitQueue(&message_recv,sizeof(char **),sizeof(char *));
	
	int bufsize = RECV_BUFSIZE;
	
	while (!this->state) {
		int recv_result;
		do {	
			char *recvbuf = (char *)malloc(bufsize * sizeof(char));
			recv_result = recv(friend_socket_fd, recvbuf, bufsize-1, 0);
			EnQueue(&message_recv,recvbuf);
			free(recvbuf);
		} while (recv_result > 0 || (recv_result < 0 && (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)));//receive all data
		int queue_length_max = QueueLength(&message_recv);
		int queue_length = queue_length_max;
		char *message = (char *)malloc(bufsize * sizeof(char) * (queue_length_max + 1));
		do {
			char *recvbuf = (char *)malloc(bufsize * sizeof(char));
			DeQueue(&message_recv,recvbuf);
			memcpy(message + sizeof(*recvbuf) * queue_length_max - queue_length, recvbuf, bufsize);
			free(recvbuf);
			queue_length = QueueLength(&message_recv);
		} while (queue_length > 0);//recombinant all data to message
		show(this->friend_name, message);
		free(message);
		usleep(500);
	}//end while
	free(this);
	close(friend_socket_fd);	
	remove_connector(&connectors, this->friend_name);
	pthread_exit((void *)NULL);
	
	//return (void *)NULL;
}


