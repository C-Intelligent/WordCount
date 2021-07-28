#!/bin/bash

if [ ! -d "./build" ]; then
  mkdir build
fi
cd build
cmake ..
make
cd ..

# gcc mapreduce.c wordcount.c -o wordcount -Wall -Werror -pthread
echo "//////////////////////////PROGRAM START////////////////////////////"
./bin/WordCount ./WordCount_test/file3 > ./WordCount_test/file3.out 
echo "///////////////////////////PROGRAM END/////////////////////////////"
