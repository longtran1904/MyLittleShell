#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#define BUFSIZE 512
#define RESET   "\033[0m"
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */

#ifndef DEBUG
    #define DEBUG 1
#endif

char *lineBuffer;
int linePos = 0, lineSize = 1;
int input_fd, output_fd;

void append(char *, int);
void printCommand(int);

void writeToOutput(){
    int pos;
    char buffer[BUFSIZE];

    int readBytes, lstart = 0; // lstart to calculate len of each line
    while ((readBytes = read(input_fd, buffer, BUFSIZE)) > 0){
        if (DEBUG) printf(YELLOW "Read %d bytes!!" RESET "\n", readBytes);
        // If a new line, process the line buffer
        lstart = 0;
        for (pos = 0; pos < readBytes; pos++){
            if (buffer[pos] == '\n'){
                int thislen = pos - lstart + 1;
                if (DEBUG) fprintf(stderr,GREEN "stream pos: %d | lstart: %d | finished line %d+%d bytes\n" RESET, pos, lstart, linePos, thislen);
                append(buffer + lstart, thislen);
                printCommand(output_fd);
                lstart = pos + 1;
                linePos = 0;
            }
        }
        if (lstart < readBytes){ // if partial line at the end of readBytes
            int thisLen = readBytes - lstart;
            if (DEBUG) fprintf(stderr, YELLOW "partial line %d+%d bytes" RESET "\n", linePos, thisLen);
            append(buffer + lstart, thisLen);
        }
    }
    if (linePos > 0){
        // file ended with partial line
        append("\n",1);
        printCommand(output_fd);
        linePos = 0;
    }
    printf("Program finished reading\n");
}

// add specified text the line buffer, expanding as necessary
// assumes we are adding at least one byte
void append(char *buf, int len){

    int newPos = linePos + len;
    int isExpanded = 0;
    while (newPos > lineSize){
        isExpanded = 1;
        lineSize *= 2;
        if (DEBUG) fprintf(stderr, YELLOW "expanding line buffer to %d" RESET "\n", lineSize);
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
    if (writeBytes == -1) printf(RED "Didn't write any byte!!!" RESET "\n");
    else if (DEBUG) printf(YELLOW "Wrote %d bytes!!" RESET "\n", writeBytes);
}

void setupInputOutput(int argc, char** argv){
    if (argc <= 1)
    {
        //set stdin stdout to terminal
        input_fd = 0;
        output_fd = 1;
        if (isatty(input_fd)) // if input_fd is pointing to terminal
            printf("[Reading from terminal]\n"); 
    }
    else
    {
        //set stdin stdout to argv[1] and "output.txt"
        char* dname = argv[1];
        // open dir
        int fd;
        fd = open(dname, O_RDONLY);
        if (fd == -1){
            perror(dname);
            exit(EXIT_FAILURE);
        } else {
            input_fd = fd;
            if (DEBUG) printf(YELLOW "Successfully opened %s" RESET "\n", dname); 
        }
        // open output
        fd = open("output.txt", O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
        if (fd == -1){
            perror("output.txt");
            exit(EXIT_FAILURE);
        } else {
            output_fd = fd;
            if (DEBUG) printf(YELLOW "Successfully opened output.txt" RESET "\n");
        }
    }
}

int main(int argc, char** argv){

    setupInputOutput(argc, argv);
    writeToOutput();
    close(input_fd);
    close(output_fd);   
    return EXIT_SUCCESS;
}