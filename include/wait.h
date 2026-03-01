#ifndef WAIT_H
#define WAIT_H 

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

/*
 *  === Quick Notes ===
 *  WIFEXITED(status)   - Did the child call exit or return normally?
 *          WEXITSTATUS(status)     - What number did the child pass to exit or return?
 *  WIFSTOPPED(status)  - Did the child pause by a signal?
 *          WSTOPSIG(status)        - What signal caused the stop? (i.e SIGTRAP)
 *  WIFSIGNALED(status) - Did the child die by a signal? (Dead, did not exit cleanly)
 *          WTERMSIG(status)        - What signal killed the child?
 *
 *  status - int variable, bit packed, not meant to be read normally, only by wait calls
 *
 *  === waitpid ===
 *  pid_t waitpid(pid_t pid, int *status, int options);
 *
 *  === options ===
 *  0           - Blocks until child changes state
 *  WNOHANG     - Do not block, return 0 immediate if nothing has happened yet
 *  WUNTRACED   - Report stopped children, not just terminated ones
 *  WCONTINUED  - Report children resumed by `SIGCONT`
 */

typedef enum {
  WAIT_STOPPED,
  WAIT_EXITED,
  WAIT_SIGNALED
} wait_state_e;

typedef struct wait_t wait_t;
struct wait_t {
  pid_t pid;
  int status;
  int option;

  /*
   *    fork is called and a child process is created 
   *    wait is used to control that child 
   *    so fork is called 
   *    then that child pid number is passed to the wait api as pid 
   *    create wait api with default settings (0, blocks until child changes state) 
   *
   *
   */
};

/**
 *  @title wait_help_me
 *  @brief Call this function in main to get a man page on using this api, not intended for use by  ... 
 *  the user, but rather a programmer or developer
 */
void wait_help_me();

/**
 *  @title wait_create 
 *  @brief [!] Creates a new wait api object, and returns a pointer to that struct in memory
 *  @return wait_t *
 */
wait_t *wait_create();

/**
 *  @title wait_destroy 
 *  @brief Destroys wait api objects that were created using `wait_create`
 */
void wait_destroy();

#endif
