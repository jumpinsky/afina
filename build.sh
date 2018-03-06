#!/bin/bash
git tag v0.0.0
rm -r -f "build"
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
