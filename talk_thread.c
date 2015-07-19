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
	//get friend_name
	
	//init data recv
	Queue *data_recv = init_split_data_recv();
	if (talk_socket_fd == 0) 
		goto end;
	
	//set connect type
	if (connect_launcher == FALSE) {//for receiver	recv connect type, set type, send ACK
		//recv head control data_length
		if (recv_unwrap_split_data(talk_socket_fd, data_recv, NULL) == FALSE) goto end;
		char *head_control_data = init_data_recombine(data_recv);
		recombine_data(data_recv, head_control_data);
		connect_type = atoi(head_control_data);
		destroy_data_recombine(head_control_data);
		send(talk_socket_fd, ACK_STR, strlen(ACK_STR), 0);
	}else{//for launcher	get and set connect type, send type, recv ACK(no need to check is data equal ACK)
		connect_type = ((struct talk_thread_arg *)arg)->connect_type;
		char *send_control_str;	
		if (connect_type == MESSAGE_CONNECT) 
			send_control_str = MESSAGE_CONNECT_STR;
		if (connect_type == FILE_CONNECT) 
			send_control_str = FILE_CONNECT_STR;
		//send head control data 
		send_wrap_split_data(talk_socket_fd, send_control_str, ETB);
		//recv head control ACK
		if (recv_equal_char(talk_socket_fd, ACK) == FALSE) goto end;
		
	}
	
	//enqueue_connector
	enqueue_connector(&connectors, friend_name, friend_thread_id, talk_socket_fd, connect_type);
	
	//init and set arg by type
	//runs service
	//destroy
	struct connect_info *cinfo;
	if (connect_type == MESSAGE_CONNECT) {
		cinfo = init_message(talk_socket_fd, friend_name, data_recv);
		show_message(cinfo);
		destroy_message(cinfo);
	}else if (connect_type == FILE_CONNECT) {
		cinfo = init_download(talk_socket_fd, friend_name, data_recv, file_trans_fd);
		download_file(cinfo);
		destroy_download(cinfo);
	}else{
		goto end;
	}
	
	
	end:
	free_safe(arg);
	destroy_split_data_recv(data_recv);	
	if (!find_connector_by_threadid(&connectors, friend_thread_id, NULL)) {
		remove_connector(&connectors, talk_socket_fd);
	}
	close_connector(talk_socket_fd);
	free_safe(friend_name);
	pthread_exit((void *)NULL);
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
	
	if (file_trans_fd == ERROR) {//Accepter recv file_name init
		if (recv_unwrap_split_data(talk_socket_fd, data_recv, NULL) == FALSE) return NULL;
		char *file_name = init_data_recombine(data_recv);
		recombine_data(data_recv, file_name);
		send(talk_socket_fd, ACK_STR, strlen(ACK_STR), 0);
		
		if (recv_unwrap_split_data(talk_socket_fd, data_recv, NULL) == FALSE) return NULL;
		char *total_size_str = init_data_recombine(data_recv);
		recombine_data(data_recv, total_size_str);
		
		send(talk_socket_fd, ACK_STR, strlen(ACK_STR), 0);
		long total_size = atol(total_size_str);
		file_trans_fd = init_file_trans(file_trans_control, FALSE, file_name, NULL, total_size);
		task = find_file_trans_task(file_trans_control, file_trans_fd);
		char *size_info = (char *)malloc_string_safe(size_info, strlen(SIZE_INFO_HEAD) + strlen(total_size_str) + strlen(SIZE_INFO_TAIL));
		sprintf(size_info, "%s%s%s", SIZE_INFO_HEAD, total_size_str, SIZE_INFO_TAIL);
		show(task->file_name, size_info, SHOW_DIRECTION_SYSTEM_INFO);
		destroy_data_recombine(file_name);
		destroy_data_recombine(total_size_str);
		free(size_info);
/*		destroy_data_recombine(md5);*/
		
	}else{//Launcher send file_name
		task = find_file_trans_task(file_trans_control, file_trans_fd);

		send_wrap_split_data(talk_socket_fd, task->file_name, ETB);
		//recv head control ACK
		if (recv_equal_char(talk_socket_fd, ACK) == FALSE) return NULL;
		
		//send total_size_str
		char *send_total_size_str = long_to_string(task->total_size);
		
		char *size_info = (char *)malloc_string_safe(size_info, strlen(SIZE_INFO_HEAD) + strlen(send_total_size_str) + strlen(SIZE_INFO_TAIL));
		sprintf(size_info, "%s%s%s", SIZE_INFO_HEAD, send_total_size_str, SIZE_INFO_TAIL);
		show(task->file_name, size_info, SHOW_DIRECTION_SYSTEM_INFO);
		free(size_info);
		
		send_wrap_split_data(talk_socket_fd, send_total_size_str, ETB);
		//recv head control ACK
		if (recv_equal_char(talk_socket_fd, ACK) == FALSE) return NULL;
		
		free_safe(send_total_size_str);
	}

	cinfo->friend_name = friend_name;
	cinfo->data_recv = data_recv;
	cinfo->connect_socket_fd = talk_socket_fd;
	cinfo->file_trans_fd = file_trans_fd;
	return cinfo;
}

void download_file(struct connect_info *cinfo)
{
	struct file_trans *task = find_file_trans_task(file_trans_control, cinfo->file_trans_fd);
	int result;
	unsigned char ch[1];
	if(task->connect_launcher == TRUE){//Launcher read send
		show(task->file_name, "send begin", SHOW_DIRECTION_SYSTEM_INFO);
		while (task->fin_size < task->total_size && !client_shutdown){
			fread(ch, sizeof(unsigned char), 1, task->file_ptr);
			result = send(cinfo->connect_socket_fd, ch, 1, 0);
			if (result == -1) break;
			task->fin_size += 1;
		}
		if (task->fin_size == task->total_size) {
			show(task->file_name, "send finished", SHOW_DIRECTION_SYSTEM_INFO);
		}
			
	}else{//Accepter recv append
		show(task->file_name, "download begin", SHOW_DIRECTION_SYSTEM_INFO);
		while (task->fin_size < task->total_size && !client_shutdown){
			result = recv(cinfo->connect_socket_fd, ch, 1, 0);
			if (result < 0 && !(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)) break;
			task->fin_size += 1;
			fwrite(ch, sizeof(unsigned char), 1, task->file_ptr);
		}
		if (task->fin_size == task->total_size) {
			show(task->file_name, "download finished", SHOW_DIRECTION_SYSTEM_INFO);
		}
		
	}
	
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
		char *recvbuf = (char *)malloc_string_safe(recvbuf, RECV_BUFSIZE * sizeof(char));
		recv_result = recv(recv_socket_fd, recvbuf, RECV_BUFSIZE - 1, 0);
		if (!compare_wrap(recvbuf, ETB)) {
			recv_end = 1;
			un_wrap(recvbuf, tail);
		}

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
		if (data != NULL){
			memcpy(data + data_length, recvbuf, strlen(recvbuf));
			data_length += strlen(recvbuf);
		}
		free_safe(recvbuf);
		queue_length = QueueLength(data_recv);
	} //recombine all data to message
}

void destroy_data_recombine(char *data)
{
	free_safe(data);
}
