#!/bin/bash
N=( 100 200 300 400 )
PROC_COUNT=( 2 3 4 5 6 7 )
TMP_MATR="/tmp/Matrix"
TMP_SEQ="/tmp/Sequential"
TMP_PAR="/tmp/Parallel_RMA"
echo Testing...
echo \'diff\' output indicates problems
for S in ${N[@]}; do
    let "SIZE = $RANDOM % $N"
    ./bin/lup_generate_matrix $TMP_MATR $SIZE
    echo Testing size = $SIZE
    ./bin/lup_sequential "$TMP_MATR" "$SIZE" "$TMP_SEQ"
    for i in ${PROC_COUNT[@]}; do
        mpirun -np $i ./bin/lup_parallel_rma "$TMP_MATR" "$SIZE" "$TMP_PAR"
        echo Number of processes = $i
        diff "$TMP_SEQ" "$TMP_PAR"
    done 
done
