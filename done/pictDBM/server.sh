#!/usr/bin/env bash

export DYLD_FALLBACK_LIBRARY_PATH=libmongoose
./pictDB_server $@ ../../provided/week07/testDB02.pictdb_dynamic
