CC = gcc
CC_FLAGS = -O2 -Wall -g

all: part_a part_a2

pdf: a1.tex part_a.eps
	latex a1.tex
	latex a1.tex
	dvips -o a1.ps a1.dvi
	ps2pdf a1.ps

part_a:	part_a.c utils.c
	$(CC) $(CC_FLAGS) -o part_a part_a.c utils.c

part_a2: part_a2.c utils.c
	$(CC) $(CC_FLAGS) -o part_a2 part_a2.c utils.c

clean:	
	rm -f *.o part_a part_a2