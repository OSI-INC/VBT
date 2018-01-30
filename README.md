# Video Blob Tracking (VBT)
This program is intended to track rodents within a video. It uses the OpenCV Library version 2.4.10 (**[version 2.4.10](https://sourceforge.net/projects/opencvlibrary/files/opencv-win/2.4.10/)**). It is derived from a (**[vehicle counting program](https://github.com/ahmetozlu/vehicle_counting)**). Unlike the vehicle counting program, this program does not consider motion between frames. Instead, it uses a blob detection routine which operates on individual frames.

## Theory
The VBT processes video frames consecutively. It considers each frame independently. Each frame is subjected to two basic thresholding operations. In the first, the frame is thresholded according to brightness. Since the VBT is intended to track rodents with black fur, dark regions are selected as candidate blob sites.

In the second thresholding operation, a Sobel edge detection routine is run to identifity homogenous regions of the frame. Typically, each region identified by the edge detection routine belongs to a single object, such as a rodent or a toy. The edge detection operation reduces the number of false positives which may be caused by textured surfaces with dark patches. For example, edge detection reduces the false positive rate associated with the shadows between wood chips.

The results of the two thresholding operations are superimposed to identify those regions of the image which are both dark and homogenous. These regions are referred to as blobs. Blobs which have shape and size characteristics corresponding to rodents are then identified. The blob positions are intended to correspond to the positions of black rodents.

A blob tracking routine keeps track of the history of blobs, and thus allows a blob to be tracked from one frame to the next while preserving its identity.

The project utlizes the [Blob Tracking](https://github.com/ahmetozlu/vehicle_counting/blob/master/VehicleDetectionAndCounting/Blob.cpp) module which provides various parameters for connecting blobs on video frames. It utilizes the [Non Maximum Suppression](https://github.com/Nuzhny007/Non-Maximum-Suppression) algorithm to associate disparate blobs which are actually associated with the same rodent.

Blob identity is typically lost when a rodent is obscured for a prolonged period or when two rodents touch one another.

## Installation

First step is to install OpenCV. Both OpenCV and the VBT builds are managed by the CMake utility so we build on MacOS, Windows, and Linux. When working on the VBT code, we assume you will be using Git to download the VBT repository and keep track of your own changes.


### MacOS:

Download OpenCV. We used this link:

https://github.com/opencv/opencv/archive/2.4.13.4.tar.gz

Decompress into a new directory, which we will assume is ~/OpenCV. Change directory into this ~/OpenCV. Install CMake command line tools.

	mkdir build_macos
	cd build_macos
	cmake -G "Unix Makefiles" ..
	make

The build takes ten minutes to complete. Now clone the VBT repository and build its executable.

	git clone https://github.com/OSI-INC/VBT.git
	cd VBT
	mkdir build_macos
	cd build_macos
	export OpenCV_DIR="../../OpenCV/build_macos"
	cmake -D OpenCV_DIR=$OpenCV_DIR -G "Unix Makefiles" ..
	make

Run the VBT executable and pass it the name of a video to process. You may pass it the name of an output video also, and a new file will be written with the blob tracking overlayed. For example:

	VBT ../../Videos/Mouse.mp4 Out.mp4

The above command applies blob tracking to the Mouse.mp4 video and produces Out.mp4.


### Windows 32-Bit

Download OpenCV using a link something like this:

https://github.com/opencv/opencv/archive/2.4.13.4.tar.gz

Decompress into a new directory, which we will assume is ~/OpenCV. Change directory into this ~/OpenCV. Install CMake command line tools, which adds the path to the cmake.exe binary to the Windows PATH variable, which in turn is incorporated into the MSYS variable of the same name. Before the MSYS make of OpenCV will work, we have to edit a file called C:/MinGW/include/commctrl.h, which is part of the MSYS installation, to include the lines:

	#if 1
	#define _WIN32_IE    0x0500
	#endif
	#endif

With that done, we navigate to ~/OpenCV in MSYS and do:

	mkdir build_windows32
	cd build_windows32
	cmake -G "MSYS Makefiles" ..
	make

The build takes twenty minutes. Now download the VBT repository and build.

	git clone https://github.com/OSI-INC/VBT.git
	cd VBT
	mkdir build_windows32
	cd build_windows32
	export OpenCV_DIR="../../OpenCV/build_windows32"
	cmake -D OpenCV_DIR=$OpenCV_DIR -G "MSYS Makefiles" ..
	make

If you get the error "unrecognised command line option -std=c++11", you must update the MSYS c-compiler so that it supports the C++11 standard. Now add the following directory to your MSYS path, or your default DOS environment path.

~/OpenCV/build_windows32/bin 

If you don’t add the above path, VBT will abort with an error saying Windows cannot find library libopencv_core2413.dll. The following command will run the blob tracking on Mouse.mp4 and produce Out.mp4.

	VBT ../../Videos/Mouse.mp4 Out.mp4


### Linux 32-Bit

Install CMake and gcc-c++ with the following commands or equivalent for your version of Linux:

	yum install gcc-c++
	yum install cmake

Download OpenCV using a link something like this:

https://github.com/opencv/opencv/archive/2.4.13.4.tar.gz

Decompress into a new directory, which we will assume is ~/OpenCV. Change directory into this ~/OpenCV and execute the following commands.

	mkdir build_linux32
	cd build_linux32
	cmake -G "Unix Makefiles" ..
	make

Now download the VBT repository and build.

	git clone https://github.com/OSI-INC/VBT.git
	cd VBT
	mkdir build_linux32
	cd build_linux32
	export OpenCV_DIR="../../OpenCV/build_linux32"
	cmake -D OpenCV_DIR=$OpenCV_DIR -G "Unix Makefiles" ..
	make

If the build fails with "unrecognised command line option -std=c++11", you must update your c-compiler so that it supports the c++11 standard.


### Linux 64-Bit

Install CMake and gcc-c++ with the following commands or equivalent for your version of Linux:

	yum install gcc-c++
	yum install cmake

Download OpenCV using a link something like this:

https://github.com/opencv/opencv/archive/2.4.13.4.tar.gz

Decompress into a new directory, which we will assume is ~/OpenCV. Change directory into this ~/OpenCV and execute the following commands.

	mkdir build_linux64
	cd build_linux64
	cmake -G "Unix Makefiles" ..
	make
	
Now download the VBT repository and build.

	git clone https://github.com/OSI-INC/VBT.git
	cd VBT
	mkdir build_linux64
	cd build_linux64
	export OpenCV_DIR="../../OpenCV/build_linux64"
	cmake -D OpenCV_DIR=$OpenCV_DIR -G "Unix Makefiles" ..
	make

If the build fails with "unrecognised command line option -std=c++11", you must update your c-compiler so that it supports the c++11 standard.


## Authors
Michael Collins, Open Source Instruments Inc.
Kevan Hashemi, Open Source Instruments Inc.

## License
This system is available under the [GNU General Public License](http://www.gnu.org/licenses/gpl.txt).


