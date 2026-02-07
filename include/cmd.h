#ifndef CMD_H
#define CMD_H

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

// ~ Struct definition
typedef struct cmd_t cmd_t;
struct cmd_t {
    // ~ Data fields first
    int running;               // ~ 1 if the command loop should keep going
    char input[256];          // ~ Raw input buffer from fgets
    char *command;            // ~ Parsed command string (e.g., "break", "cont", "regs")
    char *argument;           // ~ Parsed argument string (e.g., "0x401000")

    // ~ Function pointers ("methods") second
    void (*read_input)(cmd_t *self);
    void (*parse)(cmd_t *self);
    void (*destroy)(cmd_t **self_ptr);
};

// ~ Standalone function prototypes
cmd_t *cmd_create();
void cmd_destroy(cmd_t **self_ptr);
void cmd_read_input(cmd_t *self);
void cmd_parse(cmd_t *self);

#endif