#!/usr/bin/bash

make
sudo insmod predecode_re.ko pmc_event_no=2
make clean
sudo rmmod predecode_re
clear
sudo dmesg -c | grep "PredecodeRE"