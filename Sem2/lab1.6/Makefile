all: lab1.6

lab1.6: lab1.6.o func1.6lib
	gcc -o lab1.6 lab1.6.o -L../lab1.6 -lmythread -Wl,-rpath,.

lab1.6.o: lab1.6.c
	gcc -c lab1.6.c

func1.6.o: func1.6.c
	gcc -c -fPIC func1.6.c

func1.6lib: func1.6.o
	gcc -shared -o libmythread.so func1.6.o
