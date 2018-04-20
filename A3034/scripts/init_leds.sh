#!/bin/bash
# Initializes LED IO pins and sets them all to "Low". Note that setting them to "In" with a
# pulldown resistor enabled has been unsucessful. Setting them as low outputs by default 
# seems to work well.

# White LED IOs
for value in 2 3 9 14; do
    gpio -g mode $value out
    gpio -g write $value 0
done

# IR LED IOs
for value in 16 26 20 21; do
    gpio -g mode $value out
    gpio -g write $value 0
done
