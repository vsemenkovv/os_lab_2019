all: multModulo client server
client: client.c
	gcc client.c -L. -lmultModulo -pthread -o client
server: server.c
	gcc server.c -L. -lmultModulo -pthread -o server
multModulo: multModulo.c multModulo.h
	gcc -c multModulo.c multModulo.h
	ar rcs libmultModulo.a multModulo.o