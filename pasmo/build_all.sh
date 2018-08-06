#!/bin/bash


if [[ ! $(which cmake) ]]; then
    echo "cmake not found or not in path.. exiting.."
    exit 1
fi

if [[ ! -f ./CMakeLists.txt ]]; then
    echo "CMakeLists.txt not found.. exiting.."
    exit 1
fi

if [[  -d ./build ]]; then
    rm -rf ./build/*
else
    mkdir build
fi

cd ./build && cmake ../ && make && echo "Build completed. Pasmo can be found in ./build directory.."



