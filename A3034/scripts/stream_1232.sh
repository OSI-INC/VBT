#!/bin/bash
# Opens listening connection on port 2222 to which video will be streamed in H264 format.

# Requires an argument from -10 to 10 for exposure compensation. Positive values are brighter. "2" or "3" is recommended for black mice.

screen -dm | raspivid -t 0 -fl -w 1640 -h 1232 -ev $1 -ih -fps 20 -l -o tcp://0.0.0.0:2222
