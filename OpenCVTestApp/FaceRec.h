//
//  FaceRec.h
//  OpenCVTestApp
//
//  Created by Sachi on 11-10-30.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef OpenCVTestApp_FaceRec_h
#define OpenCVTestApp_FaceRec_h

#include <stdio.h>
// #include <conio.h>		// For _kbhit()
#include <dirent.h>		// For mkdir()
#include <vector>
#include <string>
//#include <string.h>
#include <stdio.h>
#include <string.h>
#include <opencv/cv.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cvaux.h>

 
using namespace std;



// Function prototypes

void storeEigenfaceImages();// Save all the eigenvectors as images, so that they can be checked
void learn(char *szFileTrain);// Train from the data in the given text file, and store the trained data into the file 'facedata.xml'
int  loadTrainingData(CvMat ** pTrainPersonNumMat);// Open the training data from the file 'facedata.xml'.
void storeTrainingData();// Save the training data to the file 'facedata.xml'.
int findNearestNeighbor(float * projectedTestFace, float *pConfidence, int iTrain);// Find the most likely person based on a detection. Returns the index, and stores the confidence value into pConfidence.
void doPCA();/// Do the Principal Component Analysis, finding the average image and the eigenfaces that represent any image in the given dataset.
int  loadFaceImgArray(char * filename);// Read the names & image filenames of people from a text file, and load all those images listed.
void recognizeFileList(char *szFileTest);// Recognize the face in each of the test images given, and compare the results with the truth.
IplImage* convertImageToGreyscale(const IplImage *imageSrc);// Return a new image that is always greyscale, whether the input image was RGB or Greyscale.
IplImage* resizeImage(const IplImage *origImg, int newWidth, int newHeight);// Creates a new image copy that is of a desired size.
IplImage* cropImage(const IplImage *img, const CvRect region);// Returns a new image that is a cropped version of the original image.
IplImage* convertFloatImageToUcharImage(const IplImage *srcImg);// Get an 8-bit equivalent of the 32-bit Float image.
void saveFloatImage(const char *filename, const IplImage *srcImg);// Store a greyscale floating-point CvMat image into a BMP/JPG/GIF/PNG image, since cvSaveImage() can only handle 8bit images (not 32bit float images).
CvRect detectFaceInImage(const IplImage *inputImg, const CvHaarClassifierCascade* cascade );// Perform face detection on the input image, using the given Haar cascade classifier.


#endif
