.PHONY:clean
CC=gcc
CFLAGS=-Wall -g -std=gnu99
BIN=minishell
OBJS=main.o str.o shell.o
$(BIN):$(OBJS)
	$(CC) $(CFLAGS) $^ -o $@
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f *.o $(BIN)