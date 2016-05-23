#!/usr/bin/env bash

cp provided/week07/testDB02.pictdb_dynamic ./testdb.pictdb
cd done/pictDBM
make
cp pictDBM ../../pictDBM
cd ../..
./pictDBM insert testdb.pictdb pic3 ./provided/week09/foret.jpg
#./pictDBM list testdb.pictdb
./pictDBM delete testdb.pictdb pic1
./pictDBM delete testdb.pictdb pic2
./pictDBM delete testdb.pictdb pic3
#./pictDBM list testdb.pictdb
#before size of 562570
ls -l testdb.pictdb
./pictDBM gc testdb.pictdb tmp.pictdb
ls -l testdb.pictdb
#after size of 464451

rm pictDBM