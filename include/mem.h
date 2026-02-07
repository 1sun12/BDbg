#ifndef MEM_H
#define MEM_H

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
 * - sys/ptrace.h  |   Process tracing (memory access via PTRACE_PEEKDATA)
 */
#include <sys/ptrace.h>

// ~ Forward declaration
typedef struct mem_t mem_t;

// ~ Standalone function prototypes
mem_t *mem_create(pid_t pid);
void mem_destroy(mem_t **self_ptr);
long mem_read_word(mem_t *self, unsigned long addr);
void mem_dump(mem_t *self, unsigned long addr, size_t bytes);

// ~ Struct definition
struct mem_t {
    // ~ Data fields first
    pid_t pid;                 // ~ PID of the tracee
    unsigned long last_addr;   // ~ Last address read (for convenience / "read more")

    // ~ Function pointers ("methods") second
    long (*read_word)(mem_t *self, unsigned long addr);
    void (*dump)(mem_t *self, unsigned long addr, size_t bytes);
    void (*destroy)(mem_t **self_ptr);
};

#endif