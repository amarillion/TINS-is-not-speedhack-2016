#!/bin/bash
dmd source/*.d -ofspring.exe -O -release -inline -boundscheck=off -L/SUBSYSTEM:WINDOWS:4.0
