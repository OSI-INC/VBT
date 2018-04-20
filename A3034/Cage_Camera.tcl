# Cage Camera, a Standard and Polite LWDAQ Tool
# Copyright (C) 2018 Kevan Hashemi, Open Source Instruments Inc.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA	02111-1307, USA.


proc Cage_Camera_init {} {
	upvar #0 Cage_Camera_info info
	upvar #0 Cage_Camera_config config
	global LWDAQ_Info LWDAQ_Driver
	
	LWDAQ_tool_init "Cage_Camera" "1.0"
	if {[winfo exists $info(window)]} {return 0}

	set config(scripts_dir) [file join $LWDAQ_Info(contents_dir) VBT]
	set config(record_dir) [file normalize "~/Desktop"]
	set config(segment_dir) [file join $config(scripts_dir) segments]
	set config(segment_length_s) "30"

	set info(text) stdout
	set info(state) "Idle"
	
	if {[file exists $info(settings_file_name)]} {
		uplevel #0 [list source $info(settings_file_name)]
	} 

	return 1	 
}

# Turn on the A3034X visible LEDs. We call the Windows version of ssh that 
# resides in the LWDAQ distribution. We catch the error returned by the
# Rasberri Pi and look for the phrase 
proc Cage_Camera_lampset {color state} {
	upvar #0 Cage_Camera_config config
	upvar #0 Cage_Camera_info info

		cd $config(scripts_dir)
	catch {exec ssh/ssh.exe \
		-o UserKnownHostsFile=/dev/null \
		-o StrictHostKeyChecking=no \
		-i A3034/ssh_keys/id_rsa pi@10.0.0.234 \
		< A3034/scripts/$color\_$state.sh} message
	if {[regexp {raspberrypi} $message]} {
		LWDAQ_print $info(text) "Success: Turned $color LEDs to $state."
	} {
		LWDAQ_print $info(text) "ERROR: $message"
	}
}

# Reboot the A3034X.
proc Cage_Camera_reboot {} {
	upvar #0 Cage_Camera_config config
	upvar #0 Cage_Camera_info info

	if {$info(state) == "Reboot"} {return ""}	

	cd $config(scripts_dir)
	catch {exec ssh/ssh.exe \
		 -o UserKnownHostsFile=/dev/null \
		 -o StrictHostKeyChecking=no \
		 -i A3034/ssh_keys/id_rsa pi@10.0.0.234 \
		 < A3034/scripts/reboot.sh} message
	if {[regexp {raspberrypi} $message]} {
		LWDAQ_print $info(text) "SUCCESS: Rebooted cage camera, wait one minute."
	} {
		LWDAQ_print $info(text) "ERROR: $message"
	}
}

# Display live video on the screen.
proc Cage_Camera_display {} {
	upvar #0 Cage_Camera_config config
	upvar #0 Cage_Camera_info info
	
	if {$info(state) != "Idle"} {
		LWDAQ_print $info(text) "ERROR: Wait until not $info(state)."
		return ""
	}
	set info(state) "Display"
	
	cd $config(scripts_dir)
	catch {exec ssh/ssh.exe \
		 -o UserKnownHostsFile=/dev/null \
		 -o StrictHostKeyChecking=no \
		 -i A3034/ssh_keys/id_rsa pi@10.0.0.234 \
		 < A3034/scripts/stream_display.sh} message
	if {[regexp {raspberrypi} $message]} {
		LWDAQ_print $info(text) "Started video server."
	} {
		LWDAQ_print $info(text) "ERROR: $message"
		return ""
	}
	LWDAQ_wait_ms 150
	catch {eval exec A3034/host_scripts/video_display.bat &} message
	LWDAQ_print $info(text) "Started live video display."
}

# Record live video to disk.
proc Cage_Camera_record {} {
	upvar #0 Cage_Camera_config config
	upvar #0 Cage_Camera_info info

	if {$info(state) != "Idle"} {
		LWDAQ_print $info(text) "ERROR: Wait until not $info(state)."
		return ""
	}
	set info(state) "Recording"
	
	cd $config(scripts_dir)
	catch {exec ssh/ssh.exe \
		 -o UserKnownHostsFile=/dev/null \
		 -o StrictHostKeyChecking=no \
		 -i A3034/ssh_keys/id_rsa pi@10.0.0.234 \
		 < A3034/scripts/stream_record.sh} message
	if {[regexp {raspberrypi} $message]} {
		LWDAQ_print $info(text) "Started video server."
	} {
		LWDAQ_print $info(text) "ERROR: $message"
		return ""
	}
	LWDAQ_wait_ms 150
	set location [file nativename $config(segment_dir)]
	if {[catch {
		eval exec A3034/host_scripts/video_record.bat \
			[regsub -all {\\} $location {\\\\}] $config(segment_length_s) &
		set result [eval exec [auto_execok echo] %result%]
	} message]} {
		LWDAQ_print $info(text) "ERROR: $message."
		return ""
	}
	LWDAQ_print $info(text) "Recording video segments in $location."
}

# Stop recording or streaming.
proc Cage_Camera_stop {} {
	upvar #0 Cage_Camera_config config
	upvar #0 Cage_Camera_info info

	set info(state) "Stop"
	cd $config(scripts_dir)
	catch {exec [auto_execok taskkill] /IM ffmpeg.exe /F} message
	catch {exec [auto_execok taskkill] /IM mplayer.exe /F} message
	LWDAQ_print $info(text) "Stopped recording and display."
	set info(state) "Idle"
}

#
# Cage_Camera_pick allows the user to pick a new record_dir or any other
# configuration file or directory.
#
proc Cage_Camera_pick {name {post 0}} {
	upvar #0 Cage_Camera_config config
	upvar #0 Cage_Camera_info info
	global LWDAQ_Info

	if {$info(state) != "Idle"} {
		LWDAQ_print $info(text) "ERROR: Wait until not $info(state)."
		return ""
	}
	set info(state) "Pick"

	# If we call this routine from a button, we prefer to post
	# its execution to the event queue, and this we can do by
	# adding a parameter of 1 to the end of the call.
	if {$post} {
		LWDAQ_post [list Cage_Camera_pick $name]
		return ""
	}

	if {[regexp "_file" $name]} {
		set fn [LWDAQ_get_file_name 0 [file dirname [set config($name)]]]
		if {![file exists $fn]} {
			Cage_Camera_print "WARNING: File \"$fn\" does not exist."
			return $fn
		}
		set config($name) $fn
		return $fn
	} 
	if {[regexp "_dir" $name]} {
		set dn [LWDAQ_get_dir_name [set config($name)]]
		if {![file exists $dn]} {
			Cage_Camera_print "WARNING: Directory \"$dn\" does not exist."
			return $dn
		}
		set config($name) $dn
		return $dn
	}
	
	set info(state) "Idle"
	return ""
}

proc Cage_Camera_open {} {
	upvar #0 Cage_Camera_config config
	upvar #0 Cage_Camera_info info

	set w [LWDAQ_tool_open $info(name)]
	if {$w == ""} {return 0}
	
	set f $w.controls
	frame $f
	pack $f -side top -fill x
	
	label $f.state -textvariable Cage_Camera_info(state) -fg blue 
	pack $f.state -side left -expand 1
	
	foreach a "off half on" {
		button $f.vs$a -text "Visible $a" \
			-command [list LWDAQ_post "Cage_Camera_lampset visible $a"]
		pack $f.vs$a -side left -expand 1
	}
	foreach a "off on" {
		button $f.ir$a -text "IR $a" \
			-command [list LWDAQ_post "Cage_Camera_lampset ir $a"]
		pack $f.ir$a -side left -expand 1
	}
	foreach a "Reboot Display Record Stop" {
		set b [string tolower $a]
		button $f.$b -text $a \
			-command [list LWDAQ_post "Cage_Camera_$b"]
		pack $f.$b -side left -expand 1
	}
	foreach a {Help Configure} {
		set b [string tolower $a]
		button $f.$b -text $a -command "LWDAQ_tool_$b $info(name)"
		pack $f.$b -side left -expand 1
	}

	set f $w.record
	frame $f
	pack $f -side top -fill x
	
	label $f.e -text "Redording Directory:" -anchor w
	pack $f.e -side left -expand 0
	entry $f.f -textvariable Cage_Camera_config(record_dir) -width 60
	button $f.g -text "Pick" -command "Cage_Camera_pick record_dir 1"
	pack $f.e $f.f $f.g -side left -expand 1


	set info(text) [LWDAQ_text_widget $w 100 15]

	LWDAQ_print $info(text) "$info(name) Version $info(version) \n"
	
	return 1
}

Cage_Camera_init
Cage_Camera_open
	
return 1

----------Begin Help----------

The Cage_Camera tool is a starting point form new, polite LWDAQ tools. 

Kevan Hashemi hashemi@brandeis.edu
----------End Help----------

----------Begin Data----------

----------End Data----------


