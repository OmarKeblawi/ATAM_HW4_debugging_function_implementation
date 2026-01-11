#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <unistd.h>
#include <errno.h>

void debugger_loop(pid_t child_pid, long func_addr) {
    int wait_status;
    int call_sequence_counter = 0; 
    int recursion_depth = 0;        
    unsigned long first_ret_addr = 0;
    unsigned long first_ret_orig_data = 0;
    int ret_breakpoint_set = 0;
    struct user_regs_struct regs;

    // Synchronize with the stop after execv
    waitpid(child_pid, &wait_status, 0);

    // Check for push rbp (0x55)
    unsigned long orig_func_data = ptrace(PTRACE_PEEKTEXT, child_pid, (void*)func_addr, NULL);
    if ((orig_func_data & 0xFF) == 0x55) {
        printf("PRF:: This function starts by pushing rbp\n");
    }

    // Set initial entry breakpoint
    unsigned long entry_trap = (orig_func_data & ~0xFF) | 0xCC;
    ptrace(PTRACE_POKETEXT, child_pid, (void*)func_addr, (void*)entry_trap);
    ptrace(PTRACE_CONT, child_pid, NULL, NULL);

    while (waitpid(child_pid, &wait_status, 0) && !WIFEXITED(wait_status)) {
        ptrace(PTRACE_GETREGS, child_pid, NULL, &regs);

        // CASE 1: Hit Entry Breakpoint (target_func start)
        if (regs.rip - 1 == func_addr) {
            if (recursion_depth == 0) {
                call_sequence_counter++;
                // Save return address for the outermost call only
                first_ret_addr = ptrace(PTRACE_PEEKTEXT, child_pid, (void*)regs.rsp, NULL);
                first_ret_orig_data = ptrace(PTRACE_PEEKTEXT, child_pid, (void*)first_ret_addr, NULL);
                
                // Set return breakpoint only for the outermost caller
                unsigned long ret_trap = (first_ret_orig_data & ~0xFF) | 0xCC;
                ptrace(PTRACE_POKETEXT, child_pid, (void*)first_ret_addr, (void*)ret_trap);
                ret_breakpoint_set = 1;
            }
            recursion_depth++;

            // Step over the entry trap
            regs.rip -= 1;
            ptrace(PTRACE_SETREGS, child_pid, NULL, &regs);
            ptrace(PTRACE_POKETEXT, child_pid, (void*)func_addr, (void*)orig_func_data);
            ptrace(PTRACE_SINGLESTEP, child_pid, NULL, NULL);
            waitpid(child_pid, &wait_status, 0); // Synchronize
            ptrace(PTRACE_POKETEXT, child_pid, (void*)func_addr, (void*)entry_trap);
            
            ptrace(PTRACE_CONT, child_pid, NULL, NULL);
        } 
        // CASE 2: Hit Return Breakpoint (The address saved in RSP)
        else if (ret_breakpoint_set && (regs.rip - 1 == first_ret_addr)) {
            
            // If we hit this, we are back at the original caller (e.g. main).
            // Recursion is definitely over.
            
            printf("PRF:: run #%d returned with %d\n", call_sequence_counter, (int)regs.rax);
            
            // Reset depth
            recursion_depth = 0; 
            
            // Final cleanup of the return trap
            regs.rip -= 1;
            ptrace(PTRACE_SETREGS, child_pid, NULL, &regs);
            ptrace(PTRACE_POKETEXT, child_pid, (void*)first_ret_addr, (void*)first_ret_orig_data);
            ret_breakpoint_set = 0;
            
            ptrace(PTRACE_CONT, child_pid, NULL, NULL);
        }
        else {
            // Unexpected stop (maybe signal), continue
            ptrace(PTRACE_CONT, child_pid, NULL, NULL);
        }
    }
}

int main(int argc, char** argv) {
    if (argc < 3) return 1;
    long fun_address = strtol(argv[1], NULL, 16);
    pid_t pid = fork();
    if (pid == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execv(argv[2], &argv[2]);
    } else {
        debugger_loop(pid, fun_address);
    }
    return 0;
}