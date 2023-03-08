CC = gcc
CFLAGS = -std=c99 -g -Wall -fsanitize=address,undefined

debug: mysh.c
	$(CC) $(CFLAGS) $^ -o mysh-debug

run: mysh.o
	$(CC) $(CFLAGS) $^ -o mysh

mysh.o: mysh.c
	$(CC) $(CFLAGS) -DDEBUG=0 -c $^

clean-all: clean clean-output

clean-output:
	rm -f output.txt

clean: 
	rm -f mysh *.o