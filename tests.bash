#!/usr/bin/env bash

clear

./style.bash

cd done/pictDBM

cp ../../provided/week07/testDB02.pictdb_dynamic ./testDB02.pictdb_dynamic

make

rm *_orig.jpg
rm *_small.jpg
rm *_thumb.jpg

./pictDBM test

open *_orig.jpg
open *_small.jpg
open *_thumb.jpg

#rm *.pictdb_dynamic

cd ../..