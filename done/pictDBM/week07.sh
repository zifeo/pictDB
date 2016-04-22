#!/bin/bash

clear
make

rm img0_*.jpg
rm img1_*.jpg

cp .././../provided/week07/testDB02.pictdb_dynamic testDB10.pictdb_dynamic

./pictDBM list testDB10.pictdb_dynamic
./pictDBM resize testDB10.pictdb_dynamic 0 0
./pictDBM resize testDB10.pictdb_dynamic 0 1
./pictDBM resize testDB10.pictdb_dynamic 1 0
./pictDBM resize testDB10.pictdb_dynamic 1 1
./pictDBM list testDB10.pictdb_dynamic
./pictDBM write testDB10.pictdb_dynamic img0_thumb.jpg 0 0
./pictDBM write testDB10.pictdb_dynamic img0_small.jpg 0 1
./pictDBM write testDB10.pictdb_dynamic img0_orig.jpg 0 2
./pictDBM write testDB10.pictdb_dynamic img1_thumb.jpg 1 0
./pictDBM write testDB10.pictdb_dynamic img1_small.jpg 1 1
./pictDBM write testDB10.pictdb_dynamic img1_orig.jpg 1 2

rm testDB10.pictdb_dynamic
