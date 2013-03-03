#include <stdio.h>
#include "lcs_functions.h"
#include <unistd.h>

int main(int argc, char **argv) {
    if(argc != 6) {
        fprintf(stderr, "Usage: %s str1_file str2_file out_file tmp_file_P tmp_file_S\n", argv[0]);
        return 1;
    }

    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    char *alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int alp_len = strlen(alphabet);

    char *filename_str1 = argv[1];
    char *filename_str2 = argv[2];
    char *filename_out = argv[3];
    char *filename_P = argv[4];
    char *filename_S = argv[5];
    MPI_File f_str1, f_str2, f_out, f_P, f_S;
    MPI_File_open(MPI_COMM_WORLD, filename_str1, MPI_MODE_RDONLY, MPI_INFO_NULL, &f_str1);
    MPI_File_open(MPI_COMM_WORLD, filename_str2, MPI_MODE_RDONLY, MPI_INFO_NULL, &f_str2);
    MPI_File_open(MPI_COMM_WORLD, filename_out, MPI_MODE_CREATE|MPI_MODE_RDWR, MPI_INFO_NULL, &f_out);
    MPI_File_open(MPI_COMM_WORLD, filename_P, MPI_MODE_CREATE|MPI_MODE_RDWR, MPI_INFO_NULL, &f_P);
    MPI_File_open(MPI_COMM_WORLD, filename_S, MPI_MODE_CREATE|MPI_MODE_RDWR, MPI_INFO_NULL, &f_S);

    int rows_per_process = alp_len / size;
    int row_start = rank * rows_per_process;
    int row_end = (rank + 1) * rows_per_process - 1;
    if(rank == size - 1)
        row_end = alp_len - 1;
    LCS_IO_calc_P(f_str2, alphabet, alp_len, f_P, row_start, row_end);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_File_close(&f_str1);
    MPI_File_close(&f_str2);
    MPI_File_close(&f_out);
    MPI_File_close(&f_P);
    MPI_File_close(&f_S);

    MPI_Finalize();
    return 0;
}
