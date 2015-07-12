#ifndef __Queue_H__
	#define __Queue_H__
	#include <stdio.h>
	#include <malloc.h>
	#include <memory.h>
	
	#include "ulib.h"
	
	#ifndef ERROR
	#define ERROR -1
	#endif
	
	#ifndef OK
	#define OK 0
	#endif
	
	typedef struct QNode{  /*队列的链节点*/
		struct QNode *next;
		void* pointer;//指向
	}QNode,*QueuePtr;
	
	typedef struct{
		QueuePtr front;//队头指针
		QueuePtr rear;//队尾指针
		//size_t ptr_size;//队列节点指向的数据块的指针大小
		size_t value_size;//队列节点指向的数据块的大小
	}LinkQueue,Queue;

	int InitQueue(LinkQueue *Q, size_t value_size);
	int EnQueue(LinkQueue *Q, void* pointer);
	int DeQueue(LinkQueue *Q, void* pointer);
	int DestroyQueue(LinkQueue *Q);
	int QueueLength(LinkQueue *Q);
#endif /* __Queue_H__ */ 
