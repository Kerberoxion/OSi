all: one two three four

four: hello.o libdynHello.so
	cc hello.o  -o hello -L. -ldynHello -Wl,-rpath,.

three: dynHello.o
	cc -shared -o libdynHello.so dynHello.o

two: dynHello.c
	cc -fPIC -c dynHello.c 

one: hello.c
	cc -c hello.c

