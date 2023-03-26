#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
// Header Declarations
#define BUFSIZE 512
#define DEBUG 1
char* getCurrentDir();
void changeDir();

char* getCurrentDir(){
    char buf[BUFSIZE];

    if (getcwd(buf, BUFSIZE) == NULL) {
        // if getcwd gets error
        perror("getcwd() error");
    }
    if (DEBUG) printf("Current directory is\n %s\n", buf);
    return buf;
};

// Test execution.c
int main() {
    char* cwd = getCurrentDir();
}
