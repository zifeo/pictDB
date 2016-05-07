#!/usr/bin/env bash

export DYLD_FALLBACK_LIBRARY_PATH=libmongoose
./pictDB_server $@
