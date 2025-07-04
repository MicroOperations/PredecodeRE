#!/usr/bin/bash

make
sudo insmod predecode_re.ko 
make clean
clear
sudo rmmod predecode_re
sudo dmesg -c | grep "PredecodeRE"