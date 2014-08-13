ThreadPool
==========

##Description
A simple to use thread pool implemented in c using pthread.

##Components
The thread pool executes tasks(data structures of type [task_t](https://github.com/dumrelu/ThreadPool/blob/master/src/task/task.h)). The task has **3** major components:

1. An _argument_.
2. A _work function_.
3. A _priority_.

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

Below is an example of a simple work function that adds all the elements of an array in a certain range.
```c
#include <stdio.h>
#include "task/task.h"

typedef struct {
      int start;
      int count;
      int *array;
} range_t;

void addElements(argument_t *argument)
{
      //First get the pointer to the range_t structure
      range_t *range = (range_t*) argument->arg;
      
      //Add the element in the given range
      int i, result = 0;
      for(i = range->start; i < range->count; i++)
            result += range->array[i];
            
      //Set the result
      *((int*) argument->result) = result;
}

int main(int argc, char *argv[])
{
      //Create the array
      const int ARRAY_SIZE = 10;
      int array[ARRAY_SIZE];
      
      //Initialize it
      int i;
      for(i = 0; i < ARRAY_SIZE; i++)
          array[i] = i;
          
      //This is where the result will be written
      int result = 0;
      
      //Create the range structure
      range_t range = { 0, ARRAY_SIZE, array };
      
      //Create the argument
      argument_t argument = make_argument((void*) &range, NO_DELETE, (void*) &result);
      
      //The work function
      work_function work = addElements;
      
      //Call the work function
      work(&argument);
      
      //Print the result
      printf("The sum of the elements in the array is: %d.\n", result);
      
      return 0;
}
```
