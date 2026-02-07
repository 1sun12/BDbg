#ifndef BP_H
#define BP_H

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
#include <errno.h>

/**
 * @brief System/domain-specific headers
 * - sys/ptrace.h  |   Process tracing (memory access via PTRACE_PEEKDATA/POKEDATA)
 */
#include <sys/ptrace.h>

// ~ Forward declaration
typedef struct bp_t bp_t;

// ~ Standalone function prototypes
bp_t *bp_create(pid_t pid);
void bp_destroy(bp_t **self_ptr);
int bp_enable(bp_t *self, unsigned long addr);
int bp_disable(bp_t *self);

// ~ Struct definition
struct bp_t {
    // ~ Data fields first
    unsigned long addr;        // ~ Address where the breakpoint is set
    uint8_t original_byte;     // ~ The original byte that was at addr before we wrote 0xCC
    int is_enabled;            // ~ 1 if the breakpoint is currently active, 0 if disabled
    pid_t pid;                // ~ PID of the tracee (needed for ptrace calls)

    // ~ Function pointers ("methods") second
    int (*enable)(bp_t *self, unsigned long addr);
    int (*disable)(bp_t *self);
    void (*destroy)(bp_t **self_ptr);
};

#endif