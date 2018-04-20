REM This script will be called from the VBT directory, so the mplayer executable path is 
REM valid from that location.
ffmpeg\bin\ffmpeg.exe -framerate 20 -i tcp://10.0.0.234:2222 -f segment ^
 -segment_atclocktime 1 -segment_time %2 -reset_timestamps 1 ^
 -segment_list %1\\segment_list.txt -segment_list_type ffconcat -c copy ^
 -strftime 1 %1\\SEG%%Y-%%j-%%H-%%M-%%S.mp4