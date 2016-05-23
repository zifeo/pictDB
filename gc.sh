#!/usr/bin/env bash

cp provided/week07/testDB02.pictdb_dynamic ./testdb.pictdb
cd done/pictDBM
make
cp pictDBM ../../pictDBM
cd ../..
./pictDBM insert testdb.pictdb pic3 ./provided/week09/foret.jpg
./pictDBM delete testdb.pictdb pic2
./pictDBM list testdb.pictdb
#size of 562570
echo ''
echo 'Garbage collector'
echo ''
./pictDBM gc testdb.pictdb tmp.pictdb

rm pictDBM