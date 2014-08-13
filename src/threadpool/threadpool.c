#include "threadpool.h"

#ifndef REF
#define REF(x) &(x)
#endif

void *threadpool_worker(void *tp)
{
	//Get the thread pool pointer
	threadpool_t *threadpool = (threadpool_t*) tp;

	//The task to be executed
	task_t *task;

	for( ; ; ) {
		//Aquire lock
		pthread_mutex_lock(REF(threadpool->mutex));

		//Wait for an available task
		while(!threadpool->stop && !(task = tasklist_pop(threadpool->tasks)))
			pthread_cond_wait(REF(threadpool->new_work), REF(threadpool->mutex));

		//If the thread got a signal to stop
		if(threadpool->stop) {
			pthread_mutex_unlock(REF(threadpool->mutex));
			break;
		}

		//Add task to the executing list and unlock the mutex
		tasklist_insert(threadpool->executing, task);
		pthread_mutex_unlock(REF(threadpool->mutex));

		//Execute the task
		task_exec(task);

		//Aquire lock
		pthread_mutex_lock(REF(threadpool->mutex));

		//Remove from executing list and free task
		tasklist_remove(threadpool->executing, task->ID);
		task_free(task);

		//Notify that a task was done
		pthread_cond_broadcast(REF(threadpool->task_done));

		//Unlock
		pthread_mutex_unlock(REF(threadpool->mutex));
	}

	pthread_exit(NULL);
	return NULL;	//Stop bugging me eclipse
}

threadpool_t *threadpool_create(int n_thread)
{
	//Allocate memory for the thread pool struct
	threadpool_t *threadpool = (threadpool_t*) malloc(sizeof(threadpool_t));
	if(!threadpool) {
		fprintf(stderr, "Allocation failed in threadpool_create().\n");
		return NULL;
	}

	//Initialize parameters
	threadpool->threads = (pthread_t*) malloc(sizeof(pthread_t) * n_thread);
	threadpool->n_threads = n_thread;
	pthread_mutex_init(REF(threadpool->mutex), NULL);
	threadpool->stop = 0;
	threadpool->tasks = tasklist_create();
	threadpool->executing = tasklist_create();
	pthread_cond_init(REF(threadpool->new_work), NULL);
	pthread_cond_init(REF(threadpool->task_done), NULL);

	//Start threads
	int i;
	int error_code;
	for(i = 0; i < n_thread; i++)
		if((error_code = pthread_create(REF(threadpool->threads[i]), NULL, threadpool_worker, (void*) threadpool)) != 0) {
			fprintf(stderr, "Error starting worker thread(error code %d).\n", error_code);
			exit(-1);
		}

	return threadpool;
}

taskID threadpool_addTask(threadpool_t *threadpool, task_t *task)
{
	//Acquire lock
	pthread_mutex_lock(REF(threadpool->mutex));

	//The return value
	taskID ID = 0;

	//Add to tasks
	if(tasklist_insert(threadpool->tasks, task)) {
		//If added notify a worker thread
		pthread_cond_signal(REF(threadpool->new_work));
		ID = task->ID;
	}

	//Unlock and return
	pthread_mutex_unlock(REF(threadpool->mutex));
	return ID;
}

taskID threadpool_add(threadpool_t *threadpool, work_function work, argument_t argument, priority_t priority)
{
	//Create task
	task_t *task = task_create(work, argument, priority);

	//Add to threadpool
	return threadpool_addTask(threadpool, task);
}

void threadpool_wait(threadpool_t *threadpool)
{
	//Acquire lock
	pthread_mutex_lock(REF(threadpool->mutex));

	//Wait until there are no more tasks available or executing
	while(threadpool->tasks->n_tasks || threadpool->executing->n_tasks)
		pthread_cond_wait(REF(threadpool->task_done), REF(threadpool->mutex));

	//Unlock
	pthread_mutex_unlock(REF(threadpool->mutex));
}

void threadpool_waitTask(threadpool_t *threadpool, taskID ID)
{
	//Acquire lock
	pthread_mutex_lock(REF(threadpool->mutex));

	//Wait
	while(tasklist_get(threadpool->executing, ID) || tasklist_get(threadpool->tasks, ID)) {
		pthread_cond_wait(REF(threadpool->task_done), REF(threadpool->mutex));
	}

	//Unlock
	pthread_mutex_unlock(REF(threadpool->mutex));
}

int threadpool_isDone(threadpool_t *threadpool, taskID ID)
{
	//Acquire lock
	pthread_mutex_lock(REF(threadpool->mutex));

	//Check
	int isDone = 1;
	if(tasklist_get(threadpool->executing, ID) || tasklist_get(threadpool->tasks, ID))
		isDone = 0;

	//Unlock
	pthread_mutex_unlock(REF(threadpool->mutex));

	return isDone;
}

void threadpool_free(threadpool_t *threadpool)
{
	//Wait for tasks to be executed
	threadpool_wait(threadpool);

	//Signal threads to stop
	pthread_mutex_lock(REF(threadpool->mutex));
	threadpool->stop = 1;
	pthread_cond_broadcast(REF(threadpool->new_work));
	pthread_mutex_unlock(REF(threadpool->mutex));

	//Join threads
	int i;
	for(i = 0; i < threadpool->n_threads; i++)
		pthread_join(threadpool->threads[i], NULL);

	//Free the memory
	free(threadpool->threads);
	pthread_mutex_destroy(REF(threadpool->mutex));
	tasklist_free(threadpool->tasks);
	tasklist_free(threadpool->executing);
	pthread_cond_destroy(REF(threadpool->new_work));
	pthread_cond_destroy(REF(threadpool->task_done));
	free(threadpool);
}

void *threadpool_background(void *tp)
{
	//Get pointer to the thread pool
	threadpool_t *threadpool = (threadpool_t*) tp;

	//Free memory when done
	threadpool_free(threadpool);

	pthread_exit(NULL);
	return NULL;	//Pls eclipse
}

void threadpool_freeLater(threadpool_t *threadpool)
{
	//Start thread
	pthread_t background;
	if(pthread_create(REF(background), NULL, threadpool_background, (void*) threadpool)) {
		fprintf(stderr, "Error starting background thread.\n");
		exit(-1);
	}

	//Detach thread
	pthread_detach(background);
}
