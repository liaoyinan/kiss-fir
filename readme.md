# KISS FIR

Keep It Simple, Stupid. [中文](./readme-zh.md)

KISS FIR is a particularly easy to use and embedded - friendly general purpose real-time FIR(Finite Impulse Response) filter.

This code realizes the low pass, high pass, band-pass and band-stop filter base on the two window functions of <u>Hamming</u> and <u>Blackman</u>. Trust me, you've never seen code so simple that I don't feel the need to comment it.

### USAGE：

```c
#include <stdio.h>
#include "fir.h"

int main()
{
    float in, out;
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
```

1、`create_low_pass_fir(20, 5, 100, HAMMING)` is  used to create a 20th order low-pass filter with a sampling frequency of 100Hz and a cut-off frequency of 5Hz. the function `create_low_pass_fir` returns the filter coefficient and the space required for the filter, or NULL if an exception is encountered

2、The data is filtered by `fir_filter(fir20, in)`, and the filtering result is returned by the function

3、After the filtering is completed, the space allocated by filtering is released by `fir_free(fir20)`

The low-pass filtering results are as follows:

![lowpass](.\lowpass.jpg)



### LICENSE

MIT

