#!/bin/bash

echo $1

rgbasm -o $1.obj $1.z80
rgblink -m $1.map -n $1.sym -o $1.gb $1.obj
rgbfix -p 0 -v $1.gb

wine ../../bgb.exe $1.gb