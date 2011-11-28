/**
   File: ldatrain.c                                                            
   Authors: Kai She, J. Ross Beveridge and David Bolme                         
   Date:    April. 10 2001                                                     
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

/**
   Purpose: This program loads training face images into a matrix M and        
   compute the eigen space and optionally the fisher space.                    
                                                                             
   Input: Gallery image data from binary file                                  
   Output: A combined projection space in a log file 
*/

#define OPENING  "From Training Imagery, Build a PCA or PCA+LDA Subspace."

/******************************************************************************
 *                               INCLUDES                                      *
 ******************************************************************************/

#include "csuCommon.h"
#include "LDATrain.h"

/******************************************************************************
 *                               GLOBALS                                       *
 ******************************************************************************/

/*
  main()
*/
int
lda_train()
{
  int numImages, numSubjects;
  ImageList *srt;
  Matrix images;
  Subspace subspace;
    
  char* imageList = "/Users/sachi/Documents/development/OpenCVTestApp/OpenCVTestApp/imagelists/scrap_all.srt";;
  char* imageDirectory = "/Users/sachi/Documents/development/OpenCVTestApp/OpenCVTestApp/data/csuScrapShots/normSep2002sfi";;
  char* training_filename = "/Users/sachi/Documents/development/OpenCVTestApp/OpenCVTestApp/train/scraps/LDATrain.txt";
  int vecLength = 0;
  CutOffMode cutOffMode = CUTOFF_SIMPLE;
  double cutOff = DEFAULT_CUTOFF_PERCENT_SIMPLE;;
  int dropNVectors = 0;
  int uselda = 1;
  int writeTextBasis = 0;
  int writeTextValues = 0;
  int writeTextInterm = 0;  

  MESSAGE(OPENING);
  MESSAGE(VERSION);

  /* The Matrix images is contains what is commonly calld the data matrix. It
     consists of n images, one image per column. Hence, each images data is 
     contiguous in memory. This choice facilities easy integration with the 
     Eigensolver used below */

  MESSAGE1ARG("Reading images from files in directory %s.", imageDirectory);

  images = readImages(imageList, imageDirectory, vecLength,&numImages, &numSubjects, &srt);
  
  subspaceTrain (&subspace, images, srt, numSubjects, dropNVectors, cutOffMode, cutOff, uselda, writeTextInterm);

  /* Write out text files for basis and values */
  if (writeTextBasis)
    {
      SAVE_MATRIX (subspace.basis);
      SAVE_MATRIX (subspace.mean);
      
      SAVE_MATRIX (subspace.values);
    }

  writeSubspace (&subspace, training_filename, imageList);
  
  MESSAGE("Finished Training.");
  
  return 0;
}
