#ifndef PROC_H
#define PROC_H

/**
 * @brief User headers
 */
#include "debug.h"

/**
 * @brief General headers
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * @brief System/domain-specific headers
 * - sys/ptrace.h   |   Process tracing (breakpoints, register access, memory R/W)
 * - sys/wait.h     |   Process waiting and status checking
 * - signal.h       |   Signal handling (SIGTRAP, SIGSEGV, etc.)
 */
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <signal.h>

// ~ Forward declaration
typedef struct proc_t proc_t;

// ~ Standalone function prototypes
proc_t *proc_create(const char *target_path);
void proc_destroy(proc_t **self_ptr);
void proc_launch(proc_t *self);
void proc_cont(proc_t *self);
void proc_single_step(proc_t *self);
void proc_wait_for_signal(proc_t *self);

// ~ Struct definition
struct proc_t {
    // ~ Data fields first
    pid_t child_pid;       // ~ PID of the forked child process
    int is_running;        // ~ 1 if the tracee is currently running, 0 if stopped
    int has_exited;        // ~ 1 if the tracee has exited, 0 otherwise
    char *target_path;     // ~ Path to the executable being debugged

    // ~ Function pointers ("methods") second
    void (*launch)(proc_t *self);
    void (*cont)(proc_t *self);
    void (*single_step)(proc_t *self);
    void (*wait_for_signal)(proc_t *self);
    void (*destroy)(proc_t **self_ptr);
};

#endif