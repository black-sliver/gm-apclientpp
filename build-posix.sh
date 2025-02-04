#!/bin/bash

# example command line for compiling with gcc
# this is only really used for testing/CI
# see VS2015 if you want to build with Visual Studio

CXX="${CXX:-g++}"
DLL_EXT="${DLL_EXT:-.so}"

LIBS="-pthread -lssl -lcrypto -lz $LIBS"
#CFLAGS="-g -Og $CFLAGS" # debug
CFLAGS="-s -Os $CFLAGS" # release
DEFINES="-DASIO_STANDALONE -DWSWRAP_SEND_EXCEPTIONS -DGM_APCLIENTPP_EXPORTS $DEFINES"
INCLUDE_DIRS="-I subprojects/json/include -I subprojects/valijson/include -I subprojects/wswrap/include -I subprojects/apclientpp -Isubprojects/asio/include -Isubprojects/websocketpp -Isubprojects $INCLUDE_DIRS"

mkdir -p build

$CXX -o build/gm-apclientpp$DLL_EXT src/*.cpp -shared -fPIC $DEFINES $INCLUDE_DIRS $CFLAGS $LIBS
$CXX -o build/test$EXE_EXT test/test.c -L build -l:gm-apclientpp$DLL_EXT
