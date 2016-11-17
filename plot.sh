#!/bin/bash

echo set terminal png
echo set xtics 1
echo plot \"$1\" using 1:2 with lines
