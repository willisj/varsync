

#gcc -ggdb -O0 -o test-varsync test.c varsync.c -I. -I binary-socket binary-socket/bsock.o -lpthread 

LIBS= binary-socket/bsock.o -lpthread
INCS= -I. -I binary-socket

all: example

build_dirs:
	mkdir -p bin

binary-socket/bsock.o:
	make -C binary-socket

varsync.o: varsync.c varsync.h binary-socket/bsock.o
	gcc -c varsync.c  ${LIBS} ${INCS}

example: build_dirs varsync.o
	gcc -o bin/varsync-test test.c varsync.o ${LIBS} ${INCS}

clean:
	rm *.o&
	rm bin -r&
	@wait
