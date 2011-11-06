//
//  FaceDetect.cpp
//  OpenCVTestApp
//
//  Created by Sachi on 11-11-05.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "FaceDetect.h"

int FaceDetect()
{
    
  // declarations
    CvHaarClassifierCascade * pCascade = 0;  // the face detector
    CvMemStorage * pStorage = 0;        // expandable memory buffer
    CvSeq * pFaceRectSeq;               // list of detected faces
    int i;
    int j = 1;
    string response;
  //  ofstream file;
    //File Strings
    char foldername[256];
    char folderlocation[256] = "/Users/sachi/Downloads/haarcascades/";
    char imagelocation[256];
    char imagename[256];
    char facepath[256];
    char facename[256];
    char haarcascadelocation[256] = "/Users/sachi/Downloads/haarcascades/haarcascade_frontalface_alt.xml";
    string array[25];
    char user;
    // initializations
    IplImage * image = 0;
    IplImage * faceimage = 0;
    IplImage * equalizedface = 0;
    
    ifstream file("/Users/sachi/Documents/development/OpenCVTestApp/OpenCVTestApp/myfile.txt");
    string fileline[30];
    int a = 0;
    size_t found;
    //ask for filename
    do 
    {
        cout << "Please enter the foldername: ";
        scanf("%s", foldername);
        sprintf(imagelocation, "%s%s",folderlocation, foldername); 
        opendir(imagelocation);
    
        GetDirectoryFiles(foldername);//gets the file list and saves it into myfile.txt
    
        //for(j=1; j<11; j++) //show values read from file
        while(!file.eof())
        {
            getline(file,fileline[a]);
            found = fileline[a].find(".jpg");
            if(found!=string::npos)
            {
                sprintf(imagename, "%s%s%s%s", folderlocation, foldername,"/",fileline[a].c_str());
                //sprintf(facepath,"%s%s%s",folderlocation,foldername,"/face/");
                //mkdir(facepath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                image = cvLoadImage(imagename, CV_LOAD_IMAGE_UNCHANGED);
                pStorage = cvCreateMemStorage(0);
                pCascade = (CvHaarClassifierCascade *)cvLoad
                ((haarcascadelocation),
                 0, 0, 0 );
                
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
                
                // detect faces in images
                pFaceRectSeq = cvHaarDetectObjects
                (image, pCascade, pStorage,
                 1.1,                       // increase search scale by 10% each pass
                 3,                         // drop groups of fewer than three detections
                 CV_HAAR_DO_CANNY_PRUNING,  // skip regions unlikely to contain a face
                 cvSize(0,0));              // use XML default for smallest search scale
                
                // create a window to display detected faces
                cvNamedWindow("Haar Window", CV_WINDOW_AUTOSIZE);
                
                // draw a rectangular outline around each detection
                for(i=0;i<(pFaceRectSeq? pFaceRectSeq->total:10); i++ )
                {
                    
                    CvRect * r = (CvRect*)cvGetSeqElem(pFaceRectSeq, i);
                    //CvPoint pt1 = { r->x, r->y };
                    //CvPoint pt2 = { r->x + r->width, r->y + r->height };
                    //cvRectangle(image, pt1, pt2, CV_RGB(0,255,0), 2, 4, 0);
                    //IplImage * imgcpy = cvCloneImage(image);
                    //cvRectangle(imgcpy, pt1, pt2, CV_RGB(0,255,0), 3, 4, 0);
                    cvSetImageROI(image,cvRect(r->x, r->y, r->width,r->height)); //selecting the region of interest from the image i.e. the rectangle containing the face
                    faceimage = cvCreateImage(cvGetSize(image),image->depth, image->nChannels);//creating an image of same size and dimensions of the region of interest
                    
                    cvCopy(image,faceimage,NULL);
                    
                    cvResetImageROI( image );//reset region of interest
                    
                    equalizedface = Equalize(faceimage);
                    
                    
                    sprintf(facename,"%s%s%s%d%s", folderlocation,foldername,"/face/",i, fileline[a].c_str());
                    
                    cvSaveImage(facename,equalizedface); 
                    
                    
                    //printf("%d",i);
                    
                    
                }
                
                // display face detections
                //cvShowImage("Haar Window", image);
                //cvWaitKey(0);
                //cvDestroyWindow("Haar Window");
                
                a++;
                
                // clean up and release resources
                cvReleaseImage(&image);
                cvReleaseImage(&faceimage);
                if(pCascade) cvReleaseHaarClassifierCascade(&pCascade);
                if(pStorage) cvReleaseMemStorage(&pStorage);
                
            }
            
        }
 
 
        /* for(i=0; i<5; i++) //clear array
         array[i] = " ";
         
         ifstream in("/Users/sachi/Documents/development/OpenCVTestApp/OpenCVTestApp/myfile.txt", ifstream::in);
         //in.read((char *) &array, sizeof array);
         for(i=0; !in.eof(); i++) //show values read from file
         {
         in >> array[i];
         */
        
        cout << "Do you want to continue? [y/n]" << endl;
        cin >> response;
        
    }while(response.compare("y") == 0);
    

    file.close();
    return 0;


}

