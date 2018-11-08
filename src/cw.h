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

/* File cw.h */
#ifndef FILE_CW_H_SEEN
#define FILE_CW_H_SEEN

#include "../config.h"

#include <stdbool.h>
#include <complex.h>
#include <ctype.h>
#include <string.h>

struct cw_state
{
    int element;
    bool value;
    long samples_left;
};

/** Converts the given message into a pattern and stores it in the provided pattern array.  Returns the number of values stored in the pattern array. */
int generate_cw_pattern(bool *pattern, int buffer_len, const char *message, int final_padding_spaces);

/** Calculate the number of samples per dit. */
long calc_dit_len(long samp_rate, int wpm);

/** Modulate a CW signal on to the provided tone IQ data with the given CW message. */
struct cw_state modulate_cw(complex *iq, int iq_len, int dit_len, bool *pattern, int pattern_len, struct cw_state state);

#endif /* !FILE_CW_H_SEEN */