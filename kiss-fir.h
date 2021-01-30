//
// Created by liaoy on 2021/1/27.
//

#ifndef FIR1_FIR_H
#define FIR1_FIR_H

#include <stdbool.h>

typedef struct
{
    bool is_init;
    int m;
    float *cof;
    float *buf;
    int buf_idx;
    bool buf_is_full;
} fir_t;

typedef enum
{
    HAMMING,
    BLACKMAN,
} kernel_window_e;
//
fir_t *create_low_pass_fir(int order, float frequency_cutoff, float sample_rate, kernel_window_e window);

fir_t *create_high_pass_fir(int order, float frequency_cutoff, float sample_rate, kernel_window_e window);

fir_t *create_band_pass_fir(int order, float low_frequency_band, float high_frequency_band, float sample_rate,
                            kernel_window_e window);

fir_t *create_band_stop_fir(int order, float low_frequency_band, float high_frequency_band, float sample_rate,
                            kernel_window_e window);

float fir_filter(fir_t *filter, float sample);

void fir_free(fir_t *filter);

#endif //FIR1_FIR_H
