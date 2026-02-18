
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h> /* wait, waitpid, WIFEXITED */
#include <sys/ptrace.h> /* ptrace */
#include <stdint.h> /* pid_t */
#include <unistd.h> /* fork, execve, sleep */

/*
 * === What is ptrace() ? ===
 * `ptrace()` is a Linux system call that gives one process complete control over another process.  ...
 * It can read/write the target's memory, read/write its CPU registers, single-step one             ...
 * instruction at a time, and intercept every signal the target receives. It is the foundation of   ...
 * every debugger on Linux.
 *
 * === How does it work with fork and execve? ===
 * The parent calls `fork()` to create a child process. The child calls `ptrace(PTRACE_TRACEME)`    ...
 * to opt into being traced, then calls `execve()` to become the target program. The kernel sees    ...
 * the TRACEME flag and automatically stops the child with a SIGTRAP before the new program         ...
 * executes its first instruction.
 *
 * Meanwhile, the parent calls `waitpid()` which blocks until the child stops. When waitpid         ...
 * returns, the parent uses `WIFSTOPPED(status)` to confirm the child was stopped by a signal.      ...
 * Now the parent has full control — it can inspect memory with `PTRACE_PEEKDATA`, modify memory    ...
 * with `PTRACE_POKEDATA`, read registers with `PTRACE_GETREGS`, and resume the child with          ...
 * `PTRACE_CONT`.
 *
 * === The debugger loop ===
 * The core cycle of every debugger is: waitpid (wait for child to stop) -> inspect/modify via      ...
 * ptrace -> PTRACE_CONT (resume child) -> waitpid again. This repeats until the child exits.       ...
 * Breakpoints work by writing 0xCC (INT 3) into the target's code, which causes a SIGTRAP that     ...
 * wakes up waitpid, giving the debugger control at that exact instruction.
 *
 * === About this example ===
 * This program forks a child, has it TRACEME and execve into `/bin/ls`, then the parent reads      ...
 * 8 bytes from the child's memory using PTRACE_PEEKDATA before resuming it with PTRACE_CONT.       ...
 * The address 0x400000 may return -1 (0xffffffffffffffff) because modern binaries are compiled     ...
 * as position-independent executables (PIE) and get loaded at randomized addresses.
 */

int main() {
  printf("\n\n === PTRACE EXAMPLE PROGRAM === \n\n");

  /* create a variable to store the child pid */
  pid_t child_pid = -1;

  /* create a variable to hold the wait status; what happened to child? */
  int status = -1;

  /* create a variable to store the target program */
  char *target = "/bin/ls";
  char *argv[] = {"ls", "-la", "/home/", NULL};
  char *envp[] = { NULL };

  /* create child process */
  child_pid = fork(); 

  /* some basic fork error checking */
  if (child_pid == -1) {
    perror("fork");
  }
  
  /* what to do if you are the child */
  if (child_pid == 0) {
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    printf("\nChild: sleeping for 5 then execve ls...\n");
    sleep(5);
    execve(target, argv, envp);  

    /* if anything below execve runs, it failed */
    perror("execve");
    return EXIT_FAILURE;
  }

  /* what to do if you are the parent */
  waitpid(child_pid, &status, 0);

  if (WIFSTOPPED(status)) {
    /* read 8 bytes from the beginning of the child code */
    long data = ptrace (PTRACE_PEEKDATA, child_pid, (void *)0x400000, NULL);
    printf("\nChild data at 0x400000: 0x%lx\n", data);

    /* let the child continue */
    ptrace(PTRACE_CONT, child_pid, NULL, NULL);
  }

  waitpid(child_pid, &status, 0);
  return EXIT_SUCCESS;
}
