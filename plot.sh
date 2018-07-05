#!/bin/bash

mat=(6000 8000 10000 12000 14000 16000 18000 20000 22000 24000 26000 28000 30000 32000 34000 36000 38000 40000)
#mat=(42000 44000 46000 48000 50000)

for j in "${mat[@]}"
do
  ./SP $j $j 160 80
done
for j in "${mat[@]}"
do
  ./SP $j $j 240 80
done
for j in "${mat[@]}"
do
  ./SP $j $j 320 160
done
for j in "${mat[@]}"
do
  ./SP $j $j 400 80
done
for j in "${mat[@]}"
do
  ./SP $j $j 480 240
done
for j in "${mat[@]}"
do
  ./SP $j $j 560 112
done
for j in "${mat[@]}"
do
  ./SP $j $j 640 320
done
