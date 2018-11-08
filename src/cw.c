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

#include "cw.h"

const char *cw = "A.-B-...C-.-.D-..E.F..-.G--.H....I..J.---K-.-L.-..M--N-.O---P.--.Q--.-R.-.S...T-U..-V...-W.--X-..-Y-.--Z--..1.----2..---3...--4....-5.....6-....7--...8---..9----.0----- _";
// Dits per word, based on "PARIS ".
const int DITS_PER_WORD = 50;

/** Converts the given message into a pattern and stores it in the provided pattern array.  Returns the number of values stored in the pattern array. */
int generate_cw_pattern(bool *pattern, int buffer_len, const char *message, int final_padding_spaces)
{
    int pos = 0;
    int message_len = strlen(message);
    for (int i = 0; i < message_len; i++)
    {
        char *c = strchr(cw, toupper(message[i]));
        if (c != NULL)
        {
            bool done = false;
            while (!done)
            {
                c++;
                switch (*c)
                {
                case '.':
                    // on for 1 time slot for dit
                    if (buffer_len - pos < 1)
                    {
                        done = true;
                    }
                    else
                    {
                        pattern[pos++] = true;
                    }
                    break;
                case '-':
                    // on for 3 time slots for dah
                    if (buffer_len - pos < 3)
                    {
                        done = true;
                    }
                    else
                    {
                        pattern[pos++] = true;
                        pattern[pos++] = true;
                        pattern[pos++] = true;
                    }
                    break;
                case '_':
                    // off for 7 time slots between words
                    // we only add 4 here because another 3 will be added below
                    if (buffer_len - pos < 4)
                    {
                        done = true;
                    }
                    else
                    {
                        pattern[pos++] = false;
                        pattern[pos++] = false;
                        pattern[pos++] = false;
                        pattern[pos++] = false;
                    }
                    break;
                default:
                    done = true;
                    break;
                };
                // off for 1 time slot between dits and dahs
                if (buffer_len - pos < 1)
                {
                    break;
                }
                else
                {
                    pattern[pos++] = false;
                }
            }
            // off for 3 time slots between letters
            // we only add 2 here because 1 was added above
            if (buffer_len - pos < 2)
            {
                break;
            }
            else
            {
                pattern[pos++] = false;
                pattern[pos++] = false;
            }
        }
    }
    // off for 7 time slots between words
    int padding = 7 * final_padding_spaces;
    while (buffer_len - pos > 0 && padding > 0)
    {
        pattern[pos++] = false;
        padding--;
    }
    return pos;
}

long calc_dit_len(long samp_rate, int wpm)
{
    long dits_per_sec = wpm * DITS_PER_WORD / 60;
    long samples_per_dit = samp_rate / dits_per_sec;
    return samples_per_dit;
}

struct cw_state modulate_cw(complex *iq, int iq_len, int dit_len, bool *pattern, int pattern_len, struct cw_state state)
{
    for (int index = 0; index < iq_len; index++)
    {
        double i = cimag(iq[index]);

        if (state.samples_left < 1 && i > -0.000001 && i < 0.00001)
        {
            // Move to next element when i is 0
            state.value = pattern[state.element];
            state.samples_left = dit_len;
            state.element = (state.element + 1) % pattern_len;
        }

        state.samples_left--;

        if (!state.value)
        {
            iq[index] = 0 + 1 * I;
        }
    }
    return state;
}
