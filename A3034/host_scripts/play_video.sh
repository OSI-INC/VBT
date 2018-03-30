#!/bin/bash
# Plays live video stream from the A3034 
# TO DO: Generic IP
mplayer -fps 200 -demuxer h264es ffmpeg://tcp://172.16.1.24:2222
