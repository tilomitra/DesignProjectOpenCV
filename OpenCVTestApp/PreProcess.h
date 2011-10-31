//
//  PreProcess.h
//  OpenCVTestApp
//
//  Created by Sachi on 11-10-29.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef OpenCVTestApp_PreProcess_h
#define OpenCVTestApp_PreProcess_h

#include <opencv/cv.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cvaux.h>

IplImage * faceImage = 0;
IplImage * imageProcessed = 0;

IplImage * Equalize(IplImage * img)

{
    if (img->nChannels == 3) {
    faceImage = cvCreateImage( cvSize(img->width,img->height), 
                      IPL_DEPTH_8U, 4);
    // Convert from RGB (actually it is BGR) to BGRA (4 chanels).
    cvCvtColor( img, faceImage, CV_BGR2BGRA );
    }
    else {
    // Just use the input image
    faceImage = img;
    }

    // Resize the image to be a consistent size, even if the aspect ratio changes.
    
    imageProcessed = cvCreateImage(cvSize(50, 50), IPL_DEPTH_8U, 4);
    // Make the image a fixed size.
    // CV_INTER_CUBIC or CV_INTER_LINEAR is good for enlarging, and
    // CV_INTER_AREA is good for shrinking / decimation, but bad at enlarging.
    cvResize(faceImage, imageProcessed, CV_INTER_LINEAR);
    
    // Give the image a standard brightness and contrast.
  //cvEqualizeHist(imageProcessed, imageProcessed);
    return imageProcessed;
}

#endif
