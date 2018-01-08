/*
----------------------------------------------
--- Author         : Michael Collins
--- Mail           : collins@opensourceinstruments.com
--- OpenCV Version : 2.4.10
--- Website        : opensourceinstruments.com
----------------------------------------------
*/
using namespace std;
#include "Blob.h"
#include "nms.h"
#include <fstream>
#include <string>
#include <iomanip>
#pragma warning(disable : 4996)
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>
#define SHOW_STEPS // un-comment | comment this line to show steps or not

// User defined settings
const int brightness_threshold_low = 55;
const int brightness_threshold_high = 255;
const int edge_thresh_low = 235;
const int edge_thresh_high = 255;
const int blur_size = 5;//pixels
const int minimum_blob_separation = 50;//pixels
const int blob_persistency= 5;//number of frames
const int minBlobRectArea = 100;//pixels
const int minBlobWidth = 20;//pixels
const int minBlobHeight = 20;//pixels
const int minBlobDiagonal = 20;//pixels
const int minBlobAreaDividedByRectArea = .2;//unitless

// User defined display windows
const bool show_darknessBinary = true;
const bool show_edgeDetect = true;
const bool show_finalBinary = false;
const bool show_contours = false;
const bool show_convexHulls = false;
const bool show_currentFrameBlobs = false;
const bool show_blobs = false;
const bool show_finalResult = true;

// const global variables
const cv::Scalar SCALAR_BLACK = cv::Scalar(0.0, 0.0, 0.0);
const cv::Scalar SCALAR_WHITE = cv::Scalar(255.0, 255.0, 255.0);
const cv::Scalar SCALAR_YELLOW = cv::Scalar(0.0, 255.0, 255.0);
const cv::Scalar SCALAR_GREEN = cv::Scalar(0.0, 200.0, 0.0);
const cv::Scalar SCALAR_RED = cv::Scalar(0.0, 0.0, 255.0);
const cv::Scalar SCALAR_BLUE = cv::Scalar(255.0, 0.0, 0.0);
const cv::Mat structuringElement3x3 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
const cv::Mat structuringElement5x5 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
const cv::Mat structuringElement7x7 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7));
const cv::Mat structuringElement15x15 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 15));


// function prototypes
void matchCurrentFrameBlobsToExistingBlobs(std::vector<Blob> &existingBlobs, std::vector<Blob> &currentFrameBlobs);
void addBlobToExistingBlobs(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs, int &intIndex);
void addNewBlob(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs);
double distanceBetweenPoints(cv::Point point1, cv::Point point2);
void drawAndShowContours(cv::Size imageSize, std::vector<std::vector<cv::Point> > contours, std::string strImageName);
void drawAndShowContours(cv::Size imageSize, std::vector<Blob> blobs, std::string strImageName);
void saveContours(cv::Size imageSize, std::vector<Blob> blobs, std::string strImageName);
void drawBlobInfoOnImage(std::vector<Blob> &blobs, cv::Mat &imgFrame2Copy);
void nmsBlobs(std::vector<Blob> &blobs);
void drawRectInfoOnImage(std::vector<cv::Rect> &rects, cv::Mat &imgFrame2Copy);
// global variables
std::stringstream date;
std::vector<cv::Rect> resRects;
cv::Rect region_of_interest;

int main(void) {				    
    cv::VideoCapture capVideo;
    cv::Mat imgFrame1;
    cv::Mat imgFrame2;
    std::vector<Blob> blobs;	

    // working directory is assumed to be local_parent/vbt/build
    // Videos are assumed to be in local_parent
    capVideo.open("../../2mice.avi");

    if (!capVideo.isOpened()) {                                                 // if unable to open video file
        std::cout << "error reading video file" << std::endl << std::endl;      // show error message
        return(0);                                                              // and exit program
    }

    if (capVideo.get(CV_CAP_PROP_FRAME_COUNT) < 2) {
        std::cout << "error: video file must have at least two frames";
        return(0);
    }
    std::cout << "Frame size is: " << unsigned(capVideo.get(3)) << "x" << unsigned(capVideo.get(4)) << std::endl << std::endl;      

    // Hard definition of the region of interest
    region_of_interest = cv::Rect(64,0,512,480);//uncomment to define ROI manually
    //region_of_interest = cv::Rect(0,0,capVideo.get(3),capVideo.get(4)); //uncomment to use ROI filling full original frame

    /* Video Output */
    cv::VideoWriter outputVideo;                    // Open the output
    int ex = static_cast<int>(capVideo.get(CV_CAP_PROP_FOURCC));
    // Output video size matches region of interest size
    cv::Size S = cv::Size((int) region_of_interest.width, (int) region_of_interest.height);
    // Output video goes to same directory as source video
    const string NAME = "../../output.avi"; 
    const bool askOutputType = 0; // If false it will use the inputs codec type
    if (askOutputType)
        outputVideo.open(NAME, ex=-1, capVideo.get(CV_CAP_PROP_FPS), S, true);
    else
        outputVideo.open(NAME, CV_FOURCC('M','J','P','G'), capVideo.get(CV_CAP_PROP_FPS), S, true);

    if (!outputVideo.isOpened())
    {
        std::cout << "Could not open output video for write:" << std::endl << std::endl;      // show error message
        return -1;
    }


    bool ret = true;
    ret =  capVideo.read(imgFrame1);
    if (ret == false) { return(0);}
    ret = capVideo.read(imgFrame2);
    if (ret == false) { return(0);}

    char chCheckForEscKey = 0;
    bool blnFirstFrame = true;
    int frameCount = 2;


    while (capVideo.isOpened() && chCheckForEscKey != 27) {
        cv::Mat imgThresh; // holds final thresholded image
        std::vector<Blob> currentFrameBlobs;
        cv::Mat imgFrame1Copy = imgFrame1.clone();
        imgFrame1Copy = imgFrame1Copy(region_of_interest);
        /*Uncomment for motion detection
        //cv::Mat imgFrame2Copy = imgFrame2.clone(); //uncomment for motion detection
        //imgFrame2Copy = imgFrame2Copy(region_of_interest);//uncomment for motion detection
        //cv::Mat imgDifference;//uncomment for motion detection
        //cv::Mat motion; //uncomment for motion detection
        */

        /* Show RGB channels separately - MC*/
        //Note: OpenCV uses BGR color order
        /*
        cv::Mat bgr[3];   //destination array
        split(imgFrame1Copy,bgr);//split source
        /*
        cv::imshow("blue",bgr[0]); //blue channel
        cv::imshow("green",bgr[1]); //green channel
        cv::imshow("red",bgr[2]); //red channel
        /* End RGB separation*/
        
        /* Get HSV representation
        cv::Mat cHSV;
        cv::cvtColor(imgFrame1Copy, cHSV,CV_BGR2HSV);
        */

        cv::cvtColor(imgFrame1Copy, imgFrame1Copy, CV_BGR2GRAY);
        //cv::cvtColor(imgFrame2Copy, imgFrame2Copy, CV_BGR2GRAY); //uncomment for motion detection

        cv::Mat dark;
        cv::threshold(imgFrame1Copy, dark,brightness_threshold_low, brightness_threshold_high, CV_THRESH_BINARY);
        bitwise_not (dark,dark);//invert thresholded darkness map
        cv::Mat edges = imgFrame1Copy.clone();
        cv::GaussianBlur(edges, edges, cv::Size(blur_size, blur_size), 0);

        /* Uncomment for motion detection
        //cv::GaussianBlur(imgFrame1Copy, imgFrame1Copy, cv::Size(blur_size, blur_size), 0);//uncomment for motion detection
        //cv::GaussianBlur(imgFrame2Copy, imgFrame2Copy, cv::Size(blur_size, blur_size), 0);//uncomment for motion detection
        //cv::absdiff(imgFrame1Copy, imgFrame2Copy, imgDifference);//uncomment for motion detection
        cv::threshold(imgDifference, motion, 10, 255.0, CV_THRESH_BINARY);
        */


        /* Sobel Edge Detection */
        //https://docs.opencv.org/2.4/doc/tutorials/imgproc/imgtrans/sobel_derivatives/sobel_derivatives.html
        cv::Mat grad;
        int scale = 1;
        int delta = 0;
        int ddepth = CV_16S;
        int c;
        // Generate grad_x and grad_y
        cv::Mat grad_x, grad_y;
        cv::Mat abs_grad_x, abs_grad_y;
        /// Gradient X
        //Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
        Sobel( edges, grad_x, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT );
        convertScaleAbs( grad_x, abs_grad_x );
        /// Gradient Y
        //Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
        Sobel( edges, grad_y, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT );
        convertScaleAbs( grad_y, abs_grad_y );
        /// Total Gradient (approximate)
        addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );
        /*End Sobel Edge Detection*/


        /* Loop dilates and erodes the threshold several times. A smoothing process
         with a tendeancy to dilate features and connect disjointed sections.
         This loop operates on the darkness thresholded image*/
        for (unsigned int i = 0; i < 1; i++) {
            cv::erode(dark, dark, structuringElement3x3);
            cv::erode(dark, dark, structuringElement3x3);
            cv::dilate(dark, dark, structuringElement3x3);
            cv::dilate(dark, dark, structuringElement3x3);
            cv::dilate(dark, dark, structuringElement5x5);
            cv::dilate(dark, dark, structuringElement5x5);
        }
        // Add the darkness thresholded image and the edge detected image
        bitwise_not (grad,grad);
        cv::threshold(grad, grad, edge_thresh_low, edge_thresh_high, CV_THRESH_BINARY);
        bitwise_and (dark, grad, imgThresh);
       
        /* Another erode/dilate loop
        for (unsigned int i = 0; i < 0; i++) {
            cv::erode(imgThresh, imgThresh, structuringElement3x3);
            cv::erode(imgThresh, imgThresh, structuringElement3x3);
            cv::dilate(imgThresh, imgThresh, structuringElement5x5);
        } */

        // Uncomment lines to visualize intermediate processing:
        //cv::imshow("motion",motion);
        //cv::imshow("edges",edges);
        if (show_edgeDetect) {
            cv::imshow("edge detection",grad);
        }
        if (show_darknessBinary) {
            cv::imshow("darkness binary", dark);
        }
        if (show_finalBinary) {
            cv::imshow("imgThresh", imgThresh);
        }

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(imgThresh, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        if (show_contours) {
            drawAndShowContours(imgThresh.size(), contours, "imgContours");
        }

        // Convex Hulls draw an envelope around the contours. That is, since mice are solid,
        // only the edges will be detected frame-to-frame. Convex hulls fills in their middle. -MC
        std::vector<std::vector<cv::Point> > convexHulls(contours.size());
        for (unsigned int i = 0; i < contours.size(); i++) {
            cv::convexHull(contours[i], convexHulls[i]);
        }
        if (show_convexHulls) {
            drawAndShowContours(imgThresh.size(), convexHulls, "imgConvexHulls");
        }

        /* Each convex hull is evaluated for its worth as a blob. Parameters like 
         * area and aspect ratio establish the convex hull's blobiness. Those worthy
         * are included in the list of currentFrameBlobs -MC */
        for (auto &convexHull : convexHulls) {
            Blob possibleBlob(convexHull);
            //  A convexity metric could be added here.
            // See Blob.h for more ideas
            if (possibleBlob.currentBoundingRect.area() > minBlobRectArea &&
                possibleBlob.dblCurrentAspectRatio > 0.3 &&
                possibleBlob.dblCurrentAspectRatio < 3.0 &&
                possibleBlob.currentBoundingRect.width > minBlobWidth &&
                possibleBlob.currentBoundingRect.height > minBlobHeight &&
                possibleBlob.dblCurrentDiagonalSize > minBlobDiagonal &&
                (cv::contourArea(possibleBlob.currentContour) / (double)possibleBlob.currentBoundingRect.area()) > 
                    minBlobAreaDividedByRectArea) {
					currentFrameBlobs.push_back(possibleBlob);
            }
        }

        if (show_currentFrameBlobs) {
            drawAndShowContours(imgThresh.size(), currentFrameBlobs, "imgCurrentFrameBlobs");
        }

        if (blnFirstFrame == true) {
            for (auto &currentFrameBlob : currentFrameBlobs) {
                blobs.push_back(currentFrameBlob);
            }
        } 
        else {
        // Handles blob tracking frame-to-frame. Moves currentFrameBlobs to blobs.
        // Includes parameters such as match distance and tracking time. 
            matchCurrentFrameBlobsToExistingBlobs(blobs, currentFrameBlobs);
        }

        if (show_blobs) {
            drawAndShowContours(imgThresh.size(), blobs, "imgBlobs");
        }


        imgFrame1Copy = imgFrame1.clone();	// get another copy of frame 2 since we changed the previous frame 2 copy in the processing above
        imgFrame1Copy = imgFrame1Copy(region_of_interest);

        nmsBlobs(blobs);//Non-maxima supression
        //sortRectangles(resRects);//Function not yet functional
        drawBlobInfoOnImage(blobs, imgFrame1Copy);
        drawRectInfoOnImage(resRects, imgFrame1Copy);

        if (show_finalResult) {
        cv::imshow("finalResult", imgFrame1Copy);
        }

        outputVideo.write(imgFrame1Copy);
        std::cout << "Frame number" << unsigned(frameCount) << std::endl << std::endl; //Displays progress


        /* For Frame-by-Frame visualization
        if(frameCount > 10 && frameCount < 12) {// Prints frames between count params
            char filename [50];
            char blobname [50];
            int n=sprintf (filename, "%d.png",frameCount);
            n=sprintf (blobname, "blob%d.png",frameCount);
            vector<int> compression_params;
            compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
            compression_params.push_back(9);
            cv::imwrite(filename,imgFrame1Copy,compression_params);
            saveContours(imgThresh.size(), currentFrameBlobs, blobname);
        }
        /* End frame-by-frame visualization*/

		
        // now we prepare for the next iteration
        currentFrameBlobs.clear();

        //imgFrame1 = imgFrame2.clone();// move frame 1 up to where frame 2 is//uncomment for motion detection

        if ((capVideo.get(CV_CAP_PROP_POS_FRAMES) + 1) < capVideo.get(CV_CAP_PROP_FRAME_COUNT)) {
            ret = capVideo.read(imgFrame1);
            //ret = capVideo.read(imgFrame2);//uncomment for motion detection
            if (ret == false) {return(0);}
        }
        else {
            std::cout << "end of video\n";
            break;
        }

        blnFirstFrame = false;
        frameCount++;
        chCheckForEscKey = cv::waitKey(1);
        }

        if (chCheckForEscKey != 27) {               // if the user did not press esc (i.e. we reached the end of the video)
            cv::waitKey(0);                         // hold the windows open to allow the "end of video" message to show
        }

            // note that if the user did press esc, we don't need to hold the windows open, we can simply let the program end which will close the windows
            return(0);
}


void matchCurrentFrameBlobsToExistingBlobs(std::vector<Blob> &existingBlobs, std::vector<Blob> &currentFrameBlobs) {
    for (auto &existingBlob : existingBlobs) {
        existingBlob.blnCurrentMatchFoundOrNewBlob = false;
        existingBlob.predictNextPosition();
    }

    for (auto &currentFrameBlob : currentFrameBlobs) {
        int intIndexOfLeastDistance = 0;
        double dblLeastDistance = 100000.0;

        for (unsigned int i = 0; i < existingBlobs.size(); i++) {

            if (existingBlobs[i].blnStillBeingTracked == true) {
                double dblDistance = distanceBetweenPoints(currentFrameBlob.centerPositions.back(), existingBlobs[i].predictedNextPosition);

                if (dblDistance < dblLeastDistance) {
                    dblLeastDistance = dblDistance;
                    intIndexOfLeastDistance = i;
                }
            }
        }
        // Combines blobs if their distance is less than a certain threshold

        //if (dblLeastDistance < currentFrameBlob.dblCurrentDiagonalSize * .5) { //relative
        if (dblLeastDistance < minimum_blob_separation) {//absolute
            // To make this more robust, we can add another check. This time for drastic size changes:
            // We do not yet implement it since that would require testing to make sure it doesn't hurt anything else:
            //if(cv::contourArea(currentFrameBlob.currentContour) / cv::contourArea(existingBlobs[intIndexOfLeastDistance].currentContour) > 0.5
                   //&& cv::contourArea(currentFrameBlob.currentContour) / cv::contourArea(existingBlobs[intIndexOfLeastDistance].currentContour) < 2.0)
            //{     
            addBlobToExistingBlobs(currentFrameBlob, existingBlobs, intIndexOfLeastDistance);
            //}
        }
        else {
            addNewBlob(currentFrameBlob, existingBlobs);
        }

    }

    for (auto &existingBlob : existingBlobs) {
        if (existingBlob.blnCurrentMatchFoundOrNewBlob == false) {
            existingBlob.intNumOfConsecutiveFramesWithoutAMatch++;
        }
        if (existingBlob.intNumOfConsecutiveFramesWithoutAMatch >=blob_persistency) {
            existingBlob.blnStillBeingTracked = false;
        }
    }
}


void addBlobToExistingBlobs(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs, int &intIndex) {
    existingBlobs[intIndex].currentContour = currentFrameBlob.currentContour;
    existingBlobs[intIndex].currentBoundingRect = currentFrameBlob.currentBoundingRect;
    existingBlobs[intIndex].centerPositions.push_back(currentFrameBlob.centerPositions.back());
    existingBlobs[intIndex].dblCurrentDiagonalSize = currentFrameBlob.dblCurrentDiagonalSize;
    existingBlobs[intIndex].dblCurrentAspectRatio = currentFrameBlob.dblCurrentAspectRatio;
    existingBlobs[intIndex].blnStillBeingTracked = true;
    existingBlobs[intIndex].blnCurrentMatchFoundOrNewBlob = true;
}


void addNewBlob(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs) {
    currentFrameBlob.blnCurrentMatchFoundOrNewBlob = true;
    existingBlobs.push_back(currentFrameBlob);
}


double distanceBetweenPoints(cv::Point point1, cv::Point point2) {    
    int intX = abs(point1.x - point2.x);
    int intY = abs(point1.y - point2.y);

    return(sqrt(pow(intX, 2) + pow(intY, 2)));
}


void drawAndShowContours(cv::Size imageSize, std::vector<std::vector<cv::Point> > contours, std::string strImageName) {    
	cv::Mat image(imageSize, CV_8UC3, SCALAR_BLACK);
    cv::drawContours(image, contours, -1, SCALAR_WHITE, -1);
    cv::imshow(strImageName, image);
}


void drawAndShowContours(cv::Size imageSize, std::vector<Blob> blobs, std::string strImageName) {    
    cv::Mat image(imageSize, CV_8UC3, SCALAR_BLACK);
    std::vector<std::vector<cv::Point> > contours;

    for (auto &blob : blobs) {
        if (blob.blnStillBeingTracked == true) {
            contours.push_back(blob.currentContour);
        }
    }

    cv::drawContours(image, contours, -1, SCALAR_WHITE, -1);
    cv::imshow(strImageName, image);
}

void saveContours(cv::Size imageSize, std::vector<Blob> blobs, std::string strImageName) {    
    cv::Mat image(imageSize, CV_8UC3, SCALAR_BLACK);
    std::vector<std::vector<cv::Point> > contours;

    for (auto &blob : blobs) {
        if (blob.blnStillBeingTracked == true) {
            contours.push_back(blob.currentContour);
        }
    }

    cv::drawContours(image, contours, -1, SCALAR_WHITE, -1);
    cv::imwrite(strImageName, image);
}




void nmsBlobs(std::vector<Blob> &blobs) {
     std::vector<cv::Rect> srcRects;
    std::vector<float> rectScores;
    for (unsigned int i = 0; i < blobs.size(); i++) {
        if (blobs[i].blnStillBeingTracked == true) {
            srcRects.push_back(blobs[i].currentBoundingRect);
            rectScores.push_back(blobs[i].currentBoundingRect.area());
        }
    }
    //std::cout << "Number of src rects is:" << unsigned(srcRects.size()) << std::endl << std::endl;
    nms2(srcRects,rectScores,resRects,.4f,0,0);
}


/*void sortRectangles(std::vector<cv::Rect> &rects) {
    std::multimap<float, size_t> idxs;
    for (size_t i = 0; i < size; ++i)
    {
        idxs.insert(std::pair<float, size_t>(scores[i], i));
    }
}*/

void drawRectInfoOnImage(std::vector<cv::Rect> &rects, cv::Mat &imgFrame1Copy) {
    std::cout << "Number of src rects is:" << unsigned(rects.size()) << std::endl << std::endl;      // show error message
    for (unsigned int i = 0; i < rects.size(); i++) {
    cv::rectangle(imgFrame1Copy, rects[i], SCALAR_RED, 2);
    }
}

void drawBlobInfoOnImage(std::vector<Blob> &blobs, cv::Mat &imgFrame1Copy) {
    for (unsigned int i = 0; i < blobs.size(); i++) {
        if (blobs[i].blnStillBeingTracked == true) {
            cv::rectangle(imgFrame1Copy, blobs[i].currentBoundingRect, SCALAR_BLUE, 2);
            
			int intFontFace = CV_FONT_HERSHEY_SIMPLEX;
			double dblFontScale = (imgFrame1Copy.rows * imgFrame1Copy.cols) / 300000.0;
            int intFontThickness = (int)std::round(dblFontScale * 1.0);

            cv::putText(imgFrame1Copy, std::to_string(i), blobs[i].centerPositions.back(), intFontFace, dblFontScale, SCALAR_GREEN, intFontThickness);
        }
    }
}
