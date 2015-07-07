#include "friend.h"
void init_friend_name_addr()
{
	FILE *friend_address_file;
	friend_address_file = fopen(FRIEND_ADDRESS_FILE,"r");
	InitQueue(&name_address, sizeof(struct friend_name_addr *), sizeof(struct friend_name_addr));
	char line_data[LINE_LENGTH] = {0};
	while(fgets(line_data,LINE_LENGTH,friend_address_file) != NULL){
		char *friend_name;
		char *friend_address;
		friend_name = strtok(line_data, "@");
		friend_address = strtok(NULL, "\n");
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
	EnQueue(queue, (void *)fna);
	free(fna);	
}

void dequeue_friend_name_addr(LinkQueue *queue)
{
	struct friend_name_addr *fna;
	fna = (struct friend_name_addr *)malloc(sizeof(struct friend_name_addr));		
	DeQueue(queue, (void *)fna);
	free(fna->friend_name);
	free(fna->friend_address);
	free(fna);
}

void destory_friend_name_addr(LinkQueue *queue)
{
	while(QueueLength(queue) != 0){
		dequeue_friend_name_addr(queue);
	}
}

int get_friend_address(LinkQueue *name_address_queue, char *friend_name, char *friend_ip)
{
	QNode *p = name_address_queue->front;
	while((p = p->next)){
		printf("[get_friend_address]%s %s\n",((struct friend_name_addr *)p->pointer)->friend_name,((struct friend_name_addr *)p->pointer)->friend_address);
		if (!strcmp(friend_name, ((struct friend_name_addr *)p->pointer)->friend_name)) {
			strcpy(friend_ip, ((struct friend_name_addr *)p->pointer)->friend_address);
			return OK;
		}
	}
	return ERROR;		
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
	return InitQueue(&connectors, sizeof(struct friend *), sizeof(struct friend));
}

int add_connector(LinkQueue *friend_queue, struct friend *friendinfo)
{
	return EnQueue(friend_queue, friendinfo);
}


int find_connector_by_name(LinkQueue *friend_queue, char *friend_name, struct friend *friend_find)
{
	QNode *p = friend_queue->front;
	while((p = p->next)){
		if (!strcmp(friend_name, ((struct friend *)p->pointer)->friend_name)) {
			return OK;
		}
	}
	return ERROR;
}

int find_connector_by_threadid(LinkQueue *friend_queue, pthread_t friend_thread_id, struct friend *friend_val)
{
	QNode *p = friend_queue->front;
	while((p = p->next)){
		if (!memcmp(&friend_thread_id, &((struct friend *)p->pointer)->friend_thread_id, sizeof(pthread_t))) {
			memcpy(friend_val, p, sizeof(struct friend));
			return OK;
		}
	}
	friend_val = NULL;
	return ERROR;
}


int remove_connector(LinkQueue *friend_queue, char *friend_name)
{
	QNode *p = friend_queue->front;
	QNode *before = p;
	while((p = p->next)){
		if (strcmp(friend_name, ((struct friend *)p->pointer)->friend_name)) {
			before->next = p->next;
			free(p->pointer);
			free(p);
			return OK;
		}
		before = p;
	}
	return ERROR;
}

int destory_connector()
{
	return DestoryQueue(&connectors);
}
