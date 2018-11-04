/*
	Copyright 2018, Andrew C. Young <andrew@vaelen.org>

    This file is part of Adalm-Beacon

    Adalm-Beacon is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Adalm-Beacon is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Adalm-Beacon.  If not, see <https://www.gnu.org/licenses/>.

*/

#include "iq.h"

double generate_signal(int freq, double amplitude, long samp_rate, complex *iq, int iq_len, double start)
{

    assert(samp_rate >= 2*freq);

    double two_pi = 2.0*PI;
    double i = 0.0, q = 0.0;
    double rate = (double)samp_rate/(double)freq;
	double step = two_pi / rate;
    double end = two_pi - step;

    double theta = start;

    for (int index = 0; index < iq_len; index++)
    {
        if (theta > end)
        {
            theta = 0;
        }

        i = amplitude * sin(theta);
        q = amplitude * cos(theta);

        #ifdef DEBUG
        fprintf(stderr, "freq: %d, amplitude: %f, samp_rate: %d, index: %d, rate: %f, step: %f, end: %f, theta: %f, i: %f, q: %f\n", freq, amplitude, samp_rate, index, rate, step, end, theta, i, q);
        #endif

        iq[index] = q + i*I;

        theta += step;
	}
    return theta;
}

void write_iq(FILE *out, complex *iq, int iq_len)
{
    fwrite(iq, sizeof(complex), iq_len, out);
    fflush(out);
}