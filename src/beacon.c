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

#include <iio.h>
#include <stdio.h>

#include "config.h"

const long FREQ_S = 1294500000;
const long FREQ_U = 432320000;
const long RATE_5M = 5000000;

const char* DEV_NAME = "ad9361-phy";
const char* RX_DEV_NAME = "cf-ad9361-lpc";
const char* TX_DEV_NAME = "cf-ad9361-dds-core-lpc";

void receive(struct iio_context *ctx, const long freq, const long samp_rate)
{
	struct iio_device *phy;
	struct iio_device *dev;
	struct iio_channel *rx0_i, *rx0_q;
	struct iio_buffer *rxbuf;

	phy = iio_context_find_device(ctx, DEV_NAME);
 
	// Set frequency
	iio_channel_attr_write_longlong(
		iio_device_find_channel(phy, "altvoltage0", false),
		"frequency",
		freq);
 
  	// Set baseband sampling rate
	iio_channel_attr_write_longlong(
		iio_device_find_channel(phy, "voltage0", false),
		"sampling_frequency",
		samp_rate);
 
	dev = iio_context_find_device(ctx, RX_DEV_NAME);
 
	rx0_i = iio_device_find_channel(dev, "voltage0", 0);
	rx0_q = iio_device_find_channel(dev, "voltage1", 0);
 
	iio_channel_enable(rx0_i);
	iio_channel_enable(rx0_q);
 
	rxbuf = iio_device_create_buffer(dev, 4096, false);
	if (!rxbuf) {
		perror("Could not create RX buffer");
		return;
	}
 
	while (true) {
		void *p_dat, *p_end, *t_dat;
		ptrdiff_t p_inc;
 
		iio_buffer_refill(rxbuf);
 
		p_inc = iio_buffer_step(rxbuf);
		p_end = iio_buffer_end(rxbuf);
 
		for (p_dat = iio_buffer_first(rxbuf, rx0_i); p_dat < p_end; p_dat += p_inc, t_dat += p_inc) {
			const int16_t i = ((int16_t*)p_dat)[0]; // Real (I)
			const int16_t q = ((int16_t*)p_dat)[1]; // Imag (Q)
 
			/* Process here */
 
		}
	}
 
	iio_buffer_destroy(rxbuf);
 
}

void transmit(struct iio_context *ctx, const long freq, const long samp_rate)
{
	struct iio_device *phy;
	struct iio_device *dev;
	struct iio_channel *tx0_i, *tx0_q;
	struct iio_buffer *txbuf;
 

	phy = iio_context_find_device(ctx, DEV_NAME);
 
	// Set frequency
	iio_channel_attr_write_longlong(
		iio_device_find_channel(phy, "altvoltage1", true),
		"frequency",
		freq);
 
  	// Set baseband rate to 5 MSPS
	iio_channel_attr_write_longlong(
		iio_device_find_channel(phy, "voltage0", true),
		"sampling_frequency",
		samp_rate);

	dev = iio_context_find_device(ctx, TX_DEV_NAME);
 
	tx0_i = iio_device_find_channel(dev, "voltage0", true);
	tx0_q = iio_device_find_channel(dev, "voltage1", true);
 
	iio_channel_enable(tx0_i);
	iio_channel_enable(tx0_q);

	txbuf = iio_device_create_buffer(dev, 4096, false);
	if (!txbuf) {
		perror("Could not create TX buffer");
		return;
	}

	iio_buffer_destroy(txbuf);
}
 
int main (int argc, char **argv)
{
	struct iio_context *ctx;
 
	ctx = iio_create_context_from_uri("ip:192.168.2.1");
 
	transmit(ctx, FREQ_S, RATE_5M);
 
	iio_context_destroy(ctx);
 
	return 0;
} 