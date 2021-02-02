#include <stdio.h>
#include "kiss-fir.h"
#include <stdlib.h>
#include <string.h>

int main()
{
    float in, out;
    set_fir_mem_config(malloc, free, memset);
    fir_t *fir20 = create_low_pass_fir(20, 5, 100, HAMMING);
    FILE *fp = fopen("./data.txt", "r");
    FILE *fp_out = fopen("./data_out.txt", "w+");

    while (!feof(fp))
    {
        fscanf(fp, "%f", &in);
        out = fir_filter(fir20, in);
        fprintf(fp_out, "%f\n", out);
    }

    fclose(fp);
    fclose(fp_out);
    fir_free(fir20);
    return 0;
}