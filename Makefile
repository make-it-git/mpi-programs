CC=mpicc
CFLAGS=-ggdb -O0 -Wall -Wextra

all: bin/lcs_sequential bin/test_lcs_sequential bin/test_lcs_parallel bin/lcs_parallel bin/lcs_generate_strings

obj/lcs_functions.o: src/lcs_functions.c src/lcs_functions.h
	$(CC) $(CFLAGS) -c -o $@ $<

obj/lcs_sequential.o: src/lcs_sequential.c
	$(CC) $(CFLAGS) -c -o $@ $<

bin/lcs_sequential: obj/lcs_sequential.o obj/lcs_functions.o
	$(CC) $(CFLAGS) -o $@ obj/lcs_sequential.o obj/lcs_functions.o

bin/test_lcs_sequential: src/test_lcs_sequential.c obj/lcs_functions.o
	$(CC) $(CFLAGS) -o $@ src/test_lcs_sequential.c obj/lcs_functions.o -lcheck

bin/test_lcs_parallel: src/test_lcs_parallel.c obj/lcs_functions.o
	$(CC) $(CFLAGS) -o $@ src/test_lcs_parallel.c obj/lcs_functions.o -lcheck

bin/lcs_parallel: src/lcs_parallel.c obj/lcs_functions.o
	$(CC) $(CFLAGS) -o $@ src/lcs_parallel.c obj/lcs_functions.o

bin/lcs_generate_strings: src/lcs_generate_strings.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f obj/*.o bin/*
