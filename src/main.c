
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h> /* wait, waitpid, WIFEXITED */
#include <sys/ptrace.h> /* ptrace */
#include <stdint.h> /* pid_t */
#include <unistd.h> /* fork, execve, sleep */

#include "debug.h"

int main() {
  DBG("main: Starting main...");

  pid_t child_pid = -1;
  int status = 0;

  char *target = "trgt-prog/trgt-prog";
  char *argv[] = { NULL };
  char *envp[] = { NULL };

  if ((child_pid = fork()) == -1) perror("fork");

  if (child_pid == 0) {
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    execve(target, argv, envp);

    /* execve failed */
    perror("[ERROR] execve");
    return EXIT_FAILURE;
  } else {
    /* parent is now in control, do stuff */
    waitpid(child_pid, &status, 0);

  }

  return EXIT_SUCCESS;
}
