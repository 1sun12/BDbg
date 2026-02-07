/*
==================================================
HEADERS
==================================================
*/
#include "cmd.h"

/*
==================================================
INTERNAL FUNCTION PROTOTYPES
==================================================
*/
static void cmd_read_input_impl(cmd_t *self);
static void cmd_parse_impl(cmd_t *self);
static void cmd_destroy_impl(cmd_t **self_ptr);

/*
==================================================
CONSTRUCTOR & DESTRUCTOR
==================================================
*/
cmd_t *cmd_create() {
    OUTPUT_D_MSG("cmd_create : Attempting to create a new command parser...");
    
    // ~ Create a new command parser
    cmd_t *new = NULL;
    
    // ~ Allocate memory + error check
    new = (cmd_t *)calloc(1, sizeof(cmd_t));
    if (new == NULL) {
        perror("\n[ERROR]:cmd_create");
        return NULL;
    }
    
    // ~ Initialize fields with default values
    new->running = 1;        // ~ Start with the loop running
    new->input[0] = '\0';   // ~ Clear input buffer
    new->command = NULL;
    new->argument = NULL;
    
    // ~ Wire up function pointers
    new->read_input = cmd_read_input_impl;
    new->parse = cmd_parse_impl;
    new->destroy = cmd_destroy_impl;
    
    OUTPUT_D_MSG("cmd_create : Successfully created command parser!");
    return new;
}

static void cmd_destroy_impl(cmd_t **self_ptr) {
    cmd_t *self = *self_ptr;
    
    OUTPUT_D_MSG("cmd_destroy : Command parser being destroyed...");
    
    // ~ Free any owned sub-allocations first
    free(self->command);
    self->command = NULL;
    free(self->argument);
    self->argument = NULL;
    
    // ~ Deallocate this entire memory block
    free(self);
    *self_ptr = NULL;
    
    OUTPUT_D_MSG("cmd_destroy : Command parser destroyed successfully!");
}

/*
==================================================
METHOD IMPLEMENTATIONS
==================================================
*/
static void cmd_read_input_impl(cmd_t *self) {
    does_exist(self);
    OUTPUT_D_MSG("cmd_read_input : Reading user input...");
    
    // ~ Print the debugger prompt
    printf("bdbg> ");
    
    // ~ Read input safely using fgets (bounds checking)
    // ~ fgets includes the newline character if present
    if (fgets(self->input, sizeof(self->input), stdin) == NULL) {
        // ~ fgets failed (likely EOF)
        printf("\n");
        self->running = 0;
        return;
    }
    
    // ~ Remove trailing newline if present
    size_t len = strlen(self->input);
    if (len > 0 && self->input[len - 1] == '\n') {
        self->input[len - 1] = '\0';
    }
    
    OUTPUT_D_MSG("cmd_read_input : Successfully read user input!");
}

static void cmd_parse_impl(cmd_t *self) {
    does_exist(self);
    OUTPUT_D_MSG("cmd_parse : Parsing user input...");
    
    // ~ Free any previous command/argument
    free(self->command);
    self->command = NULL;
    free(self->argument);
    self->argument = NULL;
    
    // ~ Skip leading whitespace
    char *start = self->input;
    while (*start == ' ' || *start == '\t') {
        start++;
    }
    
    // ~ Check for empty input
    if (*start == '\0') {
        self->command = strdup("");
        self->argument = strdup("");
        return;
    }
    
    // ~ Find the end of the command (first space or end of string)
    char *end = start;
    while (*end != '\0' && *end != ' ' && *end != '\t') {
        end++;
    }
    
    // ~ Extract the command
    size_t cmd_len = end - start;
    self->command = (char *)malloc(cmd_len + 1);
    if (self->command == NULL) {
        perror("\n[ERROR]:cmd_parse : malloc failed for command");
        return;
    }
    strncpy(self->command, start, cmd_len);
    self->command[cmd_len] = '\0';
    
    // ~ Skip whitespace before argument
    while (*end == ' ' || *end == '\t') {
        end++;
    }
    
    // ~ Extract the argument (if any)
    if (*end != '\0') {
        self->argument = strdup(end);
        if (self->argument == NULL) {
            perror("\n[ERROR]:cmd_parse : malloc failed for argument");
            return;
        }
    } else {
        self->argument = strdup("");
    }
    
    OUTPUT_D_MSG("cmd_parse : Successfully parsed command and argument!");
}