#include "Queue.h"

int InitQueue(LinkQueue *Q,size_t ptr_size,size_t value_size){
	Q->front = Q->rear = (QueuePtr)malloc(sizeof(QNode));
	memset(Q->rear, 0, sizeof(QNode));
	
	Q->front->pointer = (void*)malloc(value_size);
	
	
	if(!Q->front)return ERROR;
	Q->ptr_size=ptr_size;
	Q->value_size=value_size;
	Q->front->next=NULL;
	return OK;
}

int EnQueue(LinkQueue *Q,void *pointer){
	QNode *p;
	p=(QueuePtr)malloc(sizeof(QNode));//malloc由编译器分配空间的间隔会自动大于等于20!!
	memset(p, 0, sizeof(QNode));
	if(!p)return ERROR;
	p->pointer=(void*)malloc(Q->value_size);//TODO 新增代码未测试
	memset(p->pointer, 0, Q->value_size);
	if(!p->pointer)return ERROR;//TODO 新增代码未测试
	memcpy(p->pointer,pointer,Q->value_size);//p->pointer=pointer;
	p->next=NULL;
	Q->rear->next=p;
	Q->rear=p;
	return OK;
}

int DeQueue(LinkQueue *Q,void *pointer){
	QNode *p;
	if(Q->front==Q->rear){return ERROR;}
	p=Q->front->next;
	memcpy(pointer,p->pointer,Q->value_size);//*e=p->data;拷贝值
	Q->front->next=p->next;
	if(Q->rear==p)
		Q->rear=Q->front;
	free(p->pointer);
	free(p);
	return OK;
}

int DestoryQueue(LinkQueue *Q){
	while (Q->front){
		Q->rear = Q->front->next;
		free(Q->front->pointer);//TODO 新增代码未测试
		free(Q->front);
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
