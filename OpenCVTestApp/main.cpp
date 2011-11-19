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
#include "csuSubspaceTrain.h"
#include "csuSubspaceProject.h"
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

int main(int argc,char **argv)
{
    do
    {
        cout<<"Please enter an option"<<endl<<"1 - Training"<<endl<<"2 - Recognition"<<endl<<"3 - Bayesian"<<endl<<"4 - Exit"<<endl;
        cin >> option;
        switch(option)
        {
            case 1:
                FaceDetect();
                break;
            case 2:
                learn(trainfilelocation);
                recognizeFileList(testfilelocation);
                break;
            case 3:
                lda(argc, argv);
                //Bayesian(argc, argv);
                //csuBayesianTrain(argc, argv);
            case 4:
                exit(0);
        }

        cout<<"Do you want to continue? (y/n)"<<endl;
        cin >> response;
        
    }while(response.compare("y") == 0);
    
} 