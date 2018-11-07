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

/* File global.h */
#ifndef FILE_GLOBAL_H_SEEN
#define FILE_GLOBAL_H_SEEN

#include <stdio.h>
#include <stdbool.h>
#include <signal.h>

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
    RATE_2M = 2000000,
    RATE_5M = 5000000,
};

static bool stop;
static void handle_sig(int sig)
{
    fprintf(stderr, " Waiting for process to finish...\n");
    stop = true;
}
void shutdown(int code);
#endif /* !FILE_GLOBAL_H_SEEN */