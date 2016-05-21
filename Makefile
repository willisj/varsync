

all:
	gcc -o test-varsync test.c varsync.c -I. -I binary-socket binary-socket/bsock.o -lpthread -ggdb -O0
