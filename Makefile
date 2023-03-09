CC = gcc
CFLAGS = -std=c99 -g -Wall -fsanitize=address,undefined

debug: mysh.c
	$(CC) $(CFLAGS) $< -o mysh-debug

run: mysh.o
	$(CC) $(CFLAGS) $^ -o mysh

mysh.o: mysh.c
	$(CC) $(CFLAGS) -DDEBUG=0 -c $^

clean:
	rm -f output.txt
	rm -f mysh mysh-debug *.o