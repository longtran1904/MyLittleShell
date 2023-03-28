CC = gcc
CFLAGS = -std=c99 -g -Wall -fsanitize=address,undefined

debug: mysh.c execution.c tokenize.c
	$(CC) $(CFLAGS) $^ -DDEBUG=1 -o mysh-debug

run: mysh.c execution.c tokenize.c
	$(CC) $(CFLAGS) $^ -o mysh

clean:
	rm -f output.txt
	rm -f mysh mysh-debug *.o