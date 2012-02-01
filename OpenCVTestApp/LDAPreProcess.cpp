/*
 *  csuFace2Norm.c
 *  csuFace2Norm
 *
 *  Created by David  Bolme on Sun Jun 09 2002.
 *
 */

/*
Copyright (c) 2003 Colorado State University

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom
the Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/


#include "LDAPreProcess.h"

#define OPENING "csuFace2Norm - Normalize face data from pgm files"

/* default normalization parameters */
#define ELLIPSE_X    65.0
#define ELLIPSE_Y    50.0
#define ELLIPSE_A    64.0
#define ELLIPSE_B    118.0

#define EDGE_PRE     0
#define EDGE_POST    0

#define EYE_LX       30.0
#define EYE_LY       45.0
#define EYE_RX       100.0
#define EYE_RY       45.0

#define SIZE_WIDTH   130
#define SIZE_HEIGHT  150

#define NOISE        0.0
#define SCALE        1.0
#define SHIFT_X      0.0
#define SHIFT_Y      0.0

#define HIST_NONE    0
#define HIST_PRE     1
#define HIST_POST    2

char* eyeFile ="/Users/sachi/Documents/development/OpenCVTestApp/OpenCVTestApp/imagelists/coords.txt";
char* inputDir = "/Users/sachi/Documents/development/OpenCVTestApp/OpenCVTestApp/data/csuScrapShots/normSep2002pgm";
char* pgmDir = "/Users/sachi/Documents/development/OpenCVTestApp/OpenCVTestApp/data/csuScrapShots/source/pgm";
char* nrmDir = "/Users/sachi/Documents/development/OpenCVTestApp/OpenCVTestApp/data/csuScrapShots/normSep2002nrm";
char* sfiDir = "/Users/sachi/Documents/development/OpenCVTestApp/OpenCVTestApp/data/csuScrapShots/normSep2002sfi";
int sizeWidth    = SIZE_WIDTH;
int sizeHeight   = SIZE_HEIGHT;


/* Masks are no longer read from a file.  It is created by scan converting an ellipse. */
double ellipseFunc( double i, double j, double x, double y, double a, double b){
    return 1 - (SQR(x-i)/SQR(a) + SQR(y-j)/SQR(b));
}

char** generateMask(int width, int height, double x, double y, double a, double b){
    char **mask;
    int i,j;

    mask = (char**) malloc(sizeof(char*)*width);
    assert(mask); /* problem allocating space for mask */

    for(i = 0; i < width; i++){
        mask[i] = (char*) malloc(sizeof(char)*height);
        assert(mask[i]); /* problem allocating space for mask */
    }

    for(j = 0; j < height; j++){
        for(i = 0; i < width; i++){
            if(ellipseFunc(i,j,x,y,a,b) > 0){
                mask[i][j] = 1;
            }
            else{
                mask[i][j] = 0;                
            }
        }
    }

    return mask;
}


/********************* Geometric Transformation Code *************************/

/* This function performs a 3X3 two dimentional perspective transform to an image
This is used to perform geomentric normalization */
Image transformImage(Image source, int newWidth, int newHeight, const Matrix m){
    Image dest = makeImage(newWidth, newHeight, source->channels);
    Matrix point = makeMatrix(3,1);
    Matrix inv;
    Matrix pt;
    int x, y, c;

    assert(m->row_dim == 3);
    assert(m->col_dim == 3);

    /* initialize homogenius point */
    ME(point,2,0) = 1;

    /* find the inverse transformation to convert from dest to source */
    inv = invertRREF(m);

    for(x = 0; x < dest->width; x++){
        for(y = 0; y < dest->height; y++){
            /* calculate source point */
            ME(point,0,0) = x;
            ME(point,1,0) = y;
            ME(point,2,0) = 1.0;
            pt = multiplyMatrix(inv, point);
            ME(pt,0,0) = ME(pt,0,0) / ME(pt,2,0);
            ME(pt,1,0) = ME(pt,1,0) / ME(pt,2,0);

            for(c = 0; c < dest->channels; c++){
                /* interpolate value */
                IE(dest,x,y,c) = interpLinear(source, ME(pt,0,0),ME(pt,1,0),c);
            }
            /* clean up */
            freeMatrix(pt);
        }
    }
    freeMatrix(point);
    freeMatrix(inv);

    return dest;
}

/* Return a translation matrix */
Matrix translateMatrix(double dx, double dy){
    Matrix transform = makeIdentityMatrix(3);

    ME(transform,0,2) = dx;
    ME(transform,1,2) = dy;

    return transform;
}

/* Return a scale Matrix */
Matrix scaleMatrix(double s){
    Matrix transform = makeIdentityMatrix(3);
    ME(transform,0,0) = s;
    ME(transform,1,1) = s;

    return transform;
}

/* Return a rotation matrix */
Matrix rotateMatrix(double theta){
    Matrix transform = makeIdentityMatrix(3);

    ME(transform,0,0) = cos(theta);
    ME(transform,1,1) = cos(theta);
    ME(transform,0,1) = -sin(theta);
    ME(transform,1,0) = sin(theta);

    return transform;
}

Matrix reflectMatrix(int bool_x, int bool_y){
    Matrix transform = makeIdentityMatrix(3);

    if(bool_x) ME(transform,0,0) = -1;
    if(bool_y) ME(transform,1,1) = -1;
    return transform;
}




Matrix generateTransform(const TwoPoints *source, const TwoPoints *dest, int reflect){
    double sourceMidX = (source->x1 + source->x2)*0.5;
    double sourceMidY = (source->y1 + source->y2)*0.5;
    double destMidX = (dest->x1 + dest->x2)*0.5;
    double destMidY = (dest->y1 + dest->y2)*0.5;
    Matrix translate1 = translateMatrix(-sourceMidX, -sourceMidY); /* translate the left point to the origin */
    Matrix translate2 = translateMatrix(destMidX, destMidY); /* translate the origin to the left destination */

    /* compute the scale that needs to be applyed to the image */
    double sdist = sqrt(SQR(source->x1 - source->x2) + SQR(source->y1 - source->y2));
    double ddist = sqrt(SQR(dest->x1 - dest->x2) + SQR(dest->y1 - dest->y2));
    double s = ddist/sdist; 
    Matrix scale = scaleMatrix(s);

    /* compute the rotation that needs to be applyed to the image */
    double stheta = atan((source->y2 -source->y1)/(source->x2 - source->x1));
    double dtheta = atan((dest->y2 -dest->y1)/(dest->x2 - dest->x1));
    double theta  = dtheta - stheta;
    Matrix rotate = rotateMatrix(theta);

    /* compute the reflection if nessicary */
    Matrix reflection = reflectMatrix(reflect,0);

    /* build the final transformation */
    Matrix tmp1 = multiplyMatrix(scale, translate1);
    Matrix tmp2 = multiplyMatrix(rotate, tmp1);
    Matrix tmp3 = multiplyMatrix(reflection, tmp2);
    Matrix transform = multiplyMatrix(translate2,tmp3);

    /* free temporary matricies */
    freeMatrix(translate1);
    freeMatrix(translate2);
    freeMatrix(scale);
    freeMatrix(rotate);
    freeMatrix(reflection);
    freeMatrix(tmp1);
    freeMatrix(tmp2);
    freeMatrix(tmp3);

    /* return final transformation */
    return transform;
}

void convertImages(char* eyeFile, char* inputDir, char* pgmDir, char* nrmDir, char* sfiDir){
    char** mask = NULL;
    TwoPoints source, dest;
    FILE* eyeList;
    char line[ FILE_LINE_LENGTH ];
    char filename[MAX_FILENAME_LENGTH];
    char imagename[MAX_FILENAME_LENGTH];
    char suffix[MAX_FILENAME_LENGTH];
    int i;
	
	bzero (filename, MAX_FILENAME_LENGTH);
	bzero (imagename, MAX_FILENAME_LENGTH);
	bzero (suffix, MAX_FILENAME_LENGTH);

    sizeWidth  = ROUND(sizeWidth*1.0);
    sizeHeight = ROUND(sizeHeight*1.0);
    
    suffix[0] = '\0'; 

    eyeList = fopen(eyeFile,"r");

    for(i = 1;;i++){
        Image pgm;
        Image geo;
        Matrix transform;
        
        fgets(line, FILE_LINE_LENGTH, eyeList);
        if(feof(eyeList)) break;
        
        sscanf(line,"%s",filename);

        sprintf(imagename,"%s/%s.pgm",inputDir,filename);

        MESSAGE1ARG("Processing image: %s",filename);
        
        pgm = readPGMImage(imagename);

        transform = makeIdentityMatrix(3);
        geo = transformImage(pgm,sizeWidth,sizeHeight,transform);

        DEBUG(1,"   No Value Normalization. Just Applying Mask.");
        applyMask(geo, (const char **) mask);
    
        if(nrmDir){
            sprintf(imagename,"%s/%s%s.nrm", nrmDir, filename, suffix);
            DEBUG_STRING(1,"   Saving nrm: %s",imagename);
            writeFeretImage(geo,imagename);
        }
        if(pgmDir){
            sprintf(imagename,"%s/%s%s.pgm", pgmDir, filename, suffix);
            DEBUG_STRING(1,"   Saving pgm: %s",imagename);
            writePGMImage(geo,imagename,0);
        }
        if(sfiDir){
            sprintf(imagename,"%s/%s%s.sfi", sfiDir, filename, suffix);
            DEBUG_STRING(1,"   Saving sfi: %s",imagename);
            writeRawImage(geo,imagename);
        }

        freeImage(geo);
        freeImage(pgm);
        freeMatrix(transform);
    }

    fclose(eyeList);
    
}


int PreProcessNormalize(){
    
    FILE* imagefile;
    char directory[256] = "/Users/sachi/Documents/development/OpenCVTestApp/OpenCVTestApp/data/csuScrapShots/normSep2002pgm";
    struct dirent *dp;
    struct dirent *dpface;
    string line, faceline, linetest;    
    char str[1000];
    char facestr[10000];
    
    char teststr[1000];
    DIR* dfd = opendir(inputDir);
    
    FILE* pFile;
    struct stat st, facest;
    int status;
    char myfile[256] = "/Users/sachi/Documents/development/OpenCVTestApp/OpenCVTestApp/data/csuScrapShots/imagefile.txt";
    char imagelist[256] = "/Users/sachi/Documents/development/OpenCVTestApp/OpenCVTestApp/imagelists/coords.txt";
    string fileline[256];
    char namedirectory[256];
    char facedirectory[256];
    int a = 0;
    size_t found;
    
    MESSAGE(OPENING);
    MESSAGE(VERSION);

    MESSAGE("Converting Images...");
    convertImages(eyeFile,inputDir,pgmDir,nrmDir,sfiDir);

    return 0;
}






