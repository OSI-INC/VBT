#!/bin/bash
# USed for testing. Takes two arguments for number of seconds LEDs on and number of seconds LEDs off.

gpio -g mode 3 out
gpio -g mode 2 out
gpio -g mode 16 out
gpio -g mode 26 out

gpio -g write 3 0
gpio -g write 2 0
gpio -g write 16 0
gpio -g write 26 0


i="0"
while [ $i -lt 300 ]; do
    gpio -g write 3 1
    sleep $1
    gpio -g write 3 0
    sleep $2
done
