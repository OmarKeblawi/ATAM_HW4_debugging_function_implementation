/* Code sample: using ptrace for simple tracing of a child process.
**
** Note: this was originally developed for a 32-bit x86 Linux system; some
** changes may be required to port to x86-64.
**
** Eli Bendersky (http://eli.thegreenplace.net)
** This code is in the public domain.
*/
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <signal.h>
#include <syscall.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <sys/user.h>
#include <unistd.h>
#include <errno.h>


// This is just the template from the tutorial. You can edit it (and even its function signature) if needed. 
// We do not call "run_target" in our tests at all. Use it freely as you wish, even delete it if you want. We don't care :)
pid_t run_target(char** program) 
{
	pid_t pid;
	
	pid = fork();
	
    if (pid > 0) {
		return pid;
		
    } else if (pid == 0) {
		/* Allow tracing of this process */
		if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) < 0) {
			perror("ptrace");
			exit(1);
		}
		/* Replace this process's image with the given program */
		execv(program[0], program);
	}
}

void debugger_loop(pid_t child_pid, long func_addr)
{
	int wait_status;
	waitpid(child_pid, &wait_status, 0);
	int counter = 0;
	struct user_regs_struct regs;
	//check the first instruction in the function before starting the child process:
	unsigned long orig_data = ptrace(PTRACE_PEEKTEXT, child_pid,(void*)func_addr,NULL);
	printf("PRF:: First instruction at function address 0x%lx: 0x%lx\n", func_addr, orig_data);
	//check if the first instruction is "push rbp" (0x55)
	if((orig_data & 0xFF) == 0x55){
		printf("PRF:: This function starts by pushing rbp\n");
	}

	//Set a breakpoint at the beginning of the function
	unsigned long brakpoint_data = (orig_data & 0xFFFFFFFFFFFFFF00) | 0xCC;
	ptrace(PTRACE_POKETEXT, child_pid, (void*)func_addr, (void*)brakpoint_data);
	//Continue the execution
	ptrace(PTRACE_CONT, child_pid, NULL, NULL);
	waitpid(child_pid, &wait_status, 0);

	while(WIFSTOPPED(wait_status)) {
		counter++;
		ptrace(PTRACE_GETREGS, child_pid, NULL, &regs);
		regs.rip -= 1; // Adjust RIP to point back to the breakpoint instruction
		ptrace(PTRACE_SETREGS, child_pid, NULL, &regs);
		ptrace(PTRACE_POKETEXT, child_pid, (void*)func_addr, (void*)orig_data);
		ptrace(PTRACE_SINGLESTEP, child_pid, NULL, NULL);
		wait(&wait_status);	
		if(WIFSTOPPED(wait_status)){
			//Reinsert the breakpoint
			ptrace(PTRACE_POKETEXT, child_pid, (void*)func_addr, (void*)brakpoint_data);
		}
		//continue the execution
		ptrace(PTRACE_CONT, child_pid, NULL, NULL);
		waitpid(child_pid, &wait_status, 0);
	}

	if(WIFEXITED(wait_status)){
		printf("PRF:: The function was executed %d times\n", counter);
		return;
	}
	
	

}
int main(int argc, char** argv)
{
    pid_t child_pid;

	long fun_address = strtol(argv[1], NULL , 16);

    child_pid = run_target(&argv[2]);
	
	// TODO: run specific "debugger"
	debugger_loop(child_pid, fun_address);
	

    return 0;
}