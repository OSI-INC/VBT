#!/bin/bash
# TO DO: Generic IP
# Saves 60 second segments with generic filenames
ffmpeg -framerate 20 -i tcp://172.16.1.24:2222 -f segment -segment_time 60 -reset_timestamps 1 -segment_list segment_list -segment_list_type ffconcat -c copy segment%03d.mp4
