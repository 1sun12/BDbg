#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>

#include "wait.h"

int main (void) {
  
  printf("main : starting program...\n");
  
  /* variables */
  pid_t child_pid;

  child_pid = fork();

  if (child_pid == 0) {
    printf("I am the child!\n");
  } else {
    /* parent */
    printf("I am the parent!\n");
    printf("Child Pid: %d\n", child_pid);
    
    wait_t *wait_api = NULL;
    wait_api = wait_create(child_pid, 1);
    wait_api->help_me();

    wait_api->wait(wait_api);
    printf("get cpid: %d\n", wait_api->get_cpid(wait_api));
    printf("get op: %d\n", wait_api->get_op(wait_api));
  }

  printf("\n");
  
  return EXIT_SUCCESS;
}
