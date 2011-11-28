//
//  csuPreprocessNormalize.h
//  OpenCVTestApp
//
//  Created by Sachi on 11-11-20.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef OpenCVTestApp_csuPreprocessNormalize_h
#define OpenCVTestApp_csuPreprocessNormalize_h
#include "csuCommon.h"
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>

using namespace std;

typedef struct {
    FTYPE x1, y1, x2, y2;
} TwoPoints;

int PreProcessNormalize();
double ellipseFunc( double i, double j, double x, double y, double a, double b);
char** generateMask(int width, int height, double x, double y, double a, double b);
Image transformImage(Image source, int newWidth, int newHeight, const Matrix m);
Matrix translateMatrix(double dx, double dy);
Matrix scaleMatrix(double s);
Matrix rotateMatrix(double theta);
Matrix reflectMatrix(int bool_x, int bool_y);
Matrix generateTransform(const TwoPoints *source, const TwoPoints *dest, int reflect);
void convertImages(char* eyeFile, char* inputDir, char* pgmDir, char* nrmDir, char* sfiDir);

#endif
