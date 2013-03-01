#!/bin/bash
#N=( 5 10 12 15 17 25 30 50 100 )
#PROC_COUNT=( 2 3 4 5 10 15 )
#TMP_MATR="/tmp/M"
#for S in ${N[@]}; do
#    ./bin/lup_generate_matrix $TMP_MATR $S
#    echo size=$S
#    #./bin/lup_sequential "$TMP_MATR" $S >! /tmp/seq
#    for i in ${PROC_COUNT[@]}; do
#        #mpirun -np $i ./bin/lup_parallel "$TMP_MATR" $S >! /tmp/par
#        echo $i
#        #diff /tmp/seq /tmp/par
#    done 
#done
