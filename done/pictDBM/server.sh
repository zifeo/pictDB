#!/usr/bin/env bash

make
rm *.o

export DYLD_FALLBACK_LIBRARY_PATH=libmongoose
./pictDB_server $@
