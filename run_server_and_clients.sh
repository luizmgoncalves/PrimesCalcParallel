#!/bin/bash

executar_no_terminal(){
    $MY_TERM --noclose --geometry 500x300+$2+$3 -e "bash -c './$1.out $4; exec bash'" > /dev/null 2>&1 & 
} 

MY_TERM=konsole

for i in server.cpp client.cpp
do
    echo "compiling $i into ${i%".cpp"}.out"
    g++ $i -o ${i%".cpp"}.out
done


executar_no_terminal server 0 0 500000000
sleep 0.5 
executar_no_terminal client 0 400
executar_no_terminal client 500 0
executar_no_terminal client 500 400