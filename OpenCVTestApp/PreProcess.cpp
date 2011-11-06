//
//  PreProcess.cpp
//  OpenCVTestApp
//
//  Created by Sachi on 11-11-05.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "PreProcess.h"

IplImage * Equalize(IplImage * img)

{
    IplImage * faceImage = 0;
    IplImage * imageProcessed = 0;
    
    if (img->nChannels == 3) 
    {
        faceImage = cvCreateImage( cvSize(img->width,img->height),IPL_DEPTH_8U, 1);
        // Convert from RGB (actually it is BGR) to BGRA (4 chanels).
        cvCvtColor( img, faceImage, CV_BGR2GRAY);
    }
    else 
    {
        // Just use the input image
        faceImage = img;
    }
    
    // Resize the image to be a consistent size, even if the aspect ratio changes.    
    imageProcessed = cvCreateImage(cvSize(128, 128), IPL_DEPTH_8U, 1);
    
    // Make the image a fixed size.
    // CV_INTER_CUBIC or CV_INTER_LINEAR is good for enlarging, and
    // CV_INTER_AREA is good for shrinking / decimation, but bad at enlarging.
    cvResize(faceImage, imageProcessed, CV_INTER_LINEAR);
    
    // Give the image a standard brightness and contrast.
    cvEqualizeHist(imageProcessed, imageProcessed);
    return imageProcessed;
}