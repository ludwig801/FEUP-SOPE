#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <limits.h>
#include <math.h>

#define QUEUE_SIZE 10 // TO DO: test your program using different queue sizes
#define MAX_THREAD SHRT_MAX // DONE BY STUDENTS.

pthread_t tetas[MAX_THREAD];
unsigned int threads = 0;

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

	sem_t production; // semaphores and mutex for implementing the
	sem_t consumption; // producer-consumer paradigm
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

	sem_init(&((*q)->production), 0, capacity);
	sem_init(&((*q)->consumption), 0, 0);
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

// Gives the head.
QueueElem queue_top(CircularQueue *q) {
	return q->v[q->first];
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
		sem_post(&(q->consumption));
	}

}
// ************************************************************************************************************************************


CircularQueue *primes;


void *comPutaPrima(void *arg) {

	int sam;
	QueueElem prime = 0;
	CircularQueue * q = (CircularQueue *)arg;

	//printf("Thread: %d\n", threads);
	//sem_getvalue(&(q->consumption), &sam);
	//printf("valor do sam: %d\n", sam);

	//sem_getvalue(&(q->consumption), &sam);
	//printf("valor do sam: %d\n", sam);	
	sem_wait(&(q->consumption));
	//sem_getvalue(&(q->consumption), &sam);
	//printf("valor do sam: %d\n", sam);

	prime = queue_get(q);

	//printf("Prime no: %lu \n", prime);

	
	if(!prime) {

		while(sam < primes->capacity) {
			sem_post(&(primes->consumption));
		}

		//printf("Returning from thread: %d\n", threads);
		return NULL;
	}

	CircularQueue *q2;
	queue_init(&q2, q->capacity);
	
	threads++;
	pthread_create(&tetas[threads], NULL, comPutaPrima, q2);

	do {
		//printf("tou fodido\n");
		//printf("lendo: %lu \n", queue_top(q));
		sem_wait(&(q->consumption));
		//sem_getvalue(&(q->consumption), &sam);
		//printf("valor do sam: %d\n", sam);
		
		if(queue_top(q) % prime == 0) {
			// Remove that bitch.
			//printf("remove a bitch %lu \n", queue_top(q));
			queue_get(q);
		} else {
			//printf("Toma la: %lu \n", queue_top(q));
			pthread_mutex_lock(&(q2->mutex));
			queue_put(q2, queue_get(q));
			sem_post(&(q2->consumption));

			//sem_getvalue(&(q2->consumption), &sam);
			//printf("valor do sam 2: %d\n", sam);

			pthread_mutex_unlock(&(q2->mutex));
		}
		
		
	} while(queue_top(q));

	queue_put(q2, 0);
	sem_post(&(q2->consumption));



	pthread_mutex_lock(&(primes->mutex));
	queue_put(primes, prime);
	//queue_print(primes);
	sem_post(&(primes->consumption));
	pthread_mutex_unlock(&(primes->mutex));

	return NULL;	
}

int main( int argc, const char* argv[] )  {

	if (argc != 2) {
		printf("Usage: %s <max>\n", argv[0]);
		return 1;
	}

	double N = atof(argv[1]);

	CircularQueue *q;

	queue_init(&q, N);

	// ========================================
	//       .|. Container for the shit .|.
	// ========================================
	int max_primes = 1.2 * (N/log(N));
	queue_init(&primes, max_primes);
	// ========================================

	queue_fill(q, N);

	pthread_create(&tetas[threads], NULL, comPutaPrima, q); 
	//printf("Max size: %d\n", max_primes);

	int sem_value = 0;

	do {
		sem_getvalue(&(primes->consumption), &sem_value);
		//printf("valor do sam: %d\n", sem_value);
	} while(sem_value < max_primes);

	printf("<<< PRIME QUEUE >>>\n");
	queue_print(primes);

	return 0;
}

