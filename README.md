ThreadPool
==========

##Description
A simple to use thread pool implemented in C using pthread. The thread pool executes tasks using a number of threads.

##The task
The thread pool executes tasks(data structures of type [task_t](https://github.com/dumrelu/ThreadPool/blob/master/src/task/task.h)). The task has **4** major components:

1. An _argument_.
2. A _work function_.
3. A _priority_.
4. An _ID_.

###The argument
The _argument_ is an object of type [argument_t](https://github.com/dumrelu/ThreadPool/blob/master/src/task/task.h) and it's used to send data to the _work function_. It has **3** components:

1. **void** *_arg_ - This pointer should be set to point to the main data used by the task.
2. **memory_t** _del_arg_ - It can have one of **2** values: **DELETE** or **NO_DELETE**. It indicates if the memory pointed by _arg_ should be freed after the task is executed or not.
3. **void** *_result_ - This pointer should be set to point to a block of memory in which the task can write its result.


######Creating the argument
You can manually create it like this:
```c
//Method one
argument_t argument1 = { (void*) &data, NO_DELETE, (void*) &result };

//Method two
argument_t argument2;
argument2.arg = (void*) &data;
argument2.del_arg = NO_DELETE;
argument2.result = (void*) &result;
```
Or you can use the [make_argument](https://github.com/dumrelu/ThreadPool/blob/master/src/task/task.h) function:
```c
//Global variable
int result;

//Some function
int *data = (int*) malloc(sizeof(int));
//By givin DELETE there is no need to free the data pointer, it will be freed after the task is executed
argument_t argument = make_argument((void*) data, DELETE, (void*) &result);
```

######When no argument is needed
When no argument is need the NO_ARGUMENT global variable can be used, which is defined as:
```c
argument_t NO_ARGUMENT = { NULL, NO_DELETE, NULL };
```

###The work function
The _work function_ is a pointer to a function that does the work of the task(the function that is called in the thread pool).

The _work function_ definition:
```c
typedef void (*work_function)(argument_t *);
```

You can find a simple example of a work function [here](https://github.com/dumrelu/ThreadPool/blob/master/examples/simple_work_function.c).

###Priority
Every task has a priority, **LOW**, **MEDIUM** or **HIGH**. The order in which the tasks are executed by the thread pool is based on the priority.

###ID
An unique identifier(of type **taskID**, which is a typedef of an int) is assigned to every task when created. You can interact with the thread pool using only the taskID.


###Creating and executing a task
To create a task use the **task_create()** function which takes as parameters a _work function_, an _argument_ and a _priority_. If the task is not added to a thread pool you should free it using the **task_free()** function.

To execute the task call the **task_exec()** function which returns 1 on success and 0 if failed. Note that after a successful call to **task_exec()** the _task->argument.arg_ is freed if _task->argument.del_arg_ == **DELETE**. 

Sample code:
```c
//...
task_t *task = task_create(work, make_argument(...), MEDIUM);
task_exec(task);
task_free(task);
//...
```

##The thread pool
To create a thread pool just call the **threadpool_create()** function which takes as a single parameter the number of threads for the thread pool(note that the threads will automatically start).
```c
threadpool_t *threadpool = threadpool_create(5);  //Create a thread pool with 5 threads
```

To add a task to the thread pool you can either create a task and add it, or you can let the threadpool create the task for you, this way you'll never need to interact with the task_t structure. Note that both methods will return the inserted task's ID on success and 0 if failed.
```c
threadpool_t *threadpool = threadpool_create(5);  //Create a thread pool with 5 threads

//Method 1
task_t *task = task_create(work, make_argument(...), HIGH);
taskID ID = threadpool_addTask(threadpool, task);
if(ID)
      printf("Inserted task %d.\n", ID);

//Method 2
ID = threadpool_add(threadpool, work, make_argument(...), HIGH);
if(ID)
      printf("Inserted task %d.\n", ID);
```
