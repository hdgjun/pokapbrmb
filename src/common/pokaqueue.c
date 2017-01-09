#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "common.h"
#include "pokaqueue.h"
#include "switch.h"


/*
 * \Func
 *
 */
void init_cir_queue(cir_queue_t *q)
{
  int res;

  pthread_mutex_init(&q->lock, NULL);
  pthread_cond_init(&q->notempty, NULL);
  pthread_cond_init(&q->notfull, NULL);
  /* Create semaphore
  res = sem_init(&q->queue_sem, 0, 1);
  if (res != 0)
  {
    perror("Semaphore init failed.\n");
    exit(EXIT_FAILURE);
  } */
  memset(q->data, 0, QUE_SIZE*sizeof(void *));
  q->front = q->rear = 0;
  q->count = 0;
}

/*
 * \Func
 *
 */
int is_empty_cir_queue(cir_queue_t *q)
{
  return q->front == q->rear;
}

/*
 * \Func
 *
 */
int is_full_cir_queue(cir_queue_t *q)
{
  return q->rear == (QUE_SIZE - 1 + q->front)%QUE_SIZE;
}

/*
 * \Func
 *
 */
int push_cir_queue(cir_queue_t *q, void *x)
{
  pthread_mutex_lock(&q->lock);

  while(is_full_cir_queue(q))
  {
	  printf("queue is full......\n");
	  pthread_cond_wait(&q->notfull, &q->lock);
  }

  q->data[q->rear] = x;
  q->rear = (q->rear+1) % QUE_SIZE;
  q->count++;
  pthread_cond_signal(&q->notempty);
  pthread_mutex_unlock(&q->lock);

  return SUCESS;
}

/*
 * \Func
 *
 */
void *pop_cir_queue(cir_queue_t *q)
{
	DataType *temp;
	pthread_mutex_lock(&q->lock);
	while(is_empty_cir_queue(q))
	{
		printf("queue is empty......\n");
		pthread_cond_wait(&q->notempty, &q->lock);
	}

	temp = q->data[q->front];
	q->data[q->front] = 0;
	q->front = (q->front+1) % QUE_SIZE;
	q->count--;
	pthread_cond_signal(&q->notfull);
	pthread_mutex_unlock(&q->lock);
	return temp;
}


void free_data_cir_queue(void *x)
{
	if(x!=NULL){
		free(x);
	}
}

void destroy_cir_queue(cir_queue_t *q)
{
  pthread_mutex_lock(&q->lock);
  int i = q->front;
  while(i!=q->rear){
	  free_data_cir_queue(q->data[i]);
	  i = (i+1)%QUE_SIZE;
  }
  pthread_mutex_unlock(&q->lock);
  pthread_mutex_destroy(&q->lock);
  pthread_cond_destroy(&q->notempty);
  pthread_cond_destroy(&q->notfull);
  return;
}

void print_queue(cir_queue_t* q)
{
	/*
  int index;
  if (is_empty_cir_queue(q))
  {
    printf("queue is empty.\n");
    return;
  }

  sem_wait(&q->queue_sem);
  printf("QUEUE: ");
  for (index = 0; index < QUE_SIZE; index++)
  {
  	if(q->data[index]!=NULL)
  	{
  		printf("filename:%s/%s  filetype:%d",q->data[index]->filePath, q->data[index]->fileName,q->data[index]->fileType);
  	}
  }
  printf("\n");

  sem_post(&q->queue_sem);
*/
  return;
}


