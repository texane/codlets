#!/usr/bin/env sh
[ -x a.out ] || g++ -Wall main.cc -lm ;
./a.out > /tmp/p ;
gnuplot -e "plot '/tmp/p' using 1:2 with lines, '/tmp/p' using 1:3 with lines, '/tmp/p' using 1:4 with lines; pause -1;"
