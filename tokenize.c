#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>	
#include <string.h>
#include <stdbool.h>

#ifndef DEBUG
#define DEBUG 0
#endif
#define INNER_SIZE 80

/*
// returns a list of strings of all wildcard matches
char **findWildCardMatches(char *dir, char *lft, char *rht, int *count){
    // TODO: make sure patterns beginning with * (ex. *.txt) don't match with names beginning with .
    if (dir != NULL) chdir(path);

}
*/

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
    bool isRedirect = false;
    bool wasRedirect = false;
    bool isPipe = false;
    bool wasPipe = false;
    int numRedirectsToSkip = 0;
    bool isDir = false;
    int last_slash = 0;
    bool containsWildcard = false;
    int wildcard_pos = 0;
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
	    if (wasRedirect) {
		if (DEBUG) printf("tokenize error: redirect missing args!\n");
		return -1;
	    }
	    if (wasPipe) {
		if (DEBUG) printf("tokenize error: pipe missing args!\n");
		return -1;
	    }
	    if (numRedirectsToSkip > 2) {
		if (DEBUG) printf("too many redirects!\n");
		return -1;
	    } else if (numRedirectsToSkip > 0) {
		commands[j][inner_count] = NULL; 
		j += numRedirectsToSkip;
	    }
	    break;
	}

	// get word len
	while (i < strsize && !isspace(string[i]) \
		&& !((string[i] == '<') || (string[i] == '>') || (string[i] == '|')))
	{
	    if (string[i] == '*') {
		containsWildcard = true;
		wildcard_pos = i;
	    }
	    if (string[i] == '/') {
		isDir = true;
		last_slash = i;
	    }
	    wordEnd = i;
	    i++;
	}

	if ((string[wordEnd] == '<') || (string[wordEnd] == '>') || (string[wordEnd] == '|')) i++;

	wordLen = wordEnd - wordStart + 1;
	word = malloc(wordLen + 1);
	memcpy(word, string + wordStart, wordLen);
	word[wordLen] = '\0';

	char **matches = NULL;
	int num_matches = 0;
	/*
	if (containsWildcard){
	    char *dir = NULL;
	    char *wildcard_lft = NULL;
	    char *wildcard_rht = NULL;
	    if (isDir) {
		int dir_len = last_slash - wordStart + 1;
		dir = malloc(dir_len + 1);
		memcpy(dir, word, dir_len);
		dir[dir_len] = '\0';
		wildcard_lft = ;
		wildcard_rht = ;
	    }
	    wildcard_lft = ;
	    wildcard_rht = ;
	    matches = findWildCardMatches(dir, wildcard_lft, wildcard_rht, &num_matches);
	}
	*/

	if (DEBUG) printf("word:\"%s\"\tword-start: %d\tword-end: %d\tword-len: %d\n",\
		word, wordStart, wordEnd, wordLen);
	isPipe = (strcmp(word, "|")==0); 
	isRedirect = (strcmp(word, "<")==0) || (strcmp(word, ">")==0); 
	if (isPipe) {
	    // error catching
	    if (numRedirectsToSkip > 2) {
		if (DEBUG) printf("tokenize error: too many redirects!\n");
		return -1;
	    }
	    if (wasPipe) {
		if (DEBUG) printf("tokenize error: consecutive pipes!\n");
		return -1;
	    }
	    if (wasRedirect) {
		if (DEBUG) printf("tokenize error: pipe immediately after redirect!\n");
		return -1;
	    }

	    commands[j][inner_count] = NULL;
	    j += 1 + numRedirectsToSkip;
	    //printf("j reset isPipe: %d\n", j);

	    // resizing
	    if (j+1 >= commCapacity){
		if (DEBUG) printf("commands not large enough\n");
		return -2;
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
	    wasPipe = true;
	} else if (wasRedirect) { // adds word after redirect token
	    // error catching
	    if (isRedirect) {
		if (DEBUG) printf("tokenize error: consecutive redirects!\n");
		return -1;
	    }

	    commands[j][1] = word;
	    //printf("word wasRedirect: %s\n", word);
	    //printf("j wasRedirect: %d\n", j);
	    commands[j][2] = NULL;
	    wasRedirect = false;
	    j -= numRedirectsToSkip;
	    //printf("j reset wasRedirect: %d\n", j);
	} else if (isRedirect) { // adds redirect token
	    // error catching
	    if (wasPipe) {
		if (DEBUG) printf("tokenize error: redirect immediately after pipe!\n");
		return -1;
	    }

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
	    if (inner_count+1+num_matches > commjCapacity) { 
		commjCapacity *= 2;
		char **res = realloc(commands[j], commjCapacity*sizeof(char**)); 
		if ( res != NULL ){
		    commands[j] = res;
		    if (DEBUG) printf("resized commands[j] to have capacity: %d\n", commjCapacity);
		} else {
		    if (DEBUG) printf("failed to resize commands[j]\n");
		}
	    }

	    // add all wildcard matches
	    if (containsWildcard && (num_matches > 0)) {
		for (int i = 0; i < num_matches; i++ ) {
		    commands[j][inner_count++] = matches[i];
		}
	    } else {
		commands[j][inner_count++] = word;
		//printf("word else: %s\n", word);
	    }
	}

	if (!isPipe) wasPipe = false;

	//printf("i: %d\n", i);
	//printf("strsize: %d\n", strsize);
	if (i >= strsize-1) {
	    if (wasRedirect) {
		if (DEBUG) printf("tokenize error: redirect missing args!\n");
		return -1;
	    }
	    if (wasPipe) {
		if (DEBUG) printf("tokenize error: pipe missing args!\n");
		return -1;
	    }
	    commands[j][inner_count] = NULL; 
	    j += numRedirectsToSkip;
	    break;
	    //printf("j: %d\n", j);
	    //printf("inner_count: %d\n", inner_count);
	}

	wordStart = i;
	wordEnd++;
	isDir = false;
	containsWildcard = false;
    }
    return j+1;
}
