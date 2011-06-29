#!/usr/bin/env sh

# default
CFLAGS='-Wall -I.'
LFLAGS=''

# qt
CFLAGS="$CFLAGS `pkg-config --cflags QtCore QtXml QtOpenGL`"
LFLAGS="$LFLAGS `pkg-config --libs QtCore QtXml QtOpenGL`"

# qglviewer
LFLAGS="$LFLAGS -lqglviewer-qt4"

# opengl
CFLAGS="$CFLAGS `pkg-config --cflags gl`"
LFLAGS="$LFLAGS `pkg-config --libs gl`"

moc-qt4 ../src/SimulationViewer.hh -o ../src/moc_SimulationViewer.hh
g++ $CFLAGS ../src/main.cc ../src/SimulationViewer.cc $LFLAGS
