# BDbg

A Linux process debugger written in C. Attaches to a running process via ptrace; reads memory, inspects registers, and steps through execution. Built because the best way to understand a debugger is to write one from scratch.

## What It Does

BDbg forks a child process, calls `execve` to load the target, and uses `ptrace` to take control. The parent loops on `waitpid`, interprets the results, and acts accordingly. That's the core loop. Everything else builds on top of it.

**Current capabilities:**
- Launch and attach to a target process via `fork` + `execve` + `PTRACE_TRACEME`
- Set and restore software breakpoints (`INT3` injection)
- Read and display CPU registers (RIP, RSP, RAX, etc.)
- Read memory at arbitrary addresses
- Single-step through instructions
- Continue execution until the next breakpoint or exit
- Simple command loop for interacting with the target

## Architecture

The entire project follows an OOP-in-C pattern: typedef structs with function pointers wired up in constructors, double-pointer-null on destroy, `calloc` over `malloc`. Every module is self-contained with its own create/destroy lifecycle.

```
bdbg/
├── src/
│   ├── main.c       # Entry point, fork/exec, command loop
│   ├── wait.c       # waitpid abstraction layer
│   ├── trace.c      # ptrace abstraction layer
│   ├── bp.c         # Breakpoint management (INT3 inject/restore)
│   ├── regs.c       # Register inspection (PTRACE_GETREGS)
│   ├── mem.c        # Memory read/write (PTRACE_PEEKDATA/POKEDATA)
│   └── cmd.c        # Command parsing and dispatch
├── include/
│   ├── wait.h
│   ├── trace.h
│   ├── bp.h
│   ├── regs.h
│   ├── mem.h
│   └── cmd.h
├── obj/
├── bin/
└── Makefile
```

### The Wait Layer

`wait.h` / `wait.c` wrap `waitpid` into a clean interface. The status integer that comes back from `waitpid` is bit-packed and not meant to be read directly, so the wait layer decodes it using `WIFEXITED`, `WIFSTOPPED`, `WIFSIGNALED`, and their companion macros. Options are handled through an enum (`WAIT_BLOCK`, `WAIT_NO_HANG`, `WAIT_UNTRACED`, `WAIT_CONTINUED`) so you can always look up what's available just by reading the definition.

### The Trace Layer

`trace.h` / `trace.c` abstract the `ptrace` system call. Every ptrace request type gets a clean function instead of raw `ptrace(PTRACE_SOMETHING, ...)` calls scattered everywhere.

## Build

```bash
make
```

## Usage

```bash
./bin/bdbg <target_program>
```

Once attached, BDbg drops into a command prompt. From there you can set breakpoints, step, continue, inspect registers, or read memory.

## On the Horizon

- Partial x86-64 disassembler built by hand from the Intel Software Developer's Manual. No Capstone, no external disassembly libraries. Decode the opcodes directly.
- Live memory scanning and patching (think Cheat Engine, but in the terminal)
- Expanded breakpoint management (listing, enabling, disabling)

## Why This Project

Debuggers sit at the intersection of everything that matters for CNO work: process internals, memory layout, instruction execution, signal handling, and the same ptrace mechanisms used by anti-debugging techniques, process injection, and runtime analysis tools. If you can build one, you can take one apart. That's the point.

## License

MIT
