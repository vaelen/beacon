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

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>

#include <iio.h>
#include <ad9361.h>

#include "../config.h"

#define PI 3.14159265

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

static bool stop;
static long samp_rate;
static long tx_freq;

struct iio_context *ctx;
struct iio_device *phy;
struct iio_device *tx, *rx;
struct iio_channel *tx0_i, *tx0_q;
struct iio_buffer *txbuf;

static void handle_sig(int sig)
{
	printf("Waiting for process to finish...\n");
	stop = true;
}

static void shutdown()
{
	printf("* Destroying buffers\n");
	if (txbuf) { iio_buffer_destroy(txbuf); }

	printf("* Disabling streaming channels\n");
	if (tx0_i) { iio_channel_disable(tx0_i); }
	if (tx0_q) { iio_channel_disable(tx0_q); }

	printf("* Destroying context\n");
	if (ctx) { iio_context_destroy(ctx); }
	exit(0);
}

void enable_tx() 
{
	tx0_i = iio_device_find_channel(tx, "voltage0", true);
	tx0_q = iio_device_find_channel(tx, "voltage1", true);
	iio_channel_enable(tx0_i);
	iio_channel_enable(tx0_q);
}

void disable_rx()
{
	struct iio_channel *rx0_i, *rx0_q;
	rx0_i = iio_device_find_channel(rx, "voltage0", false);
	rx0_q = iio_device_find_channel(rx, "voltage1", false);
	iio_channel_disable(rx0_i);
	iio_channel_disable(rx0_q);
}

void init()
{
	phy = iio_context_find_device(ctx, DEV_NAME);
 
	// Set frequency
	iio_channel_attr_write_longlong(
		iio_device_find_channel(phy, "altvoltage1", true),
		"frequency",
		tx_freq);
 
  	// Set baseband sampling rate
	ad9361_set_bb_rate(phy, samp_rate);

	tx = iio_context_find_device(ctx, TX_DEV_NAME);
	rx = iio_context_find_device(ctx, RX_DEV_NAME);

	disable_rx();
	enable_tx();
}

void transmit()
{
	txbuf = iio_device_create_buffer(tx, samp_rate, false);
	if (!txbuf) {
		perror("Could not create TX buffer");
		shutdown();
	}

	size_t ntx = 0;
	int index = 1, sample = 1;
	double theta = 0.0;
	int i = 0, q = 0;

	int f = 800; // Hz
	double w = 2*PI*f/samp_rate;
	int phi = 0;

	int interpolation = samp_rate/f;

	//#define DEBUG

	int a = 128;

	while (!stop)
	{
		ssize_t nbytes_tx;
		char *p_dat, *p_end;
		ptrdiff_t p_inc;

		// WRITE: Get pointers to TX buf and write IQ to TX buf port 0
		p_inc = iio_buffer_step(txbuf);
		p_end = iio_buffer_end(txbuf);
		index = 1;
		for (p_dat = (char *)iio_buffer_first(txbuf, tx0_i); p_dat < p_end; p_dat += p_inc) 
		{
			#ifdef DEBUG
			printf("f: %d, interpolation: %d, sample: %d, index: %d, w: %f, ", f, interpolation, sample, index, w);
			#endif

			theta = (w*index) + phi;

			#ifdef DEBUG
			printf("theta: %f, ", theta);
			#endif

			i = a*sin(theta);
			q = a*cos(theta);

			#ifdef DEBUG
				printf("i: %d, q: %d\n", i, q);
			#endif

			// 12-bit sample needs to be MSB alligned so shift by 4
			// https://wiki.analog.com/resources/eval/user-guides/ad-fmcomms2-ebz/software/basic_iq_datafiles#binary_format
			((int16_t*)p_dat)[0] = i << 4; // Real (I)
			((int16_t*)p_dat)[1] = q << 4; // Imag (Q)

			index++;

		}

		// Schedule TX buffer
		nbytes_tx = iio_buffer_push(txbuf);
		if (nbytes_tx < 0) { printf("Error pushing buf %d\n", (int) nbytes_tx); shutdown(); }

		// Sample counter increment and status output
		ntx += nbytes_tx / iio_device_get_sample_size(tx);
		//printf("\tTX %8.2f MSmp\n", ntx/1e6);
	}

/*
	const int NUMSAMPLES= 512;
	short idata [NUMSAMPLES];    
	short qdata [NUMSAMPLES];    
	int numsamples = NUMSAMPLES;

	for(int index=0; index<numsamples; index++);    
	{    
		idata[index]=32768 * sin((2*3.14*index)/numsamples);        
		qdata[index]=32768 * cos((2*3.14*index)/numsamples);        
	}

*/

	//sleep(1);
}
 
void main (int argc, char **argv)
{

	signal(SIGINT, handle_sig);

 	ctx = iio_create_context_from_uri("ip:192.168.2.1");
 
	samp_rate = RATE_520K;
	tx_freq = FREQ_S;

	init();
	transmit();
 
	shutdown();
} 