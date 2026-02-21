/**
 *      =============================
 *      ==== Ptrace Abstractions ====
 *      =============================
 *
 *      ==== Establishing Control ====
 *
 *      PTRACE_TRACEME          - Child calls this to consent to being traced.
 *      PTRACE_ATTACH           - Attach to an already running process.
 *      PTRACE_DETACH           - Detach from the tracee, letting the process run freely again.
 *      PTRACE_KILL             - "Kill the jedi children, Anakin" - Palpatine, (sortof :] )
 *
 *      ==== Execution Control ====
 *
 *      PTRACE_CONT             - Resume execution. Pass signal number in `data` or `0` for none.
 *      PTRACE_SINGLESTEP       - Execute exactly one instruction then stop again.
 *      PTRACE_SYSCALL          - Resume but stop at the next syscall entry and exit.
 *
 *      ==== Reading State ====
 *
 *      PTRACE_PEEKDATA         - Read 8-bytes (word) from tracee memory at `addr`. Return value is the data itself, not a pointer.
 *      PTRACE_GETREGS          - Copy all CPU registers into a `struct user_regs_struct` provided by the programmer.
 *      PTRACE_GETSIGINFO       - Get detailed info about the signal that caused the current stop as a `siginfo_t`. 
 *
 *      ==== Writing State ====
 *
 *      PTRACE_POKEDATA         - Write 8-bytes (word) into tracee memory at `addr`. This is how breakpoints are added or memory patching.
 *      PTRACE_SETREGS          - Write a modified `struct user_regs_struct` back into the tracee. How you re-direct execution or fake return values.
 *
 *      ==== Adv. Flags ====
 *
 *      PTRACE_0_TRACEFORK      - Auto-attach to any children the tracee forks (grand-children).
 *      PTRACE_0_TRACEEXEC      - Stop on any `execve` call (default).
 *      PTRACE_0_EXITKILL       - Kill tracee automatically if the tracer (parent) dies. 
 *      PTRACE_0_TRACESYSGOOD   - Sets bit 7 on syscall stops so you can distinguish them.
 *
 *      ==== Summarized Toolkit ====
 *
 *      PTRACE_TRACEME      =       child setup
 *      PTRACE_CONT         =       resume execution 
 *      PTRACE_PEEKDATA     =       read memory 
 *      PTRACE_POKEDATA     =       write memory / set breakpoints
 *      PTRACE_GETREGS      =       read cpu registers 
 *      PTRACE_SETREGS      =       write cpu registers 
 *      PTRACE_SINGLESTEP   =       step one instruction
 *
 *      ==== Canonical Loop of Instructions ====
 *
 *      waitpid --> (WIFSTOPPED) + (WSTOPSIG) == SIGTRAP 
 *          --> do whatever / analysis
 *          --> PTRACE_CONT
 *      waitpid again --> repeat until WIFEXITED
 */

#ifndef TRACE_H
#define TRACE_H 

typedef struct trace_t trace_t;
struct trace_t {
    
};

#endif





