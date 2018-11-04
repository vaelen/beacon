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


#include "iq.h"

#include <stdio.h>
#include <complex.h>
#include <stdbool.h>
#include <signal.h>

static bool stop;
static void handle_sig(int sig)
{
	printf("Waiting for process to finish...\n");
	stop = true;
}

void main (int argc, char **argv);