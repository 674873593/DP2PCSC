#include "friend.h"
static pthread_rwlock_t name_addr_rwlock;
static pthread_rwlock_t connector_rwlock;
void init_friend_name_addr()
{
	printf("<init wrlock 0>\n");
	pthread_rwlock_init(&name_addr_rwlock, NULL);
	FILE *friend_address_file;
	friend_address_file = fopen(FRIEND_ADDRESS_FILE,"r");
	InitQueue(&name_address, sizeof(struct friend_name_addr *), sizeof(struct friend_name_addr));
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
	fna = (struct friend_name_addr *)malloc(sizeof(struct friend_name_addr));
	memset(fna, 0, sizeof(struct friend_name_addr));
	fna->friend_name = (char *)malloc((strlen(friend_name) + 1) * sizeof(char));
	fna->friend_address = (char *)malloc((strlen(friend_address) + 1) * sizeof(char));
	memset(fna->friend_name, 0, strlen(friend_name) * sizeof(char));
	memset(fna->friend_address, 0, strlen(friend_address) * sizeof(char));
	strcpy(fna->friend_name, friend_name);
	strcpy(fna->friend_address, friend_address);
	printf("<begin wrlock 0>%d\n",__LINE__);;
	pthread_rwlock_wrlock(&name_addr_rwlock);
	EnQueue(queue, (void *)fna);
	printf("<end wrlock 0>\n");
	pthread_rwlock_unlock(&name_addr_rwlock);
	free(fna);	
}

void dequeue_friend_name_addr(LinkQueue *queue)
{
	struct friend_name_addr *fna;
	fna = (struct friend_name_addr *)malloc(sizeof(struct friend_name_addr));	
	memset(fna, 0, sizeof(struct friend_name_addr));
	printf("<begin wrlock 0>%d\n",__LINE__);;
	pthread_rwlock_wrlock(&name_addr_rwlock);
	//printf("<return wrlock 0>%d\n",a);;	
	DeQueue(queue, (void *)fna);
	printf("[WILL DeQueue Return]\n");
	pthread_rwlock_unlock(&name_addr_rwlock);
	printf("<end wrlock 0>\n");
	free(fna->friend_name);
	free(fna->friend_address);
	free(fna);
}

void destory_friend_name_addr(LinkQueue *queue)
{
	printf("[begin destory friend_name_addr]\n");
	while(QueueLength(queue) != 0){
		printf("[name_addr_queue length]%d\n",QueueLength(queue));
		dequeue_friend_name_addr(queue);
	}
	DestoryQueue(queue);
	
	pthread_rwlock_destroy(&name_addr_rwlock);
	printf("<destory wrlock 0>\n");
}

int get_friend_address(LinkQueue *name_address_queue, char *friend_name, char *friend_ip)
{
	printf("<begin wrlock 0>%d\n",__LINE__);;
	pthread_rwlock_rdlock(&name_addr_rwlock);
	QNode *p = name_address_queue->front;
	while((p = p->next)){
		printf("[get_friend_address]%s %s\n",((struct friend_name_addr *)p->pointer)->friend_name,((struct friend_name_addr *)p->pointer)->friend_address);
		if (!strcmp(friend_name, ((struct friend_name_addr *)p->pointer)->friend_name)) {
			strcpy(friend_ip, ((struct friend_name_addr *)p->pointer)->friend_address);
			pthread_rwlock_unlock(&name_addr_rwlock);
			printf("<end wrlock 0>\n");
			return OK;
		}
	}
	pthread_rwlock_unlock(&name_addr_rwlock);
	printf("<end wrlock 0>\n");
	return ERROR;		
}

int get_friend_name(LinkQueue *name_address_queue, char *friend_ip, char *friend_name)
{
	printf("<begin wrlock 0>%d\n",__LINE__);;
	pthread_rwlock_rdlock(&name_addr_rwlock);
	QNode *p = name_address_queue->front;
	while((p = p->next)){
		printf("[get_friend_name]%s %s\n",((struct friend_name_addr *)p->pointer)->friend_name,((struct friend_name_addr *)p->pointer)->friend_address);
		if (!strcmp(friend_ip, ((struct friend_name_addr *)p->pointer)->friend_address)) {
			strcpy(friend_name, ((struct friend_name_addr *)p->pointer)->friend_name);
			pthread_rwlock_unlock(&name_addr_rwlock);
			printf("<end wrlock 0>\n");
			return OK;
		}
	}
	pthread_rwlock_unlock(&name_addr_rwlock);
	printf("<end wrlock 0>\n");
	return ERROR;		
}

int get_friend_name_length(LinkQueue *name_address_queue, char *friend_ip)
{
	printf("<begin wrlock 0>%d\n",__LINE__);;
	pthread_rwlock_rdlock(&name_addr_rwlock);
	QNode *p = name_address_queue->front;
	while((p = p->next)){
		if (!strcmp(friend_ip, ((struct friend_name_addr *)p->pointer)->friend_address)) {
			pthread_rwlock_unlock(&name_addr_rwlock);
			printf("<end wrlock 0>\n");
			return strlen(((struct friend_name_addr *)p->pointer)->friend_name);
		}
	}
	printf("<end wrlock 0>\n");
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

int init_connector()
{
	printf("<init wrlock 1>\n");
	pthread_rwlock_init(&connector_rwlock, NULL);
	return InitQueue(&connectors, sizeof(struct friend *), sizeof(struct friend));
}

/*int add_connector(LinkQueue *friend_queue, struct friend *friendinfo)*/
/*{*/
/*	return EnQueue(friend_queue, friendinfo);*/
/*}*/

int enqueue_connector(LinkQueue *friend_queue, char *friend_name, pthread_t friend_thread_id, socket_fd friend_socket_fd)
{
	struct friend *connector;
	connector = (struct friend *)malloc(sizeof(struct friend));
	memset(connector, 0, sizeof(struct friend));
	connector->friend_name = (char *)malloc((strlen(friend_name) + 1) * sizeof(char));
	memset(connector->friend_name, 0, (strlen(friend_name) + 1) * sizeof(char));
	strcpy(connector->friend_name, friend_name);
	connector->friend_thread_id = friend_thread_id;
	connector->friend_socket_fd = friend_socket_fd;
	printf("<begin wrlock 1>\n");
	pthread_rwlock_wrlock(&connector_rwlock);
	int result = EnQueue(friend_queue, (void *)connector);
	pthread_rwlock_unlock(&connector_rwlock);
	printf("<end wrlock 1>\n");
	free(connector);
	return result;	
}

int dequeue_connector_length(LinkQueue *friend_queue)
{
	printf("<begin wrlock 1>\n");
	pthread_rwlock_rdlock(&connector_rwlock);
	int result = strlen(((struct friend *)friend_queue->front->next->pointer)->friend_name);
	pthread_rwlock_unlock(&connector_rwlock);
	printf("<end wrlock 1>\n");
	return result;
}


int dequeue_connector(LinkQueue *friend_queue, struct friend *friend_val)
{
	struct friend *connector;
	connector = (struct friend *)malloc(sizeof(struct friend));		
	memset(connector, 0, sizeof(struct friend));
	printf("<begin wrlock 1>\n");
	pthread_rwlock_wrlock(&connector_rwlock);
	int result = DeQueue(friend_queue, (void *)connector);
	pthread_rwlock_unlock(&connector_rwlock);
	printf("<end wrlock 1>\n");
	if(friend_val != NULL){
		//memcpy(friend_val, connector, sizeof(struct friend));
		friend_val->friend_socket_fd = connector->friend_socket_fd;
		friend_val->friend_thread_id = connector->friend_thread_id;
		friend_val->state = connector->state;
		strcpy(friend_val->friend_name, connector->friend_name);
	}
	free(connector->friend_name);
	free(connector);
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

int find_connector_by_name(LinkQueue *friend_queue, char *friend_name, struct friend *friend_val)
{
	printf("<begin wrlock 1>\n");
	pthread_rwlock_rdlock(&connector_rwlock);
		
	QNode *p = friend_queue->front;
	while((p = p->next)){
		if (!strcmp(friend_name, ((struct friend *)p->pointer)->friend_name)) {
			if (friend_val != NULL) 
				memcpy(friend_val, p->pointer, sizeof(struct friend));
			pthread_rwlock_unlock(&connector_rwlock);
			printf("<end wrlock 1>\n");
			return OK;
		}
	}
	pthread_rwlock_unlock(&connector_rwlock);
	printf("<end wrlock 1>\n");
	return ERROR;
}

int find_connector_by_threadid(LinkQueue *friend_queue, pthread_t friend_thread_id, struct friend *friend_val)
{
	printf("<begin wrlock 1>\n");
	pthread_rwlock_rdlock(&connector_rwlock);
	QNode *p = friend_queue->front;
	printf("[address]%p\n",p);
	while((p = p->next)){
	printf("[address]%p\n",p);
		if (!memcmp(&friend_thread_id, &((struct friend *)p->pointer)->friend_thread_id, sizeof(pthread_t))) {
			if (friend_val != NULL)
				memcpy(friend_val, p->pointer, sizeof(struct friend));
			pthread_rwlock_unlock(&connector_rwlock);
			printf("<end wrlock 1>\n");
			return OK;
		}
	}
	pthread_rwlock_unlock(&connector_rwlock);
	printf("<end wrlock 1>\n");
	return ERROR;
}


int remove_connector(LinkQueue *friend_queue, char *friend_name)
{
	print_connector(friend_queue);
	printf("<begin wrlock 1>\n");
	pthread_rwlock_wrlock(&connector_rwlock);
	QNode *p = friend_queue->front;
	QNode *before = p;
	while((p = p->next)){
		printf("[\telement]%s\n",((struct friend *)p->pointer)->friend_name);
		printf("[\telement cmp]%d\n",strcmp(friend_name, ((struct friend *)p->pointer)->friend_name));
		if (!strcmp(friend_name, ((struct friend *)p->pointer)->friend_name)) {
			before->next = p->next;
			printf("[element for remove]%s\n",((struct friend *)p->pointer)->friend_name);
			free(((struct friend *)p->pointer)->friend_name);
			free(p->pointer);
			free(p);
			pthread_rwlock_unlock(&connector_rwlock);
			printf("<end wrlock 1>\n");
			return OK;
		}
		before = p;
	}
	pthread_rwlock_unlock(&connector_rwlock);
	printf("<end wrlock 1>\n");
	print_connector(friend_queue);
	return ERROR;
}

void destory_connector(LinkQueue *friend_queue)
{
	printf("[begin destory connector]\n");
	while(QueueLength(friend_queue) != 0){
		dequeue_connector(friend_queue, NULL);
	}
	DestoryQueue(friend_queue);
	pthread_rwlock_destroy(&connector_rwlock);
	printf("<destory wrlock 1>\n");
}

void print_connector(LinkQueue *queue)
{
	QNode *p = queue->front;
	
	//printf("[PTR]%p\n",p);
	//printf("[print connectors]\n");
	while((p = p->next)){
		
		printf("\t[elements]%s     sockfd = %d\n",((struct friend *)p->pointer)->friend_name,((struct friend *)p->pointer)->friend_socket_fd);
	}
	//printf("[print connectors end]\n");
}
