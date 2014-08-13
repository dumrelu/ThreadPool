/**
 * Parallel program that gets the sum of all the elements in an array.
 */
#include <stdio.h>
#include "threadpool/threadpool.h"

//The number of threads to be used
#define THREADS 3

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
      int i;
      long result = 0;
      for(i = 0; i < range->count; i++)
            result += range->array[range->start + i];

      //Set the result
      *((long*) argument->result) = result;
}

int main(int argc, char *argv[])
{
      //Create the array
      const long ARRAY_SIZE = 100;
      int array[ARRAY_SIZE];

      //Initialize it
      int i;
      for(i = 0; i < ARRAY_SIZE; i++)
          array[i] = i;

      //This is where the result will be written
      long results[THREADS];

      //The ranges of the array that each thread will add
      range_t ranges[THREADS];

      //Number of elements to add per thread
      const int elementsPerThread = (int)(ARRAY_SIZE / THREADS);

      //Create the ranges
      for(i = 0; i < THREADS; i++) {
    	  ranges[i].start = i * elementsPerThread;
    	  ranges[i].count = elementsPerThread;
    	  ranges[i].array = array;
      }

      //The excess elements will be taken care of by the last thread
      ranges[THREADS-1].count = ARRAY_SIZE - (THREADS-1) * elementsPerThread;

      //Create the thread pool
      threadpool_t *threadpool = threadpool_create(THREADS);

      //Add tasks(with a default priority(medium))
      for(i = 0; i < THREADS; i++)
    	  threadpool_add(threadpool, addElements,
    			  make_argument((void*) &ranges[i], NO_DELETE, (void*) &results[i]), DEFAULT);

      //Wait for them to finish and free memory
      threadpool_free(threadpool);

      //Add all the results together
      long final_result = 0;
      for(i = 0; i < THREADS; i++)
    	  final_result += results[i];

      //The expected result
      long n = ARRAY_SIZE-1;
      long expected_result = (n*(n+1))/2;

      //Print results
      printf("Result: %ld.\n", final_result);
      printf("Expected result: %ld.\n", expected_result);
      printf("Are the result equal? %s.\n", (final_result == expected_result) ? "Yes" : "No");

      return 0;
}
