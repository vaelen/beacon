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

/* File main.h */
#ifndef FILE_MAIN_H_SEEN
#define FILE_MAIN_H_SEEN

#include "global.h"

#include "iq.h"
#include "cw.h"

#ifdef ADALM_SUPPORT
#include "adalm.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <stdbool.h>
#include <signal.h>
#include <libgen.h>

enum device
{
    DEVICE_FILE,
    DEVICE_ADALM
};

enum modulation
{
    MOD_AM,
    MOD_FM
};

struct beacon_config
{
    enum device device;
    const char *uri;
    long samp_rate;
    long tx_freq;
    long carrier_freq;
    long tone_freq;
    int wpm;
    const char *message;
    long iq_len;
    int padding;
    double gain;
    enum modulation modulation;
    double modulation_index;
};

const char *DEFAULT_URI = "ip:192.168.2.1";
const char *LOCAL_URI = "local:";
const long DEFAULT_TONE_FREQ = 500;
const long DEFAULT_CARRIER_FREQ = 10000;
const int DEFAULT_WPM = 15;
const long DEFAULT_IQ_LEN = 65536;
const int DEFAULT_PADDING = 10;
const double DEFAULT_GAIN = 100;
const double DEFAULT_SAMP_RATE = 1000000;
const double DEFAULT_MODULATION_INDEX = 500;

void print_version(FILE *out);
void print_help(FILE *out, const char *executable_name);
struct beacon_config parse_config(int argc, char **argv);
void init(struct beacon_config config);
void main(int argc, char **argv);
void transmit(struct beacon_config config);
int write_iq_to_device(enum device device, complex *iq, long iq_len);
const char *device_name(struct beacon_config config);

#endif /* !FILE_MAIN_H_SEEN */
