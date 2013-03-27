CC=mpicc
CFLAGS=-Wall -Wextra -O2

all: bin/lcs_sequential bin/test_lcs_sequential bin/test_lcs_parallel bin/lcs_parallel bin/lcs_generate_strings bin/test_lup_functions bin/lup_sequential bin/lup_generate_matrix bin/lup_parallel bin/lcs_parallel_io bin/lup_parallel_rma bin/lcs_lis_parallel

obj/lcs_functions.o: src/lcs_functions.c
	$(CC) $(CFLAGS) -c -o $@ $^

obj/lcs_functions_mpi.o: src/lcs_functions_mpi.c
	$(CC) $(CFLAGS) -c -o $@ $^

bin/lcs_sequential: src/lcs_sequential.c obj/lcs_functions.o
	$(CC) $(CFLAGS) -o $@ $^

bin/test_lcs_sequential: src/test_lcs_sequential.c obj/lcs_functions.o
	$(CC) $(CFLAGS) -o $@ $^ -lcheck

bin/test_lcs_parallel: src/test_lcs_parallel.c obj/lcs_functions.o
	$(CC) $(CFLAGS) -o $@ $^ -lcheck

bin/lcs_parallel: src/lcs_parallel.c obj/lcs_functions_mpi.o obj/lcs_functions.o
	$(CC) $(CFLAGS) -o $@ $^

bin/lcs_generate_strings: src/lcs_generate_strings.c
	$(CC) $(CFLAGS) -o $@ $<

bin/lcs_parallel_io: src/lcs_parallel_io.c obj/lcs_functions.o obj/lcs_functions_mpi.o
	$(CC) $(CFLAGS) -o $@ $^

obj/lup_functions.o: src/lup_functions.c
	$(CC) $(CFLAGS) -c -o $@ $^

obj/lup_functions_mpi.o: src/lup_functions_mpi.c
	$(CC) $(CFLAGS) -c -o $@ $^

bin/test_lup_functions: src/test_lup_functions.c obj/lup_functions.o
	$(CC) $(CFLAGS) -o $@ $^ -lcheck

bin/lup_sequential: src/lup_sequential.c obj/lup_functions.o
	$(CC) $(CFLAGS) -o $@ $^

bin/lup_generate_matrix: src/lup_generate_matrix.c
	$(CC) $(CFLAGS) -o $@ $<

bin/lup_parallel: src/lup_parallel.c obj/lup_functions.o obj/lup_functions_mpi.o
	$(CC) $(CFLAGS) -o $@ $^

bin/lup_parallel_rma: src/lup_parallel_rma.c obj/lup_functions.o obj/lup_functions_mpi.o
	$(CC) $(CFLAGS) -o $@ $^

obj/lcs_lis_functions.o: src/lcs_lis_functions.c
	$(CC) $(CFLAGS) -c -o $@ $^

bin/lcs_lis_parallel: src/lcs_lis_parallel.c obj/lcs_lis_functions.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f obj/*.o bin/*
