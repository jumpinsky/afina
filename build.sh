#!/bin/bash
git tag v0.0.0
rm -r -f "build"
mkdir build
cd build
cmake -std=c++11 -DCMAKE_BUILD_TYPE=Debug ..
make
