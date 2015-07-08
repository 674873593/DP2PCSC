#include "client.h"
static struct sockaddr_in listen_addr_in;

static char inputbuf[INPUT_BUFSIZE];

int main (int argc, char *argv[])
{
	client_shutdown = 0;
	init_friend_name_addr();//must be init first,load friend name reflects to address from file
	init_connector();
	init_socket();
	init_show();
/*	print_name_addr(&name_address);*/
/*	sleep(1);*/
/*	client_shutdown = 1;*/
	
	while(!client_shutdown){
		input();
	}
	shutdown(listen_socket_fd, SHUT_RDWR);
	close(listen_socket_fd);
	destory_friend_name_addr(&name_address);
	destory_connector(&connectors);
	
	sleep(1);
	return 0;
}//end main-function



void init_socket()
{
	listen_socket_fd = socket(PF_INET,SOCK_STREAM,0);//PF_INET->TCP/IP Protocol Family,SOCK_STREAM->TCP
	int optval = 1;
	setsockopt(listen_socket_fd,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(SO_REUSEADDR));//enable port multiplexing
	memset(&listen_addr_in,0,sizeof(listen_addr_in));
	listen_addr_in.sin_family = AF_INET;//AF_INET->TCP/IP Address Family
	listen_addr_in.sin_addr.s_addr = htonl(INADDR_ANY);//wildcard IPv4 address
	listen_addr_in.sin_port = htons(SERVER_PORT);
	bind(listen_socket_fd,(struct sockaddr *)&listen_addr_in,sizeof(listen_addr_in));
	listen(listen_socket_fd,LISTEN_LIST_LENGTH);//set listen list length,begin listen
	//listen_thread_id = (pthread_t *)malloc(sizeof(pthread_t));
	pthread_t listen_thread_id;
	pthread_create(&listen_thread_id, NULL, listen_thread, 0);//begin listen thread

}



int input(){
	int input_bufsize = INPUT_BUFSIZE;
	char *friend_name;
	memset(inputbuf, 0, INPUT_BUFSIZE);
	setbuf(stdin, NULL);
	fgets(inputbuf, input_bufsize, stdin);
	setbuf(stdin, NULL);
	char *result;
	result = strtok(inputbuf, ":");
	char *message;
	friend_name = result;
	result = strtok(NULL, "\n");
	message = result;
	printf("[TEST EXIT] name %s message %s \n",friend_name, message);
	if (friend_name != NULL && message != NULL) {
		printf("[TEST] %s \n", result);
		printf("[TEST] message %s \n", result);		
		send_message(friend_name, message);
		return TRUE;
	}
	printf("[TEST path] %p \n", friend_name);
		
	if (!strcmp(friend_name, "exit\n")){
		client_shutdown = 1;
		return TRUE;
	}
	printf("%s","Usage\t<name><:><message>\n");
	printf("%s","\t<exit>\n");
	printf("%s","\t<file>@<name>:<location>\n");

	return FALSE;	
}

void send_message(char *friend_name,char *message){
	struct friend *this = (struct friend *)malloc(sizeof(struct friend));
	memset(this, 0, sizeof(struct friend));
	int result = find_connector_by_name(&connectors, friend_name, this);//is connectted?
	printf("findresult  %d \n", result);
	if (result) {//make new connect & create talk_thread
		socket_fd friend_socket_fd;
		memset(&friend_socket_fd, 0, sizeof(socket_fd));
		friend_socket_fd = socket(PF_INET, SOCK_STREAM, 0);//PF_INET->TCP/IP Protocol Family,SOCK_STREAM->TCP
		struct sockaddr_in dest_addr;
		char friend_ip[16] = {0};
		int gfa_result;
		gfa_result = get_friend_address(&name_address , friend_name, (char *)&friend_ip);
		
		if (gfa_result) {
			goto end;
		}
		
		dest_addr.sin_family = AF_INET;//AF_INET->TCP/IP Address Family
		dest_addr.sin_port = htons(SERVER_PORT);
		dest_addr.sin_addr.s_addr = inet_addr((char *)&friend_ip);
		memset(&(dest_addr.sin_zero), 0, sizeof(dest_addr.sin_zero));
		
		int result;
		
		result = connect(friend_socket_fd, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr));
		printf("result %d",result);
		if (result == -1) {//connect failed	
			goto end;
		}
		
		
		
		
		pthread_t talk_thread_id;
		//memset(&talk_thread_id, 0, sizeof(pthread_t));
		pthread_create(&talk_thread_id, NULL, talk_thread, (void *)&friend_socket_fd);
		//pthread_detach(talk_thread_id);
/*		{//TODO maybe move to talk_thread.c*/
		//enqueue_connector
/*			this->friend_name = (char *)malloc((strlen(friend_name) + 1) * sizeof(char));*/
/*			memset(this->friend_name, 0, (strlen(friend_name) + 1) * sizeof(char));*/
/*			memcpy(friend_name, this->friend_name, strlen(friend_name));*/
/*			this->friend_thread_id = talk_thread_id;*/
/*			this->friend_socket_fd = friend_socket_fd;*/
/*			this->state = TALK_RUNNING;*/
/*			enqueue_connector(&connectors, this->friend_name, this->friend_thread_id, this->friend_socket_fd);*/
/*			free(this->friend_name);*/
/*		}*/
	}
/*	else{*/
/*		this->friend_socket_fd = get_connector_socket_fd_by_name(&connectors, friend_name);*/
/*		printf("已经链接\n");//TODO send to socket_fd which in connectors by get_connector_socket_fd*/
/*	}*/
	
	
	int input_length = strlen(inputbuf);
	//send_message & show in local tty
	for (int i = 0; i < input_length / SEND_BUFSIZE; i += 1) {
		char *sendbuf = (char *)malloc(SEND_BUFSIZE * sizeof(char));
		memset(sendbuf, 0, SEND_BUFSIZE * sizeof(char));
		//TODO split string to sendbuf
		send(this->friend_socket_fd, sendbuf, strlen(sendbuf), 0);
		free(sendbuf);
	}
	
	show(friend_name, message, SHOW_DIRECTION_OUT);
	
	end:
	free(this);
}



