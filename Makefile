CC=mpicc
CFLAGS=-Wall -Wextra

all: bin/lcs_sequential bin/test_lcs_sequential bin/test_lcs_parallel bin/lcs_parallel bin/lcs_generate_strings bin/test_lup_functions bin/lup_sequential bin/lup_generate_matrix bin/lup_parallel

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

obj/lup_functions.o: src/lup_functions.c src/lup_functions.h
	$(CC) $(CFLAGS) -c -o $@ $<

bin/test_lup_functions: src/test_lup_functions.c obj/lup_functions.o
	$(CC) $(CFLAGS) -o $@ src/test_lup_functions.c obj/lup_functions.o -lcheck

bin/lup_sequential: src/lup_sequential.c obj/lup_functions.o
	$(CC) $(CFLAGS) -o $@ src/lup_sequential.c obj/lup_functions.o

bin/lup_generate_matrix: src/lup_generate_matrix.c
	$(CC) $(CFLAGS) -o $@ $<

bin/lup_parallel: src/lup_parallel.c obj/lup_functions.o
	$(CC) $(CFLAGS) -o $@ src/lup_parallel.c obj/lup_functions.o

clean:
	rm -f obj/*.o bin/*
