#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

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
  }
  
  
  return EXIT_SUCCESS;
}
