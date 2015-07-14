#include "talk_thread.h"

void *talk_thread(void *arg)
{
	
	pthread_detach(pthread_self());
	pthread_t friend_thread_id = pthread_self();
	//socket_fd ==> address ==> friend_name enqueue_connector
	//get socket_fd
	socket_fd talk_socket_fd = ((struct talk_thread_arg *)arg)->connect_socket_fd;
	//int state = TALK_RUNNING;
	int connect_launcher = ((struct talk_thread_arg *)arg)->connect_launcher;
	int connect_type;
	int file_trans_fd = (int)((struct talk_thread_arg *)arg)->file_trans_fd;
	
	
	//can create a new function get_friend_address(socket_fd talk_socket_fd, char *ip)
	struct sockaddr addr;
	socklen_t addr_len = sizeof(struct sockaddr);
	getpeername(talk_socket_fd, (struct sockaddr *)&addr, &addr_len);
	
	//get address
	char ip[16] = {0};
	strcpy(ip, inet_ntoa(((struct sockaddr_in *)&addr)->sin_addr));
	
	char *friend_name = (char *)malloc_string_safe(friend_name, get_friend_name_length(&name_address, ip) * sizeof(char));
	
	get_friend_name(&name_address, ip, friend_name);
	printf("[friendname]%s\n",friend_name);
	//get friend_name
	
	printf("[enqueue_connector threadid]%d\n",friend_thread_id);
	
/*	struct friend *this;*/
/*	this  = (struct friend *)malloc(sizeof(struct friend));*/
/*	memset(this, 0, sizeof(struct friend));*/
/*	find_connector_by_threadid(&connectors, friend_thread_id, this);*/
/*	if (this == NULL) {*/
/*		printf("[ERROR THIS]\n\n--------------\n\n-------------\n\n");*/
/*	}*/
	//printf("[this state]%d\n",state);
	//socket_fd friend_socket_fd = this->friend_socket_fd;
	
	
	
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
	//	while{recv message + ETB}
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
	
	//typedef void (*connect_ptr)(void *arg);
	//connect_ptr talk_listener_list[][]{
	//	{init_message,show_message,destroy_message},
	//	{init_file,download_file,close_file}
	//}
	
	//init_data_queue()--malloc
	//new function recv_queue(Queue)
	//destroy_data_queue()--free
	//struct talk_control_arg{
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
	//while {
	//	if error {set state = ONRUN/ONDESTROY}
	//	callback(talk_listener_list[type,state],arg)
	//}
	
	//init data recv
	Queue *data_recv = init_split_data_recv();
	if (talk_socket_fd == 0) 
		goto end;
	
	
	//set connect type
	if (connect_launcher == FALSE) {//for receiver	recv connect type, set type, send ACK
		printf("[it is not the launcher]\n");
		//recv head control data_length
		if (recv_unwrap_split_data(talk_socket_fd, data_recv, NULL) == FALSE) goto end;
		char *head_control_data = init_data_recombine(data_recv);
		recombine_data(data_recv, head_control_data);
		connect_type = atoi(head_control_data);
		printf("[connect accepter] type%d \n",connect_type);
		destroy_data_recombine(head_control_data);
		printf("[recv head control data] send ACK\n");
		send(talk_socket_fd, ACK_STR, strlen(ACK_STR), 0);
	}else{//for launcher	get and set connect type, send type, recv ACK(no need to check is data equal ACK)
		printf("[it is the launcher]\n");
		connect_type = ((struct talk_thread_arg *)arg)->connect_type;
		printf("[launcher connect type]%d\n",connect_type);
		char *send_control_str;	
		if (connect_type == MESSAGE_CONNECT) 
			send_control_str = MESSAGE_CONNECT_STR;
		if (connect_type == FILE_CONNECT) 
			send_control_str = FILE_CONNECT_STR;
		//send head control data 
		send_wrap_split_data(talk_socket_fd, send_control_str, ETB);
		printf("[send head control data]connect type = %s\n",send_control_str);
		//recv head control ACK
		if (recv_equal_char(talk_socket_fd, ACK) == FALSE) goto end;
		printf("[recv_ACK]\n");
	}
	
	
	//enqueue_connector
	enqueue_connector(&connectors, friend_name, friend_thread_id, talk_socket_fd, connect_type);
	
	printf("[talk thread enqueue_connector]\n");
	print_connector(&connectors);
	
	
	//init and set arg by type
	//runs service
	//destroy
	struct connect_info *cinfo;
	if (connect_type == MESSAGE_CONNECT) {
		printf("[--connect type--] message_connect\n");
		printf("[--init message--]\n");
		cinfo = init_message(talk_socket_fd, friend_name, data_recv);
		printf("[--show message--]\n");
		show_message(cinfo);
		printf("[--destroy message--]\n");
		destroy_message(cinfo);
	}else if (connect_type == FILE_CONNECT) {
		printf("[--connect type--] file_connect\n");
		printf("[--init download--]\n");
		cinfo = init_download(talk_socket_fd, friend_name, data_recv, file_trans_fd);
		printf("[--download file--]\n");
		download_file(cinfo);
		printf("[--destroy download--]\n");
		destroy_download(cinfo);
	}else{
		goto end;
	}
	
	
	end:
	printf("[End come need to free num]%d\n",QueueLength(data_recv));
/*	while (QueueLength(&message_recv)) {//prevent malloc without free*/
/*		char *recvbuf;*/
/*		DeQueue(&message_recv, &recvbuf);*/
/*		free_safe(recvbuf);*/
/*		//printf("[---------freeend---------]\n");*/
/*	}*/
/*	destroyQueue(&message_recv);*/
	//if (connect_type == MESSAGE_CONNECT)
	free_safe(arg);
	destroy_split_data_recv(data_recv);	
	
	
	
	
	
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
/*	if(file_name != NULL)*/
/*		free_safe(file_name);*/
	pthread_exit((void *)NULL);
	//return (void *)NULL;
}


struct connect_info *init_message(socket_fd talk_socket_fd, char *friend_name, Queue *data_recv)
{
	struct connect_info *cinfo = (struct connect_info*)malloc_safe(cinfo, sizeof(struct connect_info));
	cinfo->data_recv = data_recv;
	cinfo->connect_socket_fd = talk_socket_fd;
	cinfo->friend_name = friend_name;
	return cinfo;
}


void show_message(struct connect_info *cinfo)
{
	while (!client_shutdown) {
		if (recv_unwrap_split_data(cinfo->connect_socket_fd, cinfo->data_recv, NULL) == FALSE) break;
		char *message = init_data_recombine(cinfo->data_recv);
		recombine_data(cinfo->data_recv, message);
		show(cinfo->friend_name, message, SHOW_DIRECTION_IN);
		destroy_data_recombine(message);
		usleep(500);
	}
}
void destroy_message(struct connect_info *cinfo)
{
	free_safe(cinfo);
}


struct connect_info *init_download(socket_fd talk_socket_fd, char *friend_name, Queue *data_recv, int file_trans_fd)
{
	struct connect_info *cinfo = (struct connect_info*)malloc_safe(cinfo, sizeof(struct connect_info));
	struct file_trans *task;
	
	if (file_trans_fd == ERROR) {//Accepter recv file_name md5
		if (recv_unwrap_split_data(talk_socket_fd, data_recv, NULL) == FALSE) return NULL;
		char *file_name = init_data_recombine(data_recv);
		recombine_data(data_recv, file_name);
		printf("[file_name data] send ACK\n");
		send(talk_socket_fd, ACK_STR, strlen(ACK_STR), 0);
		
		if (recv_unwrap_split_data(talk_socket_fd, data_recv, NULL) == FALSE) return NULL;
		char *md5 = init_data_recombine(data_recv);
		recombine_data(data_recv, md5);
		printf("[md5 data] send ACK\n");
		send(talk_socket_fd, ACK_STR, strlen(ACK_STR), 0);
		task = find_file_trans_task(file_trans_control, init_file_trans(file_trans_control, FALSE, file_name, NULL, md5));
		destroy_data_recombine(file_name);
		destroy_data_recombine(md5);
		
	}else{//Launcher send file_name md5
		task = find_file_trans_task(file_trans_control, file_trans_fd);

		send_wrap_split_data(talk_socket_fd, task->file_name, ETB);
		printf("[send filename data] %s\n",task->file_name);
		//recv head control ACK
		if (recv_equal_char(talk_socket_fd, ACK) == FALSE) return NULL;
		printf("[recv ACK]\n");
		
		send_wrap_split_data(talk_socket_fd, task->md5, ETB);
		printf("[send md5 data] %s\n",task->md5);
		//recv head control ACK
		if (recv_equal_char(talk_socket_fd, ACK) == FALSE) return NULL;
		printf("[recv ACK]\n");
	}

	printf("[init download]sockfd=%d friend_name=%s file_tranfd=%d\n",talk_socket_fd,friend_name,file_trans_fd);
	
	cinfo->friend_name = friend_name;
	cinfo->data_recv = data_recv;
	cinfo->connect_socket_fd = talk_socket_fd;
	
	cinfo->file_trans_fd = file_trans_fd;
	
	return cinfo;
}

void download_file(struct connect_info *cinfo)
{
	struct file_trans *task = find_file_trans_task(file_trans_control, cinfo->file_trans_fd);
	printf("[download_file\tfile_trans_fd]%d\n",cinfo->file_trans_fd);
	unsigned char *block_buff = (unsigned char *)malloc_string_safe(block_buff, BLOCK_SIZE);
	if(task->connect_launcher == TRUE){//Launcher read send
		while(1){
			read_file_trans_block(file_trans_control, cinfo->file_trans_fd, block_buff);
			if (send_file_trans_block(file_trans_control, cinfo->file_trans_fd, cinfo->connect_socket_fd, block_buff) == FALSE) goto end;
			memset(block_buff, 0, BLOCK_SIZE);
		}
	}else{//Accepter recv append
		while(1){
			memset(block_buff, 0, BLOCK_SIZE);
			if(recv_file_trans_block(file_trans_control, cinfo->file_trans_fd, cinfo->connect_socket_fd, block_buff) == FALSE) goto end;
			append_file_trans_block(file_trans_control, cinfo->file_trans_fd, block_buff, BLOCK_SIZE);
		}
		
	}
	end:
	free_safe(block_buff);
	
}

void destroy_download(struct connect_info *cinfo)
{
	destroy_file_trans(file_trans_control, cinfo->file_trans_fd);
	free_safe(cinfo);
}


Queue *init_split_data_recv()
{
	Queue *data_recv = (Queue *)malloc_safe(data_recv, sizeof(Queue));
	InitQueue(data_recv, sizeof(char *));
	return data_recv;
}




int recv_equal_char(socket_fd recv_socket_fd,char ch)
{
	char chstr[2] = {ch, 0};
	Queue *data_split = init_split_data_recv();
	int result = recv_unwrap_split_data(recv_socket_fd, data_split, NULL);
	if (result == TRUE) {
		char *data = init_data_recombine(data_split);
		recombine_data(data_split, data);
		result = strcmp(data, chstr) == TRUE?TRUE:FALSE;
		destroy_data_recombine(data);
	}
	destroy_split_data_recv(data_split);
	return result;
}

int recv_unwrap_split_data(socket_fd recv_socket_fd, Queue *data_recv, char *tail)
{
	int recv_result;
	int recv_end = 0;
		
	do {	
	//	printf("[---------malloc---------]\n");
		char *recvbuf = (char *)malloc_string_safe(recvbuf, RECV_BUFSIZE * sizeof(char));
		//memset(recvbuf, 0, (RECV_BUFSIZE + 1) * sizeof(char));
		printf("[begin recv]\n");
		recv_result = recv(recv_socket_fd, recvbuf, RECV_BUFSIZE - 1, 0);
		printf("[recv result]%d\n",recv_result);
		printf("[recv buff]%s\n",recvbuf);
		//printf("[get wrap]%c\n",get_wrap(recvbuf));
		if (!compare_wrap(recvbuf, ETB)) {
			recv_end = 1;
			un_wrap(recvbuf, tail);
		}
/*			if (strcspn(recvbuf,"\x17") == (recv_result - 1)) {*/
/*				memset(recvbuf + recv_result - 1, 0, 1);*/
/*				recv_end = 1;*/
/*			}*/
		


		EnQueue(data_recv, &recvbuf);

		if (recv_result <= 0 && !(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN))
			return FALSE;
		
		
	} while (!recv_end &&( recv_result > 0 || (recv_result < 0 && (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN))));
	return TRUE;
}

void destroy_split_data_recv(Queue *data_recv)
{
	while (QueueLength(data_recv)) {//prevent malloc without free
		char *recvbuf;
		DeQueue(data_recv, &recvbuf);
		free_safe(recvbuf);
		//printf("[---------freeend---------]\n");
	}
	DestroyQueue(data_recv);
	free_safe(data_recv);
}

char *init_data_recombine(Queue *data_recv)
{
	int queue_length_max = QueueLength(data_recv);
	char *data = (char *)malloc_string_safe(data, RECV_BUFSIZE * sizeof(char) * (queue_length_max + 1));
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

void destroy_data_recombine(char *data)
{
	free_safe(data);
}
