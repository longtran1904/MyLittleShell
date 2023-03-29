#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/stat.h>
#include<stdbool.h>

#define RESET   "\033[0m"
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */

#define BUFSIZE 512
#ifndef DEBUG
    #define DEBUG 0
#endif

void changeDir(char *path){
    int res = chdir(path);
    if (res==-1) perror("cd failed");
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

void execProgram(char** prog_args){
    // Create a child process
    // Search through all directories for prog_name:
    // PATH evironments

    bool isPathGiven = false;
    if (*prog_args[0] == '/') isPathGiven = true;

    // Infos about the filePath, and stat_buf (using stat())
    struct stat stat_buf;
    char* filePath = NULL;

    if (isPathGiven)
        filePath = prog_args[0]; // take program path from command
    else
    {
        // look for program based on bare names from command
        char *PATH[6] = {
            "/usr/local/sbin/", 
            "/usr/local/bin/",
            "/usr/sbin/",
            "/usr/bin/",
            "/sbin/",
            "/bin/"
        };
        int path_count = 6;
        char* tryPath;
        int pathSize;

        for (int i = 0; i < path_count; i++)
        {   
            // concatenate PATH[i] and prog_name
            pathSize = strlen(PATH[i]) + strlen(prog_args[0]) + 1;
            pathSize = ((pathSize + 7)/8)  * 8;

            //assign and try paths
            
            tryPath = (char*) malloc(pathSize * sizeof(char));
            strcpy(tryPath, PATH[i]);
            strcat(tryPath, prog_args[0]); 

            if (stat(tryPath, &stat_buf) == 0) // found program
            {
                if (DEBUG) {
                    printf(YELLOW "stat() found file %s at dir: %s" RESET "\n", prog_args[0], tryPath);
                    int i = 0;
                    printf(YELLOW "args:");
                    while (prog_args[i] != NULL){
                        printf("[%s]   ", prog_args[i]);
                        i++;
                    }
                    printf(RESET "\n");
                }
                filePath = (char*) malloc(pathSize);
                memcpy(filePath, tryPath, pathSize);
                break; // stop searching
            }
            free(tryPath);
        }
    }

    if (filePath == NULL) {
        // program not found
        printf("Program %s not found!\n", prog_args[0]);
    }
    else
    {
        // Run program in filePath
        int pid = fork();
        if (pid == -1) { 
            perror("fork failed");
        }
        if (pid == 0) {
            // we are in the child process
            if (execv(filePath, prog_args) < 0){
                perror("execv failed");
                exit(0);
            }

            // if we reach here, something went wrong
            exit(EXIT_FAILURE);
        }
        else {
            int wstatus;
            int tpid = wait(&wstatus); // wait for child to finish
            if (tpid == -1) // wait failed
            {
                perror("wait failed");
            }    

            if (WIFEXITED(wstatus) && DEBUG){
                // child exited normally
                printf("child exited with %d\n", WEXITSTATUS(wstatus));
            }
        }
    }
    if (!isPathGiven) free(filePath);
}

void execute(char ***commands, int len) {
    if (commands == NULL) return;

    // Implemented for single command so far
    // TODO: split arguments corresponding each command and execute
	if (strcmp(**commands, "pwd") == 0){
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
	if (strcmp(**commands, "cd") == 0){
	    changeDir(*(*commands+1));
	    return;
	}
    execProgram(commands[0]);
}
