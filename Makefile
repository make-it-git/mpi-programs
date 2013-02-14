CC=gcc
CFLAGS=-ggdb -O0 -Wall -Wextra

all: bin/lcs_sequential bin/lcs_sequential_test bin/lcs_parallel_test

obj/lcs_functions.o: src/lcs_functions.c src/lcs_functions.h
	$(CC) $(CFLAGS) -c -o $@ $<

obj/lcs_sequential.o: src/lcs_sequential.c
	$(CC) $(FLAGS) -c -o $@ $<

bin/lcs_sequential: obj/lcs_sequential.o obj/lcs_functions.o
	$(CC) $(FLAGS) -o $@ obj/lcs_sequential.o obj/lcs_functions.o

bin/lcs_sequential_test: src/lcs_sequential_test.c obj/lcs_functions.o
	$(CC) $(FLAGS) -o bin/lcs_sequential_test src/lcs_sequential_test.c obj/lcs_functions.o -lcheck

bin/lcs_parallel_test: src/lcs_parallel_test.c obj/lcs_functions.o
	$(CC) $(FLAGS) -o bin/lcs_parallel_test src/lcs_parallel_test.c obj/lcs_functions.o -lcheck

clean:
	rm -f obj/*.o bin/*
