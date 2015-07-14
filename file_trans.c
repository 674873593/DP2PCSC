#include "file_trans.h"
static pthread_rwlock_t file_trans_control_rwlock;

Queue *init_file_trans_control()
{
	pthread_rwlock_init(&file_trans_control_rwlock, NULL);
	file_trans_control = (Queue *)malloc_safe(file_trans_control, sizeof(Queue));
	InitQueue(file_trans_control, sizeof(struct file_trans));
	return file_trans_control;
}

int file_trans_control_length(Queue *file_trans_control)
{
	pthread_rwlock_rdlock(&file_trans_control_rwlock);
	int length = QueueLength(file_trans_control);
	pthread_rwlock_unlock(&file_trans_control_rwlock);
	return length;
}

void destroy_file_trans_control(Queue *file_trans_control)
{
	pthread_rwlock_rdlock(&file_trans_control_rwlock);
	QNode *p = file_trans_control->front;
	while((p = p->next)){
		struct file_trans *task = (struct file_trans *)p->pointer;
		
		destroy_file_trans(file_trans_control, task->file_trans_fd);
	}
	pthread_rwlock_unlock(&file_trans_control_rwlock);
	DestroyQueue(file_trans_control);
	free_safe(file_trans_control);
}


		
/*int init_file_trans(Queue *file_trans_control, int connect_launcher, char *file_name, char *file_location, char *md5)*/
int init_file_trans(Queue *file_trans_control, int connect_launcher, char *file_name, char *file_location, long total_size)
{
	struct file_trans *task = (struct file_trans *)malloc_safe(task, sizeof(struct file_trans));
	
	pthread_rwlock_rdlock(&file_trans_control_rwlock);
	//init file_trans_control
	if (file_trans_control->front->next != NULL)
		task->file_trans_fd = ((struct file_trans *)file_trans_control->front->next->pointer)->file_trans_fd + 1;
	else 
		task->file_trans_fd = 0;
	pthread_rwlock_unlock(&file_trans_control_rwlock);
	//init connect_launcher --launcher(TRUE) -- trans direction out; recivier(FALSE) -- trans direction in
	task->connect_launcher = connect_launcher;
	
	//init file_name
	task->file_name = (char *)malloc_string_safe(task->file_name, strlen(file_name) * sizeof(char));
	strncpy(task->file_name, file_name, strlen(file_name));
	
	//init file_location md5 file_ptr total_size
	if (connect_launcher == TRUE) {//launcher
		task->file_location = (char *)malloc_string_safe(task->file_location, strlen(file_location) * sizeof(char));
		strncpy(task->file_location, file_location, strlen(file_location));
/*		task->md5 = init_md5(file_location);*/
/*		printf("[md5]%s\n",task->md5);*/
		task->file_ptr = fopen(file_location, "rb");
		printf("[openfile]%s\n",file_location);
		fseek(task->file_ptr, 0, SEEK_END);
		task->total_size = ftell(task->file_ptr);
		printf("[total_size]%ld\n",task->total_size);
		fseek(task->file_ptr, 0, SEEK_SET);
	}else{//receiver
		task->file_location = (char *)malloc_string_safe(task->file_location, (strlen(DOWNLOAD_PATH) + strlen(file_name)) * sizeof(char));
		
		strncpy(task->file_location, DOWNLOAD_PATH, strlen(DOWNLOAD_PATH));
		strncpy(task->file_location + strlen(DOWNLOAD_PATH), file_name, strlen(file_name));
		printf("[store location]%s\n",task->file_location);
/*		task->md5 = (char *)malloc_string_safe(task->md5, strlen(md5) * sizeof(char));*/
/*		strncpy(task->md5, md5, strlen(md5));*/
		task->file_ptr = fopen(task->file_location, "wb");
		printf("[openfile]%s\n",task->file_location);
		task->total_size = total_size;
		printf("[total_size]%ld\n",task->total_size);
	}

	//init total_size 	
	task->fin_size = 0;
	pthread_rwlock_wrlock(&file_trans_control_rwlock);
	EnQueue(file_trans_control, task);
	pthread_rwlock_unlock(&file_trans_control_rwlock);
	return task->file_trans_fd;
}

struct file_trans *find_file_trans_task(Queue *file_trans_control, int file_trans_fd)
{
	struct file_trans *task = NULL;
	pthread_rwlock_wrlock(&file_trans_control_rwlock);
	QNode *p = file_trans_control->front;
	while((p = p->next)){
		task = (struct file_trans *)p->pointer;
		if (file_trans_fd == task->file_trans_fd) {
			pthread_rwlock_unlock(&file_trans_control_rwlock);
			return task;
		}
	}
	pthread_rwlock_unlock(&file_trans_control_rwlock);
	return task;
}



void destroy_file_trans(Queue *file_trans_control, int file_trans_fd)
{	
	pthread_rwlock_wrlock(&file_trans_control_rwlock);
	QNode *p = file_trans_control->front;
	QNode *before = p;
	while((p = p->next)){
		struct file_trans *task = (struct file_trans *)p->pointer;
		if (file_trans_fd == task->file_trans_fd) {
			before->next = p->next;
			if (file_trans_control->rear == p)
				file_trans_control->rear = file_trans_control->front;
				
			free_safe(task->file_name);
			free_safe(task->file_location);
/*			destroy_md5(task->md5);*/
			fclose(task->file_ptr);
			free_safe(p->pointer);
			free_safe(p);
			pthread_rwlock_unlock(&file_trans_control_rwlock);
			return;
		}
		before = p;
	}
	pthread_rwlock_unlock(&file_trans_control_rwlock);
	return;
}


int read_file_trans_block(Queue *file_trans_control, int file_trans_fd, unsigned char *file_block)
{
	int block_read_length = 0;
	struct file_trans *task = find_file_trans_task(file_trans_control, file_trans_fd);
	pthread_rwlock_wrlock(&file_trans_control_rwlock);
	if (task != NULL) {
		fseek(task->file_ptr, task->fin_size, SEEK_SET);
		block_read_length = fread(file_block, sizeof(unsigned char), BLOCK_SIZE, task->file_ptr);
	}
/*	for (int i = 0; i < BLOCK_SIZE; i += 1) {*/
/*		printf("%02x ",file_block[i]);*/
/*	}//end for*/
/*	printf("\n");*/
	pthread_rwlock_unlock(&file_trans_control_rwlock);
	return block_read_length;
}

void append_file_trans_block(Queue *file_trans_control, int file_trans_fd, unsigned char *block, int write_size)
{
	struct file_trans *task = find_file_trans_task(file_trans_control, file_trans_fd);
	pthread_rwlock_wrlock(&file_trans_control_rwlock);
	if (task != NULL) {
		fseek(task->file_ptr, task->fin_size, SEEK_SET);
		int append_length = fwrite(block, sizeof(unsigned char), write_size, task->file_ptr);
		task->fin_size += append_length;
		pthread_rwlock_unlock(&file_trans_control_rwlock);
	}
	pthread_rwlock_unlock(&file_trans_control_rwlock);
}

int send_file_trans_block(Queue *file_trans_control, int file_trans_fd, socket_fd connect_socket_fd, unsigned char *file_block)
{
	int send_result;
	int block_send_length = 0;
	struct file_trans *task = find_file_trans_task(file_trans_control, file_trans_fd);
	pthread_rwlock_wrlock(&file_trans_control_rwlock);
/*	for (int i = 0; i < BLOCK_SIZE; i += 1) {*/
/*		printf("%02x",file_block[i]);*/
/*	}//end for*/
/*	printf("\n");*/
	while (block_send_length < BLOCK_SIZE && client_shutdown != TRUE) {
		send_result = send(connect_socket_fd, file_block + block_send_length, BLOCK_SIZE - block_send_length,0);
		printf("[send bloack send_result]%d\n",send_result);
		if (send_result == -1 || send_result == 0) {
			break;
		}
		block_send_length += send_result;
		task->fin_size += send_result;
	}//end while
	pthread_rwlock_unlock(&file_trans_control_rwlock);
	return send_result;
}

int recv_file_trans_block(Queue *file_trans_control, int file_trans_fd, socket_fd connect_socket_fd, unsigned char *file_block)
{
	int recv_result;
	int block_recv_length = 0;
	struct file_trans *task = find_file_trans_task(file_trans_control, file_trans_fd);
	pthread_rwlock_wrlock(&file_trans_control_rwlock);
	while (block_recv_length < BLOCK_SIZE && client_shutdown != TRUE) {
		
		recv_result = recv(connect_socket_fd, file_block + block_recv_length, BLOCK_SIZE - block_recv_length, 0);
		printf("[recv bloack recv_result]%d\n",recv_result);
		if (recv_result <= 0 && !(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)) {
			return recv_result;
		}
		block_recv_length += recv_result;
		//task->fin_size += block_recv_length;
	}//end while
	pthread_rwlock_unlock(&file_trans_control_rwlock);
/*	for (int i = 0; i < BLOCK_SIZE; i += 1) {*/
/*		printf("%02x ",file_block[i]);*/
/*	}//end for*/
/*	printf("\n");*/
	return recv_result;
}




/*char *init_md5(char *file_location)*/
/*{*/
/*	int command_length = (strlen(MD5_COMMAND_HEAD) + strlen(file_location) + strlen(MD5_COMMAND_TAIL)) * sizeof(char);*/
/*	char *command = malloc_string_safe(command, command_length);*/
/*	char *md5 = malloc_string_safe(md5, MD5_LENGTH);*/
/*	FILE *command_pipe_file = popen(command, "r");*/
/*	fgets(md5, MD5_LENGTH, command_pipe_file);*/
/*	pclose(command_pipe_file);*/
/*	return md5;*/
/*}*/

/*void destroy_md5(char *md5)*/
/*{*/
/*	free_safe(md5);*/
/*}*/
