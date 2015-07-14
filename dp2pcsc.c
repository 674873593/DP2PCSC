#include "client.h"
static struct sockaddr_in listen_addr_in;

static char inputbuf[INPUT_BUFSIZE];

int main (int argc, char *argv[])
{
	client_shutdown = 0;
	init_friend_name_addr();//must be init first,load friend name reflects to address from file
	init_connector(&connectors);
	init_socket();
	init_show();
	init_file_trans_control();
/*	print_name_addr(&name_address);*/
/*	sleep(1);*/
/*	client_shutdown = 1;*/
	
	while(!client_shutdown){
		input();
	}
	shutdown(listen_socket_fd, SHUT_RDWR);
	close(listen_socket_fd);
	printf("here!!\n");
	destroy_friend_name_addr(&name_address);
	printf("there!\n");
	close_all_connector(&connectors);//talk_thread can't be closed by themself because recv is blocking
	while (connector_length(&connectors)){
		printf("[Connectors Length]%d\n",connector_length(&connectors));
		usleep(50);
	}
	destroy_connector(&connectors);
	destroy_file_trans_control(file_trans_control);
	destroy_show_tty();
	sleep(1);//wait for other thread exit,not necssary but that can make it easy for valbrind check memory leak (include still reachable)
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
	//listen_thread_id = (pthread_t *)malloc_safe(sizeof(pthread_t));
	pthread_t listen_thread_id;
	pthread_create(&listen_thread_id, NULL, listen_thread, 0);//begin listen thread

}



int input(){
	printf("> ");
	memset(inputbuf, 0, INPUT_BUFSIZE);
	setbuf(stdin, NULL);
	fgets(inputbuf, INPUT_BUFSIZE, stdin);
	setbuf(stdin, NULL);
	printf("[inputbuf length]%ld\n",strlen(inputbuf)*sizeof(char));
	char *friend_name = strtok(inputbuf, ":");
	char *message = strtok(NULL, "\n");
	//printf("[TEST EXIT] name %s message %s \n",friend_name, message);
	if (friend_name != NULL && message != NULL) {
	//	printf("[TEST] %s \n", result);
	//	printf("[TEST] message %s \n", result);		
		send_message(friend_name, message);//EOF 0x04
		return TRUE;
	}
	//printf("[TEST path] %p \n", friend_name);
		
	if (!strcmp(friend_name, "exit\n")){
		client_shutdown = 1;
		return TRUE;
	}
	
	if (!strcmp(friend_name, "file\n")){
		file_mode();
		return TRUE;
	}
	
	printf("%s", "usage\t<$name><:><$message>\n");
	printf("%s", "\t<exit>\n");
	printf("%s", "\t<file>\n");
	
/*	if (friend_name != NULL)*/
/*		free_safe(friend_name);*/
/*	if (message != NULL)*/
/*		free_safe(message);*/
	
	return FALSE;	
}

int file_mode(){
	printf("[file_mode]> ");
	memset(inputbuf, 0, INPUT_BUFSIZE);
	setbuf(stdin, NULL);
	fgets(inputbuf, INPUT_BUFSIZE, stdin);
	setbuf(stdin, NULL);
	char *friend_name;
	char *trans_location;
	friend_name = strtok(inputbuf, ":");
	trans_location = strtok(NULL, "\n");
	
	printf("[send] %s %s \n", friend_name, trans_location);	
	if (friend_name != NULL && trans_location != NULL) {
	//	printf("[TEST] %s \n", result);
		printf("[bingo] %s %s \n", friend_name, trans_location);		
		send_file(friend_name, trans_location);//EOF 0x04 EOT 0x04
		return TRUE;
	}
	
	printf("%s", "file mode usage\t<$name><:><$location>\n");
	return FALSE;
}

void send_file(char *friend_name, char *file_location){
	//is friend exist?
	//is file exist?
	//trans file thread and connect success ;;;; must Init File Server Socket in main
	printf("[send file]%s %s\n",friend_name,file_location);
	
	
	
	//is dir? tar
	//else is file? get filename
	//is / in it? yes->get dir and filename
	//is / in it? no->get filename 
	char *file_name;
	socket_fd friend_socket_fd;
	int file_trans_fd;
	file_name = strrchr(file_location, '/');
	if (file_name != NULL) {
		file_name++;
	}else{
		file_name = file_location;
	}
	//file_name = strtok(file_name, "\n");
	DIR *dir = opendir(file_location);
	if (dir != NULL) {
		printf("error: can't trans a directory\n");
		closedir(dir);
		return;
	}
	
	FILE *file = fopen(file_location, "rb");
	if (file == NULL) {
		printf("error: can't find file %s %s\n",file_location,file_name);
		//printf("[ferror %d]\n",ferror(file));
	}else{
		printf("file_name %s\n",file_name);
		
		friend_socket_fd = connect_TCP_by_name(friend_name);
		if (friend_socket_fd <= 2) 
			goto end;
		//register file_trans to file_trans_control
		file_trans_fd = init_file_trans(file_trans_control, TRUE, file_name, file_location, 0);
		
		
		//create thread
		pthread_t talk_thread_id;
		struct talk_thread_arg *tt_arg = (struct talk_thread_arg *)malloc_safe(tt_arg, sizeof(struct talk_thread_arg));
		tt_arg->connect_socket_fd = friend_socket_fd;
		tt_arg->connect_launcher = TRUE;
		tt_arg->connect_type = FILE_CONNECT;
		printf("[connect_type main]%d\n",tt_arg->connect_type);
		tt_arg->file_trans_fd = file_trans_fd;
		pthread_create(&talk_thread_id, NULL, talk_thread, (void *)tt_arg);	
		
			
/*		//send head control data */
/*		send_wrap_split_data(friend_socket_fd, FILE_CONNECT_STR, ETB);*/
/*		printf("[send head control data]connect type = %s\n",MESSAGE_CONNECT_STR);*/
/*		//recv head control ACK*/
/*		if (recv_equal_char(friend_socket_fd, ACK) == FALSE) goto end;*/
/*		printf("[recv_ACK]\n");*/
		


		fclose(file);
	}
	
	end:
		return;
	//nonblock mode
	//Sender: open file;init struct file_trans;
	//Recvier: init struct file_trans;recv
	//	Sender: SOH+filename+ETB;
	//	Recvier: struct file_trans->name = filename;file_trans->id=id++;file_trans->accept_state = FILE_UNSURE_ACCEPT
	//		Sender: ENQ;wait for recv in while
	//		Recvier: break recv while;show Servername+file_trans->filename+file_trans->id;wait for file_trans->accept_state==FILE_ACCEPT(FILE_ACCEPT;FILE_REFUSED;FILE_UNSURE_ACCEPT);ACK/CAN;if CAN close socket;goto end
	//		Recv FileMode:<fileaccept><:><$id>,find_trans by id,file_trans->accept_state=FILE_ACCEPT
	//				<filerefused><:><$id>,file_trans->accept_state=FILE_REFUSED
	//		Recvier: open file
	//			repeat
	//				Sender: STX+package+ETB;recv for ACK
	//				Recvier: fwrite package into file;send ACK
	//			endrepeat
	//				Sender: EOT;
	//				end:	Recvier: close socket;close file
	//				end:	Sender:close socket;close file
	//Send will exit when client_shutdown==1 in while-check 
}

void send_message(char *friend_name, char *message){
	struct friend *this = (struct friend *)malloc_safe(this, sizeof(struct friend));
	
	int result = find_connector_by_name(&connectors, friend_name, this, MESSAGE_CONNECT);//is connectted?
	print_connector(&connectors);
	printf("findresult  %d \n", result);
	if (result) {//TODO new function  make new connect & create talk_thread
			//TODO ensure this->friend_socket_fd be refreshed
		socket_fd friend_socket_fd;
		memset(&friend_socket_fd, 0, sizeof(socket_fd));
/*		friend_socket_fd = socket(PF_INET, SOCK_STREAM, 0);//PF_INET->TCP/IP Protocol Family,SOCK_STREAM->TCP*/
/*		*/
/*		struct sockaddr_in dest_addr;*/
/*		char friend_ip[16] = {0};*/
/*		int gfa_result;*/
/*		gfa_result = get_friend_address(&name_address , friend_name, (char *)&friend_ip);*/
/*		printf("[gfa_result]%d\n",gfa_result);*/
/*		printf("[get_address result]%s\n",friend_ip);*/
/*		if (gfa_result) {*/
/*			goto end;*/
/*		}*/


/*		*/
/*		dest_addr.sin_family = AF_INET;//AF_INET->TCP/IP Address Family*/
/*		dest_addr.sin_port = htons(SERVER_PORT);*/
/*		dest_addr.sin_addr.s_addr = inet_addr((char *)&friend_ip);*/
/*		memset(&(dest_addr.sin_zero), 0, sizeof(dest_addr.sin_zero));*/
/*		*/
/*		int result;*/
/*		printf("new connect\n\n\n\n!!!!!\n\n\n\n!!!!\n\n\n");	*/
/*		result = connect(friend_socket_fd, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr));*/
/*		printf("[connect result] %d\n",result);*/
/*		if (result == -1) {//connect failed	*/
/*			goto end;*/
/*		}*/
		
		friend_socket_fd = connect_TCP_by_name(friend_name);
		if (friend_socket_fd <= 2) 
			goto end;
		
		pthread_t talk_thread_id;
		struct talk_thread_arg *tt_arg = malloc_safe(tt_arg, sizeof(struct talk_thread_arg));
		tt_arg->connect_socket_fd = friend_socket_fd;
		tt_arg->connect_launcher = TRUE;
		tt_arg->connect_type = MESSAGE_CONNECT;
		tt_arg->file_trans_fd = -1;
		//memset(&talk_thread_id, 0, sizeof(pthread_t));
		pthread_create(&talk_thread_id, NULL, talk_thread, (void *)tt_arg);
		//pthread_detach(talk_thread_id);
/*		{//TODO maybe move to talk_thread.c*/
		//enqueue_connector
/*			this->friend_name = (char *)malloc((strlen(friend_name) + 1) * sizeof(char));*/
/*			memset(this->friend_name, 0, (strlen(friend_name) + 1) * sizeof(char));*/
/*			memcpy(friend_name, this->friend_name, strlen(friend_name));*/
/*			this->friend_thread_id = talk_thread_id;*/
		this->friend_socket_fd = friend_socket_fd;
		
		
/*		//send head control data */
/*		send_wrap_split_data(this->friend_socket_fd, MESSAGE_CONNECT_STR, ETB);*/
/*		printf("[send head control data]connect type = %s\n",MESSAGE_CONNECT_STR);*/
/*		//recv head control ACK*/
/*		if (recv_equal_char(this->friend_socket_fd, ACK) == FALSE) goto end;*/
/*		printf("[recv_ACK]\n");*/
		
		
/*			this->state = TALK_RUNNING;*/
/*			enqueue_connector(&connectors, this->friend_name, this->friend_thread_id, this->friend_socket_fd);*/
/*			free_safe(this->friend_name);*/
/*		}*/
	}

/*	int message_length = strlen(message);*/
/*	int wrap_message_length = message_length + 2;*/
/*	*/
/*	//change to wrap and unwrap*/
/*	//TODO make into a new function message_insert(char *src,char *insert,callback *function)*/
/*	//TODO new dst = malloc(srclen+insertlen+1);memset()*/
/*	//TODO dst = src+insert*/
/*	//TODO callback(dst)*/
/*	//TODO free_safe(dst)*/
/*	*/
/*	char *wrap_message = (char *)malloc(wrap_message_length * sizeof(char));*/
/*	memset(wrap_message, 0, wrap_message_length * sizeof(char));*/
/*	strncpy(wrap_message, message, message_length);*/
/*	strncpy((wrap_message + message_length), "\x4", 1);*/
/*	int wrap_message_length = strlen(message) + 2;*/
/*	char *wrap_message = (char *)malloc_safe(wrap_message, wrap_message_length);*/
/*	wrap(message, ETB, wrap_message);*/
/*	printf("[wrap]%s\n",wrap_message);*/
/*	printf("[message]%s\n",message);*/
/*	//int input_length = strlen(message);*/
/*	//send_message & show in local tty*/
/*	printf("[begin send]\n");*/
/*	printf("[INPUTSIZE]%d",strlen(wrap_message));*/
/*	printf("[this->fd]%d\n",this->friend_socket_fd);*/
/*	int send_result;*/
/*	*/
/*	//TODO make a new function send_split(char *data) recv_split(LinkQueue *)*/
/*	for (int i = 0; i <= wrap_message_length / SEND_BUFSIZE; i += 1) {*/
/*		char *sendbuf = (char *)malloc_safe(sendbuf, (SEND_BUFSIZE + 1) * sizeof(char));*/
/*		strncpy(sendbuf, wrap_message + i * SEND_BUFSIZE, SEND_BUFSIZE);*/
/*		send_result = send(this->friend_socket_fd, sendbuf, strlen(sendbuf), 0);*/
/*		free_safe(sendbuf);*/
/*	}*/
/*	free_safe(wrap_message);*/
/*	*/
	
	send_wrap_split_data(this->friend_socket_fd, message, ETB);
	printf("[send data end]\n");
	show(friend_name, message, SHOW_DIRECTION_OUT);
	
	end:
	free_safe(this);
	
}

void send_wrap_split_data(socket_fd send_socket_fd, char *data, char tail)
{
	int wrap_data_length = strlen(data) * sizeof(char) + sizeof(tail);
	char *wrap_data = (char *)malloc_string_safe(wrap_data, wrap_data_length);
	wrap(data, tail, wrap_data);
	send_split_data(send_socket_fd, wrap_data);
	free_safe(wrap_data);
	
}

void send_split_data(socket_fd send_socket_fd, char *send_data)
{
	int send_data_length = strlen(send_data) + 2;
	for (int i = 0; i <= send_data_length / SEND_BUFSIZE; i += 1) {
		char *sendbuf = (char *)malloc_string_safe(sendbuf, SEND_BUFSIZE * sizeof(char));
		strncpy(sendbuf, send_data + i * SEND_BUFSIZE, SEND_BUFSIZE);
		send(send_socket_fd, sendbuf, strlen(sendbuf), 0);
		free_safe(sendbuf);
	}
}

int connect_TCP_by_name(char *friend_name)
{
	int connect_socket_fd = socket(PF_INET, SOCK_STREAM, 0);//PF_INET->TCP/IP Protocol Family,SOCK_STREAM->TCP
		
	struct sockaddr_in dest_addr;
	char friend_ip[16] = {0};
	int gfa_result = get_friend_address(&name_address , friend_name, (char *)&friend_ip);
	printf("[gfa_result]%d\n",gfa_result);
	printf("[get_address result]%s\n",friend_ip);
	if (gfa_result) 
		return ERROR;
		
	dest_addr.sin_family = AF_INET;//AF_INET->TCP/IP Address Family
	dest_addr.sin_port = htons(SERVER_PORT);
	dest_addr.sin_addr.s_addr = inet_addr((char *)&friend_ip);
	memset(&(dest_addr.sin_zero), 0, sizeof(dest_addr.sin_zero));
	printf("new connect\n\n\n\n!!!!!\n\n\n\n!!!!\n\n\n");	
	
	int result = connect(connect_socket_fd, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr));
	printf("[connect result] %d\n",result);
	if (result == -1) //connect failed	
		return ERROR;
	
	return connect_socket_fd;
}
