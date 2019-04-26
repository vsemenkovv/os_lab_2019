CC=gcc
CFLAGS=-I.

parallel_min_max : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o parallel_min_max utils.o find_min_max.o parallel_min_max.c $(CFLAGS)

utils.o : utils.h
	$(CC) -o utils.o -c utils.c $(CFLAGS)

find_min_max.o : utils.h find_min_max.h
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)

	
process_memory : process_memory.c
	$(CC) -o process_memory process_memory.c $(CFLAGS)
parallel_sum : parallel_sum.c utils.c
	$(CC) -o parallel parallel_sum.c utils.c -lpthread $(CFLAGS)
clean :
	rm process_memory parallel_min_max parallel