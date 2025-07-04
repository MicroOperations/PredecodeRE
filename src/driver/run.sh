#!/usr/bin/bash

make
sudo insmod predecode_re.ko 
make clean
cd ../user
gcc -o main main.c
clear
sudo ./main 
rm main
cd ../driver
sudo rmmod predecode_re
sudo dmesg -c | grep "PredecodeRE"