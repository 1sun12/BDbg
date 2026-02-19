
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h> /* wait, waitpid, WIFEXITED */
#include <sys/ptrace.h> /* ptrace */
#include <stdint.h> /* pid_t */
#include <unistd.h> /* fork, execve, sleep */

int main() {
  pid_t child_pid = -1;
  status = 0;

  char *target = "../trgt-prog/trgt-prog";
  char *argv = { NULL };
  char *envp[] = { NULL };

  if ((child_pid = fork()) == -1) perror("fork");

  if (child_pid == 0) {
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    execve(target, argv, envp);

    /* execve failed */
    perror("execve");
    return EXIT FAILURE;
  } else {
    /* parent is now in control, do stuff */
  }

  return EXIT_SUCCESS;
}
