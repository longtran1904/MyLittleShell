#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>	
#include <string.h>

#ifndef DEBUG
    #define DEBUG 0
#endif
#define BUFSIZE 512

// write array of words to commands, return size of commands
int *tokenize(char*** commands, char* string, int strsize, int *len){
    int* sizes = malloc(sizeof(int)*100);
    sizes[0] = 0;
    int sizes_len = 100;
    char* word = NULL;
    int wordLen = 0;
    int wordEnd = 0;
    int wordStart = 0;
    int inner_count = 0;
    int i = 0;
    int j = 0;
    commands[j] = malloc(BUFSIZE);
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
	    int isPipeOrRedirect = (strcmp(word, "<")==0) || (strcmp(word, ">")==0) || (strcmp(word, "|")==0);
	    if (isPipeOrRedirect) {
		commands[j][inner_count] = NULL;
		commands[++j] = malloc(sizeof(char*));
		commands[j][0] = word;
		sizes[j] = 1;

		commands[++j] = malloc(BUFSIZE);
		sizes[j] = 0;
		inner_count=0;
	    }
	    if (inner_count+1 > sizeof(commands[j])){
		char **res = realloc(commands[j], 2*sizeof(commands[j]));
		if ( res != NULL ){
		    commands[j] = res;
		} else {
		    printf("failed to resize commands[j]\n");
		}
	    }
	    if (!isPipeOrRedirect) {
		commands[j][inner_count++] = word;
		if ( j >= sizes_len ) {
		    int *res = realloc(sizes, 2*sizes_len);
		    if ( res != NULL ){
			sizes = res;
			sizes_len *= 2;
		    } else {
			printf("failed to resize sizes\n");
		    }
		}
		sizes[j]++;
	    }
	}

	if (DEBUG) printf("word:\"%s\"\tword-start: %d\tword-end: %d\tword-len: %d\n", word, wordStart, wordEnd, wordLen);

	wordStart = i;
	wordEnd++;
    }
    *len = j+1;
    return sizes;
}
