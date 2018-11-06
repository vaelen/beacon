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

/** Converts the given message into a pattern and stores it in the provided pattern array.  Returns the number of values stored in the pattern array. */
int generate_cw_pattern(char *pattern, int buffer_len, const char *message)
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
                        pattern[pos++] = 1;
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
                        pattern[pos++] = 1;
                        pattern[pos++] = 1;
                        pattern[pos++] = 1;
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
                        pattern[pos++] = 0;
                        pattern[pos++] = 0;
                        pattern[pos++] = 0;
                        pattern[pos++] = 0;
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
                    pattern[pos++] = 0;
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
                pattern[pos++] = 0;
                pattern[pos++] = 0;
            }
        }
    }
    return pos;
}

/** Modulate the provided tone IQ data with the given CW message. */
int apply_cw(complex *iq, int iq_len, int samples_per_dit, char *pattern, int pattern_len, int start)
{
    return start;
}
