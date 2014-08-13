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
