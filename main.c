#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <limits.h>
#include <math.h>

//#define QUEUE_SIZE 10 // TO DO: test your program using different queue sizes
#define MAX_THREAD 1000 // DONE BY STUDENTS.

pthread_t tids[MAX_THREAD];
unsigned int threads = 0;
unsigned int max_primes = 0;
unsigned int N = 0;

unsigned short done = 0;

int QUEUE_SIZE = 10;


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

	sem_t production; // semaphores and mutex for implementing the     FULL
	sem_t consumption; // producer-consumer paradigm		   EMPTY
	pthread_mutex_t mutex;
	int number;

} CircularQueue;

//------------------------------------------------------------------------------------------
// Allocates space for circular queue 'q' having 'capacity' number of elements
// Initializes semaphores & mutex needed to implement the producer-consumer paradigm
// Initializes indexes of the head and tail of the queue
// TO DO BY STUDENTS: ADD ERROR TESTS TO THE CALLS & RETURN a value INDICATING (UN)SUCESS
void queue_init(CircularQueue **q, unsigned int capacity) // TO DO: change return value
{
	*q = (CircularQueue *) malloc(sizeof(CircularQueue));

	sem_init(&((*q)->consumption), 0, capacity);
	sem_init(&((*q)->production), 0, 0);
	pthread_mutex_init(&((*q)->mutex), NULL);

	(*q)->v = (QueueElem *) malloc(capacity * sizeof(QueueElem));
	(*q)->capacity = capacity;
	(*q)->first = 0;
	(*q)->last = 0;

	(*q)->number = 0;
}

//------------------------------------------------------------------------------------------
// Inserts 'value' at the tail of queue 'q'
// DONE BY STUDENTS.
void queue_put(CircularQueue *q, QueueElem value)
{
	sem_wait(&(q->consumption));
	q->v[q->last] = value;
	(q->last)++;
	q->last = q->last % q->capacity;
	sem_post(&(q->production));
}

//------------------------------------------------------------------------------------------
// Removes element at the head of queue 'q' and returns its 'value'
QueueElem queue_get(CircularQueue *q)
{	
	
	sem_wait(&(q->production));
	QueueElem value = q->v[q->first];
	(q->first)++;
	q->first = q->first % q->capacity;	
	sem_post(&(q->consumption));
	return value;
}

//------------------------------------------------------------------------------------------
// Removes element at the head of queue 'q' and returns its 'value'
QueueElem queue_get_special(CircularQueue *q)
{	
	QueueElem value = q->v[q->first];
	(q->first)++;
	q->first = q->first % q->capacity;	
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
	//free(q->v);
	//free(q);
}

// Prints queue.
// DONE BY STUDENTS.
void queue_print(CircularQueue *q) {

	QueueElem value;
	int count = 0;

	sem_getvalue(&(q->production), &count);
	printf("Size of queue: %d\n", count);

	for(; count >= 0; count--) {
		value = queue_get(q);
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

	queue_put(q, 0);
}
// ************************************************************************************************************************************


QueueElem * primes;
unsigned int p_count = 0;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

void initPrimes(int size) {
	primes = (QueueElem *) malloc(size * sizeof(QueueElem));
}

void printPrimes() {
	unsigned int i = 0;
	printf("====================\n");
	for(; i < p_count; i++) {
		printf("%lu\n", primes[i]);
	}
	printf("====================\n");
}

void insertPrime(QueueElem value) {

	pthread_mutex_lock(&mut);
	primes[p_count] = value;
	//printf("Inserted: %lu, in: %d\n", value, p_count);
	p_count++;
	pthread_mutex_unlock(&mut);
	
}

void *computePrimes(void *arg) {

	CircularQueue * queue = (CircularQueue *)arg;
	QueueElem prime = 0;
	QueueElem value = 0;

	prime = queue_get(queue);

	//
	// Stoping condition: prime > sqrt(N)
	//
	if(prime > (int) sqrt(N)) {

		while((value = queue_get(queue)) != 0) {

			//
			// Insert the prime in the prime array
			//	
			insertPrime(value);
		}

	}
	else {
		// Creates shared queue
		CircularQueue *new_queue;
		queue_init(&new_queue, QUEUE_SIZE);
		new_queue->number = queue->number + 1;
	
		// Creates new thread
		threads++;
		pthread_create(&tids[threads], NULL, computePrimes, new_queue);

		do {
			value = queue_get(queue);
			// if not multiple of the last computed prime (possible new prime)
			if((value % prime) != 0)  {
				queue_put(new_queue, value);
			}
		} while(value != 0);

		queue_put(new_queue, 0);	/* terminator value */	
	}

	//
	// Insert the prime in the prime array
	//
	insertPrime(prime);

	queue_destroy(queue);

	pthread_exit(NULL);	
}

int cmpfunc (const void * a, const void * b)
{
   return ( *(QueueElem*)a - *(QueueElem*)b );
}



int main( int argc, const char* argv[] )  {

	if (argc != 2) {
		printf("Usage: %s <max>\n", argv[0]);
		return 1;
	}

	CircularQueue *initial_queue;

	// ===============================================
	// 			INPUT
	// ===============================================
	N = atoi(argv[1]);
	max_primes = 1.2 * (((double)N)/log(N));
	// ===============================================

	initPrimes(max_primes);

	// All possible integers queue.
	queue_init(&initial_queue, N);
	queue_fill(initial_queue, N);

	// Creates initial thread
	pthread_create(&tids[threads], NULL, computePrimes, initial_queue);

	unsigned int i = 0;
	for(; i <= threads; i++) {
		pthread_join(tids[i], NULL);
	}
	
	qsort(primes, p_count, sizeof(QueueElem), cmpfunc);
	printPrimes();

	return 0;
}

