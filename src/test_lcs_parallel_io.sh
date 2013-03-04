#!/bin/bash
N=( 100 150 200 300 500 )
PROC_COUNT=( 2 5 7 8 9 13 17 )
TMP_STR1="/tmp/str1"
TMP_STR2="/tmp/str2"
TMP_PAR="/tmp/par_io_out"
TMP_P="/tmp/P"
TMP_S="/tmp/S"
TMP_SEQ="/tmp/lcs_seq_out"
TMP_MD5="/tmp/lcs_md5"
echo Testing...
echo \'diff\' output indicates problems
for S in ${N[@]}; do
    let "STR1_LEN = $RANDOM % $N"
    let "STR2_LEN = $RANDOM % $N"
    ./bin/lcs_generate_strings "$TMP_STR1" "$STR1_LEN" "$TMP_STR2" "$STR2_LEN"
    echo Testing str1 length = $STR1_LEN, str2 length = $STR2_LEN
    ./bin/lcs_sequential "$TMP_STR1" "$TMP_STR2" "$TMP_SEQ"
    mpirun -np 1 ./bin/lcs_parallel_io "$TMP_STR1" "$TMP_STR2" "$TMP_PAR" "$TMP_P" "$TMP_S"
    md5sum "$TMP_P" "$TMP_S" > "$TMP_MD5"
    for i in ${PROC_COUNT[@]}; do
        mpirun -np $i ./bin/lcs_parallel_io "$TMP_STR1" "$TMP_STR2" "$TMP_PAR" "$TMP_P" "$TMP_S"
        echo Number of processes = $i
        diff "$TMP_SEQ" "$TMP_PAR"
        md5sum -c --quiet "$TMP_MD5"
    done 
done
