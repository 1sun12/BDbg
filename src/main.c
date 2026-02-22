
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h> /* wait, waitpid, WIFEXITED */
#include <sys/ptrace.h> /* ptrace */
#include <stdint.h> /* pid_t */
#include <unistd.h> /* fork, execve, sleep */

#include "debug.h"
#include "wait.h"

int main() {
  DBG("main: Starting main...");
  
  /**
   *    @title child_pid
   *    @brief Variable storing the process ID number for the child
   */
  pid_t child_pid = -1;

  /**
   *    @title status 
   *    @brief Variable storing the result of `waitpid()`. `waitpid()` is how the parent is notified...
   *    of when anything of signifigance happens to the child process.
   */
  int status = 0;

  /**
   *    @title *target 
   *    @brief (execve) Variable storing the string representation of the target program to be      ...
   *    debugged.
   */
  char *target = "trgt-prog/trgt-prog";

  /**
   *    @title *argv[]
   *    @brief (execve) Variable storing the arguments being passed to the target program. When I   ...
   *    say arguments, I mean something like `-la` when using `ls -la` in bash.
   */
  char *argv[] = { NULL };

  /**
   *    @title *envp[]
   *    @brief (execve) Variable storing the environment pointers being passed to the target program.
   */
  char *envp[] = { NULL };

  /**
   *    if      child_pid is equal to -1 after forking a new child.
   *    then    print an error message.
   *    else    just continue.
   */
  DBG("main: Creating fork()...");
  if ((child_pid = fork()) == -1) perror("fork");

  /**
   *    if      child_pid is equal to 0.
   *        then    THIS is the child process created from the earlier fork. Call `ptrace()` using  ...
   *                `PTRACE_TRACEME` which gives the parent process tracing capabilities over the   ... 
   *                child.
   *    else    child_pid is not the child.
   *        then    THIS is the parent, use `waitpid()` and other to manipulate the child program.
   *                1st, use `waitpid()` to capture the child's current status. 
   *                2nd, do something based on this status.
   *                3rd, use `PTRACE_CONT` to press play again on the child, letting it run.
   *                4th, use `waitpid()` again; child will either stop again or exit.
   */
  if (child_pid == 0) {
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    execve(target, argv, envp);

    /* execve failed */
    perror("[ERROR] execve");
    return EXIT_FAILURE;
  } else {
    /* parent is now in control, do stuff */
    wait_t *waiter = NULL;
    waiter = wait_create();
    printf("\n\ntest num: %d\n", waiter->test);
    waitpid(child_pid, &status, 0);

    


    
    // child is currently paused by a SIGTRAP 
    // obtain status and compare it to a wait macro
    // if this macro --> do this action
    // tell the child they can continue 
    //
  }

  return EXIT_SUCCESS;
}
