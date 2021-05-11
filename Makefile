CFLAGS = -g -Wall -std=gnu99
all: main

main: main.c
	gcc ${CFLAGS} -o $@ $^

clean:
	rm -f main
	
hard_clean: 
	rm -f main notes data

