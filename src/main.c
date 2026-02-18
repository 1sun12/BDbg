
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* includes fork */
#include <stdint.h> /* for pid_t */

/*
 * === What is fork() ? ===
 * Fork is a system call provided by <unistd.h> that makes a perfect copy of the existing program   ...
 * and allows it to run seperaetly by using different CPU page tables on memory with its own        ...
 * stack and heap.
 *
 * === How does this differ from threads? ===
 * A thread is similar to a fork; A child process is created. However, a thread is a parent-child   ...
 * that share the same memory space. While a fork is a parent-child that share independent memory   ...
 * spaces. You can reference a global var shared by a thread parent-child, but not for a fork       ...
 * parent-child.
 *
 * === getpid(), getppid(), and pid 0 ===
 * When you call `getpid()`, it will return the PID of THIS forked program. `getppid()` will return ...
 * the pid of the parent process. `pid_t` is a data-type that under the right context, stores pid.  ...
 * Its actually just a signed integer typedef but being explicit in C is quite important.
 *
 * When `child_pid` is equal to 0 in the example below, we know that THIS program is the child. If  ...
 * it is -1 we have an error, anything else is the parent. 
 */

int main () {
  printf("\n=== Starting Fork Example ===");
  
  pid_t child_pid;

  /* create a child of this program using fork */
  child_pid = fork();

  /* there was an error with fork */
  if (child_pid == -1) {
    perror("fork");
    return EXIT_FAILURE;
  } else if (child_pid == 0) {
    printf("\nChild exiting, pid 0\n");
    printf("getpid(): %jd\n", getpid());
    return EXIT_SUCCESS;
  } else {
    printf("\nChild PID: %jd\n", child_pid);
    printf("\nParent exiting\n");
    printf("getpid(): %jd\n", getpid());
    return EXIT_SUCCESS;
  }

  return EXIT_SUCCESS;
}
