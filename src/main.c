
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h> /* wait() */
#include <stdint.h>  /* pid_t */
#include <unistd.h> /* fork + sleep */

int main() {
  printf("\n\n=== Starting wait example ===\n\n");
  /* create a pid_t that stores the pid of the child process */
  pid_t child_pid;

  printf("Calling fork()\n");
  child_pid = fork();

  /* if pid is -1, fork failed */
  if (child_pid == -1) {
    perror("fork");
    return EXIT_FAILURE;
  }

  /* if pid is 0, then this is the child process */
  if (child_pid == 0) {
    
    /* child is doing some work... */
    printf("Child is doing some work...\n");
    sleep(2);
    return 42;
  }

  /* as the parent, wait for the child to finish working and return something... */
  int status;
  waitpid(child_pid, &status, 0);

  /* defined in <sys/wait.h> are specific macros used in waiting, see `man 2 wait` for full info */
  if (WIFEXITED(status)){
    printf("Child exited with the status code: %d\n", WEXITSTATUS(status));
  } else if (WIFSTOPPED(status)) {
    printf("Child stopped by signal: %d\n", WSTOPSIG(status));
  }

  return EXIT_SUCCESS;
}
