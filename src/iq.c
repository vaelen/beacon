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

//#define DEBUG

struct sample_table
{
  double *samples;
  long len;
};

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

long generate_tone(long freq, long samp_rate, double *samples, int samples_len, long start)
{

    double theta = 0.0;
  
    struct sample_table table = generate_sample_table(freq, samp_rate);
    
    for (int index = 0; index < samples_len; index++)
    {
        start = (start + 1) % table.len;
        theta = table.samples[start];
        samples[index] = cos(theta);
    }

    free(table.samples);
    return start;
}

long generate_carrier(long freq, long samp_rate, complex *iq, int iq_len, long start)
{
    double i = 0.0, q = 0.0, theta = 0.0;

    struct sample_table table = generate_sample_table(freq, samp_rate);
    
    for (int index = 0; index < iq_len; index++)
    {
        start = (start + 1) % table.len;
        theta = table.samples[start];
        i = cos(theta);
        q = sin(theta);

#ifdef DEBUG
        fprintf(stderr, "freq: %ld, samp_rate: %ld, index: %d, theta: %f, i: %f, q: %f\n", freq, samp_rate, index, theta, i, q);
#endif

        iq[index] = q + i * I;

    }

    free(table.samples);
    return start;
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
