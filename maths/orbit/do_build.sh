#!/usr/bin/env sh

gcc \
-Wall \
-I. \
`pkg-config --cflags sdl` \
main.c \
x.c \
-lm \
`pkg-config --libs sdl` \
-lSDL_draw
