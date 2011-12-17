#!/usr/bin/env sh

gcc \
-Wall \
-I. \
`pkg-config --cflags SDL_gfx` \
main.c \
x.c \
-lm \
`pkg-config --libs SDL_gfx` \
-lSDL_draw
