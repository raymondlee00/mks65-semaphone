ifeq ($(DEBUG), true)
	CC = gcc -g
else
	CC = gcc
endif

all: control.c write.c
	$(CC) -o control control.c
	$(CC) -o write write.c

clean:
	rm control
	rm write
	rm *~

memcheck:
	valgrind --leak-check=yes ./control