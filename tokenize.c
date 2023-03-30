#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>	
#include <string.h>
#include <stdbool.h>

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
    bool wasRedirect = false;
    int numRedirectsToSkip = 0;
    while (i < strsize)
    {
        // skip space
        while (i < strsize && isspace(string[i])) 
        {
	    wordStart++; 
	    wordEnd++;
            i++;
        }

	if (wordStart >= strsize){
	    if (numRedirectsToSkip > 0){
		commands[j][inner_count] = NULL; 
		j += numRedirectsToSkip;
	    }
	    break;
	}

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
	    if (DEBUG) printf("word:\"%s\"\tword-start: %d\tword-end: %d\tword-len: %d\n",\
		    word, wordStart, wordEnd, wordLen);
	    bool isPipe = (strcmp(word, "|")==0); 
	    bool isRedirect = (strcmp(word, "<")==0) || (strcmp(word, ">")==0); 
	    if (isPipe) {
		// TODO: if (wasRedirect) set error -> > or < then |
		// TODO: also need to catch, [|, < or >], [< or >, |], [|,|], [<or>, <or>]
		// TODO: also check if EXACTLY one set of <,> per in between pipes 
		commands[j][inner_count] = NULL;
		j += 1 + numRedirectsToSkip;
		//printf("j reset isPipe: %d\n", j);

		// resizing
		if (j+1 >= commCapacity){
		    if (DEBUG) printf("commands not large enough\n");
		    return -1;
		}

		// store the pipe token
		commands[j] = malloc(2*sizeof(char**));
		commands[j][0] = word;
		//printf("word isPipe: %s\n", word);
		//printf("j isPipe: %d\n", j);
		commands[j][1] = NULL; 

		// start new inner list
		commands[++j] = malloc(INNER_SIZE);
		//printf("j reset isPipe: %d\n", j);
		commjCapacity = (int) (INNER_SIZE / sizeof(char**));
		inner_count=0;
		numRedirectsToSkip = 0;
	    } else if (wasRedirect) { // adds word after redirect token
		// TODO: catch errors if after redirect comes |
		commands[j][1] = word;
		//printf("word wasRedirect: %s\n", word);
		//printf("j wasRedirect: %d\n", j);
		commands[j][2] = NULL;
		wasRedirect = false;
		j -= numRedirectsToSkip;
		//printf("j reset wasRedirect: %d\n", j);
	    } else if (isRedirect) { // adds redirect token
		j += 1 + numRedirectsToSkip;
		//printf("j reset isRedirect: %d\n", j);

		// resizing
		if (j >= commCapacity){
		    if (DEBUG) printf("commands not large enough\n");
		    return -1;
		}

		// store the redirect token
		commands[j] = malloc(3*sizeof(char**));
		commands[j][0] = word;
		//printf("word isRedirect: %s\n", word);
		//printf("j isRedirect: %d\n", j);

		wasRedirect = true;
		numRedirectsToSkip++;
	    } else {
		// resizing
		if (inner_count+1 > commjCapacity) { 
		    commjCapacity *= 2;
		    char **res = realloc(commands[j], commjCapacity*sizeof(char**)); 
		    if ( res != NULL ){
			commands[j] = res;
			if (DEBUG) printf("resized commands[j] to have capacity: %d\n", commjCapacity);
		    } else {
			if (DEBUG) printf("failed to resize commands[j]\n");
		    }
		}
		commands[j][inner_count++] = word;
		//printf("word else: %s\n", word);
	    }

	    //printf("i: %d\n", i);
	    //printf("strsize: %d\n", strsize);
	    if (i >= strsize-1){
		commands[j][inner_count] = NULL; 
		j += numRedirectsToSkip;
		break;
		//printf("j: %d\n", j);
		//printf("inner_count: %d\n", inner_count);
	    }

	    wordStart = i;
	    wordEnd++;

	} else {
	    //TODO: return error -> memcpy word failed
	}
    }
    return j+1;
}
