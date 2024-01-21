#!/bin/bash

# see build-posix.sh for explanation
CXX="i686-w64-mingw32-g++"

LIBS="-lcrypt32 -lws2_32"
CFLAGS="-Wl,--subsystem,windows"

source ./build-posix.sh
