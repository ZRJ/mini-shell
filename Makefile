.PHONY:clean
CC=gcc
CFLAGS=-Wall -g -std=c99
BIN=minishell
OBJS=main.o parse.o init.o execute.o buildin.o str.o
$(BIN):$(OBJS)
	$(CC) $(CFLAGS) $^ -o $@
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f *.o $(BIN)