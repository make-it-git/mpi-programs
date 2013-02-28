#include <stdio.h>
#include "lup_functions.h"
#include <unistd.h>

int main(int argc, char **argv) {
    if(argc != 3) {
        fprintf(stderr, "Usage: %s filename N\n", argv[0]);
        fprintf(stderr, "\tN denotes matrix size\n");
        return 1;
    }
    double t_swap_rows_in_P = 0;
    double t_scatter_data = 0;
    double t_swap_rows_in_the_same_process = 0;
    double t_swap_rows_in_different_processes = 0;
    double t_start;
    double t_start_data_out = 0;
    double t_end_data_out = 0;
    double t_calcs = 0;
    char *filename = argv[1];
    int N = atoi(argv[2]);
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if(size == 1) {
        fprintf(stderr, "At least 2 processes expected\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    double *A = NULL;
    double *C = NULL;
    int *P = NULL; //identity matrix
    int i;
    t_start = MPI_Wtime();
    if(rank == 0) { //read matrix A from file
        LUP_mpi_matrix_create(&A, N);
        LUP_mpi_matrix_create(&C, N);//matrix C will contain result
        P = (int*)malloc(sizeof(int) * N);
        for(i = 0; i < N; i++) {
            P[i] = i;
        }
        FILE *f;
        f = fopen(filename, "rb");
        if(!f) {
            fprintf(stderr, "Unable to open file '%s'\n", filename);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        for(i = 0; i < N; i++) {//fill array with data generated by lup_generate_matrix
            fread(A + i*N, sizeof(double), N, f);
        }
        fclose(f);
        /*int k;
        for(i = 0; i < N; i++) {
            for(k = 0; k < N; k++) {
                printf("%f\t", A[i*N + k]);
            }
            printf("\n");
        }*/
    }

    //calculate how many rows goes to every process
    int rows_per_process = N / (size - 1); //(size-1), because rank=0 does not work here
    int rows_per_last_process = N - (size - 2) * rows_per_process;
    int rp = (rank == size - 1) ? rows_per_last_process : rows_per_process;

    double *rows = NULL; //buffer for rows at every process with rank > 0
    if(rank > 0) {
        //int rp = (rank == size - 1) ? rows_per_last_process : rows_per_process;
        rows = (double*)malloc(sizeof(double) * N * rp);
    }
    if(rank == 0) {
        int *sendcounts = (int*)malloc(sizeof(int) * size);
        int *displs = (int*)malloc(sizeof(int) * size);
        sendcounts[0] = displs[0] = 0;//rank=0 does not send anything (and does not receive, too)
        for(i = 1; i < size; i++) {
            if(i == size-1) {
                sendcounts[i] = rows_per_last_process * N;
            } else {
                sendcounts[i] = rows_per_process * N;
            }
            displs[i] = (i-1) * sendcounts[i-1];
        }
        //spread rows across processes
        MPI_Scatterv(A, sendcounts, displs, MPI_DOUBLE, NULL, 0, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        free(sendcounts);
        free(displs);
    } else {
        MPI_Scatterv(NULL, NULL, NULL, MPI_DOUBLE, rows, rp*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }
    /*if(rank > 0) {
        sleep(rank);
        printf("rank=%d:\n", rank);
        //int rp = (rank == size - 1) ? rows_per_last_process : rows_per_process;
        int k;
        for(i = 0; i < rp; i++) {
            for(k = 0; k < N; k++) {
                printf("%f\t", rows[i*N + k]);
            }
            printf("\n");
        }
    }*/
    /*if(rank > 0) {
        //int rp = (rank == size - 1) ? rows_per_last_process : rows_per_process;
        printf("rank=%d, rows=%d\n", rank, rp);
    }*/
    t_scatter_data = MPI_Wtime();

    //find pivot element
    double pivot_value;
    int pivot_row;
    double *pivot_values = NULL;
    int *pivot_rows = NULL;
    if(rank == 0) {
        pivot_values = (double*)malloc(sizeof(double) * size);
        pivot_rows = (int*)malloc(sizeof(int) * size);
    }
    int first_row;
    int last_row;
    int proc;
    double proc_max_value = 0;
    int proc_max_row = -1;
    double Cii;
    double *prev_row = NULL;
    //if(rank >= 1) { commented out, because rank=0 will receive this too in MPI_Bcast
        prev_row = (double*)malloc(sizeof(double) * N);
    //}
    int i_row, max_row;
    int i_proc, max_proc;
    for(i = 0; i < N-1; i++) {
        if(rank > 0) {
            if(rank == size - 1) last_row = N-1;
            else last_row = rank*rows_per_process - 1;
            first_row = (rank-1)*rows_per_process;
            //LUP_mpi_find_pivot(rows, first_row, last_row, i, N, &pivot_value, &pivot_row);
            if(LUP_mpi_find_pivot(rows, first_row, last_row, i, N, &pivot_value, &pivot_row) == 0) {
                if(fabs(pivot_value) < 1E-6) {
                    fprintf(stderr, "Matrix is singular\n");
                    MPI_Abort(MPI_COMM_WORLD, 1);
                }
            }
            MPI_Gather(&pivot_value, 1, MPI_DOUBLE, NULL, 0, MPI_DOUBLE, 0, MPI_COMM_WORLD);
            MPI_Gather(&pivot_row, 1, MPI_INT, NULL, 0, MPI_INT, 0, MPI_COMM_WORLD);
            if(i >= first_row && i <= last_row) {
                MPI_Send(&rank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            }
            MPI_Bcast(&i_row, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(&max_row, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(&i_proc, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(&max_proc, 1, MPI_INT, 0, MPI_COMM_WORLD);
            //printf("...\n");
            if(i_row >= first_row && i_row <= last_row && max_row >= first_row && max_row <= last_row) {//both rows are in the same process
                //printf("%d\n", rank);
                //sleep(8);
                t_swap_rows_in_the_same_process += MPI_Wtime() - t_swap_rows_in_the_same_process;
                LUP_mpi_swap_rows(rows, i_row, max_row, first_row, N);
                t_swap_rows_in_the_same_process += MPI_Wtime() - t_swap_rows_in_the_same_process;
            } else if(i_row >= first_row && i_row <= last_row) {
                t_swap_rows_in_different_processes += MPI_Wtime() - t_swap_rows_in_different_processes;
                MPI_Sendrecv_replace(rows + (i_row - first_row)*N, N, MPI_DOUBLE, max_proc, 0, max_proc, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                t_swap_rows_in_different_processes += MPI_Wtime() - t_swap_rows_in_different_processes;
            } else if(max_row >= first_row && max_row <= last_row) {
                t_swap_rows_in_different_processes += MPI_Wtime() - t_swap_rows_in_different_processes;
                MPI_Sendrecv_replace(rows + (max_row - first_row)*N, N, MPI_DOUBLE, i_proc, 0, i_proc, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                t_swap_rows_in_different_processes += MPI_Wtime() - t_swap_rows_in_different_processes;
            }
            //spread C[i][i] across processes
            if(i >= first_row && i <=last_row) {
                MPI_Bcast(rows + (i - first_row)*N + i, 1, MPI_DOUBLE, rank, MPI_COMM_WORLD); 
                Cii = rows[(i-first_row)*N + i];
            } else {
                MPI_Bcast(&Cii, 1, MPI_DOUBLE, i_proc, MPI_COMM_WORLD);
            }
        }
        if(rank == 0) {
            MPI_Gather(pivot_values, 1, MPI_DOUBLE, pivot_values, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
            MPI_Gather(pivot_rows, 1, MPI_INT, pivot_rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
            proc_max_value = 0;
            proc_max_row = -1;
            int proc_last_row;
            int max_proc = -1;
            for(proc = 1; proc < size; proc++) {
                if(proc == size - 1) proc_last_row = N-1;
                else proc_last_row = proc*rows_per_process - 1;
                if(i <= proc_last_row) {
                    if(pivot_values[proc] > proc_max_value) {
                        proc_max_value = pivot_values[proc];
                        proc_max_row = pivot_rows[proc];
                        max_proc = proc;
                    }
                }
            }
            //swap rows in P
            //if(i == 3)
            //    sleep(10);
            t_swap_rows_in_P += MPI_Wtime() - t_swap_rows_in_P;
            int tmp_i, tmp_r1, tmp_r2;
            for(tmp_i = 0; tmp_i < N; tmp_i++) {
                if(P[tmp_i] == proc_max_row)
                    tmp_r1 = tmp_i;
                if(P[tmp_i] == i)
                    tmp_r2 = tmp_i;
            }
            int tmp = P[tmp_r1];
            P[tmp_r1] = P[tmp_r2];
            P[tmp_r2] = tmp;
            t_swap_rows_in_P += MPI_Wtime() - t_swap_rows_in_P;
            //tmp = P[proc_max_row];
            //P[proc_max_row] = P[i];
            //P[i] = tmp;
            //printf("%d %d %d %d %d, SWAP %d %d\n", i, P[0], P[1], P[2], P[3], proc_max_row, i);
            //int i_proc;
            MPI_Recv(&i_proc, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            //swap rows in C (swap row i with row proc_max_row, proc_max_row is in process max_proc)
            MPI_Bcast(&i, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(&proc_max_row, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(&i_proc, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(&max_proc, 1, MPI_INT, 0, MPI_COMM_WORLD);
            //printf("i=%d, max_value=%f, max_row=%d\n", i, proc_max_value, proc_max_row);
            //printf("C[%d][%d]=%f\n", i, i, dtmp);
            double dtmp;//not used, just to receive broadcasted C[i][i]
            MPI_Bcast(&dtmp, 1, MPI_DOUBLE, i_proc, MPI_COMM_WORLD);
            MPI_Bcast(prev_row, N, MPI_DOUBLE, i_proc, MPI_COMM_WORLD);//meaningless for root
        }//pivot row found

        if(rank > 0) {
            //if(i == 0)
            //    sleep(9);
            if(rank == size - 1) last_row = N-1;
            else last_row = rank*rows_per_process - 1;
            first_row = (rank-1)*rows_per_process;
            if(rank == i_proc)
                MPI_Bcast(rows + (i_row - first_row)*N, N, MPI_DOUBLE, i_proc, MPI_COMM_WORLD);
            else
                MPI_Bcast(prev_row, N, MPI_DOUBLE, i_proc, MPI_COMM_WORLD);
            int j, k;
            int start, end;
            //if(i == 0)
            //    sleep(10);
            /*if(i == last_row && rank < size-1) {
                MPI_Send(rows + (i-first_row)*N, N, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD);
            } else if((i+1) == first_row) {
                MPI_Recv(prev_row, N, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }*/
            /*if(rank == 1 && size > 2) {
                MPI_Send(rows + (last_row - first_row)*N, N, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD);
            } else if(rank < size - 1) {
                MPI_Sendrecv(rows + (last_row - first_row)*N, N, MPI_DOUBLE, rank+1, 0, prev_row, N, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            } else if(rank == size-1) {
                MPI_Recv(prev_row, N, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }*/
            //if((i+1) >= first_row && (i+1) <= last_row) {
            t_calcs += MPI_Wtime() - t_calcs;
            if((i+1) <= last_row) {
                if((i+1) >= first_row)
                    start = i+1;
                else
                    start = first_row;
                end = last_row - first_row;
                start = start - first_row;
                for(j = start; j <= end; j++) {
                    //if(j >= first_row && j <= last_row) {
                        rows[j*N + i] /= Cii;
                        for(k = i+1; k < N; k++) {
                            //rows[j*N + k] -= rows[j*N + i] * rows[i*N + k];;
                            //if(i+1 == first_row)
                            if(rank == i_proc)
                                rows[j*N + k] -= rows[j*N + i] * rows[(i-first_row) * N + k];
                            else
                                rows[j*N + k] -= rows[j*N + i] * prev_row[k];
                            //else
                            //    rows[j*N + k] -= rows[j*N + i] * rows[(i-first_row)*N + k];
                        }
                    //}
                }
            }
            t_calcs += MPI_Wtime() - t_calcs;
        }
        
    }
    if(rank == 0) {
        int *recvcounts = (int*)malloc(sizeof(int) * size);
        int *displs = (int*)malloc(sizeof(int) * size);
        recvcounts[0] = displs[0] = 0;//rank=0 does not send anything (and does not receive, too)
        for(i = 1; i < size; i++) {
            if(i == size-1) {
                recvcounts[i] = rows_per_last_process * N;
            } else {
                recvcounts[i] = rows_per_process * N;
            }
            displs[i] = (i-1) * recvcounts[i-1];
        }
        MPI_Gatherv(C, 0, MPI_DOUBLE, C, recvcounts, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        free(recvcounts);
        free(displs);
        int k;
        t_start_data_out = MPI_Wtime();
        printf("C\n");
        for(i = 0; i < N; i++) {
            for(k = 0; k < N; k++) {
                printf("%f\t", C[i*N + k]);
            }
            printf("\n");
        }
        printf("P\n");
        for(i = 0; i < N; i++) {
            for(k = 0; k < N; k++) {
                if(i == P[k]) {
                    printf("1\t");
                } else {
                    printf("0\t");
                }
            }
            printf("\n");
        }
        t_end_data_out = MPI_Wtime();
    }
    if(rank > 0) {
        //sleep(9);
        MPI_Gatherv(rows, N*rp, MPI_DOUBLE, NULL, NULL, NULL, MPI_DOUBLE, 0, MPI_COMM_WORLD);    
    }

    //sleep(10);
    if(rank >= 1) {
        free(prev_row);
    }

    if(rank == 0) {
        LUP_mpi_matrix_free(&A);
        LUP_mpi_matrix_free(&C);
        free(P);
        free(pivot_values);
        free(pivot_rows);
        fprintf(stderr, "scatter data. time = %f\n", t_scatter_data - t_start);
        fprintf(stderr, "swap rows in P. time = %f\n", t_swap_rows_in_P - t_start);
        fprintf(stderr, "DATA OUT. time = %f\n", t_end_data_out - t_start_data_out);
        fprintf(stderr, "Total time = %f\n", MPI_Wtime() - t_start);
    }
    if(rank > 0) {
        fprintf(stderr, "rank = %d, swap rows in the same process. time = %f\n", rank, t_swap_rows_in_the_same_process - t_start);
        fprintf(stderr, "rank = %d, swap rows in different processes. time = %f\n", rank, t_swap_rows_in_different_processes - t_start);
        fprintf(stderr, "rank = %d, Calculation. time = %f\n", rank, t_calcs - t_start);
        free(rows);
        rows = NULL;
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}