#!/bin/bash

mat=(6000 8000 10000 12000 14000 16000 18000 20000 22000 24000 26000 28000 30000 32000 34000 36000 38000 40000)
#mat=(6000 8000 10000 12000 14000 16000 18000 20000 22000 24000)
#mat=(26000 28000 30000 32000 34000)
#mat=(36000 38000 40000)

for j in "${mat[@]}"
do
    ./RT $j $j 160 80
done
for j in "${mat[@]}"
do
    ./RT $j $j 240 80
done
for j in "${mat[@]}"
do
    ./RT $j $j 320 160
done
for j in "${mat[@]}"
do
    ./RT $j $j 400 80
done
for j in "${mat[@]}"
do
    ./RT $j $j 480 240
done
for j in "${mat[@]}"
do
    ./RT $j $j 560 112
done
for j in "${mat[@]}"
do
    ./RT $j $j 640 320
done
