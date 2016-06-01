#!/usr/bin/env bash

clear
cp provided/week07/testDB02.pictdb_dynamic ./testdb.pictdb
cd done/pictDBM
make
rm *.o
cp pictDBM ../../pictDBM
cd ../..
./pictDBM interpretor testdb.pictdb

rm pictDBM