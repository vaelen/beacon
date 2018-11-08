Beacon
------------

Copyright 2018, Andrew C. Young <andrew@vaelen.org>

[![License: GPLv3](https://img.shields.io/badge/license-GPL3-blue.svg)](COPYING)

This program implements a simple CW beacon using the IIO library.  It is designed to be used with the ADALM-PLUTO device from Analog Devices.

Currently it only broadcasts using full AM modulation.

To build:
```
./autogen.sh
./configure
make
```

Usage:
```
beacon [options] <message>
```