#!/bin/bash
N=( 5 10 12 15 17 25 30 50 100 )
PROC_COUNT=( 2 3 4 5 10 15 )
TMP_MATR="/tmp/Matrix"
TMP_SEQ="/tmp/Sequential"
TMP_PAR="/tmp/Parallel_RMA"
echo Testing...
echo \'diff\' output indicates problems
for S in ${N[@]}; do
    ./bin/lup_generate_matrix $TMP_MATR $S
    echo Testing size = $S
    ./bin/lup_sequential "$TMP_MATR" "$S" "$TMP_SEQ"
    for i in ${PROC_COUNT[@]}; do
        mpirun -np $i ./bin/lup_parallel_rma "$TMP_MATR" "$S" "$TMP_PAR"
        echo Number of processes = $i
        diff "$TMP_SEQ" "$TMP_PAR"
    done 
done
