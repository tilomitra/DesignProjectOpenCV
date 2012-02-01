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
#include <curl/curl.h> 
#include <sys/stat.h>
#include <sys/time.h>
#include <opencv/cv.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cvaux.h>
#include "FaceRec.h"
#include "FaceDetect.h"
#include "csuBayesianTrain.h"
#include "csuBayesianProject.h"
#include "LDATrain.h"
#include "LDA.h"
#include "LDAPreProcess.h"
#include "jpg2pgm.h"
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>


using namespace std;

//File Strings
char trainfilelocation[256] = "/Users/sachi/Documents/development/OpenCVTestApp/OpenCVTestApp/train.txt";
char testfilelocation[256] = "/Users/sachi/Downloads/haarcascades/test.txt";

//User Inputs
string response;
int option;
//char option[100];
int argc;
bool saveNextFaces = FALSE;
char newPersonName[256];
int newPersonFaces = 0;
FILE *trainFile;
char cstr[256];
int Persons = 0; 
int n;
int i=17;
char dstr[256];
FILE *LDAtrainFile;
FILE *LDAtestFile;
FILE *CoordsFile;
string lda_result;
vector<string> eigenface_result;
IplImage *source;
IplImage *greyImg;
IplImage *equalizedImg;
char testpgm[] = "/Users/sachi/Documents/development/OpenCVTestApp/OpenCVTestApp/DemoImages/test/test.pgm";
char testsourcepgm[] = "/Users/sachi/Documents/development/OpenCVTestApp/OpenCVTestApp/data/csuScrapShots/source/pgm/test.pgm";
char testnormpgm[] = "/Users/sachi/Documents/development/OpenCVTestApp/OpenCVTestApp/data/csuScrapShots/normSep2002pgm/test.pgm";
char testjpg[] = "/Users/sachi/Documents/development/OpenCVTestApp/OpenCVTestApp/DemoImages/test/test.jpg";
int main(int argc,char **argv)
{
    do
    {
//        cout<<"Please enter an option"<<endl<<"1 - Normalize"<<endl<<"2 - JPGtoPGM"<<endl<<"3 - Face Detection"<<endl<<"4 - EigenFaces"<<endl<<"5 - LDA Training"<<endl<<"6 - LDA Testing"<<endl<<"7 - exit"<<endl<<"8 - new person"<<endl;        
//        cin.getline (option,100,'\n');
//        CoordsFile = fopen("/Users/sachi/Documents/development/OpenCVTestApp/OpenCVTestApp/imagelists/coords.txt", "a");
//        LDAtrainFile = fopen("/Users/sachi/Documents/development/OpenCVTestApp/OpenCVTestApp/imagelists/scrap_all.srt", "a");
//        LDAtestFile = fopen("/Users/sachi/Documents/development/OpenCVTestApp/OpenCVTestApp/imagelists/scrap_all_x4.srt", "a");
//    trainFile = fopen("/Users/sachi/Documents/development/OpenCVTestApp/OpenCVTestApp/train.txt", "a");
//        for (n=1;n<10;n++)
//        {
//            sprintf(dstr, "/Users/sachi/Documents/development/OpenCVTestApp/OpenCVTestApp/DemoImages/%s/%d_%s%d.pgm", option,i, option, n);
//
//            fprintf(LDAtrainFile,"%d_%s%d.sfi ", i, option, n);
//            fprintf(LDAtestFile,"%d_%s%d.sfi\n", i, option, n);
//            fprintf(CoordsFile,"%d_%s%d\n", i, option, n);
//            fprintf(trainFile, "%d %s %s\n", i, option, dstr);
//
//        }
//        i++;
//        fprintf(LDAtrainFile,"\n");
//        fclose(LDAtrainFile);
//        fclose(LDAtestFile);
//        fclose(CoordsFile);
//         fclose(trainFile);

        
////jpg to pgm conversion
//        IplImage* test = 0;
//        test = cvLoadImage("/Users/sachi/Documents/development/faces94/male/ajones/ajones.20.jpg");
//        test = convertImageToGreyscale(test);
//        cvSaveImage("/Users/sachi/Documents/development/faces94/male/ajones/ajone20.pgm",test);
        
        cout<<"Please enter an option"<<endl<<"1 - Training"<<endl<<"2 - Testing"<<endl;
        cin>>option;
            switch(option)
            {      
                case 1:
                    TrainNewPerson();
                    PreProcessNormalize();
                    lda_train();
                    break;
                case 2:
                    source = cvLoadImage(testjpg);
                    greyImg = convertImageToGreyscale(source);
                    equalizedImg = cvCreateImage(cvGetSize(greyImg), 8, 1);	// Create an empty greyscale image
                    cvEqualizeHist(greyImg, equalizedImg);
                    equalizedImg = resizeImage(equalizedImg, 128, 128);
                    cvSaveImage(testpgm,equalizedImg);
                    cvSaveImage(testsourcepgm,equalizedImg);
                    cvSaveImage(testnormpgm,equalizedImg);
                    PreProcessNormalize();
                    FaceDetect(testpgm);
                    eigenface_result = recognizeFileList(testpgm);
                    lda_result = lda();
                    cout<<eigenface_result[0]<<" "<<lda_result<<"\n";
                    break;
                case 3:
                    lda();
                    break;
                case 4:
                    PreProcessNormalize();
                    break;
                case 5:
                    break;
            }
//        
            
//        cout<<"Do you want to continue? (y/n)"<<endl;
//        cin >> response;
        
    }while(1);
    
} 