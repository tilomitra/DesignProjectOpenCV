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
#include "FaceRec.h"
#include "FaceDetect.h"
//#include "csuBayesianTrain.h"
//#include "csuBayesianProject.h"
#include "LDATrain.h"
#include "LDA.h"
#include "LDAPreProcess.h"
#include "jpg2pgm.h"
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
using namespace std;

//File Strings
char trainfilelocation[256] = "/Users/sachi/Downloads/haarcascades/train.txt";
char testfilelocation[256] = "/Users/sachi/Downloads/haarcascades/test.txt";

//User Inputs
string response;
int option;
int argc;
bool saveNextFaces = FALSE;
char newPersonName[256];
int newPersonFaces = 0;
FILE *trainFile;
char cstr[256];
int Persons = 0; 

int main(int argc,char **argv)
{
    do
    {
        cout<<"Please enter an option"<<endl<<"1 - Normalize"<<endl<<"2 - JPGtoPGM"<<endl<<"3 - Face Detection"<<endl<<"4 - EigenFaces"<<endl<<"5 - LDA Training"<<endl<<"6 - LDA Testing"<<endl<<"7 - exit"<<endl<<"8 - new person"<<endl;
        cin >> option;
        switch(option)
        {
            case 1:
                PreProcessNormalize();
                break;
            case 2:
                jpg2pgm();
                break;
            case 3:
                FaceDetect();
                break;
            case 4:
              //learn(trainfilelocation);
//                recognizeFileList(testfilelocation);
            recognizeFromCam();
                break;                
            case 5:    
                lda_train();
                break;
            case 6:
                lda();
                break;
            case 7:
                exit(0);
            case 8:	
                NewPerson();
                //StartTraining();
                //recognizeFromCam();
                break;
            	
                
                
                        
                
        }

        cout<<"Do you want to continue? (y/n)"<<endl;
        cin >> response;
        
    }while(response.compare("y") == 0);
    
} 