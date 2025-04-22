#!/bin/sh
gcc main.c perlin_noise.c -O3 -o exe -g -lm -lSDL2
