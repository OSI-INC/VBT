#!/bin/bash
# Opens listening connection on port 2222 to which video will be streamed in H264 format for
# display on the screen. We woule like to stream the same format as we record, but have not
# figure out how to get it to do that yet.

# The -ev specifies exposure compensation. Positive values are brighter. We recommend value
# "2" or "3" for black mice. In the following example command we specify "2" along with 
# frame width 1640 and height 1232.
#raspivid -t 0 -fl -w 1640 -h 1232 -ev 2 -ih -fps 20 -l -o tcp://0.0.0.0:2222 >& log.txt &

# For a smaller image, use the following.
raspivid -t 0 -fl -w 820 -h 616 -ev 2 -ih -fps 20 -l -o tcp://0.0.0.0:2222 >& log.txt &
