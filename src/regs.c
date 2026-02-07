/*
==================================================
HEADERS
==================================================
*/
#include "regs.h"

/*
==================================================
INTERNAL FUNCTION PROTOTYPES
==================================================
*/
static void regs_fetch_impl(regs_t *self);
static void regs_dump_impl(regs_t *self);
static unsigned long regs_get_rip_impl(regs_t *self);
static void regs_set_rip_impl(regs_t *self, unsigned long rip);
static void regs_destroy_impl(regs_t **self_ptr);

/*
==================================================
CONSTRUCTOR & DESTRUCTOR
==================================================
*/
regs_t *regs_create(pid_t pid) {
    OUTPUT_D_MSG("regs_create : Attempting to create a new register inspector...");
    
    // ~ Create a new register inspector
    regs_t *new = NULL;
    
    // ~ Allocate memory + error check
    new = (regs_t *)calloc(1, sizeof(regs_t));
    if (new == NULL) {
        perror("\n[ERROR]:regs_create");
        return NULL;
    }
    
    // ~ Initialize fields with default values
    new->pid = pid;
    // ~ gp_regs will be filled by fetch()
    
    // ~ Wire up function pointers
    new->fetch = regs_fetch_impl;
    new->dump = regs_dump_impl;
    new->get_rip = regs_get_rip_impl;
    new->set_rip = regs_set_rip_impl;
    new->destroy = regs_destroy_impl;
    
    OUTPUT_D_MSG("regs_create : Successfully created register inspector!");
    return new;
}

static void regs_destroy_impl(regs_t **self_ptr) {
    regs_t *self = *self_ptr;
    
    OUTPUT_D_MSG("regs_destroy : Register inspector being destroyed...");
    
    // ~ No owned sub-allocations to free (gp_regs is part of struct)
    
    // ~ Deallocate this entire memory block
    free(self);
    *self_ptr = NULL;
    
    OUTPUT_D_MSG("regs_destroy : Register inspector destroyed successfully!");
}

/*
==================================================
METHOD IMPLEMENTATIONS
==================================================
*/
static void regs_fetch_impl(regs_t *self) {
    does_exist(self);
    OUTPUT_D_MSG("regs_fetch : Attempting to fetch registers from tracee...");
    
    // ~ Call PTRACE_GETREGS to read all general-purpose registers
    // ~ This fills the gp_regs struct with the current CPU state
    if (ptrace(PTRACE_GETREGS, self->pid, NULL, &self->gp_regs) == -1) {
        perror("\n[ERROR]:regs_fetch : PTRACE_GETREGS failed");
        return;
    }
    
    OUTPUT_D_MSG("regs_fetch : Successfully fetched registers!");
}

static void regs_dump_impl(regs_t *self) {
    does_exist(self);
    OUTPUT_D_MSG("regs_dump : Attempting to dump register contents...");
    
    // ~ Fetch the current register values first
    self->fetch(self);
    
    // ~ Print registers in a formatted layout similar to GDB
    // ~ This shows the complete CPU state at the moment of stopping
    printf("\n=== CPU Registers ===\n");
    printf("RIP: 0x%016llx    RSP: 0x%016llx\n", self->gp_regs.rip, self->gp_regs.rsp);
    printf("RAX: 0x%016llx    RBX: 0x%016llx\n", self->gp_regs.rax, self->gp_regs.rbx);
    printf("RCX: 0x%016llx    RDX: 0x%016llx\n", self->gp_regs.rcx, self->gp_regs.rdx);
    printf("RSI: 0x%016llx    RDI: 0x%016llx\n", self->gp_regs.rsi, self->gp_regs.rdi);
    printf("RBP: 0x%016llx    R8:  0x%016llx\n", self->gp_regs.rbp, self->gp_regs.r8);
    printf("R9:  0x%016llx    R10: 0x%016llx\n", self->gp_regs.r9, self->gp_regs.r10);
    printf("R11: 0x%016llx    R12: 0x%016llx\n", self->gp_regs.r11, self->gp_regs.r12);
    printf("R13: 0x%016llx    R14: 0x%016llx\n", self->gp_regs.r13, self->gp_regs.r14);
    printf("R15: 0x%016llx    RFLAGS: 0x%08llx\n", self->gp_regs.r15, self->gp_regs.eflags);
    printf("=====================\n");
    
    OUTPUT_D_MSG("regs_dump : Successfully dumped register contents!");
}

static unsigned long regs_get_rip_impl(regs_t *self) {
    if (self == NULL) {
        perror("\n[ERROR]:regs_get_rip : self is NULL");
        return 0;
    }
    OUTPUT_D_MSG("regs_get_rip : Attempting to get instruction pointer...");
    
    // ~ Return the current RIP value from the cached register struct
    // ~ This is much faster than calling ptrace every time
    unsigned long rip = self->gp_regs.rip;
    
    OUTPUT_D_MSG("regs_get_rip : Successfully retrieved instruction pointer!");
    return rip;
}

static void regs_set_rip_impl(regs_t *self, unsigned long rip) {
    does_exist(self);
    OUTPUT_D_MSG("regs_set_rip : Attempting to set instruction pointer...");
    
    // ~ Modify the RIP value in the cached struct
    self->gp_regs.rip = rip;
    
    // ~ Write the modified register struct back to the tracee
    // ~ This is essential for breakpoint handling (backing up RIP by 1)
    if (ptrace(PTRACE_SETREGS, self->pid, NULL, &self->gp_regs) == -1) {
        perror("\n[ERROR]:regs_set_rip : PTRACE_SETREGS failed");
        return;
    }
    
    OUTPUT_D_MSG("regs_set_rip : Successfully set instruction pointer!");
}