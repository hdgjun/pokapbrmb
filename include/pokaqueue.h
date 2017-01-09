#ifndef __POKA_QUEUE_H__
#define __POKA_QUEUE_H__

//#include <semaphore.h>
#include <pthread.h>
#define QUE_SIZE 100

typedef struct data_type
{
	char fileName[200];
	char filePath[300];
	unsigned long  filesize;
	int  fileType;
	int  threadid;
}DataType;

typedef struct cir_queue_t
{
  void *data[QUE_SIZE];
  int front;
  int rear;
  int count;
  //sem_t queue_sem;
  pthread_mutex_t lock;//зда§Ыј
  pthread_cond_t notempty;
  pthread_cond_t notfull;
}cir_queue_t;

cir_queue_t gQue;

void init_cir_queue(cir_queue_t* q);
int is_empty_cir_queue(cir_queue_t* q);
int is_full_cir_queue(cir_queue_t* q);
int push_cir_queue(cir_queue_t* q, void *x);
void *pop_cir_queue(cir_queue_t* q);
//DataType *top_cir_queue(cir_queue_t* q);
void destroy_cir_queue(cir_queue_t* q);
void free_data_cir_queue(void *x);
void print_queue(cir_queue_t* q);


#endif
