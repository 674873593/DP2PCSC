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
	
	while(!client_shutdown){
		input();
	}
	shutdown(listen_socket_fd, SHUT_RDWR);
	close(listen_socket_fd);
	destroy_friend_name_addr(&name_address);
	close_all_connector(&connectors);//talk_thread can't be closed by themself because recv is blocking
	while (connector_length(&connectors)){//wait for thread free
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
	pthread_t listen_thread_id;
	pthread_create(&listen_thread_id, NULL, listen_thread, 0);//begin listen thread

}



int input(){
	printf("> ");
	memset(inputbuf, 0, INPUT_BUFSIZE);
	setbuf(stdin, NULL);
	fgets(inputbuf, INPUT_BUFSIZE, stdin);
	setbuf(stdin, NULL);
	char *friend_name = strtok(inputbuf, ":");
	char *message = strtok(NULL, "\n");
	if (friend_name != NULL && message != NULL) {
		send_message(friend_name, message);//EOF 0x04
		return TRUE;
	}
		
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
	
	if (friend_name != NULL && trans_location != NULL) {
		send_file(friend_name, trans_location);
		return TRUE;
	}
	
	printf("%s", "file mode usage\t<$name><:><$location>\n");
	return FALSE;
}

void send_file(char *friend_name, char *file_location){
	//is friend exist?
	//is file exist?
	//trans file thread and connect success ;;;; must Init File Server Socket in main
	printf("[send file] %s %s\n",friend_name,file_location);
	
	
	
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

	}else{
		friend_socket_fd = connect_TCP_by_name(friend_name);
		if (friend_socket_fd <= 2) 
			goto end;
		//register file_trans to file_trans_control
		file_trans_fd = init_file_trans(file_trans_control, TRUE, file_name, file_location, 0);
		
		
		//create talk thread file_mode
		pthread_t talk_thread_id;
		struct talk_thread_arg *tt_arg = (struct talk_thread_arg *)malloc_safe(tt_arg, sizeof(struct talk_thread_arg));
		tt_arg->connect_socket_fd = friend_socket_fd;
		tt_arg->connect_launcher = TRUE;
		tt_arg->connect_type = FILE_CONNECT;
		tt_arg->file_trans_fd = file_trans_fd;
		pthread_create(&talk_thread_id, NULL, talk_thread, (void *)tt_arg);	

		fclose(file);
	}
	
	end:
		return;
}

void send_message(char *friend_name, char *message){
	struct friend *this = (struct friend *)malloc_safe(this, sizeof(struct friend));
	
	int result = find_connector_by_name(&connectors, friend_name, this, MESSAGE_CONNECT);//is connectted?
	if (result) {
			
		socket_fd friend_socket_fd;
		memset(&friend_socket_fd, 0, sizeof(socket_fd));
		
		
		//connect
		friend_socket_fd = connect_TCP_by_name(friend_name);
		if (friend_socket_fd <= 2) 
			goto end;
		
		//create talk thread message mode
		pthread_t talk_thread_id;
		struct talk_thread_arg *tt_arg = malloc_safe(tt_arg, sizeof(struct talk_thread_arg));
		tt_arg->connect_socket_fd = friend_socket_fd;
		tt_arg->connect_launcher = TRUE;
		tt_arg->connect_type = MESSAGE_CONNECT;
		tt_arg->file_trans_fd = -1;
		pthread_create(&talk_thread_id, NULL, talk_thread, (void *)tt_arg);
		
		//wait fot connect established
		while(find_connector_by_name(&connectors, friend_name,  NULL, MESSAGE_CONNECT)){
			usleep(50);
		}
		this->friend_socket_fd = friend_socket_fd;
	}
	
	//send message	
	send_wrap_split_data(this->friend_socket_fd, message, ETB);

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
	//init sock addr
	struct sockaddr_in dest_addr;
	char friend_ip[16] = {0};
	int gfa_result = get_friend_address(&name_address , friend_name, (char *)&friend_ip);
	if (gfa_result) 
		return ERROR;
	dest_addr.sin_family = AF_INET;//AF_INET->TCP/IP Address Family
	dest_addr.sin_port = htons(SERVER_PORT);
	dest_addr.sin_addr.s_addr = inet_addr((char *)&friend_ip);
	memset(&(dest_addr.sin_zero), 0, sizeof(dest_addr.sin_zero));
	
	int result = connect(connect_socket_fd, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr));
	if (result == -1) //connect failed	
		return ERROR;
	
	return connect_socket_fd;
}
