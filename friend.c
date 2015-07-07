#include "friend.h"

int init_connector()
{
	
	InitQueue(&connectors, sizeof(struct friend *), sizeof(struct friend));
}

int add_connector(LinkQueue *friend_queue, struct friend *friendinfo)
{
	return EnQueue(friend_queue, friendinfo);
}


int find_connector_by_name(LinkQueue *friend_queue, char *friend_name, struct friend *friend_find)
{
	QNode *p = friend_queue->front;
	int length = 0;
	while(p = p->next){
		length++;
		if (!strcmp(friend_name, ((struct friend *)p)->friend_name)) {
			return OK;
		}
	}
	return ERROR;
}

int find_connector_by_threadid(LinkQueue *friend_queue, pthread_t friend_thread_id, struct friend *friend_val)
{
	QNode *p = friend_queue->front;
	int length = 0;
	while(p = p->next){
		length++;
		if (!memcmp(&friend_thread_id, &((struct friend *)p)->friend_thread_id, sizeof(pthread_t))) {
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
	int length = 0;
	while(p = p->next){
		length++;
		if (strcmp(friend_name, ((struct friend *)p)->friend_name)) {
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
	DestoryQueue(&connectors);
}
