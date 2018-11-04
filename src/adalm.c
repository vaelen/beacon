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

#include "adalm.h"

static void shutdown()
{
    fprintf(stderr, "* Destroying buffers\n");
    if (txbuf) { iio_buffer_destroy(txbuf); }

    fprintf(stderr, "* Disabling streaming channels\n");
    if (tx0_i) { iio_channel_disable(tx0_i); }
    if (tx0_q) { iio_channel_disable(tx0_q); }

    fprintf(stderr, "* Destroying context\n");
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

void init(long samp_rate, long tx_freq)
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

void transmit(complex *iq, int iq_len)
{
    txbuf = iio_device_create_buffer(tx, iq_len, false);
    if (!txbuf) {
        perror("Could not create TX buffer");
        shutdown();
    }

    ssize_t nbytes_tx;
    char *p_dat, *p_end;
    ptrdiff_t p_inc;
    int index = 0;
    double i = 0.0, q = 0.0;

    // WRITE: Get pointers to TX buf and write IQ to TX buf port 0
    p_inc = iio_buffer_step(txbuf);
    p_end = iio_buffer_end(txbuf);
    for (p_dat = (char *)iio_buffer_first(txbuf, tx0_i); p_dat < p_end; p_dat += p_inc) 
    {
        i = imag(iq[index]);
        q = real(iq[index]);

        // 12-bit sample needs to be MSB alligned so shift by 4
        // https://wiki.analog.com/resources/eval/user-guides/ad-fmcomms2-ebz/software/basic_iq_datafiles#binary_format
        ((int16_t*)p_dat)[0] = (short)i << 4; // Real (I)
        ((int16_t*)p_dat)[1] = (short)q << 4; // Imag (Q)

        index++;
    }

    // Schedule TX buffer
    nbytes_tx = iio_buffer_push(txbuf);
    if (nbytes_tx < 0) { fprintf(stderr, "Error pushing buf %d\n", (int) nbytes_tx); shutdown(); }
}
 
void main_adalm (int argc, char **argv)
{
    signal(SIGINT, handle_sig);

     ctx = iio_create_context_from_uri("ip:192.168.2.1");
 
    samp_rate = RATE_520K;
    tx_freq = FREQ_S;

    init();
    transmit();
 
    shutdown();
} 