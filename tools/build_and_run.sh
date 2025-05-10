#!/bin/bash

# ...seu código aqui...
cmake -S . -B build
cd build
make
echo "=============================================="
./Speed_Racer

cd ..