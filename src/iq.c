/*
    Copyright 2018, Andrew C. Young <andrew@vaelen.org>

    This file is part of Beacon

    Beacon is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Beacon is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Beacon.  If not, see <https://www.gnu.org/licenses/>.

*/

#include "iq.h"

struct sample_table generate_sample_table(long freq, long samp_rate)
{
    assert(samp_rate >= 2 * freq);
    double two_pi = 2.0 * PI;
    long rate = (long)((double)samp_rate / (double)freq);
    double step = two_pi / rate;

    double *samples = malloc(rate * sizeof(double));

    double theta = 0;
    
    for (int i = 0; i < rate; i++)
    {
      theta += step;
      samples[i] = theta;
    }

    struct sample_table table;
    table.samples = samples;
    table.len = rate;
    
    return table;
}


struct iq_state* init_state(long freq, long samp_rate)
{
    struct iq_state *state = malloc(sizeof(struct iq_state));
    state->start = 0;
    state->table = generate_sample_table(freq, samp_rate);
    state->iq.values = NULL;
    state->samples.samples = NULL;
    return state;
}

void destroy_iq_state(struct iq_state *state)
{
    if (state != NULL)
    {
        if (state->table.samples != NULL)
        {
            free(state->table.samples);
        }
        if (state->samples.samples != NULL)
        {
            free(state->samples.samples);
        }
        if (state->iq.values != NULL)
        {
            free(state->iq.values);
        }
        free(state);
    }
}

struct iq_state *generate_tone(long freq, long samp_rate, double *samples, int samples_len, struct iq_state *state)
{
    double theta = 0.0;
    double value = 0.0;
    int index = 0;

    if (state == NULL)
    {
        state = init_state(freq, samp_rate);
    }

    if (state->samples.samples == NULL)
    {
        state->samples.len = state->table.len;
        state->samples.samples = malloc(sizeof(double)*state->table.len);
        for (index = 0; index < state->samples.len; index++)
        {
            theta = state->table.samples[index];
            value = cos(theta);
            state->samples.samples[index] = value;
#ifdef DEBUG
            fprintf(stderr, "freq: %ld, samp_rate: %ld, index: %d, len: %ld, theta: %0.3f, value: %0.3f\n", freq, samp_rate, index, state->samples.len, theta, i, q);
#endif
        }
    }

    long start = state->start;
    double *current = samples;
    long left = samples_len;

    double *precomputed = state->samples.samples;
    long precomputed_len = state->samples.len;

    if (start > 0)
    {
#ifdef DEBUG
        fprintf(stderr, "Align Tone, start: %ld\n", start);
#endif
        // Align to the start of the pre-computed IQ
        double *align = precomputed += start;
        long align_len = precomputed_len-start;
        memcpy(current, align, align_len);
        current += align_len;
        left -= align_len;
    }

    while (left >= precomputed_len)
    {
#ifdef DEBUG
        fprintf(stderr, "Copy Tone, left: %ld\n", left);
#endif
        memcpy(current, precomputed, precomputed_len);
        current += precomputed_len;
        left -= precomputed_len;
    }

    if (left > 0)
    { 
#ifdef DEBUG
        fprintf(stderr, "Fill Remaining Tone, left: %ld\n", left);
#endif
        // Fill the remaining samples
        memcpy(current, precomputed, left);
    }

    state->start = left;

    return state;
}

struct iq_state* generate_carrier(long freq, long samp_rate, complex *iq, int iq_len, struct iq_state *state)
{
    double i = 0.0, q = 0.0, theta = 0.0;
    int index = 0;

    if (state == NULL)
    {
        state = init_state(freq, samp_rate);
    }
    if (state->iq.values == NULL)
    {
        state->iq.len = state->table.len;
        state->iq.values = malloc(sizeof(complex)*state->iq.len);
        for (index = 0; index < state->iq.len; index++)
        {
            theta = state->table.samples[index];
            i = cos(theta);
            q = sin(theta);
#ifdef DEBUG
            fprintf(stderr, "freq: %ld, samp_rate: %ld, index: %d, len: %ld, theta: %f, i: %f, q: %f\n", freq, samp_rate, index, state->iq.len, theta, i, q);
#endif
            state->iq.values[index] = q + i * I;
        }
    }

    long start = state->start;
    complex *current = iq;
    long left = iq_len;

    complex *precomputed = state->iq.values;
    long precomputed_len = state->iq.len;

    if (start > 0)
    {
#ifdef DEBUG
        fprintf(stderr, "Align Carrier IQ, start: %ld\n", start);
#endif
        // Align to the start of the pre-computed IQ
        complex *align = precomputed += start;
        long align_len = precomputed_len-start;
        memcpy(current, align, align_len);
        current += align_len;
        left -= align_len;
    }

    while (left >= precomputed_len)
    {
#ifdef DEBUG
        fprintf(stderr, "Copy Carrier IQ, left: %ld\n", left);
#endif
        memcpy(current, precomputed, precomputed_len);
        current += precomputed_len;
        left -= precomputed_len;
    }

    if (left > 0)
    { 
#ifdef DEBUG
        fprintf(stderr, "Fill Remaining Carrier IQ, left: %ld\n", left);
#endif
        // Fill the remaining samples
        memcpy(current, precomputed, left);
    }

    state->start = left;
    
    return state;
}

void modulate_am(complex *carrier, double *baseband, int iq_len, double modulation_index)
{
    for (int index = 0; index < iq_len; index++)
    {
        double carrier_i = cimag(carrier[index]);
        double carrier_q = creal(carrier[index]);

        // Modulate the baseband onto the carrier.
        // This produces double side bands.
        double modulated_i = carrier_i * baseband[index] * modulation_index;
        double modulated_q = carrier_q * baseband[index] * modulation_index;

        // Make sure the carrier has an acceptable amplitude.
        // This will keep squelch open on receivers.
        carrier_i *= modulation_index / 10;
        carrier_q *= modulation_index / 10;

        carrier[index] = (carrier_q + carrier_i * I) + (modulated_q + modulated_i * I);
    }
}

/** TODO: This does not work yet. */
void modulate_fm(complex *carrier, double *baseband, int iq_len, double modulation_index)
{
    for (int index = 0; index < iq_len; index++)
    {
        double carrier_i = cimag(carrier[index]);
        double carrier_q = creal(carrier[index]);

        double theta_i = acos(carrier_i);
        double theta_q = asin(carrier_q);

        double modulated_i = sin(2*PI*((carrier_i + modulation_index) * baseband[index]));
        double modulated_q = cos(2*PI*((carrier_q + modulation_index) * baseband[index]));

        carrier[index] = modulated_q + modulated_i * I;
    }
}

void write_iq(FILE *out, complex *iq, int iq_len)
{
    uint32_t buf[iq_len*2];
    for (int index = 0; index < iq_len; index++)
    {
        double i = cimag(iq[index]);
        double q = creal(iq[index]);
        buf[(index*2)] = htole32((uint32_t)i);
        buf[(index*2)+1] = htole32((uint32_t)q);
                    
    }
    fwrite(buf, sizeof(uint32_t), iq_len*2, out);
    fflush(out);
}
