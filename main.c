#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

#define QUEUE_SIZE 10 // TO DO: test your program using different queue sizes

//------------------------------------------------------------------------------------------
// Type of the circular queue elements
typedef unsigned long QueueElem;

//------------------------------------------------------------------------------------------
// Struct for representing a "circular queue"
// Space for the queue elements will be allocated dinamically by queue_init()
typedef struct
{
	QueueElem *v; // pointer to the queue buffer

	unsigned int capacity; // queue capacity
	unsigned int first; // head of the queue
	unsigned int last; // tail of the queue

	sem_t empty; // semaphores and mutex for implementing the
	sem_t full; // producer-consumer paradigm
	pthread_mutex_t mutex;
} CircularQueue;

//------------------------------------------------------------------------------------------
// Allocates space for circular queue 'q' having 'capacity' number of elements
// Initializes semaphores & mutex needed to implement the producer-consumer paradigm
// Initializes indexes of the head and tail of the queue
// TO DO BY STUDENTS: ADD ERROR TESTS TO THE CALLS & RETURN a value INDICATING (UN)SUCESS
void queue_init(CircularQueue **q, unsigned int capacity) // TO DO: change return value
{
	*q = (CircularQueue *) malloc(sizeof(CircularQueue));

	sem_init(&((*q)->empty), 0, capacity);
	sem_init(&((*q)->full), 0, 0);
	pthread_mutex_init(&((*q)->mutex), NULL);

	(*q)->v = (QueueElem *) malloc(capacity * sizeof(QueueElem));
	(*q)->capacity = capacity;
	(*q)->first = 0;
	(*q)->last = 0;
}

//------------------------------------------------------------------------------------------
// Inserts 'value' at the tail of queue 'q'
// DONE BY STUDENTS.
void queue_put(CircularQueue *q, QueueElem value)
{
	q->v[q->last] = value;
	q->last++;

}

//------------------------------------------------------------------------------------------
// Removes element at the head of queue 'q' and returns its 'value'
QueueElem queue_get(CircularQueue *q)
{	
	QueueElem value = q->v[q->first];
	q->first++;
	return value;
}

//------------------------------------------------------------------------------------------
// Frees space allocated for the queue elements and auxiliary management data
// Must be called when the queue is no more needed
void queue_destroy(CircularQueue *q)
{
	// TO DO BY STUDENTS
}

// Prints queue.
// DONE BY STUDENTS.
void queue_print(CircularQueue *q) {

	QueueElem value;
	while((value = queue_get(q)) != 0) {
		printf("Value: %lu \n", value);
	}

}

// Fills a queue with value starting from 2 to n. 
// DONE BY STUDENTS.
void queue_fill(CircularQueue *q, const QueueElem n) {
	
	QueueElem value = 1;
	while(n > value) {
		value++;
		queue_put(q, value);
	}

}
// ************************************************************************************************************************************

int main( int argc, const char* argv[] )  {

	CircularQueue *q;
	queue_init(&q, QUEUE_SIZE);

	queue_fill(q, 20);

	queue_print(q);

	return 0;
}

