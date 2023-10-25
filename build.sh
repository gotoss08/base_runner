#!/bin/sh
set -xe

CC="gcc"
CFLAGS="-Wall -Wextra -pedantic -g"
LIBS="-lraylib"

$CC -o main.exe $CFLAGS anyascii.c fuzzy_match.c main.c $LIBS
