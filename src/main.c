/*
==================================================
HEADERS
==================================================
*/
#include "debug.h"
#include "proc.h"
#include "cmd.h"
#include "regs.h"
#include "mem.h"
#include "bp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/*
==================================================
MAIN
==================================================
*/
int main(int argc, char **argv) {
    OUTPUT_D_MSG("~ MAIN EXECUTION STARTING ~");
    
    // ~ Check command line arguments
    if (argc < 2) {
        printf("Usage: %s <target_program>\n", argv[0]);
        printf("Example: %s ./test_target\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    printf("BDbg starting...\n");
    
    // ~ Create a process manager for debugging the target program
    proc_t *proc = proc_create(argv[1]);
    if (proc == NULL) {
        exit_with_error("Failed to create process manager");
    }
    
    // ~ Create a command parser
    cmd_t *cmd = cmd_create();
    if (cmd == NULL) {
        exit_with_error("Failed to create command parser");
    }
    
    // ~ Create a register inspector for the target process
    regs_t *regs = regs_create(0); // ~ Will be set to actual PID after launch
    if (regs == NULL) {
        exit_with_error("Failed to create register inspector");
    }
    
    // ~ Create a memory accessor for the target process
    mem_t *mem = mem_create(0); // ~ Will be set to actual PID after launch
    if (mem == NULL) {
        exit_with_error("Failed to create memory accessor");
    }
    
    // ~ Create a breakpoint manager for the target process
    bp_t *bp = bp_create(0); // ~ Will be set to actual PID after launch
    if (bp == NULL) {
        exit_with_error("Failed to create breakpoint manager");
    }
    
    // ~ Launch the target process
    proc->launch(proc);
    regs->pid = proc->child_pid; // ~ Set the actual PID after launch
    mem->pid = proc->child_pid; // ~ Set the actual PID after launch
    bp->pid = proc->child_pid; // ~ Set the actual PID after launch
    
    // ~ Variable to track if we have an active breakpoint
    int has_breakpoint = 0;
    
    // ~ Main command loop
    while (cmd->running && !proc->has_exited) {
        // ~ Read user input
        cmd->read_input(cmd);
        
        // ~ Parse the input
        cmd->parse(cmd);
        
        // ~ Dispatch commands (add "break" for Step 7)
        if (strcmp(cmd->command, "cont") == 0) {
            printf("Continuing execution...\n");
            
            // ~ If we have an active breakpoint, we need special handling
            if (has_breakpoint) {
                // ~ Disable breakpoint to restore original instruction
                bp->disable(bp);
                
                // ~ Back up RIP by 1 because INT3 (0xCC) is 1 byte
                // ~ and RIP has already advanced past it when SIGTRAP fires
                unsigned long current_rip = regs->get_rip(regs);
                regs->set_rip(regs, current_rip - 1);
                
                // ~ Single step one instruction to execute the original instruction
                proc->single_step(proc);
                
                // ~ Re-enable the breakpoint for future hits
                bp->enable(bp, bp->addr);
            }
            
            proc->cont(proc);
        } else if (strcmp(cmd->command, "regs") == 0) {
            regs->dump(regs);
        } else if (strcmp(cmd->command, "mem") == 0) {
            // ~ Parse the address argument for the mem command
            if (strlen(cmd->argument) == 0) {
                printf("Usage: mem <address>\n");
                printf("Example: mem 0x401000\n");
            } else {
                // ~ Convert hex string to unsigned long
                unsigned long addr = strtoul(cmd->argument, NULL, 16);
                mem->dump(mem, addr, 64); // ~ Dump 64 bytes by default
            }
        } else if (strcmp(cmd->command, "break") == 0) {
            // ~ Parse the address argument for the break command
            if (strlen(cmd->argument) == 0) {
                printf("Usage: break <address>\n");
                printf("Example: break 0x401000\n");
            } else {
                // ~ Convert hex string to unsigned long
                unsigned long addr = strtoul(cmd->argument, NULL, 16);
                if (bp->enable(bp, addr) == 0) {
                    printf("Breakpoint successfully set\n");
                    has_breakpoint = 1; // ~ Mark that we have an active breakpoint
                } else {
                    printf("Failed to set breakpoint\n");
                }
            }
        } else if (strcmp(cmd->command, "step") == 0) {
            printf("Single stepping...\n");
            proc->single_step(proc);
        } else if (strcmp(cmd->command, "quit") == 0) {
            printf("Quitting debugger...\n");
            cmd->running = 0;
        } else if (strlen(cmd->command) > 0) {
            printf("Unknown command: %s\n", cmd->command);
            printf("Available commands: cont, step, regs, mem <addr>, break <addr>, quit\n");
        }
    }
    
    // ~ Cleanup
    bp->destroy(&bp);
    mem->destroy(&mem);
    regs->destroy(&regs);
    cmd->destroy(&cmd);
    proc->destroy(&proc);
    
    printf("\nGoodbye!\n");
    return 0;
}