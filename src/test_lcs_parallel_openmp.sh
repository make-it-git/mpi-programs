#!/bin/bash
N=( 1000 2000 3000 4000 5000 )
TMP_STR1="/tmp/str1"
TMP_STR2="/tmp/str2"
TMP_PAR="/tmp/par_out"
TMP_SEQ="/tmp/seq_out"
echo Testing...
echo \'diff\' output indicates problems
for S in ${N[@]}; do
    let "STR1_LEN = $RANDOM % $N"
    let "STR2_LEN = $RANDOM % $N"
    ./bin/lcs_generate_strings "$TMP_STR1" "$STR1_LEN" "$TMP_STR2" "$STR2_LEN"
    echo Testing str1 length = $STR1_LEN, str2 length = $STR2_LEN
    ./bin/lcs_sequential "$TMP_STR1" "$TMP_STR2" "$TMP_SEQ"
    ./bin/lcs_parallel_openmp "$TMP_STR1" "$TMP_STR2" "$TMP_PAR"
    diff "$TMP_SEQ" "$TMP_PAR"
done
rm -f "$TMP_STR1" "$TMP_STR2" "$TMP_PAR" "$TMP_SEQ"
