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

/* File adalm.h */
#ifndef FILE_ADALM_H_SEEN
#define FILE_ADALM_H_SEEN

#include "../config.h"
#include "global.h"

#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <complex.h>

#include <iio.h>
#include <ad9361.h>

enum frequency
{
    FREQ_S = 1294500000,
    FREQ_U = 432320000,
};

// https://ez.analog.com/university-program/f/q-a/77879/adalm-pluto-sample-rate
enum rate
{
    RATE_520K = 520000,
    RATE_1M = 1000000,
    RATE_5M = 5000000,
};

void adalm_enable_tx();
void adalm_disable_tx();
void adalm_enable_rx();
void adalm_disable_rx();
void adalm_init(char *uri, long samp_rate, long tx_freq, int buf_len);
void adalm_transmit(complex *iq, int iq_len);
void adalm_shutdown();

#endif /* !FILE_ADALM_H_SEEN */