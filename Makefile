CC=gcc
CFLAGS=-ggdb -O0 -Wall -Wextra

all: bin/lcs_sequential bin/test_lcs_sequential bin/test_lcs_parallel

obj/lcs_functions.o: src/lcs_functions.c src/lcs_functions.h
	$(CC) $(CFLAGS) -c -o $@ $<

obj/lcs_sequential.o: src/lcs_sequential.c
	$(CC) $(CFLAGS) -c -o $@ $<

bin/lcs_sequential: obj/lcs_sequential.o obj/lcs_functions.o
	$(CC) $(CFLAGS) -o $@ obj/lcs_sequential.o obj/lcs_functions.o

bin/test_lcs_sequential: src/test_lcs_sequential.c obj/lcs_functions.o
	$(CC) $(CFLAGS) -o bin/test_lcs_sequential src/test_lcs_sequential.c obj/lcs_functions.o -lcheck

bin/test_lcs_parallel: src/test_lcs_parallel.c obj/lcs_functions.o
	$(CC) $(CFLAGS) -o bin/test_lcs_parallel src/test_lcs_parallel.c obj/lcs_functions.o -lcheck

clean:
	rm -f obj/*.o bin/*
