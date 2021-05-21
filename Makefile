CFLAGS = -g -Wall -std=gnu99
FILES = note s_notes data
all: main

main: main.c
	gcc ${CFLAGS} -o $@ $^
	touch ${FILES}

clean:
	rm -f main
	
hard_clean: 
	rm -f main notes data s_notes


