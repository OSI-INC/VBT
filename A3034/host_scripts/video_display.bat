REM This script will be called from the VBT directory, so the mplayer executable path is 
REM valid from that point. We specify the size of the video window with the -x and -y parameters.
mplayer\mplayer.exe -x 820 -y 616 -geometry 50:50 -fps 200 -demuxer h264es ffmpeg://tcp://10.0.0.234:2222
