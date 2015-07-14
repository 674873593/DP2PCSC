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
		fseek(task->file_ptr, 0, SEEK_END);
		task->total_size = ftell(task->file_ptr);
		fseek(task->file_ptr, 0, SEEK_SET);
	}else{//receiver
		task->file_location = (char *)malloc_string_safe(task->file_location, (strlen(DOWNLOAD_PATH) + strlen(file_name)) * sizeof(char));
		
		strncpy(task->file_location, DOWNLOAD_PATH, strlen(DOWNLOAD_PATH));
		strncpy(task->file_location + strlen(DOWNLOAD_PATH), file_name, strlen(file_name));
		task->file_ptr = fopen(task->file_location, "wb");
		task->total_size = total_size;
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

