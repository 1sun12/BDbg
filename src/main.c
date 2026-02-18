
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h> /* wait() */
#include <stdint.h>  /* pid_t */
#include <unistd.h> /* fork + sleep */

/*
 *  === What is wait / waitpid ? ===
 * `wait` or `waitpid` is how you obtain information from a child process. Without it, a parent     ...
 *  would never know when to do anything after something happens to its children. This example uses ...
 *  `fork()` to create a copy of *this* program, and then the parent uses `waitpid()` to wait for   ...
 *  something to happen to its child first before continuing with the rest of the code. 
 *
 *  === What are those macros used on line 46 - 49 ? ===
 *  The wait syscall has macros to help you decipher what exactly happened to your child. Did it    ...
 *  exit from a failure? A return value? Killed by another process? etc. 
 *
 *  The variable `status` is not just a simple number, but the kernal packs multiple pieces of      ...
 *  information into each bit of that single integer. The low 7 bits mean one thing, bit 7 means    ...
 *  something, the upper bits are also something else. WEXITSTATUS(status) is really just           ...
 *  doing `(status >> 8) & 0xFF`; shifting right 8-bits and masking. For simplicity sakes, the      ...
 *  original authors of `wait` created these macros to make using it more simple.
 */

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
