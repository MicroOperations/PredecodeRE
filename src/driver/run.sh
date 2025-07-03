#!/usr/bin/bash

make
sudo insmod predecode_re.ko 
make clean
sudo rmmod predecode_re
clear
sudo dmesg -c | grep "PredecodeRE"