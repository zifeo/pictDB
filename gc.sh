#!/usr/bin/env bash

clear
cp provided/week07/testDB02.pictdb_dynamic ./testdb.pictdb
cd done/pictDBM
make
rm *.o
cp pictDBM ../../pictDBM
cd ../..
./pictDBM insert testdb.pictdb pic3 ./provided/week09/foret.jpg
#./pictDBM list testdb.pictdb
./pictDBM delete testdb.pictdb pic1
./pictDBM delete testdb.pictdb pic2
./pictDBM delete testdb.pictdb pic3
#./pictDBM list testdb.pictdb
ls -l testdb.pictdb
./pictDBM gc testdb.pictdb tmp.pictdb
ls -l testdb.pictdb

#remove size of removed   new size
#   	    -          	 562'570.00
#pic1	 72'876.00 	     489'694.00
#pic2	 98'119.00 	     464'451.00
#pic3	 369'911.00 	 192'659.00
#pic12	 170'995.00 	 391'575.00
#pic13	 442'787.00 	 119'783.00
#pic23	 468'030.00 	 94'540.00
#pic123	 540'906.00 	 21'664.00

rm pictDBM