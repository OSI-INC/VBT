#!/bin/bash
# TO DO: Generic IP

path='/Users/collins/OSI/VBT/A3034'

# Turns on Pi Video Camera and sets it to listen on socket 2222
# Forks such that script will proceed past opening the video socket
ssh -i $path/ssh_keys/id_rsa pi@10.0.0.234 'bash -s' < $path/scripts/white_leds_on_full_brightness.sh &
ssh -i $path/ssh_keys/id_rsa pi@10.0.0.234 'bash -s' < $path/scripts/stream_1232.sh &
# Waits 3 seconds, allowing port to be opened
sleep 3

touch segment_list
tail -f segment_list | ts "%.s " > segment_end_times.txt &

# Saves 60 second segments with generic filenames
ffmpeg -framerate 20 -i tcp://10.0.0.234:2222 -f segment -segment_time 30 -reset_timestamps 1 -segment_list segment_list -segment_list_type ffconcat -c copy segment%03d.mp4
