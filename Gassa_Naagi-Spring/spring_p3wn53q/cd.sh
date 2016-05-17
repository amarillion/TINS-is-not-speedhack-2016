#!/bin/bash
dmd source/*.d -ofspring.exe -O -inline -debug -g -unittest -L/SUBSYSTEM:CONSOLE:4.0
