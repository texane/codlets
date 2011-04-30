#!/usr/bin/env sh
gcc -Wall -O2 -I../src -DSTL_CONFIG_UNIT=1 ../src/stl.c
