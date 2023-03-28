#include<ctype.h>
#include<stdlib.h>
#include<string.h>

#ifndef DEBUG
    #define DEBUG 0
#endif

char* buffer;
int bufsize = 1;

// write array of words to commands, return size of commands
int tokenize(char** commands, char* string, int strsize){
    int commands_count = 0;
    char* word = NULL;
    int wordlen = 1;
    int wordpos = 0;
    int start = 0;
    int i = 0;
    while (i < strsize)
    {
        // skip space
        while (i < strsize && isspace(string[i]))
        {
            start++; 
            i++;
        }
        if (start >= strsize) break;

        // get word len
        while (i < strsize && !isspace(string[i]) )
        {
            wordpos = i;
            i++;
        }
        wordlen = wordpos - start + 1;
        word = malloc(wordlen + 1);
        // printf("start: %d, wordpos: %d len: %d\n", start, wordpos, wordlen);
        word = memcpy(word, string + start, wordlen);
        if (word) {
            word[wordlen] = '\0';
            commands[commands_count++] = word;
        }
        // if (DEBUG) printf("word: %s\n", word);
        start = i;
    }
    return commands_count;
}