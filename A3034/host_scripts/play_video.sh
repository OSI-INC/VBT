#!/bin/bash
# Plays live video stream from the A3034 
# TO DO: Generic IP
mplayer -xy 600 -geometry 50:50 -fps 200 -demuxer h264es ffmpeg://tcp://10.0.0.234:2222
