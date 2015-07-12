#include "friend.h"
static pthread_rwlock_t name_addr_rwlock;
static pthread_rwlock_t connector_rwlock;
void init_friend_name_addr()
{
	//printf("<init wrlock 0>\n");
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
	fna->friend_name = (char *)malloc_safe(fna->friend_name, (strlen(friend_name) + 1) * sizeof(char));
	fna->friend_address = (char *)malloc_safe(fna->friend_address, (strlen(friend_address) + 1) * sizeof(char));
	strcpy(fna->friend_name, friend_name);
	strcpy(fna->friend_address, friend_address);
	//printf("<begin wrlock 0>%d\n",__LINE__);;
	pthread_rwlock_wrlock(&name_addr_rwlock);
	EnQueue(queue, (void *)fna);
	//printf("<end wrlock 0>\n");
	pthread_rwlock_unlock(&name_addr_rwlock);
	free_safe(fna);	
}

void dequeue_friend_name_addr(LinkQueue *queue)
{
	struct friend_name_addr *fna;
	fna = (struct friend_name_addr *)malloc_safe(fna, sizeof(struct friend_name_addr));	
	//printf("<begin wrlock 0>%d\n",__LINE__);;
	pthread_rwlock_wrlock(&name_addr_rwlock);
	//printf("<return wrlock 0>%d\n",a);;	
	DeQueue(queue, (void *)fna);
	//printf("[WILL DeQueue Return]\n");
	pthread_rwlock_unlock(&name_addr_rwlock);
	//printf("<end wrlock 0>\n");
	free_safe(fna->friend_name);
	free_safe(fna->friend_address);
	free_safe(fna);
}

void destroy_friend_name_addr(LinkQueue *queue)
{
	//printf("[begin destroy friend_name_addr]\n");
	while(QueueLength(queue) != 0){
		printf("[name_addr_queue length]%d\n",QueueLength(queue));
		dequeue_friend_name_addr(queue);
	}
	DestroyQueue(queue);
	
	pthread_rwlock_destroy(&name_addr_rwlock);
	//printf("<destroy wrlock 0>\n");
}

int get_friend_address(LinkQueue *name_address_queue, char *friend_name, char *friend_ip)
{
	//printf("<begin wrlock 0>%d\n",__LINE__);;
	pthread_rwlock_rdlock(&name_addr_rwlock);
	QNode *p = name_address_queue->front;
	while((p = p->next)){
		//printf("[get_friend_address]%s %s\n",((struct friend_name_addr *)p->pointer)->friend_name,((struct friend_name_addr *)p->pointer)->friend_address);
		if (!strcmp(friend_name, ((struct friend_name_addr *)p->pointer)->friend_name)) {
			strcpy(friend_ip, ((struct friend_name_addr *)p->pointer)->friend_address);
			pthread_rwlock_unlock(&name_addr_rwlock);
		//	printf("<end wrlock 0>\n");
			return OK;
		}
	}
	pthread_rwlock_unlock(&name_addr_rwlock);
	//printf("<end wrlock 0>\n");
	return ERROR;		
}

int get_friend_name(LinkQueue *name_address_queue, char *friend_ip, char *friend_name)
{
	//printf("<begin wrlock 0>%d\n",__LINE__);;
	pthread_rwlock_rdlock(&name_addr_rwlock);
	QNode *p = name_address_queue->front;
	while((p = p->next)){
		//printf("[get_friend_name]%s %s\n",((struct friend_name_addr *)p->pointer)->friend_name,((struct friend_name_addr *)p->pointer)->friend_address);
		if (!strcmp(friend_ip, ((struct friend_name_addr *)p->pointer)->friend_address)) {
			strcpy(friend_name, ((struct friend_name_addr *)p->pointer)->friend_name);
			pthread_rwlock_unlock(&name_addr_rwlock);
		//	printf("<end wrlock 0>\n");
			return OK;
		}
	}
	pthread_rwlock_unlock(&name_addr_rwlock);
	//printf("<end wrlock 0>\n");
	return ERROR;		
}

int get_friend_name_length(LinkQueue *name_address_queue, char *friend_ip)
{
	//printf("<begin wrlock 0>%d\n",__LINE__);;
	pthread_rwlock_rdlock(&name_addr_rwlock);
	QNode *p = name_address_queue->front;
	while((p = p->next)){
		if (!strcmp(friend_ip, ((struct friend_name_addr *)p->pointer)->friend_address)) {
			pthread_rwlock_unlock(&name_addr_rwlock);
	//		printf("<end wrlock 0>\n");
			return strlen(((struct friend_name_addr *)p->pointer)->friend_name);
		}
	}
	//printf("<end wrlock 0>\n");
	pthread_rwlock_unlock(&name_addr_rwlock);
	return 0;
}
/*void print_name_addr(LinkQueue *queue)*/
/*{*/
/*	QNode *p = queue->front;*/
/*	*/
/*	//printf("[PTR]%p\n",p);*/
/*	*/
/*	while((p = p->next)){*/
/*		*/
/*		printf("[look!]%s %s\n",((struct friend_name_addr *)p->pointer)->friend_name,((struct friend_name_addr *)p->pointer)->friend_address);*/
/*	}*/
/*}*/

int init_connector(LinkQueue *friend_queue)
{
	//printf("<init wrlock 1>\n");
	pthread_rwlock_init(&connector_rwlock, NULL);
	int result = InitQueue(friend_queue, sizeof(struct friend));
	printf("[init_connector connectors front]%p\n", friend_queue->front);
	printf("[init_connector rear]%p\n", friend_queue->rear);
	printf("[init_connector front->next]%p\n",friend_queue->front->next);
	printf("[init_connector rear->next]%p\n",friend_queue->rear->next);
	return result;
}

/*int add_connector(LinkQueue *friend_queue, struct friend *friendinfo)*/
/*{*/
/*	return EnQueue(friend_queue, friendinfo);*/
/*}*/

int enqueue_connector(LinkQueue *friend_queue, char *friend_name, pthread_t friend_thread_id, socket_fd friend_socket_fd, int connect_type)
{
	struct friend *connector = (struct friend *)malloc_safe(connector, sizeof(struct friend));
	connector->friend_name = (char *)malloc_safe(connector->friend_name, (strlen(friend_name) + 1) * sizeof(char));
	strcpy(connector->friend_name, friend_name);
	connector->friend_thread_id = friend_thread_id;
	connector->friend_socket_fd = friend_socket_fd;
	connector->connect_type = connect_type;
	//printf("<begin wrlock 1>\n");
	pthread_rwlock_wrlock(&connector_rwlock);
	int result = EnQueue(friend_queue, (void *)connector);
	pthread_rwlock_unlock(&connector_rwlock);
	//printf("<end wrlock 1>\n");
	free_safe(connector);
	printf("[enqueue_connector connectors front]%p\n", friend_queue->front);
	printf("[enqueue_connector connectors rear]%p\n", friend_queue->rear);
	printf("[enqueue_connector connectors front->next]%p\n",friend_queue->front->next);
	printf("[enqueue_connector connectors rear->next]%p\n",friend_queue->rear->next);
	return result;	
}

int dequeue_connector_length(LinkQueue *friend_queue)
{
	//printf("<begin wrlock 1>\n");
	pthread_rwlock_rdlock(&connector_rwlock);
	int result = strlen(((struct friend *)friend_queue->front->next->pointer)->friend_name);
	pthread_rwlock_unlock(&connector_rwlock);
	//printf("<end wrlock 1>\n");
	return result;
}


int dequeue_connector(LinkQueue *friend_queue, struct friend *friend_val)
{
	struct friend *connector = (struct friend *)malloc_safe(connector, sizeof(struct friend));		
	//printf("<begin wrlock 1>\n");
	pthread_rwlock_wrlock(&connector_rwlock);
	int result = DeQueue(friend_queue, (void *)connector);
	pthread_rwlock_unlock(&connector_rwlock);
	//printf("<end wrlock 1>\n");
	if(friend_val != NULL){
		//memcpy(friend_val, connector, sizeof(struct friend));
		friend_val->friend_socket_fd = connector->friend_socket_fd;
		friend_val->friend_thread_id = connector->friend_thread_id;
		//friend_val->state = connector->state;
		strcpy(friend_val->friend_name, connector->friend_name);
	}
	free_safe(connector->friend_name);
	free_safe(connector);
	{
	//printf("[in dequeue connector]\n");
	//print_connector(friend_queue);
	}
	return result;
}

/*int get_connector_socket_fd_by_name(LinkQueue *friend_queue, char *friend_name){*/
/*	QNode *p = friend_queue->front;*/
/*	while((p = p->next)){*/
/*		if (!strcmp(friend_name, ((struct friend *)p->pointer)->friend_name)) {*/
/*			return ((struct friend *)p->pointer)->friend_socket_fd;*/
/*		}*/
/*	}*/
/*	return 0;*/
/*}*/

int find_connector_by_name(LinkQueue *friend_queue, char *friend_name, struct friend *friend_val, int connect_type)
{
	//printf("<begin wrlock 1>\n");
	pthread_rwlock_rdlock(&connector_rwlock);
		
	QNode *p = friend_queue->front;
	while((p = p->next)){
		if (!strcmp(friend_name, ((struct friend *)p->pointer)->friend_name) && (((struct friend *)p->pointer)->connect_type == connect_type)) {
			if (friend_val != NULL) 
				memcpy(friend_val, p->pointer, sizeof(struct friend));
			pthread_rwlock_unlock(&connector_rwlock);
	//		printf("<end wrlock 1>\n");
			return OK;
		}
	}
	pthread_rwlock_unlock(&connector_rwlock);
	//printf("<end wrlock 1>\n");
	return ERROR;
}

int find_connector_by_threadid(LinkQueue *friend_queue, pthread_t friend_thread_id, struct friend *friend_val)
{
	//printf("<begin wrlock 1>\n");
	pthread_rwlock_rdlock(&connector_rwlock);
	QNode *p = friend_queue->front;
	while((p = p->next)){
		if (!memcmp(&friend_thread_id, &((struct friend *)p->pointer)->friend_thread_id, sizeof(pthread_t))) {
			if (friend_val != NULL)
				memcpy(friend_val, p->pointer, sizeof(struct friend));
			pthread_rwlock_unlock(&connector_rwlock);
	//		printf("<end wrlock 1>\n");
			return OK;
		}
	}
	pthread_rwlock_unlock(&connector_rwlock);
	//printf("<end wrlock 1>\n");
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
	//print_connector(friend_queue);
	//printf("<begin wrlock 1>\n");
	pthread_rwlock_wrlock(&connector_rwlock);
	QNode *p = friend_queue->front;
	QNode *before = p;
	while((p = p->next)){
		printf("[\telement]%s\n",((struct friend *)p->pointer)->friend_name);
		printf("[\telement fdcmp]%d %d\n",talk_socket_fd, ((struct friend *)p->pointer)->friend_socket_fd);
		//if (!strcmp(friend_name, ((struct friend *)p->pointer)->friend_name)) {
		if (talk_socket_fd == ((struct friend *)p->pointer)->friend_socket_fd) {
			before->next = p->next;
			if (friend_queue->rear == p)
				friend_queue->rear = friend_queue->front;
			printf("[element for remove]%s\n",((struct friend *)p->pointer)->friend_name);
			
			free_safe(((struct friend *)p->pointer)->friend_name);
			free_safe(p->pointer);
			free_safe(p);
			
	//		printf("<end wrlock 1>\n");
			
			//if (friend_queue->front->next == NULL) {
			
			//print_connector(friend_queue);//TODO remove
			
			printf("[remove_connector connectors front]%p\n", friend_queue->front);
			printf("[remove_connector connectors rear]%p\n", friend_queue->rear);
			printf("[remove_connector connectors front->next]%p\n",friend_queue->front->next);
			printf("[remove_connector connectors rear->next]%p\n",friend_queue->rear->next);
			pthread_rwlock_unlock(&connector_rwlock);
			return OK;
		}
		before = p;
	}
	pthread_rwlock_unlock(&connector_rwlock);
	//printf("<end wrlock 1>\n");
	//print_connector(friend_queue);//TODO remove
	return ERROR;
}

void close_connector(socket_fd talk_socket_fd)
{
/*	struct friend *this;*/
/*	this  = (struct friend *)malloc_safe(this, sizeof(struct friend));*/
/*	*/

	
	shutdown(talk_socket_fd, SHUT_RDWR);
	close(talk_socket_fd);	
	
/*	free_safe(this);*/
}



void close_all_connector(LinkQueue *friend_queue)
{
/*	while(QueueLength(connectors)){*/
/*		struct friend *this = (struct friend *)malloc(sizeof(struct friend));*/
/*		memset(this, 0, sizeof(struct friend));*/
/*		int friend_name_length = dequeue_connector_length(connectors) + 1;*/
/*		this->friend_name = (char *)malloc(friend_name_length * sizeof(char));*/
/*		memset(this->friend_name, 0, friend_name_length * sizeof(char));*/
/*		printf("[begin dequeue one connector]!!\n");*/
/*		dequeue_connector(connectors, this);*/
/*		close_talk_thread(this->friend_socket_fd, this->friend_name);*/
/*		free_safe(this->friend_name);*/
/*		free_safe(this);*/
/*	}*/
	pthread_rwlock_rdlock(&connector_rwlock);
	QNode *p = friend_queue->front;
/*	if(connectors->front == NULL)*/
/*		printf("[FUCKKKKKKKK]、你\n");*/
	//printf("[PTR]%p\n",p);
	//printf("[print connectors]\n");
	while((p = p->next)){
		close_connector(((struct friend *)p->pointer)->friend_socket_fd);
	}
	pthread_rwlock_unlock(&connector_rwlock);
}

void destroy_connector(LinkQueue *friend_queue)
{
	pthread_rwlock_wrlock(&connector_rwlock);
	printf("[begin destroy connector]\n");
	while(QueueLength(friend_queue) != 0){
		int a = dequeue_connector(friend_queue, NULL);
		printf("[dequeue result]%d\n",a);
	}
	DestroyQueue(friend_queue);
	pthread_rwlock_unlock(&connector_rwlock);
	pthread_rwlock_destroy(&connector_rwlock);
	//printf("<destroy wrlock 1>\n");
}

void print_connector(LinkQueue *queue)
{
	pthread_rwlock_rdlock(&connector_rwlock);
	QNode *p = queue->front;
	
	//printf("[PTR front]%p %p\n",p,p->next);
	//printf("[print connectors]\n");
	while((p = p->next)){
		//printf("[PTR]%p %p\n",p,p->next);
		printf("\t[ALL ELEMents]name = %s     sockfd = %d    connect_type = %d\n",((struct friend *)p->pointer)->friend_name,((struct friend *)p->pointer)->friend_socket_fd,((struct friend *)p->pointer)->connect_type);
	}
	pthread_rwlock_unlock(&connector_rwlock);
	//printf("[print connectors end]\n");
}
