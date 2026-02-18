
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* execve() sys call */

/*
 * === What is execve() ? ===
 * `execve()` is a Linux system call that executes another program (typically already compiled) and ...
 * completely replaces it with the current one. 
 *
 * For example, this program will attempt to run `ls` with the arguments `ls -la /home/`. ls can    ...
 * is a standard binary found in /bin/ls. When `execve` launches ls, it completely destroys this    ...
 * program and completely replaces the memory space (using the same PID) with the newly executed    ...
 * program.
 *
 * === Why would I destroy my current program with a new one? ===
 * A good combination is using `fork()` + `execve()` to create a parent child relationship between  ...
 * two programs. You can fork *this* program, and then call `execve()` to replace the child with    ...
 * another program all-together, keep the same PID the child had.
 */

int main () {
  printf("=== Using execve() to run ls ===");

  /* the target program we wish to execute */
  char *target = "/bin/ls";

  /* arguments getting passed to the child usurper */
  char *argv[] = {"ls", "-la", "/home/", NULL};

  /* environment getting passed to the child usurper */
  char *envp[] = { NULL };

  /* call execve, filling in the arguments created above */
  execve(target, argv, envp);

  /* if execve failed, this printf below will print */
  printf("\n\n=== EXECVE FAILED ===\n\n");

  return EXIT_FAILURE;
}
