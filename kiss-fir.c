//
// Created by liaoy on 2021/1/27.
//

#include "kiss-fir.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define FIR_F_PI 3.14159265358979323846f
#define FIR_EPS  1e-6f
#define FIR_ABS(a, b) ((a) - (b) >= 0 ? ((a) - (b)) : ((b) - (a)))

static inline fir_t *fir_alloc(int order)
{
    fir_t *fir = malloc(sizeof(fir_t));
    if (fir != NULL)
    {
        fir->is_init = false;
        if (order % 2 == 0)
        {
            order += 1;
        }
        fir->m = order;
        fir->cof = malloc((fir->m) * sizeof(float));
        fir->buf = malloc((fir->m) * sizeof(float));
        fir->buf_idx = 0;
        fir->buf_is_full = false;

        if (fir->cof != NULL && fir->buf != NULL)
        {
            fir->is_init = true;
        }
    }
    return fir;
}

static void apply_window(float *cof, int len, kernel_window_e window)
{
    float m = (float) len - 1;
    switch (window)
    {
        case HAMMING:
            for (int i = 0; i < len; i++)
            {
                cof[i] *= 0.54f + 0.46f * cosf(2.0f * FIR_F_PI * ((float) i / m - 0.5f));
            }
            break;
        case BLACKMAN:
            for (int i = 0; i < len; i++)
            {
                cof[i] *= (0.42f
                           + 0.5f * cosf(2.0f * FIR_F_PI * ((float) i / m - 0.5f))
                           + 0.08f * cosf(4.0f * FIR_F_PI * ((float) i / m - 0.5f)));
            }
            break;
        default:
            for (int i = 0; i < len; i++)
            {
                cof[i] *= 0.54f + 0.46f * cosf(2.0f * FIR_F_PI * ((float) i / m - 0.5f));
            }
            break;
    }
}

static void compute_fir_sinc(float *cof, int len, float frequency_cutoff, float sample_rate)
{
    float half_len = (float) (len / 2);
    float fc = frequency_cutoff / sample_rate;
    for (int i = 0; i < len; i++)
    {
        if (FIR_ABS((float) i, half_len) < FIR_EPS)
        {
            cof[i] = 2.0f * FIR_F_PI * fc;
        }
        else
        {
            cof[i] = sinf(2.0f * FIR_F_PI * fc * ((float) i - half_len)) / ((float) i - half_len);
        }
    }
}

static void fir_cof_normalized(float *cof, int len)
{
    float fir_cof_sum = 0;
    for (int i = 0; i < len; i++)
    {
        fir_cof_sum += cof[i];
    }
    if (fir_cof_sum == 0)
    {
        return;
    }
    for (int i = 0; i < len; i++)
    {
        cof[i] /= fir_cof_sum;
    }
}

static void fir_reverse(float *cof, int len)
{
    for (int i = 0; i < len; i++)
    {
        cof[i] = -1.0f * cof[i];
    }
    cof[len - 1] += 1.0f;
}

static void compute_convolution(float *product, const float *cof1, int len1, const float *cof2, int len2)
{
    memset(product, 0, (len1 + len2 - 1) * sizeof(float));
    for (int i = 0; i < len1; i++)
    {
        for (int j = 0; j < len2; j++)
        {
            product[i + j] += cof1[i] * cof2[j];
        }
    }
}

fir_t *create_low_pass_fir(int order, float frequency_cutoff, float sample_rate, kernel_window_e window)
{
    if (order < 1 || frequency_cutoff * 2 > sample_rate)
    {
        return NULL;
    }

    fir_t *fir = fir_alloc(order);
    if (fir != NULL && fir->is_init)
    {
        compute_fir_sinc(fir->cof, fir->m, frequency_cutoff, sample_rate);
        apply_window(fir->cof, fir->m, window);
        fir_cof_normalized(fir->cof, fir->m);
    }
    return fir;
}

fir_t *create_high_pass_fir(int order, float frequency_cutoff, float sample_rate, kernel_window_e window)
{
    if (order < 1 || frequency_cutoff * 2 > sample_rate)
    {
        return NULL;
    }
    fir_t *fir = fir_alloc(order);
    if (fir != NULL && fir->is_init)
    {
        compute_fir_sinc(fir->cof, fir->m, frequency_cutoff, sample_rate);
        apply_window(fir->cof, fir->m, window);
        fir_cof_normalized(fir->cof, fir->m);
        fir_reverse(fir->cof, fir->m);
    }
    return fir;
}

fir_t *create_band_pass_fir(int order, float low_frequency_band, float high_frequency_band, float sample_rate,
                            kernel_window_e window)
{
    if (order < 1 || low_frequency_band >= high_frequency_band)
    {
        return NULL;
    }
    fir_t *fir = fir_alloc(order);
    int len = order / 2 + 1;
    float fir_low[len];
    float fir_high[len];
    if (fir != NULL && fir->is_init)
    {
        // Create the low pass finite impulse response
        compute_fir_sinc(fir_low, len, high_frequency_band, sample_rate);
        apply_window(fir_low, len, window);
        fir_cof_normalized(fir_low, len);

        // Create the high pass finite impulse response
        compute_fir_sinc(fir_high, len, low_frequency_band, sample_rate);
        apply_window(fir_high, len, window);
        fir_cof_normalized(fir_high, len);
        fir_reverse(fir_high, len);

        // compute the convolution product of the two FIR
        compute_convolution(fir->cof, fir_low, len, fir_high, len);
    }
    return fir;
}

fir_t *create_band_stop_fir(int order, float low_frequency_band, float high_frequency_band, float sample_rate,
                            kernel_window_e window)
{
    if (order < 1 || low_frequency_band >= high_frequency_band)
    {
        return NULL;
    }
    fir_t *fir = fir_alloc(order);
    int len = order / 2 + 1;
    float fir_low[len];
    float fir_high[len];
    if (fir != NULL && fir->is_init)
    {
        // Create the low pass finite impulse response
        compute_fir_sinc(fir_low, len, high_frequency_band, sample_rate);
        apply_window(fir_low, len, window);
        fir_cof_normalized(fir_low, len);

        // Create the high pass finite impulse response
        compute_fir_sinc(fir_high, len, low_frequency_band, sample_rate);
        apply_window(fir_high, len, window);
        fir_cof_normalized(fir_high, len);
        fir_reverse(fir_high, len);

        // compute the convolution product of the two FIR
        compute_convolution(fir->cof, fir_low, len, fir_high, len);
        fir_reverse(fir->cof, fir->m);
    }
    return fir;
}

#define INCREASE_IDX(i) ((i) >= filter->m ? (i - filter->m) : (i))

float fir_filter(fir_t *filter, float sample)
{
    float fir_result = 0;
    if (filter == NULL || !filter->is_init)
    {
        return sample;
    }

    filter->buf[filter->buf_idx] = sample;
    filter->buf_idx += 1;
    if (filter->buf_idx == filter->m)
    {
        filter->buf_idx = 0;
        filter->buf_is_full = true;
    }

    if (!filter->buf_is_full)
    {
        for (int i = 0; i < filter->m; i++)
        {
            fir_result += filter->cof[i] * filter->buf[i % filter->buf_idx];
        }
        return fir_result;
    }
    int cur_buf_idx = filter->buf_idx;
    for (int i = 0; i < filter->m; i++)
    {
        int iter_idx = INCREASE_IDX(cur_buf_idx + i);
        fir_result += filter->cof[i] * filter->buf[iter_idx];
    }

    return fir_result;
}

void fir_free(fir_t *filter)
{
    if (filter != NULL)
    {
        if (filter->cof != NULL)
        {
            free(filter->cof);
        }
        if (filter->buf != NULL)
        {
            free(filter->buf);
        }
        free(filter);
        filter = NULL;
    }
}
