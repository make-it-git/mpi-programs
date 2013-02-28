#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int main(int argc, char **argv) {
    if(argc != 3) {
        fprintf(stderr, "Usage: %s filename N\n", argv[0]);
        fprintf(stderr, "\tN denotes array size\n");
        return 1;
    }
    char *filename;
    filename = argv[1];
    int N = atoi(argv[2]);
    FILE *f;
    f = fopen(filename, "wb");
    if(!f) {
        fprintf(stderr, "Unable to open file '%s' for writing.\n", filename);
        return 1;
    }
    srand(time(NULL));
    int i, k;
    double d;
    double t1, t2;
    for(i = 0; i < N; i++) {
        for(k = 0; k < N; k++) {
            /*t1 = fabs(rand());
            t2 = fabs(rand());
            if(t2 < 1E-6) t2 = t2 + 0.1;
            d = t1/t2; */
            d = i*N + k;
            fwrite(&d, sizeof(double), 1, f);
            printf("%f\t", d);
        }
        printf("\n");
    }
    fclose(f);
    return 0;
}
