// Online C compiler to run C program online
#include <stdio.h>

pid_t run_target(char **argv){
    pid_t pid = fork();
    if(pid > 0){
        // parent process
        return pid;
    }
    else if(pid == 0){
        // child process
        execv(argv[0], argv);
    }
    else{
        // fork failed
        perror("fork");
        exit(1);
    }
}
int main(int argc, char **argv){
    if(argc < 3){
        printf("PRF:: wrong number of inputs\n");
        return 1;
    }
    // get the address.
    long fun_address = strtol(argv[1], NULL , 16);

    pid_t child_pid;

    child_pid = run_target(&argv[2]);
    
    
    
    
    return 0;
}
