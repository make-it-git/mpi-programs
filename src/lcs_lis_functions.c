#include "lcs_lis_functions.h"

int lower_bound(int **dec_sequences, int *dec_sequences_lengths, int dec_sequences_count, int letter_position) {
    int left = 0;
    int right = dec_sequences_count - 1;
    int medium;
    while(left < right) {
        medium = left + (right - left)/2;
        if(letter_position <= dec_sequences[medium][dec_sequences_lengths[medium] - 1])
            right = medium;
        else
            left = medium + 1;
    }
    return letter_position <= dec_sequences[left][dec_sequences_lengths[left] - 1] ? left : -1;
}
