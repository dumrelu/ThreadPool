#include "task.h"
#include <assert.h>


//================
// Work functions.
//================

int noArgument_int = 0;
void work_noArgument(argument_t *argument)
{
	noArgument_int = 1;
}


//=======
// Tests.
//=======


/**
 * Argument tests.
 */

void make_argument_test()
{
	//Create argument
	int *myArg = (int*) malloc(sizeof(int));
	int result;
	argument_t myArgument = make_argument((void*) myArg, DELETE, (void*) &result);

	//Asserts
	assert(myArgument.arg == myArg);
	assert(myArgument.del_arg == DELETE);
	assert(myArgument.result == &result);

	//Free memory
	free(myArg);
}

/**
 * Task tests.
 */

void task_create_test()
{
	//Create the task
	task_t *task = task_create(work_noArgument, NO_ARGUMENT, HIGH);

	//Asserts
	assert(task->work == work_noArgument);
	assert(task->priority == HIGH);
	assert(task->status == NOT_EXECUTED);
	assert(task->next == NULL);

	//Free memory
	free(task);
}

void task_exec_test()
{
	//Create the task
	task_t *task = task_create(work_noArgument, NO_ARGUMENT, HIGH);

	//Execute task
	task_exec(task);

	//Asserts
	assert(task->status == EXECUTED);
	assert(task_exec(task) == 0);
	assert(noArgument_int == 1);
	noArgument_int = 0;

	//Free memory
	free(task);
}


/**
 * Task list tests.
 */

void tasklist_create_test()
{
	//Create task list
	tasklist_t *tasklist = tasklist_create();

	//Asserts
	assert(tasklist->head == NULL);
	assert(tasklist->n_tasks == 0);

	//Free memory
	free(tasklist);
}

void tasklist_insert_test()
{
	//Create task list
	tasklist_t *tasklist = tasklist_create();

	//Create tasks
	task_t *low = task_create(work_noArgument, NO_ARGUMENT, LOW);
	task_t *medium = task_create(work_noArgument, NO_ARGUMENT, MEDIUM);
	task_t *high = task_create(work_noArgument, NO_ARGUMENT, HIGH);

	//Insert them in the list
	tasklist_insert(tasklist, high);
	tasklist_insert(tasklist, medium);
	tasklist_insert(tasklist, low);

	//Asserts
	assert(tasklist->n_tasks == 3);
	assert(tasklist->head == high);
	assert(high->next == medium);
	assert(medium->next == low);

	//Free memory
	free(tasklist);
	free(low);
	free(medium);
	free(high);
}

void tasklist_pop_test()
{
	//Create task list
	tasklist_t *tasklist = tasklist_create();

	//Create tasks
	task_t *low = task_create(work_noArgument, NO_ARGUMENT, LOW);
	task_t *medium = task_create(work_noArgument, NO_ARGUMENT, MEDIUM);
	task_t *high1 = task_create(work_noArgument, NO_ARGUMENT, HIGH);
	task_t *high2 = task_create(work_noArgument, NO_ARGUMENT, HIGH);

	//Insert them in the list
	tasklist_insert(tasklist, medium);
	tasklist_insert(tasklist, high1);
	tasklist_insert(tasklist, high2);
	tasklist_insert(tasklist, low);

	//Asserts
	assert(tasklist_pop(tasklist) == high1);
	assert(tasklist_pop(tasklist) == high2);
	assert(tasklist_pop(tasklist) == medium);
	assert(tasklist_pop(tasklist) == low);
	assert(tasklist->n_tasks == 0);
	tasklist_insert(tasklist, medium);
	assert(tasklist_pop(tasklist) == medium);

	//Free memory
	free(tasklist);
	free(low);
	free(medium);
	free(high1);
	free(high2);
}

void tasklist_remove_test()
{
	//Create task list
	tasklist_t *tasklist = tasklist_create();

	//Create tasks
	task_t *low = task_create(work_noArgument, NO_ARGUMENT, LOW);
	task_t *medium = task_create(work_noArgument, NO_ARGUMENT, MEDIUM);
	task_t *high = task_create(work_noArgument, NO_ARGUMENT, HIGH);

	//Insert them in the list
	tasklist_insert(tasklist, high);
	tasklist_insert(tasklist, medium);
	tasklist_insert(tasklist, low);

	//Asserts
	assert(tasklist_remove(tasklist, medium->ID) == medium);
	assert(tasklist_remove(tasklist, low->ID) == low);
	assert(tasklist_remove(tasklist, high->ID) == high);
	assert(tasklist->n_tasks == 0);
	assert(tasklist_remove(tasklist, high->ID) != high);

	//Free memory
	free(tasklist);
	free(low);
	free(medium);
	free(high);
}

void tasklist_clear_test()
{
	//Create task list
	tasklist_t *tasklist = tasklist_create();

	//Create tasks
	task_t *low = task_create(work_noArgument, NO_ARGUMENT, LOW);
	task_t *medium = task_create(work_noArgument, NO_ARGUMENT, MEDIUM);
	task_t *high = task_create(work_noArgument, NO_ARGUMENT, HIGH);

	//Insert them in the list
	tasklist_insert(tasklist, high);
	tasklist_insert(tasklist, medium);
	tasklist_insert(tasklist, low);

	//Clear
	tasklist_clear(tasklist);

	//Asserts
	assert(tasklist->n_tasks == 0);
}

//===============
// Execute tests.
//===============
void task_h_runTests()
{
	//Argument tests
	make_argument_test();

	//Task tests
	task_create_test();
	task_exec_test();

	//Task list tests
	tasklist_create_test();
	tasklist_insert_test();
	tasklist_pop_test();
	tasklist_remove_test();
	tasklist_clear_test();
}
