#!/bin/bash
rm -rf build/*

# ...seu código aqui...
cmake -S . -B build
cd build
make
echo "=============================================="
./Speed_Racer

cd ..