#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>	
#include <string.h>
#include <stdbool.h>
#include <dirent.h> 
#include <limits.h>
#include <unistd.h>
#include "execution.h"

#ifndef DEBUG
#define DEBUG 0
#endif
#define MAX_ARG_LEN 10
#define MAX_NUM_FILES 100

char **getAllFileNames(int *count){
    *count = 0;
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    char **allFiles = malloc(MAX_NUM_FILES*sizeof(char**));
    int allFilesCapacity = MAX_NUM_FILES;
    if (d) {
	while ((dir = readdir(d)) != NULL) {
	    // resize allFiles if needed
	    if (*count >= allFilesCapacity){
		allFilesCapacity *= 2;
		char **res = realloc(allFiles, allFilesCapacity*sizeof(char**)); 
		if ( res != NULL ){
		    allFiles = res;
		    if (DEBUG) printf("resized allFiles to have capacity: %d\n", allFilesCapacity);
		} else {
		    if (DEBUG) printf("failed to resize allFiles!\n");
		}
	    }

	    char *file = malloc(FILENAME_MAX);
	    if (DEBUG) printf("file: %s\n", dir->d_name);
	    file = dir->d_name;
	    allFiles[(*count)++] = file;
	}
	allFiles[*count] = NULL;
	closedir(d);
    }
    return allFiles;
}

// returns a list of strings of all wildcard matches
char **findWildCardMatches(char *dir, char *lft, char *rht, int *count){
    *count = 0;
    char **files;
    int num_files = 0;
    char **matches;
    char buffer[PATH_MAX];
    if (dir != NULL) {
	// store pwd
	if (getcwd(buffer, PATH_MAX) == NULL) {
	    if (DEBUG) printf("couldn't get pwd!\n");
	    return NULL;
	}
	changeDir(dir);
    }
    files = getAllFileNames(&num_files);
    printf("numFiles: %d\n", num_files);


    // TODO: make sure patterns beginning with * (ex. *.txt) don't match with names beginning with .

    // cd back to what it was
    if (dir != NULL) changeDir(buffer);
    // TODO: change to return matches
    return files;
}

// write array of words to commands, return size of commands
int tokenize(char*** commands, int commCapacity, char* string, int strsize){
    char* word = NULL;
    int wordLen = 0;
    int wordEnd = 0;
    int wordStart = 0;
    int inner_count = 0;
    int i = 0;
    int j = 0;
    int commjCapacity = MAX_ARG_LEN;
    commands[j] = malloc(commjCapacity*sizeof(char**));
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
	if (containsWildcard){
	    char *dir = NULL;
	    char *wildcard_lft = NULL;
	    char *wildcard_rht = NULL;
	    int wild_lft_len = wildcard_pos - wordStart;
	    int wild_rht_len = wordEnd - wildcard_pos;
	    if (isDir) {
		int dir_len = last_slash - wordStart + 1;
		dir = malloc(dir_len + 1);
		memcpy(dir, word, dir_len);
		dir[dir_len] = '\0';

		wild_lft_len -= dir_len;
		wildcard_lft = malloc(wild_lft_len + 1);
		memcpy(wildcard_lft, word+dir_len, wild_lft_len);
		wildcard_lft[wild_lft_len] = '\0';

		wildcard_rht= malloc(wild_rht_len + 1);
		memcpy(wildcard_rht, word+dir_len+wild_lft_len+1, wild_rht_len);
		wildcard_rht[wild_rht_len] = '\0';

		if (DEBUG) {
		    printf("directory containing wildcard filename: %s\n", dir);
		}
	    } else {
		wildcard_lft = malloc(wild_lft_len + 1);
		memcpy(wildcard_lft, word, wild_lft_len);
		wildcard_lft[wild_lft_len] = '\0';

		wildcard_rht= malloc(wild_rht_len + 1);
		memcpy(wildcard_rht, word+wild_lft_len+1, wild_rht_len);
		wildcard_rht[wild_rht_len] = '\0';
	    }

	    if (DEBUG) {
		printf("wildcard left (of *): %s\n", wildcard_lft);
		printf("wildcard right (of *): %s\n", wildcard_rht);
	    }

	    matches = findWildCardMatches(dir, wildcard_lft, wildcard_rht, &num_matches);
	}

	if (DEBUG) printf("word:\"%s\"\tword-start: %d\tword-end: %d\tword-len: %d\n",\
		word, wordStart, wordEnd, wordLen);
	isPipe = (strcmp(word, "|")==0); 
	isRedirect = (strcmp(word, "<")==0) || (strcmp(word, ">")==0); 
	if (isPipe) {
	    // error catching
	    if (j==0 && inner_count==0) {
		if (DEBUG) printf("tokenize error: commands starts with pipe!\n");
		return -1;
	    }
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

	    //jprintf("commjCapacity: %d\n", commjCapacity);
	    //printf("inner_count: %d\n", inner_count);
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
	    commjCapacity = MAX_ARG_LEN;
	    commands[++j] = malloc(commjCapacity*sizeof(char**));
		    
	    //printf("j reset isPipe: %d\n", j);
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
	    if (j==0 && inner_count==0) {
		if (DEBUG) printf("tokenize error: command starts with redirect!\n");
		return -1;
	    }
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
	    if (inner_count+1+num_matches >= commjCapacity) { 
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
