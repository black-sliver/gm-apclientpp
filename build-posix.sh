#!/bin/bash

# example command line for compiling with gcc
# this is only really used for testing/CI
# see VS2015 if you want to build with Visual Studio

CXX="${CXX:-g++}"

LIBS="-pthread -lssl -lcrypto $LIBS"
CFLAGS="-Os $CFLAGS"
DEFINES="-DASIO_STANDALONE -DWSWRAP_SEND_EXCEPTIONS -DGM_APCLIENTPP_EXPORTS $DEFINES"
INCLUDE_DIRS="-I subprojects/json/include -I subprojects/valijson/include -I subprojects/wswrap/include -I subprojects/apclientpp -Isubprojects/asio/include -Isubprojects/websocketpp -Isubprojects $INCLUDE_DIRS"

mkdir -p build

$CXX -o build/gm-apclientpp.dll src/*.cpp -s -shared -fPIC $DEFINES $INCLUDE_DIRS $CFLAGS $LIBS
