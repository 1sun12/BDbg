/*
==================================================
HEADERS
==================================================
*/
#include "proc.h"

/*
==================================================
INTERNAL FUNCTION PROTOTYPES
==================================================
*/
static void proc_launch_impl(proc_t *self);
static void proc_cont_impl(proc_t *self);
static void proc_single_step_impl(proc_t *self);
static void proc_wait_for_signal_impl(proc_t *self);
static void proc_destroy_impl(proc_t **self_ptr);

/*
==================================================
CONSTRUCTOR & DESTRUCTOR
==================================================
*/
proc_t *proc_create(const char *target_path) {
    OUTPUT_D_MSG("proc_create : Attempting to create a new process manager...");
    
    // ~ Create a new process manager
    proc_t *new = NULL;
    
    // ~ Allocate memory + error check
    new = (proc_t *)calloc(1, sizeof(proc_t));
    if (new == NULL) {
        perror("\n[ERROR]:proc_create");
        return NULL;
    }
    
    // ~ Initialize fields with default values
    new->child_pid = 0;
    new->is_running = 0;
    new->has_exited = 0;
    
    // ~ Copy target path - this is owned memory, so we need to allocate and copy
    if (target_path != NULL) {
        new->target_path = strdup(target_path);
        if (new->target_path == NULL) {
            perror("\n[ERROR]:proc_create : strdup failed");
            free(new);
            return NULL;
        }
    } else {
        new->target_path = NULL;
    }
    
    // ~ Wire up function pointers
    new->launch = proc_launch_impl;
    new->cont = proc_cont_impl;
    new->single_step = proc_single_step_impl;
    new->wait_for_signal = proc_wait_for_signal_impl;
    new->destroy = proc_destroy_impl;
    
    OUTPUT_D_MSG("proc_create : Successfully created process manager!");
    return new;
}

static void proc_destroy_impl(proc_t **self_ptr) {
    proc_t *self = *self_ptr;
    
    OUTPUT_D_MSG("proc_destroy : Process manager being destroyed...");
    
    // ~ Free any owned sub-allocations first
    free(self->target_path);
    self->target_path = NULL;
    
    // ~ Deallocate this entire memory block
    free(self);
    *self_ptr = NULL;
    
    OUTPUT_D_MSG("proc_destroy : Process manager destroyed successfully!");
}

/*
==================================================
METHOD IMPLEMENTATIONS
==================================================
*/
static void proc_launch_impl(proc_t *self) {
    does_exist(self);
    OUTPUT_D_MSG("proc_launch : Attempting to launch target process...");
    
    // ~ Fork the process - this creates two copies: parent (debugger) and child (tracee)
    pid_t pid = fork();
    if (pid == -1) {
        perror("\n[ERROR]:proc_launch : fork failed");
        return;
    }
    
    if (pid == 0) {
        // ~ CHILD PROCESS: This will become the tracee (target program)
        
        // ~ Call ptrace(PTRACE_TRACEME) to consent to being traced by parent
        // ~ This MUST be called before execvp, otherwise the parent will hang forever
        if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1) {
            perror("\n[ERROR]:proc_launch : PTRACE_TRACEME failed");
            exit(EXIT_FAILURE);
        }
        
        // ~ Execute the target program
        // ~ execvp replaces the child process image with the target program
        // ~ The TRACEME flag survives in the kernel's task_struct
        char *args[] = {self->target_path, NULL};
        if (execvp(self->target_path, args) == -1) {
            perror("\n[ERROR]:proc_launch : execvp failed");
            exit(EXIT_FAILURE);
        }
        
        // ~ execvp only returns on error
    } else {
        // ~ PARENT PROCESS: This is the debugger
        
        // ~ Store the child PID
        self->child_pid = pid;
        
        // ~ Wait for the child to stop
        // ~ After execvp, the OS sends SIGTRAP to stop the child for debugging
        self->wait_for_signal(self);
        
        // ~ Child is now stopped and under our control
        self->is_running = 0;
        printf("Target process launched and stopped (PID: %d). Ready for debugging!\n", self->child_pid);
    }
    
    OUTPUT_D_MSG("proc_launch : Successfully launched target process!");
}

static void proc_cont_impl(proc_t *self) {
    does_exist(self);
    OUTPUT_D_MSG("proc_cont : Attempting to continue execution...");
    
    // ~ Send PTRACE_CONT to resume the tracee's execution
    // ~ This tells the OS "let this process run until the next signal/breakpoint"
    if (ptrace(PTRACE_CONT, self->child_pid, NULL, NULL) == -1) {
        perror("\n[ERROR]:proc_cont : PTRACE_CONT failed");
        return;
    }
    
    // ~ Wait for the tracee to stop again
    self->wait_for_signal(self);
    
    OUTPUT_D_MSG("proc_cont : Successfully continued execution!");
}

static void proc_single_step_impl(proc_t *self) {
    does_exist(self);
    OUTPUT_D_MSG("proc_single_step : Attempting to single step...");
    
    // ~ Send PTRACE_SINGLESTEP to execute exactly one instruction
    // ~ This is the heart of instruction-level debugging
    if (ptrace(PTRACE_SINGLESTEP, self->child_pid, NULL, NULL) == -1) {
        perror("\n[ERROR]:proc_single_step : PTRACE_SINGLESTEP failed");
        return;
    }
    
    // ~ Wait for the tracee to stop after the single instruction
    self->wait_for_signal(self);
    
    OUTPUT_D_MSG("proc_single_step : Successfully stepped one instruction!");
}

static void proc_wait_for_signal_impl(proc_t *self) {
    does_exist(self);
    OUTPUT_D_MSG("proc_wait_for_signal : Waiting for child process signal...");
    
    int status;
    
    // ~ Call waitpid to block until the child process changes state
    // ~ waitpid returns the child's status which tells us WHY it stopped
    if (waitpid(self->child_pid, &status, 0) == -1) {
        perror("\n[ERROR]:proc_wait_for_signal : waitpid failed");
        return;
    }
    
    // ~ Check the different possible states of the child process
    if (WIFEXITED(status)) {
        // ~ Child called exit() or returned from main normally
        int exit_code = WEXITSTATUS(status);
        printf("Target process exited with code: %d\n", exit_code);
        self->has_exited = 1;
        self->is_running = 0;
    } else if (WIFSTOPPED(status)) {
        // ~ Child was stopped by a signal
        int signal = WSTOPSIG(status);
        printf("Target process stopped by signal: %d\n", signal);
        
        if (signal == SIGTRAP) {
            printf("  (SIGTRAP: Breakpoint hit or single-step completed)\n");
            // ~ Return a special value to indicate this was a SIGTRAP
            // ~ Main will handle the breakpoint logic (RIP backup, etc.)
            self->is_running = 0;
            return; // ~ Return early to let main handle breakpoint logic
        } else if (signal == SIGSEGV) {
            printf("  (SIGSEGV: Segmentation fault - invalid memory access)\n");
        }
        
        self->is_running = 0;
    } else if (WIFSIGNALED(status)) {
        // ~ Child was killed by a signal (not stopped)
        int signal = WTERMSIG(status);
        printf("Target process killed by signal: %d\n", signal);
        self->has_exited = 1;
        self->is_running = 0;
    }
    
    OUTPUT_D_MSG("proc_wait_for_signal : Successfully processed child signal!");
}