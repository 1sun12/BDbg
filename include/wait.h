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
 *  status - int variable, bit packed, not meant to be read normally, only by wait macros
 *
 *  === waitpid ===
 *  pid_t waitpid(pid_t pid, int *status, int options);
 *
 *  === options (wait_op_e) ===
 *  0           - Blocks until child changes state
 *  WNOHANG     - Do not block, return 0 immediate if nothing has happened yet
 *  WUNTRACED   - Report stopped children, not just terminated ones
 *  WCONTINUED  - Report children resumed by `SIGCONT`
 *
 *  === states (wait_state_e) ===
 *  WAIT_STOPPED    - Child was stopped by a signal (WSTOPSIG gives the signal)
 *  WAIT_EXITED     - Child exited normally (WEXITSTATUS gives the exit code)
 *  WAIT_SIGNALED   - Child was killed by a signal (WTERMSIG gives the signal)
 *  WAIT_UNKNOWN    - Could not determine what happened (shouldn't happen normally)
 */

/**
 *  @title wait_op_e
 *  @brief Enumeration for selecting the waitpid option (3rd argument)
 *  Count from 0 to see your options at a glance.
 */
typedef enum {
  WAIT_BLOCK,       /* 0 - Blocks until child changes state */
  WAIT_NO_HANG,     /* 1 - Do not block, return 0 if nothing happened yet */
  WAIT_UNTRACED,    /* 2 - Report stopped children, not just terminated (used with ptrace) */
  WAIT_CONTINUED    /* 3 - Report children resumed by SIGCONT */
} wait_op_e;

/**
 *  @title wait_state_e
 *  @brief Enumeration representing what happened to the child after waitpid returns.
 *  Populated by wait_wait() after decoding the bit-packed status.
 */
typedef enum {
  WAIT_STOPPED,     /* 0 - Child was stopped by a signal */
  WAIT_EXITED,      /* 1 - Child exited normally */
  WAIT_SIGNALED,    /* 2 - Child was killed by a signal */
  WAIT_UNKNOWN      /* 3 - Could not determine child state */
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

  /**
   *    @title state
   *    @brief The decoded state of the child after wait_wait() returns.
   *    Use this to determine what happened: stopped, exited, or signaled.
   */
  wait_state_e state;

  /**
   *    @title stop_sig
   *    @brief The signal that caused the child to stop (only valid when state == WAIT_STOPPED).
   *    Example: SIGTRAP (5) after a breakpoint or single-step in ptrace.
   */
  int stop_sig;

  /**
   *    @title exit_code
   *    @brief The exit code the child passed to exit() or return (only valid when state == WAIT_EXITED).
   *    Example: 0 for success, non-zero for failure.
   */
  int exit_code;

  /**
   *    @title term_sig
   *    @brief The signal that killed the child (only valid when state == WAIT_SIGNALED).
   *    Example: SIGSEGV (11) if the child segfaulted.
   */
  int term_sig;

  /* ===== Psuedo Methods ===== */
  void (*help_me)(void);
  void (*destroy)(wait_t **);
  wait_state_e (*wait)(wait_t *);
  void (*set_cpid)(wait_t *, pid_t);
  pid_t (*get_cpid)(wait_t *);
  void (*set_op)(wait_t *, int);
  int (*get_op)(wait_t *);
};

/**
 *  @title wait_help_me
 *  @brief Call this function to get a man page on using this api.
 *  Not intended for end users, but rather the programmer or developer.
 */
void wait_help_me(void);

/**
 *  @title wait_create 
 *  @brief [!] Creates a new wait api object, and returns a pointer to that struct in memory
 *  @param cpid The child process id to wait on
 *  @param option The waitpid option (use wait_op_e enum values: 0 = WAIT_BLOCK, etc.)
 *  @return wait_t * or NULL on failure
 */
wait_t *wait_create(pid_t cpid, int option);

/**
 *  @title wait_destroy 
 *  @brief Destroys wait api objects that were created using `wait_create`
 *  @param trash_ptr Double pointer to the wait api to be freed and NULLed
 */
void wait_destroy(wait_t **trash_ptr);

/**
 *  @title wait_wait
 *  @brief The public-facing wait method. Calls waitpid internally, then decodes the
 *  bit-packed status into the state enum and populates stop_sig, exit_code, or term_sig.
 *  @param self The wait api object (must have cpid and option set beforehand)
 *  @return wait_state_e indicating what happened to the child
 */
wait_state_e wait_wait(wait_t *self);

/**
 *  @title wait_set_cpid
 *  @brief Set the pid of the child; Who are we waiting for?
 */
void wait_set_cpid(wait_t *self, pid_t cpid);

/**
 *  @title wait_get_cpid
 *  @brief Get the pid of the child; Who are we waiting for?
 */
pid_t wait_get_cpid(wait_t *self);

/**
 *  @title wait_set_op
 *  @brief Set the third argument, options, when calling waitpid.
 *  Pass a wait_op_e enum value (0 = WAIT_BLOCK, 1 = WAIT_NO_HANG, etc.)
 */
void wait_set_op(wait_t *self, int op);

/**
 *  @title wait_get_op
 *  @brief Get the third argument, options, when calling waitpid
 */
int wait_get_op(wait_t *self);

#endif
