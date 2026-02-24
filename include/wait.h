
/**
 *      ==============================
 *      ==== Waitpid Abstractions ====
 *      ==============================
 *
 *      WIFEXITED(status)   - Did the child call exit or return from main normally?
 *              WEXITSTATUS(status) - What number did the child pass to exit or return?
 *      WIFSTOPPED(status)  - Did the child pause by a signal?
 *              WSTOPSIG(status)    - What signal caused the stop? (i.e SIGTRAP)
 *      WIFSIGNALED(status) - Did the child die by a signal? (Dead, did not exit cleanly)
 *              WTERMSIG(status)    - What signal killed the child? 
 *
 *      =========================
 *      ==== Waitpid Options ====
 *      =========================
 *      
 *      pid_t waitpid(pid_t pid, int *status, int options);
 *
 *      ==== Options (?) ====
 *
 *      0           - Blocks until the child changes state (default). 
 *      WNOHANG     - Do not block, return 0 immediate if nothing has happened yet. 
 *      WUNTRACED   - Report stopped children, not just terminated ones. 
 *      WCONTINUED  - Report children resumed by `SIGCONT`. 
 */

#ifndef WAIT_H
#define WAIT_H 

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct wait_t wait_t;
struct wait_t {
  void (*destroy)(wait_t **);
};

wait_t *wait_create();
void wait_destroy(wait_t **self);
#endif
