#!/bin/bash
# Initializes LED IO pins and sets them all to "Low". Note that setting them to "In" with a pulldown resistor enabled has been unsucessful. Setting them as low outputs by default seems to work well.

# IR LED IOs
for value in 16 26 20 21; do
    gpio -g mode $value out
    gpio -g write $value 0
done

gpio -g write 16 0
gpio -g write 26 0
gpio -g write 20 0
gpio -g write 21 0
