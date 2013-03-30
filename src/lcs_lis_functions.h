#ifndef _LCS_LIS_FUNCTIONS_H_
#define _LCS_LIS_FUNCTIONS_H_

#include <mpi.h>

int lower_bound(int **dec_sequences, int *dec_sequences_lengths, int dec_sequences_count, int letter_position);

#endif
