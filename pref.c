// Online C compiler to run C program online
#include <stdio.h>

int main(int argc, char **argv){
    if(argc < 3){
        printf("PRF:: wrong number of inputs\n");
        return 1;
    }
    long fun_address = strtol(argv[1], NULL , 16);
    char* target_prog = argv[2];
    
    
    
    
    return 0;
}
