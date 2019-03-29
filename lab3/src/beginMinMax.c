#include <stdio.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

int main(void)
{
        pid_t child_pid = fork();
        if(child_pid == 0){
            execl("sequential_min_max", "sequential_min_max", "25", "1000", NULL );
        }
        else
            exit(0);
}