#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/stat.h>
// Header Declarations
#define BUFSIZE 512
#ifndef DEBUG
    #define DEBUG 0
#endif

void changeDir(char *path){
    int res = chdir(path);
    if (res==-1) perror("cd failed!");
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

void execProgram(char* prog_name, char** prog_args){
    // Create a child process
    // Search through all directories for prog_name:
    // PATH evironments
    char *PATH[6] = {
        "/usr/local/sbin", 
        "/usr/local/bin",
        "/usr/sbin",
        "/usr/bin",
        "/sbin",
        "/bin"
    };
    int path_count = 6;

    struct stat stat_buf;
    for (int i = 0; i < path_count; i++)
    {    
        char* filePath = strcat(PATH[i], strcat("/", prog_name));
        if (stat(filePath, &stat_buf) == 0) // success
        {
            printf("stat() found file %s\nat dir: %s\n",prog_name, PATH[i]);
            int pid = fork();
            if (pid == -1) { 
                perror("fork failed");
            }
            if (pid == 0) {
                // we are in the child process

                execv(prog_name, prog_args);
                exit(EXIT_FAILURE);
            }
            int wstatus;
            int tpid = wait(&wstatus); // wait for child to finish
            if (tpid == -1) // wait failed
            {
                perror("wait failed");
            }    

            if (WIFEXITED(wstatus)){
                // child exited normally
                printf("child exited with %d\n", WEXITSTATUS(wstatus));
            }
        }
    }
}

// given a program path, execute it
void execProgGivenPath(char *prog_path, char **prog_args){
    int pid = fork();
    if (pid == -1) { 
	perror("fork failed");
    }
    if (pid == 0) {
	// we are in the child process
	execv(prog_path, prog_args);
	exit(EXIT_FAILURE);
    }
    int wstatus;
    int tpid = wait(&wstatus); // wait for child to finish
    if (tpid == -1) // wait failed
    {
	perror("wait failed");
    }    

    if (WIFEXITED(wstatus)){
	// child exited normally
	printf("child exited with %d\n", WEXITSTATUS(wstatus));
    }
}

void execute(char **commands, int len) {
    if (commands == NULL) return;

    // Implemented for single command so far
    // TODO: split arguments corresponding each command and execute
    if (commands[0] != NULL && *commands[0] == '/'){
	// run program in path given
	char** args = malloc(strlen(commands[1]));
	args[0] = commands[1];
	execProgGivenPath(commands[0], &commands[1]);
    }
    else {
	if (strcmp(commands[0], "pwd") == 0){
	    char buffer[BUFSIZE];
	    int res = getCurDir(buffer);
	    if (res == 0){  
		printf("%s\n", buffer);
	    }
	    else {
		printf("pwd failed\n");
	    }
	    return;
	}
	if (strcmp(commands[0],"cd") == 0){
	    changeDir(commands[1]);
	    return;
	}

	char** args = malloc(strlen(commands[1]));
	args[0] = commands[1];
	execProgram(commands[0], args);
    }


}
