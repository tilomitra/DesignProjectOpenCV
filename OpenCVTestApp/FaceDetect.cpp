//
//  FaceDetect.cpp
//  OpenCVTestApp
//
//  Created by Sachi on 11-11-05.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "FaceDetect.h"

int FaceDetect(char inputimage[256])
{
    
    // declarations
    CvHaarClassifierCascade * pCascade = 0;  // the face detector
    CvMemStorage * pStorage = 0;        // expandable memory buffer
    CvSeq * pFaceRectSeq;               // list of detected faces
    int i;
    
    string response;
    //  ofstream file;
    //File Strings
    char foldername[256];
    char folderlocation[256] = "/Users/sachi/Downloads/haarcascades/";
    char imagelocation[256];
    char imagename[256];    
    char facename[256];
    char haarcascadelocation[256] = "/Users/sachi/Downloads/haarcascades/haarcascade_frontalface_alt.xml";
    char myfile[256] = "/Users/sachi/Documents/development/OpenCVTestApp/OpenCVTestApp/myfile.txt";
    char testface[256] = "/Users/sachi/Documents/development/OpenCVTestApp/OpenCVTestApp/DemoImages/test/testface.pgm";
    // initializations
    IplImage * image = 0;
    IplImage * faceimage = 0;
    IplImage * equalizedface = 0;
    
    
    string fileline[100];
    int a = 0;
    size_t found;
    //ask for filename
//    cout << "Please enter name of the person: ";
//    scanf("%s", foldername);
//    sprintf(imagelocation, "%s%s",folderlocation, foldername); 
//    opendir(imagelocation);
//    ifstream file(myfile);
//    GetDirectoryFiles(foldername);//gets the file list and saves it into myfile.txt
    
    //for(j=1; j<11; j++) //show values read from file
//    while(!file.eof())
//    {
//        getline(file,fileline[a]);
//        found = fileline[a].find(".jpg");
//        if(found!=string::npos)
//        {
//            sprintf(imagename, "%s%s%s%s", folderlocation, foldername,"/",fileline[a].c_str());
//            //sprintf(facepath,"%s%s%s",folderlocation,foldername,"/face/");
//            //mkdir(facepath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            image = cvLoadImage(inputimage, CV_LOAD_IMAGE_UNCHANGED);
            pStorage = cvCreateMemStorage(0);
            pCascade = (CvHaarClassifierCascade *)cvLoad
            ((haarcascadelocation),
             0, 0, 0 );
//            
            // validate that everything initialized properly
            if( !image || !pStorage || !pCascade )
            {
                printf("Initialization failed: %s \n",
                       (!image)?  "didn't load images file" :
                       (!pCascade)? "didn't load Haar cascade -- "
                       "make sure path is correct" :
                       "failed to allocate memory for data storage");
                exit(-1);
            }
//            
            // detect faces in images
            pFaceRectSeq = cvHaarDetectObjects
            (image, pCascade, pStorage,
             1.1,                       // increase search scale by 10% each pass
             3,                         // drop groups of fewer than three detections
             CV_HAAR_DO_CANNY_PRUNING,  // skip regions unlikely to contain a face
             cvSize(0,0));              // use XML default for smallest search scale
            
            // draw a rectangular outline around each detection
            for(i=0;i<(pFaceRectSeq? pFaceRectSeq->total:10); i++ )
            {
                
                CvRect * r = (CvRect*)cvGetSeqElem(pFaceRectSeq, i);
                cvSetImageROI(image,cvRect(r->x, r->y, r->width,r->height)); //selecting the region of interest from the image i.e. the rectangle containing the face
                faceimage = cvCreateImage(cvGetSize(image),image->depth, image->nChannels);//creating an image of same size and dimensions of the region of interest
                
                cvCopy(image,faceimage,NULL);
                
                cvResetImageROI( image );//reset region of interest
                
                equalizedface = Equalize(faceimage);
                
//                IplImage * mirrorimage = Equalize(faceimage);
                
//                sprintf(facename,"%s%s%s%d%s", folderlocation,foldername,"/face/",i, fileline[a].c_str());
              
//                cvFlip(equalizedface,mirrorimage,-1);
                cvSaveImage(testface,equalizedface);

            }
            
            a++;
            
            // clean up and release resources
            cvReleaseImage(&image);
            cvReleaseImage(&faceimage);
            if(pCascade) cvReleaseHaarClassifierCascade(&pCascade);
            if(pStorage) cvReleaseMemStorage(&pStorage);
            
//        }
        
//    }
    
//    file.close();
    return 0;
    
}

