#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>	
#include <string.h>

#ifndef DEBUG
    #define DEBUG 0
#endif

char* buffer;
int bufsize = 1;

// write array of words to commands, return size of commands
int tokenize(char** commands, char* string, int strsize){
    int commands_count = 0;
    char* word = NULL;
    int wordLen = 0;
    int wordEnd = 0;
    int wordStart = 0;
    int i = 0;
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
	    commands[commands_count++] = word;
        }

        if (DEBUG) printf("word:\"%s\"\tword-start: %d\tword-end: %d\tword-len: %d\n", word, wordStart, wordEnd, wordLen);

	wordStart = i;
	wordEnd++;
    }

    commands[commands_count++] = NULL;
    return commands_count;
}
