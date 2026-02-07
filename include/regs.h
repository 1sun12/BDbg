#ifndef REGS_H
#define REGS_H

/**
 * @brief User headers
 */
#include "debug.h"

/**
 * @brief General headers
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

/**
 * @brief System/domain-specific headers
 * - sys/user.h    |   User-level register structures (user_regs_struct)
 * - sys/ptrace.h  |   Process tracing (register access via PTRACE_GETREGS)
 */
#include <sys/user.h>
#include <sys/ptrace.h>

// ~ Forward declaration
typedef struct regs_t regs_t;

// ~ Standalone function prototypes
regs_t *regs_create(pid_t pid);
void regs_destroy(regs_t **self_ptr);
void regs_fetch(regs_t *self);
void regs_dump(regs_t *self);
unsigned long regs_get_rip(regs_t *self);
void regs_set_rip(regs_t *self, unsigned long rip);

// ~ Struct definition
struct regs_t {
    // ~ Data fields first
    struct user_regs_struct gp_regs;   // ~ General purpose register snapshot
    pid_t pid;                       // ~ PID of the tracee

    // ~ Function pointers ("methods") second
    void (*fetch)(regs_t *self);
    void (*dump)(regs_t *self);
    unsigned long (*get_rip)(regs_t *self);
    void (*set_rip)(regs_t *self, unsigned long rip);
    void (*destroy)(regs_t **self_ptr);
};

#endif