REM This script will be called from the VBT directory, so the mplayer executable path is 
REM valid from that location.
..\ffmpeg\bin\ffmpeg.exe -f concat -i segment_list.txt -c:v libx264 Combined.mp4