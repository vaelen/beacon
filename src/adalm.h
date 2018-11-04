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

#include "../config.h"

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

const char* DEV_NAME = "ad9361-phy";
const char* RX_DEV_NAME = "cf-ad9361-lpc";
const char* TX_DEV_NAME = "cf-ad9361-dds-core-lpc";

struct iio_context *ctx;
struct iio_device *phy;
struct iio_device *tx, *rx;
struct iio_channel *tx0_i, *tx0_q;
struct iio_buffer *txbuf;

static void shutdown();
void enable_tx();
void disable_rx();
void init(long samp_rate, long tx_frew);
void transmit(complex *iq, int iq_len);