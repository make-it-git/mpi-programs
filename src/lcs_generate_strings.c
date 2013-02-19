#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv) {
    if(argc != 5) {
        fprintf(stderr, "Usage: %s file_for_str1 str1_length file_for_str2 str2_length\n", argv[0]);
        return 1;
    }
    char *filename1, *filename2;
    filename1 = argv[1];
    filename2 = argv[3];
    int str1_len = atoi(argv[2]);
    int str2_len = atoi(argv[4]);
    FILE *f1, *f2;
    f1 = fopen(filename1, "wb");
    if(!f1) {
        fprintf(stderr, "Unable to open file '%s' for writing.\n", filename1);
        return 1;
    }
    f2 = fopen(filename2, "wb");
    if(!f2) {
        fprintf(stderr, "Unable to open file '%s' for writing.\n", filename2);
        fclose(f1);
        return 1;
    }
    srand(time(NULL));
    int i;
    unsigned char c;
    for(i = 0; i < str1_len; i++) {
        c = (unsigned char)abs(rand()) % 26 + 'A';
        fprintf(f1, "%c", c);
    }
    for(i = 0; i < str2_len; i++) {
        c = (unsigned char)abs(rand()) % 26 + 'A';
        fprintf(f2, "%c", c);
    }
    fclose(f1);
    fclose(f2);
    return 0;
}
