CC = gcc
CC_FLAGS = -O2 -Wall -g

all:	part_a part_a2

part_a:	part_a.c utils.c
	$(CC) $(CC_FLAGS) -o part_a part_a.c utils.c

part_a2: part_a2.c utils.c
	$(CC) $(CC_FLAGS) -o part_a2 part_a2.c utils.c

clean:	
	rm -f *.o *~ part_a part_a2