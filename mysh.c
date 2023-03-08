#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#define BUFSIZE 512

#ifndef DEBUG
    #define DEBUG 1
#endif

char *lineBuffer;
int linePos = 0, lineSize = 1;

void append(char *, int);
void printCommand(int);

void writeToOutput(char* dname){
    int pos;
    char buffer[BUFSIZE];

    // open dir
    int fd = open(dname, O_RDONLY);
    if (fd == -1){
        perror(dname);
        exit(EXIT_FAILURE);
    }
    // open output
    int output_fd = open("output.txt", O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
    if (output_fd == -1){
        perror("output.txt");
        exit(EXIT_FAILURE);
    } else {
        if (DEBUG) printf("Successfully opened output.txt\n");
    }
    int readBytes, lstart = 0; // lstart to calculate len of each line
    while ((readBytes = read(fd, buffer, BUFSIZE)) > 0){
        // If a new line, process the line buffer
        for (pos = 0; pos < readBytes; pos++){
            if (buffer[pos] == '\n'){
                int thislen = pos - lstart + 1;
                append(buffer + lstart, thislen);
                if (DEBUG) {
                    for (int i = 0; i < linePos; i++)
                        printf("%c", lineBuffer[i]);
                }
                printCommand(output_fd);
                lstart = pos + 1;
                linePos = 0;
            }
        }
        if (lstart < readBytes){ // if partial line at the end of readBytes
            int thisLen = readBytes - lstart;
            if (DEBUG) fprintf(stderr, "partial line %d+%d bytes\n", linePos, thisLen);
            append(buffer + lstart, thisLen);
        }
    }
    if (linePos > 0){
        // file ended with partial line
        append("\n",1);
        printCommand(output_fd);
        linePos = 0;
    }
    close(fd);
    close(output_fd);
}

// add specified text the line buffer, expanding as necessary
// assumes we are adding at least one byte
void append(char *buf, int len){

    int newPos = linePos + len;
    int isExpanded = 0;
    while (newPos > lineSize){
        isExpanded = 1;
        lineSize *= 2;
        if (DEBUG) fprintf(stderr, "expanding line buffer to %d\n", lineSize);
    }   
    if (isExpanded) {
        // realloc buffer if resized
        lineBuffer = realloc(lineBuffer, lineSize);
        if (lineBuffer == NULL){
            perror("line buffer");
            exit(EXIT_FAILURE);
        }
    }

    memcpy(lineBuffer + linePos, buf, len);
    linePos = newPos;
}

// Print each command from buffer to file fd
void printCommand(int fd) {
    int writeBytes = write(fd, lineBuffer, linePos);
    if (writeBytes == -1) printf("Didn't write any byte!!!\n");
    else printf("Wrote %d bytes!!\n", writeBytes);
}

void interactive(){
    printf("Need Implemening");
}

void batch(char* dname){

    writeToOutput(dname);
    printf("Exit batch mode\n");
}

int main(int argc, char** argv){

    if (argc <= 1)
        interactive();
    else
    {
        char* dname = argv[1];
        batch(dname);
    }

    return EXIT_SUCCESS;
}