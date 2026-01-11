Sherlock Function Debugger (HW4)
Project Overview
This project, developed for the Computer Organization and Programming course at the Technion, is a specialized debugger ("Sherlock") that traces the execution of a target function within a program. It uses the Linux ptrace system call to monitor function calls, detect specific assembly prologues, and capture return values—even in complex recursive and multi-recursive scenarios.

Features
Prologue Detection: Before tracing begins, Sherlock checks if the target function starts with the push %rbp instruction (opcode 0x55) and reports it once.

Recursive Sequence Tracking: Correcty handles recursive and mutual recursive chains (e.g., foo -> bar -> foo).

Execution Counting: Increments a counter for each unique call sequence to the target function.

Return Value Capture: Intercepts and prints the int return value (from the eax register) only for the original (outermost) call in a sequence.

Repository Structure
Based on the current development state:

debugger_template.c: The main source code for the Sherlock debugger.

Tests/: A directory containing various test cases:

test1_standard.c: Basic non-recursive function testing.

test2_recursion.c: Standard recursion testing (e.g., factorial).

test3_mutual.c / test5_mutual2.c: Multi-recursive chain testing.

test4_no_prologue.s: Assembly test for functions without a standard prologue.

match.c / no_match.s: Initial functional testing for breakpoint logic.

Usage
Compilation
The project must be compiled using the C99 standard:

Running a Test
To trace a function, you must provide its hexadecimal address (starting with 0x) and the target executable with its arguments:

Example
Tracing a function at address 0x400497:

Technical Implementation Notes
Position Independent Code (PIE): For accurate address tracing via objdump, test programs should be compiled with the -no-pie flag.

Control Flow Protection: To ensure the 0x55 opcode is at the very beginning of the function, use -fcf-protection=none during test compilation.

Breakpoint Logic: The debugger utilizes software breakpoints (INT3 / 0xCC) and carefully manages the Instruction Pointer (RIP) and Stack Pointer (RSP) to handle recursion depth.
