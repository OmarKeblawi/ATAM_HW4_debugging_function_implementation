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
pid_t run_target(const char* programname) 
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
		execl(programname, programname, NULL);
		
	} else {
		// fork error
		perror("fork");
        exit(1);
    }
}

int main(int argc, char** argv)
{
    pid_t child_pid;

    child_pid = run_target(/*TODO: Complete the correct parameter for the debugger to work*/);
	
	// TODO: run specific "debugger"

    return 0;
}