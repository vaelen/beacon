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

#include "main.h"

const int IQ_BUFFER_SIZE = 4096;

struct beacon_config parse_config(int argc, char **argv)
{
    struct beacon_config config;
#ifdef ADALM_SUPPORT
    config.device = DEVICE_ADALM;
#else
    config.device = DEVICE_FILE;
#endif
    config.samp_rate = RATE_520K;
    config.tx_freq = FREQ_S;
    config.amplitude = DEFAULT_AMPLITUDE;
    config.tone_freq = DEFAULT_TONE_FREQ;

    /*
    char *cvalue = NULL;
    int index;
    int c;

    opterr = 0;

    while ((c = getopt(argc, argv, "abc:")) != -1)
    {
        switch (c)
        {
        case 'a':
            aflag = 1;
            break;
        case 'b':
            bflag = 1;
            break;
        case 'c':
            cvalue = optarg;
            break;
        case '?':
            if (optopt == 'c')
                fprintf(stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint(optopt))
                fprintf(stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf(stderr,
                        "Unknown option character `\\x%x'.\n",
                        optopt);
            exit(1);
        default:
            abort();
        }
    }
    */
    return config;
}

void transmit(struct beacon_config config)
{
    complex iq[IQ_BUFFER_SIZE];
    double start = 0;
    long samples = 0, samples_written = 0;

    char *device_name;
    switch (config.device)
    {
    case DEVICE_ADALM:
        device_name = "Adalm-Pluto";
        break;
    default:
        device_name = "STDOUT";
        break;
    }

    fprintf(stderr, "Writing IQ Data - Device: %s, Sampling Rate: %ld\n", device_name, config.samp_rate);
    while (!stop)
    {
        start = generate_signal(config.tone_freq, config.amplitude, config.samp_rate, iq, IQ_BUFFER_SIZE, start);
        samples_written = write_iq_to_device(config.device, iq, IQ_BUFFER_SIZE);
        if (samples_written == 0)
        {
            fprintf(stderr, "Couldn't Write Samples.\n");
        }
        else
        {
            fprintf(stderr, "Wrote %ld Samples (%ld Total).\n", samples_written, samples);
        }
        samples += samples_written;
    }
}

void init(struct beacon_config config)
{
#ifdef ADALM_SUPPORT
    adalm_init("ip:192.168.2.1", config.samp_rate, config.tx_freq, IQ_BUFFER_SIZE);
#endif
}

void shutdown(int code)
{
#ifdef ADALM_SUPPORT
    adalm_shutdown();
#endif
    exit(code);
}

int write_iq_to_device(enum device device, complex *iq, long iq_len)
{
    switch (device)
    {
    case DEVICE_ADALM:
#ifdef ADALM_SUPPORT
        adalm_transmit(iq, iq_len);
        break;
#else
        return 0;
#endif
    default:
        write_iq(stdout, iq, IQ_BUFFER_SIZE);
        break;
    }
    return IQ_BUFFER_SIZE;
}

void main(int argc, char **argv)
{
    signal(SIGINT, handle_sig);
    struct beacon_config config = parse_config(argc, argv);
    init(config);
    transmit(config);
    shutdown(0);
}