all: atdate
CFLAGS=-include /usr/include/errno.h
atdate: atdate.c
atdate: atdate.h

clean:
	rm -f *.o atdate
