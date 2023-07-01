#!/bin/sh

#fast way to compile all cpp files

for i in *.cpp
do
    echo "compiling $i into ${i%".cpp"}.out"
    g++ $i -o ${i%".cpp"}.out
done
