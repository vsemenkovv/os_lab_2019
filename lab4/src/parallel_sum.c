#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#include <getopt.h>

#include "utils.h"

struct SumArgs {
  int *array;
  int begin;
  int end;
};

int Sum(const struct SumArgs *args) {
  int sum = 0;
  for(int i = args->begin; i < args->end; i++)
  {
      sum += args->array[i];
  }
  return sum;
}
int* Subarray(int * array, int begin, int end)
{
     int *subarray = malloc(sizeof(int) * (end - begin));
     for( int i = begin; i < end; i++)
     {
         subarray[i-begin] = array[i];
     }
     return subarray;
}

void *ThreadSum(void *args) {
  struct SumArgs *sum_args = (struct SumArgs *)args;
  return (void *)(size_t)Sum(sum_args);
}

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int threads_num = -1;

  while (1) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"threads_num", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            // your code here
            // error handling
            if (seed <= 0)
            {
                printf("Seed must be a positive number");
                return 1;
            }
            break;
          case 1:
            array_size = atoi(optarg);
            // your code here
            // error handling
            if(array_size <= 0)
            {
                printf("Array size must be a positive number");
                return 1;
            }
            break;
          case 2:
            threads_num = atoi(optarg);
            // your code here
            // error handling
            if(threads_num <= 0)
            {
                printf("Number of threads must be a positive");
                return 1;
            }
            break;
          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case '?':
        break;
      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || threads_num == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }

  pthread_t threads[threads_num];
  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array,array_size, seed);
  for(int i = 0; i < array_size;i++)
  {
      printf("%d ", array[i]);
  }
  printf("\n");
  struct SumArgs args[threads_num];
  for(int i = 0; i < threads_num; i++)
  {
      int * subarray = malloc(sizeof(int)*array_size/threads_num);
      if(i != threads_num - 1)
      {
        subarray = Subarray(array,i*array_size/threads_num,(i+1)*array_size/threads_num);
        args[i].array = subarray;
        args[i].begin = 0;
        args[i].end =array_size/threads_num;
      }
      else
      {
        subarray = Subarray(array,i*array_size/threads_num,array_size);
        args[i].array = subarray;
        args[i].begin = 0;
        args[i].end = array_size - i*array_size/threads_num;
      }
  }
  for (uint32_t i = 0; i < threads_num; i++) {
    if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args[i])) {
      printf("Error: pthread_create failed!\n");
      return 1;
    }
  }

  int total_sum = 0;
  for (uint32_t i = 0; i < threads_num; i++) {
    int sum = 0;
    pthread_join(threads[i], (void **)&sum);
    total_sum += sum;
  }
  free(array);
  printf("Total: %d\n", total_sum);
  return 0;
}