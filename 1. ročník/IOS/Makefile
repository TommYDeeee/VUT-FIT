
NAME=proj2

CC=gcc
CFLAGS= -std=gnu99 -Wall -pedantic -Wextra -g -pthread

$(NAME): $(NAME).c
	$(CC) $(CFLAGS) $(NAME).c -lm -o $(NAME)

clean:
	rm -f *~ $(NAME) *.bak *.o

