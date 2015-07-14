#include "Queue.h"

int InitQueue(LinkQueue *Q, size_t value_size){
	Q->front = Q->rear = (QueuePtr)malloc_safe(Q->front, sizeof(QNode));
	Q->front->pointer = (void*)malloc_safe(Q->front->pointer, value_size);
	
	if(!Q->front)return ERROR;
	Q->value_size=value_size;
	Q->front->next=NULL;
	return OK;
}

int EnQueue(LinkQueue *Q, void *pointer){
	QNode *p = (QueuePtr)malloc_safe(p, sizeof(QNode));//malloc由编译器分配空间的间隔会自动大于等于20!!
	if(!p)return ERROR;
	p->pointer=(void*)malloc_safe(p->pointer, Q->value_size);
	if(!p->pointer)return ERROR;
	memcpy(p->pointer,pointer,Q->value_size);//p->pointer=pointer;
	p->next=NULL;
	Q->rear->next=p;
	Q->rear=p;
	return OK;
}

int DeQueue(LinkQueue *Q, void *pointer){
	QNode *p;
	if(Q->front==Q->rear){return ERROR;}
	p=Q->front->next;
	memcpy(pointer,p->pointer,Q->value_size);//pointer=p->pointer;
	Q->front->next=p->next;
	if(Q->rear==p)
		Q->rear=Q->front;
	free_safe(p->pointer);
	free_safe(p);
	return OK;
}

int DestroyQueue(LinkQueue *Q){
	while (Q->front){
		Q->rear = Q->front->next;
		free_safe(Q->front->pointer);
		free_safe(Q->front);
		Q->front=Q->rear;
	}//end while
	return OK;
}

int QueueLength(LinkQueue *Q){
	QNode *p=Q->front;
	int length=0;
	while((p = p->next)){
		length++;
	}
	return length;
}
