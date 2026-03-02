#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "wait.h"

int main (int argc, char *argv) {
  
  printf("main : starting program...\n");

  pid_t child_pid;

  child_pid = fork();

  if (child_pid == 0) {
    printf("I am the child!\n");
  } else {
    /* parent */
    printf("I am the parent!\n");
    printf("Child Pid: %d\n", child_pid);

    int status = 0;
    int options = 0;
    
    /* wait for something to happen to child */
    waitpid(child_pid, &status, options);
    
    /* parent testing the wait api */
    printf("~ Testing wait api ~\n");
    wait_t *wait_api = NULL;
    wait_api = wait_create();
    wait_api->help_me();
    wait_destroy(&wait_api);
    printf("~ Testing wait api ~\n");
  }
  
  
  return EXIT_SUCCESS;
}
