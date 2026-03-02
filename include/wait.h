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
  WAIT_BLOCK,
  WAIT_NO_HANG,
  WAIT_UNTRACED,
  WAIT_CONTINUED
} wait_op_e;

typedef enum {
  WAIT_STOPPED,
  WAIT_EXITED,
  WAIT_SIGNALED
} wait_state_e;

typedef struct wait_t wait_t;
struct wait_t {
  /* ===== Variables ===== */
  /**
   *    @title cpid 
   *    @brief Child process id; The pid number of the child program.
   */
  pid_t cpid;

  /**
   *    @title status 
   *    @brief 16-Bit packed integer containing a few context clues of what waitpid returned. 
   *    Bits 0 - 6      0000000001111111    Signal number
   *    Bit 7           0000000010000000    Core was dumped? (t or f)
   *    Bits 8 - 15     1111111100000000    Exit code
   */
  int status;

  /**
   *    @title option 
   *    @brief Option code that is passed as the third argument to waitpid
   */
  int option;
  
  /**
   *    @title options_e 
   *    @brief Enumeration used in setting option
   */
  wait_op_e options_e;

  /* psuedo methods */
  void (*help_me)(void);
  void (*destroy)(wait_t **);
  void (*wait)(wait_t *);
  void (*set_cpid)(wait_t *, pid_t);
  pid_t (*get_cpid)(wait_t *);
  void (*set_op)(wait_t *, int);
  int (*get_op)(wait_t *);
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
wait_t *wait_create(pid_t cpid, int option);

/**
 *  @title wait_destroy 
 *  @brief Destroys wait api objects that were created using `wait_create`
 */
void wait_destroy(wait_t **trash_ptr);

/**
 *  @title wait_wait
 *  @brief The actual system call to waitpid, wait api is to be configured first before calling
 */
void wait_wait(wait_t *self);

/**
 *  @title wait_set_cpid
 *  @brief Set the pid of the child; Who are we waiting for?
 */
void wait_set_cpid(wait_t* self, pid_t cpid);

/**
 *  @title wait_get_cpid
 *  @brief Get the pid of the child; Who are we waiting for?
 */
pid_t wait_get_cpid(wait_t* self);

/**
 *  @title wait_set_op
 *  @brief Set the third argument, options, when calling waitpid
 */
void wait_set_op(wait_t *self, int op);

/**
 *  @title wait_get_op
 *  @brief Get the third argument, options, when calling waitpid
 */
int wait_get_op(wait_t *self);

#endif
