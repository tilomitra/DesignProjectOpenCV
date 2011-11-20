/**
File: csuFileCommon.c                                                       
Authors: J. Ross Beveridge, David Bolme and Kai She                        
Date:    March 15, 2002                                                     
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

#ifndef CSU_FILE_COMMON_INCLUDED
#define CSU_FILE_COMMON_INCLUDED

#include "csuCommon.h"

/* A ListOfStrings is simply an array of char pointers. */

typedef char** ListOfStrings;

/**
  image_list
  
  This is a data structure that sorts the images by the subject and replicates. s inticates
  a subject or paticular person and r indicates a replicate of that person.  The final data
  structure should look like this:
    s1r1-s1r2-s1r3-...-NULL
     |
    s2r1-s2r2-s3r3-...-NULL
     |
    ...
     |
    NULL
  
  This list is to allow for different sized classes for use in the LDA training.
  
  Node fields:
    filename:        name of file image was read from.
    image_number:    index to image in the data matrix
    next_replicate:  pointer to node containing the next replicate
    next_subject:    pointer to node contianing the next subject
*/

typedef struct image_list_node {
    char* filename;
    int imageIndex;
    struct image_list_node* next_replicate;
    struct image_list_node* next_subject;
}
ImageList;

/* Function Prototypes */

/* Low level binary I/O */
void writeInt(FILE* file, int);
void writeFloat(FILE* file, float);
void writeDouble(FILE* file, double);

void readInt(FILE* file, int*);
void readFloat(FILE* file, float*);
void readDouble(FILE* file, double*);


char *makePath (const char *directoryName, const char *fileName);
void checkWriteableDirectory (const char *directory, const char *message);
void checkReadableDirectory (const char *directory, const char *message);
void checkReadableFile (const char *file, const char *message);

ListOfStrings readListOfStrings (const char *fileName, int *nStrings);
void freeListOfStrings (ListOfStrings list);

void sortSubjectsBySimilarityToProbe (char *probe, ListOfStrings subjects, char *distanceMatrix, int *indices);

void readFile(const char *fname, int n, Matrix images);
ImageList* getImageNames(char* imageNamesFile, int *numImages);
void freeImageNames (ImageList *list);
Matrix readImages(char *imageNamesFile, char *imageDirectory, int *numPixels, int *numImages, int *numSubjects, ImageList **srt);
void readFile(const char *fname, int n, Matrix images);

int numImageInImageList(ImageList *srt);
int autoFileLength(const char* imageName);

float *readFeretRaster(const char *fn, int numpix);
float *writeFeretRaster(const char *fn, float *data, int numpix);

unsigned char *readImagePGM(const char *filename, int *w, int *h, int verbose);
void writeImagePGM(const char *fn, float *data, int numpix, int w, int h);

#endif /* CSU_FILE_COMMON_INCLUDED */




