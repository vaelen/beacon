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

/* File iq.h */
#ifndef FILE_IQ_H_SEEN
#define FILE_IQ_H_SEEN

#include "../config.h"

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <complex.h>
#include <stdint.h>
#include <endian.h>

#define PI 3.14159265

/** Generate a carrier signal at the given frequency */
long generate_tone(long freq, long samp_rate, double *samples, int sample_len, long start);

/** Generate a carrier signal at the given frequency */
long generate_carrier(long freq, long samp_rate, complex *iq, int iq_len, long start);

/** Module a baseband signal onto a carrier signal using amplitude modulation, overwriting the carrier IQ data. */
void modulate_am(complex *carrier, double *baseband, int iq_len, double modulation_index);

/** Module a baseband signal onto a carrier signal using frequency modulation, overwriting the carrier IQ data. */
void modulate_fm(complex *carrier, double *baseband, int iq_len, double modulation_index);

/** Write the given IQ data to a file. */
void write_iq(FILE *out, complex *iq, int iq_len);

#endif /* !FILE_IQ_H_SEEN */
