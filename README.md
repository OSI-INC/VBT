# Video Blob Tracking (VBT)
This program is intended to track rodents within a video. It uses the OpenCV Library version 2.4.10 (**[version 2.4.10](https://sourceforge.net/projects/opencvlibrary/files/opencv-win/2.4.10/)**). It is derived from a (**[vehicle counting program](https://github.com/ahmetozlu/vehicle_counting)**). Unlike the vehicle counting program, this program does not consider motion between frames. Instead, it uses a blob detection routine which operates on individual frames.

## Theory
The VBT processes video frames consecutively. It considers each frame independently. Each frame is subjected to two basic thresholding operations. In the first, the frame is thresholded according to brightness. Since the VBT is intended to track rodents with black fur, dark regions are selected as candidate blob sites.

In the second thresholding operation, a Sobel edge detection routine is run to identifity homogenous regions of the frame. Typically, each region identified by the edge detection routine belongs to a single object, such as a rodent or a toy. The edge detection operation reduces the number of false positives which may be caused by textured surfaces with dark patches. For example, edge detection reduces the false positive rate associated with the shadows between wood chips.

The results of the two thresholding operations are superimposed to identify those regions of the image which are both dark and homogenous. These regions are referred to as blobs. Blobs which have shape and size characteristics corresponding to rodents are then identified. The blob positions are intended to correspond to the positions of black rodents.

A blob tracking routine keeps track of the history of blobs, and thus allows a blob to be tracked from one frame to the next while preserving its identity.

The project utlizes the [Blob Tracking](https://github.com/ahmetozlu/vehicle_counting/blob/master/VehicleDetectionAndCounting/Blob.cpp) module which provides various parameters for connecting blobs on video frames. It utilizes the [Non Maximum Suppression] (https://github.com/Nuzhny007/Non-Maximum-Suppression) algorithm to associate disparate blobs which are actually associated with the same rodent.

Blob identity is typically lost when a rodent is obscured for a prolonged period or when two rodents touch one another.


## Installation

**Building the project using CMake from the command-line:**

Linux:

    export OpenCV_DIR="~/OpenCV/build"
    mkdir build
    cd build
    cmake -D OpenCV_DIR=$OpenCV_DIR ..
    make 

MacOSX (Xcode):

    export OpenCV_DIR="~/OpenCV/build"
    mkdir build
    cd build
    cmake -G Xcode -D OpenCV_DIR=$OpenCV_DIR ..    

Windows (MS Visual Studio):

    set OpenCV_DIR="C:\OpenCV\build"
    mkdir build
    cd build
    cmake -G "Visual Studio 12 2013" -D OpenCV_DIR=%OpenCV_DIR% ..  

## Author
Michael Collins

## License
This system is available under the [GNU General Public License](http://www.gnu.org/licenses/gpl.txt).
