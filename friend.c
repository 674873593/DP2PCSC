#include "friend.h"
static pthread_rwlock_t name_addr_rwlock;
static pthread_rwlock_t connector_rwlock;
void init_friend_name_addr()
{
	pthread_rwlock_init(&name_addr_rwlock, NULL);
	FILE *friend_address_file;
	friend_address_file = fopen(FRIEND_ADDRESS_FILE,"r");
	InitQueue(&name_address, sizeof(struct friend_name_addr));
	char line_data[LINE_LENGTH] = {0};
	while(fgets(line_data,LINE_LENGTH,friend_address_file) != NULL){
		char *friend_name;
		char *friend_address;
		friend_name = strtok(line_data, "@");
		friend_address = strtok(NULL, "\n");
		if(friend_name == NULL || friend_address == NULL)
			continue;
		enqueue_friend_name_addr(&name_address, friend_name, friend_address);
		memset(line_data, 0, sizeof(line_data));		
	}
	fclose(friend_address_file);
}

void enqueue_friend_name_addr(LinkQueue *queue, char *friend_name,char *friend_address)
{
	struct friend_name_addr *fna;
	fna = (struct friend_name_addr *)malloc_safe(fna, sizeof(struct friend_name_addr));
	fna->friend_name = (char *)malloc_string_safe(fna->friend_name, strlen(friend_name) * sizeof(char));
	fna->friend_address = (char *)malloc_string_safe(fna->friend_address, strlen(friend_address) * sizeof(char));
	strcpy(fna->friend_name, friend_name);
	strcpy(fna->friend_address, friend_address);
	pthread_rwlock_wrlock(&name_addr_rwlock);
	EnQueue(queue, (void *)fna);
	pthread_rwlock_unlock(&name_addr_rwlock);
	free_safe(fna);	
}

void dequeue_friend_name_addr(LinkQueue *queue)
{
	struct friend_name_addr *fna;
	fna = (struct friend_name_addr *)malloc_safe(fna, sizeof(struct friend_name_addr));	
	pthread_rwlock_wrlock(&name_addr_rwlock);
	DeQueue(queue, (void *)fna);
	pthread_rwlock_unlock(&name_addr_rwlock);
	free_safe(fna->friend_name);
	free_safe(fna->friend_address);
	free_safe(fna);
}

void destroy_friend_name_addr(LinkQueue *queue)
{
	while(QueueLength(queue) != 0){
		dequeue_friend_name_addr(queue);
	}
	DestroyQueue(queue);
	pthread_rwlock_destroy(&name_addr_rwlock);
}

int get_friend_address(LinkQueue *name_address_queue, char *friend_name, char *friend_ip)
{
	pthread_rwlock_rdlock(&name_addr_rwlock);
	QNode *p = name_address_queue->front;
	while((p = p->next)){
		if (!strcmp(friend_name, ((struct friend_name_addr *)p->pointer)->friend_name)) {
			strcpy(friend_ip, ((struct friend_name_addr *)p->pointer)->friend_address);
			pthread_rwlock_unlock(&name_addr_rwlock);
			return OK;
		}
	}
	pthread_rwlock_unlock(&name_addr_rwlock);
	return ERROR;		
}

int get_friend_name(LinkQueue *name_address_queue, char *friend_ip, char *friend_name)
{
	pthread_rwlock_rdlock(&name_addr_rwlock);
	QNode *p = name_address_queue->front;
	while((p = p->next)){
		if (!strcmp(friend_ip, ((struct friend_name_addr *)p->pointer)->friend_address)) {
			strcpy(friend_name, ((struct friend_name_addr *)p->pointer)->friend_name);
			pthread_rwlock_unlock(&name_addr_rwlock);
			return OK;
		}
	}
	pthread_rwlock_unlock(&name_addr_rwlock);
	return ERROR;		
}

int get_friend_name_length(LinkQueue *name_address_queue, char *friend_ip)
{
	pthread_rwlock_rdlock(&name_addr_rwlock);
	QNode *p = name_address_queue->front;
	while((p = p->next)){
		if (!strcmp(friend_ip, ((struct friend_name_addr *)p->pointer)->friend_address)) {
			pthread_rwlock_unlock(&name_addr_rwlock);
			return strlen(((struct friend_name_addr *)p->pointer)->friend_name);
		}
	}
	pthread_rwlock_unlock(&name_addr_rwlock);
	return 0;
}

int init_connector(LinkQueue *friend_queue)
{
	pthread_rwlock_init(&connector_rwlock, NULL);
	int result = InitQueue(friend_queue, sizeof(struct friend));
	return result;
}

int enqueue_connector(LinkQueue *friend_queue, char *friend_name, pthread_t friend_thread_id, socket_fd friend_socket_fd, int connect_type)
{
	struct friend *connector = (struct friend *)malloc_safe(connector, sizeof(struct friend));
	connector->friend_name = (char *)malloc_string_safe(connector->friend_name, strlen(friend_name) * sizeof(char));
	strcpy(connector->friend_name, friend_name);
	connector->friend_thread_id = friend_thread_id;
	connector->friend_socket_fd = friend_socket_fd;
	connector->connect_type = connect_type;
	pthread_rwlock_wrlock(&connector_rwlock);
	int result = EnQueue(friend_queue, (void *)connector);
	pthread_rwlock_unlock(&connector_rwlock);
	free_safe(connector);
	return result;	
}

int dequeue_connector_length(LinkQueue *friend_queue)
{
	pthread_rwlock_rdlock(&connector_rwlock);
	int result = strlen(((struct friend *)friend_queue->front->next->pointer)->friend_name);
	pthread_rwlock_unlock(&connector_rwlock);
	return result;
}


int dequeue_connector(LinkQueue *friend_queue, struct friend *friend_val)
{
	struct friend *connector = (struct friend *)malloc_safe(connector, sizeof(struct friend));		
	pthread_rwlock_wrlock(&connector_rwlock);
	int result = DeQueue(friend_queue, (void *)connector);
	pthread_rwlock_unlock(&connector_rwlock);
	if(friend_val != NULL){
		friend_val->friend_socket_fd = connector->friend_socket_fd;
		friend_val->friend_thread_id = connector->friend_thread_id;
		strcpy(friend_val->friend_name, connector->friend_name);
	}
	free_safe(connector->friend_name);
	free_safe(connector);
	return result;
}

int find_connector_by_name(LinkQueue *friend_queue, char *friend_name, struct friend *friend_val, int connect_type)
{
	pthread_rwlock_rdlock(&connector_rwlock);
		
	QNode *p = friend_queue->front;
	while((p = p->next)){
		if (!strcmp(friend_name, ((struct friend *)p->pointer)->friend_name) && (((struct friend *)p->pointer)->connect_type == connect_type)) {
			if (friend_val != NULL) 
				memcpy(friend_val, p->pointer, sizeof(struct friend));
			pthread_rwlock_unlock(&connector_rwlock);
			return OK;
		}
	}
	pthread_rwlock_unlock(&connector_rwlock);
	return ERROR;
}

int find_connector_by_threadid(LinkQueue *friend_queue, pthread_t friend_thread_id, struct friend *friend_val)
{
	pthread_rwlock_rdlock(&connector_rwlock);
	QNode *p = friend_queue->front;
	while((p = p->next)){
		if (!memcmp(&friend_thread_id, &((struct friend *)p->pointer)->friend_thread_id, sizeof(pthread_t))) {
			if (friend_val != NULL)
				memcpy(friend_val, p->pointer, sizeof(struct friend));
			pthread_rwlock_unlock(&connector_rwlock);
			return OK;
		}
	}
	pthread_rwlock_unlock(&connector_rwlock);
	return ERROR;
}

int connector_length(LinkQueue *friend_queue)
{
	pthread_rwlock_rdlock(&connector_rwlock);
	int length = QueueLength(friend_queue);
	pthread_rwlock_unlock(&connector_rwlock);
	return length;
}

int remove_connector(LinkQueue *friend_queue, socket_fd talk_socket_fd)
{
	pthread_rwlock_wrlock(&connector_rwlock);
	QNode *p = friend_queue->front;
	QNode *before = p;
	while((p = p->next)){
		if (talk_socket_fd == ((struct friend *)p->pointer)->friend_socket_fd) {
			before->next = p->next;
			if (friend_queue->rear == p)
				friend_queue->rear = friend_queue->front;
			free_safe(((struct friend *)p->pointer)->friend_name);
			free_safe(p->pointer);
			free_safe(p);
			pthread_rwlock_unlock(&connector_rwlock);
			return OK;
		}
		before = p;
	}
	pthread_rwlock_unlock(&connector_rwlock);
	return ERROR;
}

void close_connector(socket_fd talk_socket_fd)
{
	shutdown(talk_socket_fd, SHUT_RDWR);
	close(talk_socket_fd);	
}

void close_all_connector(LinkQueue *friend_queue)
{
	pthread_rwlock_rdlock(&connector_rwlock);
	QNode *p = friend_queue->front;
	while((p = p->next)){
		close_connector(((struct friend *)p->pointer)->friend_socket_fd);
	}
	pthread_rwlock_unlock(&connector_rwlock);
}

void destroy_connector(LinkQueue *friend_queue)
{
	pthread_rwlock_wrlock(&connector_rwlock);
	while(QueueLength(friend_queue) != 0){
		dequeue_connector(friend_queue, NULL);
	}
	DestroyQueue(friend_queue);
	pthread_rwlock_unlock(&connector_rwlock);
	pthread_rwlock_destroy(&connector_rwlock);
}

