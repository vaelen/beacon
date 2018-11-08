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

const double M = 1000000;
const double K = 1000;

struct beacon_config parse_config(int argc, char **argv)
{
    struct beacon_config config;
#ifdef ADALM_SUPPORT
    config.device = DEVICE_ADALM;
#else
    config.device = DEVICE_FILE;
#endif
    config.samp_rate = RATE_2M;
    config.tx_freq = FREQ_S;
    config.carrier_freq = DEFAULT_CARRIER_FREQ;
    config.carrier_amplitude = DEFAULT_CARRIER_AMPLITUDE;
    config.tone_freq = DEFAULT_TONE_FREQ;
    config.tone_amplitude = DEFAULT_TONE_AMPLITUDE;
    config.wpm = DEFAULT_WPM;
    config.message = "";
    config.iq_len = DEFAULT_IQ_LEN;
    config.padding = DEFAULT_PADDING;
    config.gain = DEFAULT_GAIN;

    bool help_flag = false;

    while (1)
    {
        static struct option long_options[] =
            {
                {"sampling-rate", required_argument, 0, 's'},
                {"frequency", required_argument, 0, 'f'},
                {"carrier-offset", required_argument, 0, 'c'},
                {"carrier-amplitude", required_argument, 0, 'a'},
                {"tone", required_argument, 0, 't'},
                {"tone-amplitude", required_argument, 0, 'A'},
                {"wpm", required_argument, 0, 'w'},
                {"padding", required_argument, 0, 'p'},
                {"buffer-length", required_argument, 0, 'b'},
                {"gain", required_argument, 0, 'g'},
                {"uhf", no_argument, 0, 'u'},
                {"sband", no_argument, 0, 's'},
                {"stdout", no_argument, 0, 'o'},
                {"help", no_argument, 0, 'h'},
                {"version", no_argument, 0, 'v'},
                {0, 0, 0, 0}};

        /* getopt_long stores the option index here. */
        int option_index = 0;

        int c = getopt_long(argc, argv, "s:f:c:a:t:A:w:p:b:g:usohv",
                            long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1)
            break;

        switch (c)
        {
        case 0:
            /* If this option set a flag, do nothing else now. */
            if (long_options[option_index].flag != 0)
                break;
            printf("option %s", long_options[option_index].name);
            if (optarg)
                printf(" with arg %s", optarg);
            printf("\n");
            break;

        case 's':
            config.samp_rate = atol(optarg);
            break;

        case 'f':
            config.tx_freq = (long)(atof(optarg) * M);
            break;

        case 'c':
            config.carrier_freq = atol(optarg);
            break;

        case 'a':
            config.carrier_amplitude = atof(optarg);
            break;

        case 't':
            config.tone_freq = atol(optarg);
            break;

        case 'A':
            config.tone_amplitude = atof(optarg);
            break;

        case 'w':
            config.wpm = atoi(optarg);
            break;

        case 'p':
            config.padding = atoi(optarg);
            break;

        case 'b':
            config.iq_len = atol(optarg);
            break;

        case 'g':
            config.gain = atof(optarg);
            break;

        case 'o':
            config.device = DEVICE_FILE;
            break;

        case 'u':
            config.tx_freq = FREQ_U;
            break;

        case 'S':
            config.tx_freq = FREQ_S;
            break;

        case 'h':
            help_flag = true;
            break;

        case 'v':
            fprintf(stderr, "%s\n", PACKAGE_STRING);
            exit(1);
            break;

        case '?':
            /* getopt_long already printed an error message. */
            break;

        default:
            abort();
        }
    }

    if (help_flag || optind >= argc)
    {
        // Print help
        fprintf(stderr, "Usage: %s [options] <message>\n", argv[0]);
        fprintf(stderr, "Broadcasts a CW (morse code) beacon.\n");
        fprintf(stderr, "\n");
        fprintf(stderr, "Common Options:\n");
        fprintf(stderr, "-U, --uhf\ttransmit using default UHF frequency of %0.3f MHz\n", FREQ_U / M);
        fprintf(stderr, "-S, --sband\ttransmit using default S-Band frequency %0.3f MHz (default)\n", FREQ_S / M);
        fprintf(stderr, "CW (morse code) Options:\n");
        fprintf(stderr, "-t, --tone\tsets the tone frequency in Hz (default: %ld Hz)\n", DEFAULT_TONE_FREQ);
        fprintf(stderr, "-w, --wpm\tsets the cw (morse code) speed in words per minute (default: %d WPM)\n", DEFAULT_WPM);
        fprintf(stderr, "-p, --padding\t sets the amount of time to pause between transmissions (default: %d)\n", DEFAULT_PADDING);
        fprintf(stderr, "Hardware Options:\n");
        fprintf(stderr, "-g, --gain\tsets the hardware gain (0 to 90, default: %0.3f)\n", DEFAULT_GAIN);
        fprintf(stderr, "-s, --sampling_rate\tsets the sampling rate of the device (default: %d)\n", RATE_2M);
        fprintf(stderr, "-f, --frequency\tsets the transmission frequency in MHz (default: %0.3f MHz)\n", FREQ_S / M);
        fprintf(stderr, "-o, --stdout\twrite IQ data to STDOUT\n");
        fprintf(stderr, "Advanced Options:\n");
        fprintf(stderr, "-c, --carrier-offset\tsets the carrier offset frequency in Hz (default: %ld Hz)\n", DEFAULT_CARRIER_FREQ);
        fprintf(stderr, "-a, --carrier-amplitude\tsets the carrier amplitude (default: %0.3f)\n", DEFAULT_CARRIER_AMPLITUDE);
        fprintf(stderr, "-A, --tone-amplitude\tsets the tone amplitude (default: %0.3f)\n", DEFAULT_TONE_AMPLITUDE);
        fprintf(stderr, "-b, --buffer-length\t (default: %ld)\n", DEFAULT_IQ_LEN);
        fprintf(stderr, "-v, --version\tprints the name and version of this program\n");
        fprintf(stderr, "-h, --helps\tprints this message\n");
        exit(1);
    }

    /* Print any remaining command line arguments (not options). */
    if (optind < argc)
    {
        config.message = argv[optind++];
    }

    if (config.message == "")
    {
        fprintf(stderr, "Usage: beacon <MESSAGE>\n");
        exit(1);
    }
    return config;
}

void transmit(struct beacon_config config)
{
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

    long dit_len = calc_dit_len(config.samp_rate, config.wpm);
    fprintf(stderr,
            "Device: %s, Sampling Rate: %0.3f Ms/s, Gain: %0.3f, Frequency: %0.3f MHz, Carrier: %0.3f KHz, Tone: %ld Hz, WPM: %d, Samples Per Dit: %ld, Padding: %d, Message: %s\n",
            device_name, config.samp_rate / M, config.gain, config.tx_freq / M, config.carrier_freq / K, config.tone_freq, config.wpm, dit_len, config.padding, config.message);

    complex carrier[config.iq_len], tone[config.iq_len];
    double carrier_start = 0, tone_start = 0;
    long samples = 0;
    int i = 0;
    double amp = 0;
    double amp_delta = .1;

    struct cw_state state;
    state.element = 0;
    state.samples_left = 0;
    state.value = false;

    int cw_len = (strlen(config.message) + config.padding + 1) * 10;

    bool cw_pattern[cw_len];
    int cw_pattern_length = generate_cw_pattern(cw_pattern, cw_len, config.message, config.padding);

    while (!stop)
    {
        carrier_start = generate_signal(config.carrier_freq, config.carrier_amplitude, config.samp_rate, carrier, config.iq_len, carrier_start);
        tone_start = generate_signal(config.tone_freq, config.tone_amplitude, config.samp_rate, tone, config.iq_len, tone_start);
        state = modulate_cw(tone, config.iq_len, dit_len, cw_pattern, cw_pattern_length, state);
        modulate_am(carrier, tone, config.iq_len);
        samples = write_iq_to_device(config.device, carrier, config.iq_len);
        if (samples == 0)
        {
            fprintf(stderr, "Couldn't Write Samples.\n");
        }
#ifdef DEBUG
        else
        {
            fprintf(stderr, "Wrote %ld Samples.\n", samples);
        }
#endif
    }
}

void init(struct beacon_config config)
{
#ifdef ADALM_SUPPORT
    adalm_init("ip:192.168.2.1", config.samp_rate, config.gain, config.tx_freq, config.iq_len);
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
        write_iq(stdout, iq, iq_len);
        break;
    }
    return iq_len;
}

void main(int argc, char **argv)
{
    signal(SIGINT, handle_sig);
    struct beacon_config config = parse_config(argc, argv);
    init(config);
    transmit(config);
    shutdown(0);
}