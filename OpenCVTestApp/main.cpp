//
//  ImageDetection.cpp
//  OpenCVTest
//
//  Created by Tilo Mitra on 11-10-22.
//  Copyright (c) 2011 University of Waterloo. All rights reserved.
//
//  This file demonstrates how to do facial recognition on static images.
//  
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <opencv/cv.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cvaux.h>
#include "PreProcess.h"
#include "FaceRec.h"
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
using namespace std;



// Haar Cascade file, used for Face Detection


 int main( int argc, char** argv )
{
    // declarations
    CvHaarClassifierCascade * pCascade = 0;  // the face detector
    CvMemStorage * pStorage = 0;        // expandable memory buffer
    CvSeq * pFaceRectSeq;               // list of detected faces
    int i,j;
    string response;
    char name[32];
    char folder[256];
    char image[256];
    char imagename[256];
    char folderpath[256];
    
    // initializations
    IplImage * pInpImg = 0;
    IplImage * img = 0;
    IplImage * imageProcessed = 0;
    
    //ask for filename
    /*do {
    cout << "Please enter a filename: ";
    scanf("%s", folder);
    sprintf(image, "%s%s", "/Users/sachi/Downloads/haarcascades/", folder);

    //printf("%s",name);
    
    opendir(image);
    
    for(j=1;j<11;j++)
    {
    sprintf(imagename, "%s%s%s%s%i.jpg", "/Users/sachi/Downloads/haarcascades/", folder,"/",folder,j);
    sprintf(folderpath,"%s%s%s","/Users/sachi/Downloads/haarcascades/",folder,"/face/");
    mkdir(folderpath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    pInpImg = cvLoadImage(imagename, CV_LOAD_IMAGE_UNCHANGED);
    pStorage = cvCreateMemStorage(0);
    pCascade = (CvHaarClassifierCascade *)cvLoad
    (("/Users/sachi/Downloads/haarcascades/haarcascade_frontalface_alt.xml"),
     0, 0, 0 );
    
    // validate that everything initialized properly
    if( !pInpImg || !pStorage || !pCascade )
    {
        printf("Initialization failed: %s \n",
               (!pInpImg)?  "didn't load image file" :
               (!pCascade)? "didn't load Haar cascade -- "
               "make sure path is correct" :
               "failed to allocate memory for data storage");
        exit(-1);
    }
    
    // detect faces in image
    pFaceRectSeq = cvHaarDetectObjects
    (pInpImg, pCascade, pStorage,
     1.1,                       // increase search scale by 10% each pass
     3,                         // drop groups of fewer than three detections
     CV_HAAR_DO_CANNY_PRUNING,  // skip regions unlikely to contain a face
     cvSize(0,0));              // use XML default for smallest search scale
    
    // create a window to display detected faces
    cvNamedWindow("Haar Window", CV_WINDOW_AUTOSIZE);
        
    // File strings
        char filepath[256];
      
        
    
    // draw a rectangular outline around each detection
    for(i=0;i<(pFaceRectSeq? pFaceRectSeq->total:10); i++ )
    {
        CvRect * r = (CvRect*)cvGetSeqElem(pFaceRectSeq, i);
        CvPoint pt1 = { r->x, r->y };
        CvPoint pt2 = { r->x + r->width, r->y + r->height };
        cvRectangle(pInpImg, pt1, pt2, CV_RGB(0,255,0), 2, 4, 0);
        //IplImage * imgcpy = cvCloneImage(pInpImg);
        //cvRectangle(imgcpy, pt1, pt2, CV_RGB(0,255,0), 3, 4, 0);
        cvSetImageROI(pInpImg,cvRect(r->x, r->y, r->width,r->height)); //selecting the region of interest from the image i.e. the rectangle containing the face
        img = cvCreateImage(cvGetSize(pInpImg),pInpImg->depth, pInpImg->nChannels);//creating an image of same size and dimensions of the region of interest

        cvCopy(pInpImg,img,NULL);
                
        cvResetImageROI( pInpImg );//reset region of interest
        
        imageProcessed = Equalize(img);


        sprintf(filepath,"%s%s%s%s%d_%d.jpg", "/Users/sachi/Downloads/haarcascades/",folder,"/face/",folder, j, i);
        
        cvSaveImage(filepath,imageProcessed); 
        
        
        //printf("%d",i);
       
        
    }
    
    // display face detections
    //cvShowImage("Haar Window", pInpImg);
    //cvWaitKey(0);
    //cvDestroyWindow("Haar Window");
    

        
    // clean up and release resources
    cvReleaseImage(&pInpImg);
    cvReleaseImage(&img);
    if(pCascade) cvReleaseHaarClassifierCascade(&pCascade);
    if(pStorage) cvReleaseMemStorage(&pStorage);
        
    }
        

        
    cout << "Do you want to continue? [y/n]" << endl;
    cin >> response;
    }while(response.compare("y") == 0);
*/    
    learn("/Users/sachi/Downloads/haarcascades/train.txt");
    recognizeFileList("/Users/sachi/Downloads/haarcascades/test.txt");
    
    return 0;
 
    
   
} 