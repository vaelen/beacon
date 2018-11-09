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

void print_version(FILE *out)
{
    fprintf(out, "%s\n", PACKAGE_STRING);
    fprintf(out, "Copyright (C) 2018 Andrew C. Young (NU8W)\n");
    fprintf(out, "\n");
    fprintf(out, "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.\n");
    fprintf(out, "This is free software: you are free to change and redistribute it.\n");
    fprintf(out, "There is NO WARRANTY, to the extent permitted by law.\n");
    fprintf(out, "\n");
    fprintf(out, "Homepage: <%s>\n", PACKAGE_URL);
}

void print_help(FILE *out, const char *executable_name)
{
    // Print help
    fprintf(out, "%s\n", PACKAGE_STRING);
    fprintf(out, "Usage: %s [options] <message>\n", executable_name);
    fprintf(out, "Broadcasts a CW (morse code) beacon.\n");
    fprintf(out, "\n");
    fprintf(out, "Common Options:\n");
    fprintf(out, "-U, --uhf\ttransmit using default UHF frequency of %0.3f MHz\n", FREQ_U / M);
    fprintf(out, "-S, --sband\ttransmit using default S-Band frequency %0.3f MHz (default)\n", FREQ_S / M);
    fprintf(out, "CW (morse code) Options:\n");
    fprintf(out, "-t, --tone\tsets the tone frequency in Hz (default: %ld Hz)\n", DEFAULT_TONE_FREQ);
    fprintf(out, "-w, --wpm\tsets the cw (morse code) speed in words per minute (default: %d WPM)\n", DEFAULT_WPM);
    fprintf(out, "-p, --padding\t sets the amount of time to pause between transmissions (default: %d)\n", DEFAULT_PADDING);
    fprintf(out, "Hardware Options:\n");
    fprintf(out, "-u, --uri\thardware URI (default: %s)\n", DEFAULT_URI);
    fprintf(out, "-l, --local\tsets the hardware URI to %s (use this when running locally on the SDR).\n", LOCAL_URI);
    fprintf(out, "-g, --gain\tsets the hardware gain (0 to 90, default: %0.3f)\n", DEFAULT_GAIN);
    fprintf(out, "-s, --sampling_rate\tsets the sampling rate of the device (default: %d)\n", RATE_2M);
    fprintf(out, "-f, --frequency\tsets the transmission frequency in MHz (default: %0.3f MHz)\n", FREQ_S / M);
    fprintf(out, "-o, --stdout\twrite IQ data to STDOUT\n");
    fprintf(out, "Advanced Options:\n");
    fprintf(out, "-c, --carrier-offset\tsets the carrier offset frequency in Hz (default: %ld Hz)\n", DEFAULT_CARRIER_FREQ);
    fprintf(out, "-a, --carrier-amplitude\tsets the carrier amplitude (default: %0.3f)\n", DEFAULT_CARRIER_AMPLITUDE);
    fprintf(out, "-A, --tone-amplitude\tsets the tone amplitude (default: %0.3f)\n", DEFAULT_TONE_AMPLITUDE);
    fprintf(out, "-b, --buffer-length\t (default: %ld)\n", DEFAULT_IQ_LEN);
    fprintf(out, "-v, --version\tprints version, copyright, and contact information\n");
    fprintf(out, "-h, --helps\tprints this message\n");
    fprintf(out, "\n");
    fprintf(out, "Copyright (C) 2018 Andrew C. Young (NU8W)\n");
    fprintf(out, "Homepage: <%s>\n", PACKAGE_URL);
}

struct beacon_config parse_config(int argc, char **argv)
{
    struct beacon_config config;
#ifdef ADALM_SUPPORT
    config.device = DEVICE_ADALM;
#else
    config.device = DEVICE_FILE;
#endif
    config.uri = DEFAULT_URI;
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
                {"uri", required_argument, 0, 'u'},
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
                {"uhf", no_argument, 0, 'U'},
                {"sband", no_argument, 0, 'S'},
                {"stdout", no_argument, 0, 'o'},
                {"local", no_argument, 0, 'l'},
                {"help", no_argument, 0, 'h'},
                {"version", no_argument, 0, 'v'},
                {0, 0, 0, 0}};

        /* getopt_long stores the option index here. */
        int option_index = 0;

        int c = getopt_long(argc, argv, "u:s:f:c:a:t:A:w:p:b:g:USolhv",
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

        case 'u':
            config.uri = optarg;
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

        case 'l':
            config.uri = LOCAL_URI;
            break;

        case 'U':
            config.tx_freq = FREQ_U;
            break;

        case 'S':
            config.tx_freq = FREQ_S;
            break;

        case 'h':
            help_flag = true;
            break;

        case 'v':
            print_version(stderr);
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
        print_help(stderr, basename(argv[0]));
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
    long dit_len = calc_dit_len(config.samp_rate, config.wpm);
    fprintf(stderr, "WPM: %d, Samples Per Dit: %ld, Padding: %d, Message: %s\n", config.wpm, dit_len, config.padding, config.message);

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
    adalm_init(config.uri, config.samp_rate, config.gain, config.tx_freq, config.iq_len);
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

const char *device_name(struct beacon_config config)
{
    switch (config.device)
    {
    case DEVICE_ADALM:
        return "Adalm-Pluto";
    default:
        return "STDOUT";
    }
    return "";
}

void main(int argc, char **argv)
{
    signal(SIGINT, handle_sig);
    struct beacon_config config = parse_config(argc, argv);
    fprintf(stderr,
            "Device: %s, URI: %s, Sampling Rate: %0.3f Ms/s, Gain: %0.3f, Frequency: %0.3f MHz, Carrier: %0.3f KHz, Tone: %ld Hz\n",
            device_name(config), config.uri, config.samp_rate / M, config.gain, config.tx_freq / M, config.carrier_freq / K, config.tone_freq);

    init(config);
    transmit(config);
    shutdown(0);
}