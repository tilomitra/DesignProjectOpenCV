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
//#include "GetDirectoryFiles.h"

#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
using namespace std;

//File Strings
char trainfilelocation[256] = "/Users/sachi/Downloads/haarcascades/train.txt";
char testfilelocation[256] = "/Users/sachi/Downloads/haarcascades/test.txt";
string response;
int option;
// Haar Cascade file, used for Face Detection


 int main()
{
    do
    {
        cout<<"Please enter an option"<<endl<<"1 - Face Detection"<<endl<<"2 - Face Rec"<<endl;
        cin >> option;
    switch(option)
        {
        case 1:
            FaceDetect();
        case 2:
            learn(trainfilelocation);
            recognizeFileList(testfilelocation);
        }
        
  //  FaceDetect();
  //  GetDirectoryFiles("sjobs");
    
  // learn(trainfilelocation);
  //  recognizeFileList(testfilelocation);
     
    return 0;
    cout << "Do you want to continue? [y/n]" << endl;
    cin >> response;
        
    }while(response.compare("y") == 0);

  
   
} 