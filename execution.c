#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
// Header Declarations
#define BUFSIZE 512
#ifndef DEBUG
    #define DEBUG 0
#endif

void changeDir(char *path){
    int res = chdir(path);
    if (res == 0) printf("Directory changed to %s\n", path);
    else {
        perror("Change Dir Failed");
    }
    return;
}

// sucess: return 0 and info to buf
// failed: return -1
int getCurDir(char* buf){

    if (getcwd(buf, BUFSIZE) == NULL) {
        // if getcwd gets error
        perror("getcwd() error");
        return -1;
    }
    return EXIT_SUCCESS;
};

void execute(char **commands) {
    if (commands == NULL) return;

    // Implemented for single command so far
    // TODO: split arguments corresponding each command and execute
    if (commands[0] != NULL && *commands[0] == '/'){
        // run program in path given
    }
    else {
        if (strcmp(commands[0], "pwd") == 0){
            char buffer[BUFSIZE];
            int res = getCurDir(buffer);
            if (res == 0){  
                printf("Current directory is\n%s\n", buffer);
            }
            else {
                printf("pwd failed\n");
            }
            return;
        }
        if (strcmp(commands[0],"cd") == 0){
            changeDir(commands[1]);
        }
    }


}