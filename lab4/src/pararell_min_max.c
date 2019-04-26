#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>
#include "find_min_max.h"
#include "utils.h"
int pnum;
int* pids;
void handle(int signum)
{
    printf("Processes : ");
    for (int i = 0; i < pnum; i++)
     {
         if(pids[i] != 0)
            printf("%d ", pids[i]);
     }    
    printf("have been killed\n");
    if(kill(0,SIGKILL)  == 0)
        printf("Process killed\n");
    else
        printf("Error when killing process\n");      
}

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  pnum = -1;
  bool with_files = false;
  int timeout = -1;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {"timeout", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            if (seed <= 0)
            {
                printf("Seed must be a positive number");
                return 1;
            }
            break;
          case 1:
            array_size = atoi(optarg);
            if(array_size <= 0)
            {
                printf("Array size must be a positive number");
                return 1;
            }
            break;
          case 2:
            pnum = atoi(optarg);
            if(pnum <= 0)
            {
                printf("Number of procceses must be a positive");
                return 1;
            }
            break;
          case 3:
            with_files = true;
            break;
          case 4:
            timeout = atoi(optarg);
            if(timeout <= 0)
            {
                printf("Timeout must be a positive number");
                return 1;
            }
            break;
          default:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
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

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;
  struct timeval start_time;
  gettimeofday(&start_time, NULL);
  int pipe1[2];
  pipe(pipe1);
  pids = malloc(sizeof(pid_t) * pnum); 
  for (int i = 0; i < pnum; i++) {
    pid_t child_pid = fork();
    if (child_pid >= 0) {
      active_child_processes += 1;
      if (child_pid == 0) 
      {
            struct MinMax min_max;
            if (i != pnum - 1)
            {
                min_max = GetMinMax(array,i*array_size/pnum,(i+1)*array_size/pnum);
            }
            else
            {
                min_max = GetMinMax(array,(array_size - array_size/pnum),array_size);
            }
            if (with_files) 
            {
              FILE* fp = fopen("results.txt", "a");
              fwrite(&min_max, sizeof(struct MinMax), 1, fp);
              fclose(fp);
            } else 
            {
                close(pipe1[0]);
                write(pipe1[1], &min_max, sizeof(struct MinMax));
            }
            return 0;
        }
        else
        {
            pids[i] = child_pid;
        }
    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }
  if(timeout != -1)
  {
      int alarm1 = ualarm(timeout*1000,0);
      int waiter;
      while (active_child_processes > 0) {
          waiter = waitpid(0,NULL,WCONTINUED);
          signal(SIGALRM, handle);
          if(waiter != 0)
          {
              for (int i = 0; i < pnum; i++)
              {
                  if(pids[i] == waiter)
                    pids[i] = 0;
              }
              //printf("Process %d successfully executed\n", waiter);
              active_child_processes -= 1; 
          }
      }

  }
else
{
  while (active_child_processes > 0) {
    close(pipe1[1]);
    wait(NULL);
    active_child_processes -= 1;
  }
}

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (int i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;
    struct MinMax temp;
    if (with_files) {
      FILE* fp = fopen("results.txt", "r");
      fseek(fp,i*sizeof(struct MinMax),SEEK_SET);

      fread(&temp,sizeof(struct MinMax), 1, fp);
      fclose(fp);
    } else {
        struct MinMax temp;
        read(pipe1[0], &temp, sizeof(struct MinMax));
    }

    if (temp.min < min_max.min) min_max.min = temp.min;
    if (temp.max > min_max.max) min_max.max = temp.max;
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);
  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  remove("results.txt");
  return 0;
}