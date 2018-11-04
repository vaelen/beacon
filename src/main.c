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


#include "main.h"

const int IQ_BUFFER_SIZE = 4096;

void main (int argc, char **argv)
{
	signal(SIGINT, handle_sig);
 
    int tone = 600;
    double amp = 10;
    long samp_rate = 48000;
    complex iq[IQ_BUFFER_SIZE];
    double start = 0;
    long samples = 0;

    fprintf(stderr, "Writing IQ Data - Sampling Rate: %d ", samp_rate);
    while (!stop)
    {
	    start = generate_signal(tone, amp, samp_rate, iq, IQ_BUFFER_SIZE, start);
        write_iq(stdout, iq, IQ_BUFFER_SIZE);
        samples += IQ_BUFFER_SIZE;
        fprintf(stderr, ".");
    }
    fprintf(stderr, " Wrote %d Samples.\n", samples);
} 