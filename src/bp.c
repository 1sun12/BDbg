/*
==================================================
HEADERS
==================================================
*/
#include "bp.h"

/*
==================================================
INTERNAL FUNCTION PROTOTYPES
==================================================
*/
static int bp_enable_impl(bp_t *self, unsigned long addr);
static int bp_disable_impl(bp_t *self);
static void bp_destroy_impl(bp_t **self_ptr);

/*
==================================================
CONSTRUCTOR & DESTRUCTOR
==================================================
*/
bp_t *bp_create(pid_t pid) {
    OUTPUT_D_MSG("bp_create : Attempting to create a new breakpoint manager...");
    
    // ~ Create a new breakpoint manager
    bp_t *new = NULL;
    
    // ~ Allocate memory + error check
    new = (bp_t *)calloc(1, sizeof(bp_t));
    if (new == NULL) {
        perror("\n[ERROR]:bp_create");
        return NULL;
    }
    
    // ~ Initialize fields with default values
    new->pid = pid;
    new->addr = 0;
    new->original_byte = 0;
    new->is_enabled = 0;
    
    // ~ Wire up function pointers
    new->enable = bp_enable_impl;
    new->disable = bp_disable_impl;
    new->destroy = bp_destroy_impl;
    
    OUTPUT_D_MSG("bp_create : Successfully created breakpoint manager!");
    return new;
}

static void bp_destroy_impl(bp_t **self_ptr) {
    bp_t *self = *self_ptr;
    
    OUTPUT_D_MSG("bp_destroy : Breakpoint manager being destroyed...");
    
    // ~ If breakpoint is currently enabled, disable it first to restore original byte
    if (self->is_enabled) {
        self->disable(self);
    }
    
    // ~ No owned sub-allocations to free
    
    // ~ Deallocate this entire memory block
    free(self);
    *self_ptr = NULL;
    
    OUTPUT_D_MSG("bp_destroy : Breakpoint manager destroyed successfully!");
}

/*
==================================================
METHOD IMPLEMENTATIONS
==================================================
*/
static int bp_enable_impl(bp_t *self, unsigned long addr) {
    if (self == NULL) {
        perror("\n[ERROR]:bp_enable : self is NULL");
        return -1;
    }
    OUTPUT_D_MSG("bp_enable : Attempting to set breakpoint...");
    
    // ~ Clear errno before the ptrace call to distinguish real errors from valid -1 data
    errno = 0;
    
    // ~ Read the full word at the breakpoint address
    // ~ PTRACE_PEEKDATA returns a full word (8 bytes on x86_64)
    long word = ptrace(PTRACE_PEEKDATA, self->pid, addr, NULL);
    
    // ~ Check for real errors: ptrace returns -1 AND sets errno
    if (word == -1 && errno != 0) {
        perror("\n[ERROR]:bp_enable : PTRACE_PEEKDATA failed");
        return -1;
    }
    
    // ~ Save the original lowest byte (this is what we'll restore later)
    self->original_byte = (uint8_t)(word & 0xFF);
    
    // ~ Replace the lowest byte with INT3 (0xCC)
    // ~ (word & ~0xFF) clears the lowest byte, then | 0xCC sets it
    long modified = (word & ~0xFF) | 0xCC;
    
    // ~ Write the modified word back to inject the breakpoint
    if (ptrace(PTRACE_POKEDATA, self->pid, addr, modified) == -1) {
        perror("\n[ERROR]:bp_enable : PTRACE_POKEDATA failed");
        return -1;
    }
    
    // ~ Update breakpoint state
    self->addr = addr;
    self->is_enabled = 1;
    
    printf("Breakpoint set at 0x%016lx (original byte: 0x%02x)\n", addr, self->original_byte);
    
    OUTPUT_D_MSG("bp_enable : Successfully set breakpoint!");
    return 0;
}

static int bp_disable_impl(bp_t *self) {
    if (self == NULL) {
        perror("\n[ERROR]:bp_disable : self is NULL");
        return -1;
    }
    OUTPUT_D_MSG("bp_disable : Attempting to remove breakpoint...");
    
    // ~ Check if breakpoint is actually enabled
    if (!self->is_enabled) {
        printf("Breakpoint is not currently enabled\n");
        return 0;
    }
    
    // ~ Clear errno before the ptrace call
    errno = 0;
    
    // ~ Read the current word at the breakpoint address
    long word = ptrace(PTRACE_PEEKDATA, self->pid, self->addr, NULL);
    
    // ~ Check for real errors
    if (word == -1 && errno != 0) {
        perror("\n[ERROR]:bp_disable : PTRACE_PEEKDATA failed");
        return -1;
    }
    
    // ~ Restore the original byte
    // ~ Clear the lowest byte and OR it with the saved original byte
    long restored = (word & ~0xFF) | self->original_byte;
    
    // ~ Write the restored word back to remove the breakpoint
    if (ptrace(PTRACE_POKEDATA, self->pid, self->addr, restored) == -1) {
        perror("\n[ERROR]:bp_disable : PTRACE_POKEDATA failed");
        return -1;
    }
    
    // ~ Update breakpoint state
    self->is_enabled = 0;
    
    printf("Breakpoint removed from 0x%016lx (restored byte: 0x%02x)\n", self->addr, self->original_byte);
    
    OUTPUT_D_MSG("bp_disable : Successfully removed breakpoint!");
    return 0;
}