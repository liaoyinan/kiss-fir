//
// Created by liaoy on 2021/1/27.
//

#ifndef FIR1_FIR_H
#define FIR1_FIR_H

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    int m;
    float *cof;
    float *buf;
    int buf_idx;
    bool buf_is_full;
    bool is_init;
} fir_t;

typedef void *(*fir_malloc_t)(size_t size);

typedef void (*fir_free_t)(void *p);

typedef void *(*fir_memset_t)(void *dst, int val, size_t size);

typedef enum
{
    HAMMING,
    BLACKMAN,
} kernel_window_e;

void set_fir_mem_config(fir_malloc_t p_fir_malloc, fir_free_t p_fir_free, fir_memset_t p_fir_memset);

fir_t *create_low_pass_fir(int order, float frequency_cutoff, float sample_rate, kernel_window_e window);

fir_t *create_high_pass_fir(int order, float frequency_cutoff, float sample_rate, kernel_window_e window);

fir_t *create_band_pass_fir(int order, float low_frequency_band, float high_frequency_band, float sample_rate,
                            kernel_window_e window);

fir_t *create_band_stop_fir(int order, float low_frequency_band, float high_frequency_band, float sample_rate,
                            kernel_window_e window);

float fir_filter(fir_t *filter, float sample);

void fir_free(fir_t *filter);

#endif //FIR1_FIR_H
