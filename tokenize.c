#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>	
#include <string.h>

#ifndef DEBUG
#define DEBUG 0
#endif
#define INNER_SIZE 80

// write array of words to commands, return size of commands
int tokenize(char*** commands, int commCapacity, char* string, int strsize){
    char* word = NULL;
    int wordLen = 0;
    int wordEnd = 0;
    int wordStart = 0;
    int inner_count = 0;
    int i = 0;
    int j = 0;
    commands[j] = malloc(INNER_SIZE);
    int commjCapacity = (int) (INNER_SIZE / sizeof(char**));
    while (i < strsize)
    {
        // skip space
        while (i < strsize && isspace(string[i]))
        {
	    wordStart++; 
	    wordEnd++;
            i++;
        }
        if (wordStart >= strsize) break;

        // get word len
        while (i < strsize && !isspace(string[i]) \
		&& !((string[i] == '<') || (string[i] == '>') || (string[i] == '|')))
	{
	    wordEnd = i;
	    i++;
        }
	if ((string[wordEnd] == '<') || (string[wordEnd] == '>') || (string[wordEnd] == '|')) i++;
        wordLen = wordEnd - wordStart + 1;
        word = malloc(wordLen + 1);
        word = memcpy(word, string + wordStart, wordLen);
        if (word) {
            word[wordLen] = '\0';
	    int isPipeOrRedirect = (strcmp(word, "<")==0) ||\
				   (strcmp(word, ">")==0) || (strcmp(word, "|")==0);
	    if (isPipeOrRedirect) {
		commands[j][inner_count] = NULL;

		if (j+2 >= commCapacity){
		    if (DEBUG) printf("commands not large enough\n");
		    return -1;
		}

		commands[++j] = malloc(2*sizeof(char**));
		commands[j][0] = word;
		commands[j][1] = NULL; 

		commands[++j] = malloc(INNER_SIZE);
		commjCapacity = (int) (INNER_SIZE / sizeof(char**));
		inner_count=0;
	    }
	    if (inner_count+1 > commjCapacity) { 
		commjCapacity *= 2;
		char **res = realloc(commands[j], commjCapacity*sizeof(char**)); 
		if ( res != NULL ){
		    commands[j] = res;
		    if (DEBUG) printf("resized commands[j] to %d\n", commjCapacity);
		} else {
		    if (DEBUG) printf("failed to resize commands[j]\n");
		}
	    }
	    if (!isPipeOrRedirect){
		commands[j][inner_count++] = word;
		if (i >= strsize-1) commands[j][inner_count] = NULL; 
	    }
	}

	if (DEBUG) printf("word:\"%s\"\tword-start: %d\tword-end: %d\tword-len: %d\n", word, wordStart, wordEnd, wordLen);

	wordStart = i;
	wordEnd++;
    }
    return j+1;
}
