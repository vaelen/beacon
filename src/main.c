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
    fprintf(out, "-U, --uhf\t\ttransmit using default UHF frequency of %0.3f MHz\n", FREQ_U / M);
    fprintf(out, "-S, --sband\t\ttransmit using default S-Band frequency %0.3f MHz\n", FREQ_S / M);
    fprintf(out, "-m, --modulation\tsets the modulation (options: AM,FM default: AM)\n");
    fprintf(out, "-A, --am\t\tsets the modulation to AM\n");
    fprintf(out, "-F, --fm\t\tsets the modulation to FM\n");
    fprintf(out, "\n");
    fprintf(out, "CW (morse code) Options:\n");
    fprintf(out, "-t, --tone\t\tsets the tone frequency in Hz (default: %ld Hz)\n", DEFAULT_TONE_FREQ);
    fprintf(out, "-w, --wpm\t\tsets the cw (morse code) speed in words per minute (default: %d WPM)\n", DEFAULT_WPM);
    fprintf(out, "-p, --padding\t\tsets the amount of time to pause between transmissions (default: %d)\n", DEFAULT_PADDING);
    fprintf(out, "\n");
    fprintf(out, "Hardware Options:\n");
    fprintf(out, "-u, --uri\t\thardware URI (default: %s)\n", DEFAULT_URI);
    fprintf(out, "-l, --local\t\tsets the hardware URI to %s (use this when running locally on the SDR).\n", LOCAL_URI);
    fprintf(out, "-g, --gain\t\tsets the hardware gain (0 to 90, default: %0.3f)\n", DEFAULT_GAIN);
    fprintf(out, "-s, --sampling_rate\tsets the sampling rate of the device (default: %d)\n", DEFAULT_SAMP_RATE);
    fprintf(out, "-f, --frequency\t\tsets the transmission frequency in MHz (default: %0.3f MHz)\n", FREQ_S / M);
    fprintf(out, "-o, --stdout\t\twrite IQ data to STDOUT\n");
    fprintf(out, "\n");
    fprintf(out, "Advanced Options:\n");
    fprintf(out, "-c, --carrier-offset\tsets the carrier offset frequency in Hz (default: %ld Hz)\n", DEFAULT_CARRIER_FREQ);
    fprintf(out, "-m, --modulation-index\tsets the modulation index (default: %0.3f)\n", DEFAULT_MODULATION_INDEX);
    fprintf(out, "-b, --buffer-length\tsets the length of the internal IQ buffer (default: %ld)\n", DEFAULT_IQ_LEN);
    fprintf(out, "\n");
    fprintf(out, "Misc Options:\n");
    fprintf(out, "-v, --version\t\tprints version, copyright, and contact information\n");
    fprintf(out, "-h, --helps\\ttprints this message\n");
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
    config.samp_rate = DEFAULT_SAMP_RATE;
    config.tx_freq = FREQ_U;
    config.carrier_freq = DEFAULT_CARRIER_FREQ;
    config.tone_freq = DEFAULT_TONE_FREQ;
    config.wpm = DEFAULT_WPM;
    config.message = "";
    config.iq_len = DEFAULT_IQ_LEN;
    config.padding = DEFAULT_PADDING;
    config.gain = DEFAULT_GAIN;
    config.modulation = MOD_AM;
    config.modulation_index = DEFAULT_MODULATION_INDEX;

    bool help_flag = false;

    while (1)
    {
        static struct option long_options[] =
            {
                {"uri", required_argument, 0, 'u'},
                {"sampling-rate", required_argument, 0, 's'},
                {"frequency", required_argument, 0, 'f'},
                {"carrier-offset", required_argument, 0, 'c'},
                {"modulation", required_argument, 0, 'm'},
                {"modulation-index", required_argument, 0, 'i'},
                {"tone", required_argument, 0, 't'},
                {"wpm", required_argument, 0, 'w'},
                {"padding", required_argument, 0, 'p'},
                {"buffer-length", required_argument, 0, 'b'},
                {"gain", required_argument, 0, 'g'},
                {"uhf", no_argument, 0, 'U'},
                {"sband", no_argument, 0, 'S'},
                {"am", no_argument, 0, 'A'},
                {"fm", no_argument, 0, 'F'},
                {"stdout", no_argument, 0, 'o'},
                {"local", no_argument, 0, 'l'},
                {"help", no_argument, 0, 'h'},
                {"version", no_argument, 0, 'v'},
                {0, 0, 0, 0}};

        /* getopt_long stores the option index here. */
        int option_index = 0;

        int c = getopt_long(argc, argv, "u:s:f:c:a:m:i:t:w:p:b:g:USolhv",
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

        case 'i':
            config.modulation_index = atof(optarg);
            break;

        case 'm':
	    if (strcasecmp(optarg, "FM") == 0)
	    {
  	        config.modulation = MOD_FM;
	    }
	    else if (strcasecmp(optarg, "AM") == 0)
	    {
  	        config.modulation = MOD_AM;
	    }
            break;

        case 't':
            config.tone_freq = atol(optarg);
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

        case 'A':
 	    config.modulation = MOD_AM;
            break;

        case 'F':
            config.modulation = MOD_FM;
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

    complex carrier[config.iq_len];
    double tone[config.iq_len];
    long carrier_start = 0, tone_start = 0, samples = 0;
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
        carrier_start = generate_carrier(config.carrier_freq, config.samp_rate, carrier, config.iq_len, carrier_start);
        tone_start = generate_tone(config.tone_freq, config.samp_rate, tone, config.iq_len, tone_start);
        state = modulate_cw(tone, config.iq_len, dit_len, cw_pattern, cw_pattern_length, state);
        switch(config.modulation)
        {
        case MOD_FM:
            modulate_fm(carrier, tone, config.iq_len, config.modulation_index);
            break;
        default:
            modulate_am(carrier, tone, config.iq_len, config.modulation_index);
            break;
        }
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

const char *modulation_name(struct beacon_config config)
{
    switch (config.modulation)
    {
    case MOD_FM:
        return "FM";
    default:
        return "AM";
    }
    return "";
}

void main(int argc, char **argv)
{
    signal(SIGINT, handle_sig);
    struct beacon_config config = parse_config(argc, argv);
    fprintf(stderr,
            "Device: %s, URI: %s, Sampling Rate: %0.3f Ms/s, Gain: %0.3f, Transmission Frequency: %0.3f MHz\n",
            device_name(config), config.uri, config.samp_rate / M, config.gain, config.tx_freq / M);
    fprintf(stderr,
            "Carrier Offset: %0.3f KHz, Tone Frequency: %ld Hz, Modulation: %s, Modulation Index: %.3f\n",
            config.carrier_freq / K, config.tone_freq, modulation_name(config), config.modulation_index);
    init(config);
    transmit(config);
    shutdown(0);
}
