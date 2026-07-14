#!/bin/bash
#compiles GERRY files for command line usage

gcc -c helpers.c
gcc -c initializeDomain.c
gcc -c initializeSchedule.c
gcc -c iterativeRepair.c
gcc -c main.c
gcc -c manageCircularBuffer.c
gcc -c manageCircularBuffer.c
gcc -c manageLog.c
gcc -c manageSchedule.c
gcc -c manageTasks.c
gcc -c montecarlosimulation.c
gcc -o gerry helpers.c initializeDomain.c initializeSchedule.c iterativeRepair.c main.c manageCircularBuffer.c manageLog.c manageSchedule.c manageTasks.c montecarlosimulation.c -lm
