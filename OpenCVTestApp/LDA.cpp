/**
File: csuSubspaceProject.c                                                     
Authors: J. Ross Beveridge, David Bolme, Kai She                            
Date: May 24, 2002                                                       
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

#define OPENING  "Project Test Images int Subspace and Compute Pairwise Distances."
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <tr1/unordered_map>

#include "FaceRec.h"
#include <termios.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "LDAPreProcess.h"

using namespace std;

/*
 Purpose: This program projects face images into a subspace and then
 computes the distances between faces with one of several alternative
 distance measures. The subspace is defined in a training file produced
 by csuSubspaceTrain.  The subspace may be either a simple PCA
 subspace, or it may be a PCA followed by LDA subspace. The images are
 specified in a file of image names. These may be grouped either as one
 name per line or several, although for this program there is no
 difference between the two in terms of what is done.  The resulting
 distances are written out to a series of files, one per image, all
 placed in a user specified directory.
*/

/*  CSU INCLUDES */
#include "csuCommon.h"
#include "LDA.h"
#include <math.h>

/* ===========================================================================
 New distance measure functions. These functions take a single matrix of
 column vectors, each column representing a different image after projection
 into subspace. They also take the column indices of the two images to be
 compared. 
 
 All of these functions return "distances" in the sense that smaller is better.
 So, for example, covariance and correlation are flipped and shifted so that
 zero is the best score between two images.
 
 The last measures also take the variance estimates for the subspace dimension,
 in other words the eigenvalues from the subspace decomposition. These are used
 to normalize measures by variance along the dimensions.
*/

/*  The L1 norm and L2 norms are standard. This code actually does the square root
 for L2, not strictly speaking necessary for a nearest neighbor classifier.
*/
FTYPE distanceCityBlock(const Matrix ims, int i, int j) {
    int k;
    FTYPE sum = 0.0;
    for (k = 0; k < ims->row_dim; k++) {
        sum += ABS((ME(ims, k, i) - ME(ims, k, j)));
    }
    return sum;
}

FTYPE distanceEuclidean(const Matrix ims, int i, int j) {
    int k;
    FTYPE sum = 0.0;
    for (k = 0; k < ims->row_dim; k++) {
        sum += (ME(ims, k, i) - ME(ims, k, j)) * (ME(ims, k, i) - ME(ims, k, j));
    }
    return sqrt(sum) ;
}

FTYPE distancePNorm(const Matrix ims, int i, int j, double p) {
   int k;   
   FTYPE sum = 0.0;
   for(k =0; k < ims->row_dim; k++) {
      sum += pow(ABS(ME(ims, k, i) - ME(ims, k, j)), p);
   }
   return (FTYPE)pow((double)sum, 1.0/p);
}

/* This is the standard covariance definition, it is the cosine
 of the angle between the two vectors. When the vectors are 
 identical, it is 1.0, when they are orthogonal, it is zero. 
 
 However, in order produce a distance, this function returns
 one minus the covariance. 
*/

FTYPE distanceCovariance(const Matrix ims, int i, int j) {
    int k;
    FTYPE magi = 0.0, magj = 0.0, dot = 0.0;

    for (k = 0; k < ims->row_dim; k++) {
        magi += ME(ims, k, i) * ME(ims, k, i);
        magj += ME(ims, k, j) * ME(ims, k, j);
        dot += ME(ims, k, i) * ME(ims, k, j);
    }
    return (1.0 - (dot / (sqrt(magi) * sqrt(magj))));
}

/**
    This is a standard correlation measure between two vectors with
 one major caveat.  Because all subsequent processing treats these
 measures as distance, the correlation mapped into the range 0 to 2
 as follows:
  correlation  1  goes to 0
  correlation -1  goes to 2.
*/

FTYPE distanceCorrelation(const Matrix ims, int i, int j) {
    int k;
    FTYPE n;
    FTYPE mui = 0.0, muj = 0.0, stdi = 0.0, stdj = 0.0, dot = 0.0, corr;

    n = (FTYPE) ims->row_dim;
    for (k = 0; k < n; k++) {
        mui += ME(ims, k, i);
        muj += ME(ims, k, j);
    }
    mui = mui / n;
    muj = muj / n;

    for (k = 0; k < n; k++) {
        stdi += (ME(ims, k, i) - mui) * (ME(ims, k, i) - mui);
        stdj += (ME(ims, k, j) - muj) * (ME(ims, k, j) - muj);
        dot += (ME(ims, k, i) - mui) * (ME(ims, k, j) - muj);
    }
    stdi = sqrt((1.0 / (n - 1.0)) * stdi);
    stdj = sqrt((1.0 / (n - 1.0)) * stdj);

    corr = ( 1.0 / (n - 1.0)) * (dot / (stdi * stdj));

    return ( 1.0 - corr);
}

FTYPE distanceCosine(Matrix ims, int i, int j) {
   FTYPE sum = 0.0;
   FTYPE sum_i = 0.0;
   FTYPE sum_j = 0.0;
   int k;
   FTYPE scale_i = 0.0;
   FTYPE scale_j = 0.0;
   /* Transform the vector into Mahalanobis space */
   for(k = 0; k < ims->row_dim; k++){
      sum_i += SQR(ME(ims,k,i));
      sum_j += SQR(ME(ims,k,j));
   }
   
   scale_i = 1.0/sqrt(sum_i);
   scale_j = 1.0/sqrt(sum_j);
   
   /* Unit length the vectors and find the dot product (cos of angle) */
   for(k = 0; k < ims->row_dim; k++){
      sum += ME(ims,k,i)*ME(ims,k,j);
   }
   sum *= (scale_i * scale_j);
   return -sum;
}

/*
 * YamborAngle This is the same measure as MahAngle in
 * version 4.0 and before.  It has been depricated
 * because the measure is not properly formulated.
 * MahCosine has replaced it in newer versions.
 * This code was kept so that the new system would
 * retain all of the earlier functionality of the old
 * system.  The name was changed to force a switch to
 * MahCosine or YamborAngle in any new tests. MahAngle
 * no longer works.
 */
FTYPE distanceYamborAngle(Matrix ims, int i, int j, Matrix values) {
    int k;
    FTYPE sum = 0.0;
    FTYPE sum_i = 0.0;
    FTYPE sum_j = 0.0;
    for (k = 0;k < ims->row_dim;k++) {
        sum_i += ME(ims, k, i) * ME(ims, k, i);
        sum_j += ME(ims, k, j) * ME(ims, k, j);
    }
    for (k = 0; k < ims->row_dim; k++) {
        sum -= ME(ims, k, i) / sqrt(sum_i) * (ME(ims, k, j) / sqrt(sum_j)) * (1 / (sqrt(ME(values, k, 0))));
    }
    return sum;
}

/*
 * This distance measure computes the cosine of the angle between the two
 * image vectors in Mahalinobis. This measure appears to work better than
 * MahAngle which was not really in mahalinobis space and not really and
 * angle, or a cosine of an angle.
 *
 * IT SHOULD BE NOTED that this function allocates memory internal to the
 * function that is never freed.  The memory is used to cache values that
 * are reused in subsequent function calls.  This is not the most graceful
 * way to perform this operation, however, it is much faster and it maintians
 * the function interface
 */
FTYPE distanceMahCosine(Matrix ims, int i, int j, Matrix values) {
    static int first_call = 1;
    static Matrix optVals = NULL;
    int k;
    Matrix vi = makeMatrix(ims->row_dim,1);
    Matrix vj = makeMatrix(ims->row_dim,1);
    FTYPE sum_i = 0.0;
    FTYPE sum_j = 0.0;
    FTYPE scale_i = 0.0;
    FTYPE scale_j = 0.0;
    FTYPE sum = 0.0;
    
    if(first_call){
        first_call = 0;
        optVals = duplicateMatrix(values);
        for(k = 0; k < optVals->row_dim; k++){
            ME(optVals,k,0) = 1.0/sqrt(ME(values, k, 0));
        }
    }

    /* Transform the vector into Mahalanobis space */
    for(k = 0; k < vi->row_dim; k++){
        ME(vi,k,0) = ME(ims,k,i)*ME(optVals,k,0);
        sum_i += ME(vi,k,0)*ME(vi,k,0);
        ME(vj,k,0) = ME(ims,k,j)*ME(optVals,k,0);
        sum_j += ME(vj,k,0)*ME(vj,k,0);
    }
    
    scale_i = 1.0/sqrt(sum_i);
    scale_j = 1.0/sqrt(sum_j);

    /* Unit length the vectors and find the dot product (cos of angle) */
    for(k = 0; k < vi->row_dim; k++){
        sum += ME(vi,k,0)*ME(vj,k,0);
    }
    sum = sum * scale_i * scale_j;

    /* Free the temporary vectors. */
    freeMatrix(vi);
    freeMatrix(vj);

    /* return the negetive similarity or distance */
    return -sum;
}


/*
 * This fucntion computes the standard L2 (Euclidean) distance in Mahalinobis
 * space.
 *
 * IT SHOULD BE NOTED that this function allocates memory internal to the
 * function that is never freed.  The memory is used to cache values that
 * are reused in subsequent function calls.  This is not the most graceful
 * way to perform this operation, however, it is much faster and it maintians
 * the function interface
 */
FTYPE distanceMahL2(const Matrix ims, int i, int j, const Matrix values) {
    int k;
    FTYPE sum = 0.0;
    static int first_call = 1;
    static Matrix optVals = NULL;

    if(first_call){
        first_call = 0;
        optVals = duplicateMatrix(values);
        for(k = 0; k < optVals->row_dim; k++){
            ME(optVals,k,0) = 1.0/ME(values, k, 0);
        }
    }

    for (k = 0; k < ims->row_dim; k++) {
        sum += (ME(ims, k, i) - ME(ims, k, j)) * (ME(ims, k, i) - ME(ims, k, j)) * ME(optVals,k,0);
    }

    return sqrt(sum);
}


/*
 * This function computes a Mahalinobis based distance as described in
 * Wendy yambors thesis.  The measure has not been used in the recent versions
 * of the CSU system. This measure was created to study the effect of different
 * Mahalinobis based measures. For more information see the Users Manual.
 *
 * IT SHOULD BE NOTED that this function allocates memory internal to the
 * function that is never freed.  The memory is used to cache values that
 * are reused in subsequent function calls.  This is not the most graceful
 * way to perform this operation, however, it is much faster and it maintians
 * the function interface
 */
FTYPE distanceYamborDistance(const Matrix ims, int i, int j, const Matrix values) {
    int k;
    FTYPE sum = 0.0;
    static int first_call = 1;
    static Matrix optVals = NULL;

    if(first_call){
        first_call = 0;
        optVals = duplicateMatrix(values);
        for(k = 0; k < optVals->row_dim; k++){
            ME(optVals,k,0) = 1.0/ME(values, k, 0);
        }
    }

    for (k = 0; k < ims->row_dim; k++) {
        sum += ME(ims, k, i)*ME(ims, k, j)*ME(optVals,k,0);
    }

    return -sum;
}

/*
 * This function computes the L1 (Cityblock) distance measure in Mahalinobis space.
 *
 * IT SHOULD BE NOTED that this function allocates memory internal to the
 * function that is never freed.  The memory is used to cache values that
 * are reused in subsequent function calls.  This is not the most graceful
 * way to perform this operation, however, it is much faster and it maintians
 * the function interface
 */
FTYPE distanceMahL1(const Matrix ims, int i, int j, const Matrix values) {
    int k;
    FTYPE sum = 0.0;
    static int first_call = 1;
    static Matrix optVals = NULL;

    if(first_call){
        first_call = 0;
        optVals = duplicateMatrix(values);
        for(k = 0; k < optVals->row_dim; k++){
            ME(optVals,k,0) = 1.0/sqrt(ME(values, k, 0));
        }
    }
    
    for (k = 0; k < ims->row_dim; k++) {
        sum += ABS(ME(ims, k, i) - ME(ims, k, j)) * ME(optVals,k,0);
    }
    return sum;
}

/* The soft distance measure proposed by WenYi Zhao is essentially the L2 norm
 with each axis weighted by the associated eigen value, i.e. variance, raised
 to the the power 0.2.  This is not obvious, but considerable discussion
 of this design appears in WenYi's Dissertation.
*/

FTYPE distanceLDASoft(Matrix ims, int i, int j, Matrix values) {
    int k;
    FTYPE sum = 0.0;
    for (k = 0; k < ims->row_dim; k++) {
        sum += pow(ME(values, k, 0), 0.2) * (ME(ims, k, i) - ME(ims, k, j)) * (ME(ims, k, i) - ME(ims, k, j));
    }
    return sum;
}


char** getNameByIndex(ImageList **srt, int numImages) {
    int i;
    ImageList *subject, *replicate;

    char** nameByIndex;

    /*  Allocate space for the image names for each index in images matrix */
    nameByIndex = (char**) malloc(sizeof(char*) * numImages);
    assert(nameByIndex);
    for (i = 0; i < numImages; i++) {
        nameByIndex[i] = (char*) malloc(sizeof(char) * FILE_LINE_LENGTH);
        assert(nameByIndex[i]);
    }

    /*  Move across columns and down rows of subject replicates table constructing
     an array of image file names indexed by the same index as the images matrix. */
    i = 0;
    for (subject = *srt; subject; subject = subject->next_subject) {
        for (replicate = subject; replicate; replicate = replicate->next_replicate) {
            if ((i != replicate->imageIndex) || (i == numImages)) {
                fprintf(stderr, "Error: replicate indices off or out of bounds.\n");
                exit(1);
            }
            strcpy(nameByIndex[i], replicate->filename);
            i++;
        }
    }
    return nameByIndex;
}

void whitenSubspaceMatrix(Matrix ims, const Matrix values, int numImages) {
   int i,k;
   static Matrix optVals = NULL;
   /* set up the mah values */
   optVals = duplicateMatrix(values);
   for(k = 0; k < optVals->row_dim; k++){
      ME(optVals,k,0) = 1.0/sqrt(ME(values, k, 0));
   }
   
   /* modify the actual matrix */
   for(i = 0; i < numImages; i++) {
      writeProgress("Whitening the space for each image", i,numImages);
      for (k = 0; k < ims->row_dim; k++) {
            ME(ims,k,i) *=  ME(optVals,k,0); 
      }
   }   
}

Matrix computeDistances(const Matrix ims, const Matrix values, int numImages, char* distName) {
    int i, j;
    Matrix distances = makeMatrix(numImages, numImages);

    if (strcmp(distName, "Euclidean") == 0) {
        for (i = 0; i < numImages; i++) {
            for (j = 0; j < numImages; j++) {
                ME(distances, i, j) = distanceEuclidean(ims, i, j);
            }
        }
    } else if ((strcmp(distName, "Cityblock") == 0) || (strcmp(distName, "CityBlock") == 0)) {
        for (i = 0; i < numImages; i++) {
            for (j = 0; j < numImages; j++) {
                ME(distances, i, j) = distanceCityBlock(ims, i, j);
            }
        }
    } else if (strcmp(distName, "Covariance") == 0) {
        for (i = 0; i < numImages; i++) {
            for (j = 0; j < numImages; j++) {
                ME(distances, i, j) = distanceCovariance(ims, i, j);
            }
        }
    } else if (strcmp(distName, "Correlation") == 0) {
        for (i = 0; i < numImages; i++) {
            for (j = 0; j < numImages; j++) {
                ME(distances, i, j) = distanceCorrelation(ims, i, j);
            }
        }
    } else if (strcmp(distName, "Cosine") == 0) {
       for (i = 0; i < numImages; i++) {
          for (j = 0; j < numImages; j++) {
             ME(distances, i, j) = distanceCosine(ims, i, j);
          }
       }
    }else if (strcmp(distName, "YamborAngle") == 0) {
        for (i = 0; i < numImages; i++) {
            for (j = 0; j < numImages; j++) {
                ME(distances, i, j) = distanceYamborAngle(ims, i, j, values);
            }
        }
    } else if (strcmp(distName, "MahCosine") == 0) {
        for (i = 0; i < numImages; i++) {
            writeProgress("Computing distances for MahCosine", i,numImages);
            for (j = 0; j < numImages; j++) {
                ME(distances, i, j) = distanceMahCosine(ims, i, j, values);
            }
        }
    } else if (strcmp(distName, "MahL1") == 0) {
        for (i = 0; i < numImages; i++) {
            writeProgress("Computing distances for MahL1", i,numImages);
            for (j = 0; j < numImages; j++) {
                ME(distances, i, j) = distanceMahL1(ims, i, j, values);
            }
        }
    } else if (strcmp(distName, "MahL2") == 0) {
        for (i = 0; i < numImages; i++) {
            writeProgress("Computing distances for MahL2", i,numImages);
            for (j = 0; j < numImages; j++) {
                ME(distances, i, j) = distanceMahL2(ims, i, j, values);
            }
        }
    } else if (strcmp(distName, "YamborDistance") == 0) {
        for (i = 0; i < numImages; i++) {
            writeProgress("Computing distances for Windy", i,numImages);
            for (j = 0; j < numImages; j++) {
                ME(distances, i, j) = distanceYamborDistance(ims, i, j, values);
            }
        }
    } else if ((strcmp(distName, "ldaSoft") == 0) || (strcmp(distName, "LDASoft") == 0)) {
       for (i = 0; i < numImages; i++) {
          for (j = 0; j < numImages; j++) {
             ME(distances, i, j) = distanceLDASoft(ims, i, j, values);
          }
       }
    } else if (strcmp(distName, "LHalf") == 0){
       for (i = 0; i < numImages; i++) {
          writeProgress("Computeing distances for L 1/2", i, numImages);
          for (j = 0; j < numImages; j++) {
             ME(distances, i, j) = distancePNorm(ims, i, j, 0.5);
          }
       }
    }  else if (strcmp(distName, "LTenth") == 0){
        for (i = 0; i < numImages; i++) {
           writeProgress("Computeing distances for L 1/10th", i, numImages);
            for (j = 0; j < numImages; j++) {
                ME(distances, i, j) = distancePNorm(ims, i, j, 0.10);
            }
        }
    } else {
        fprintf(stderr, "Error: The distance measure %s is not implemented! \n", distName);
        exit(1);
    }

    return distances;
}

int ComputeLDAdist(const Matrix ims, const Matrix values, int numImages, char* distName)
{
    int i, j;
    double distances = 0;
    double dist = DBL_MAX;
    for (i = 0; i < numImages; i++) 
    {
        for (j = 0; j < numImages; j++) 
        {
            distances = distances + distanceLDASoft(ims, i, j, values);
        }
        
    }
}

void writeDistancesForImage(char* distDirectory, char* filename, const Matrix distances, int col, char** names) {
    char name[MAX_FILENAME_LENGTH];
    FILE *file;
    int i;
    sprintf(name, "%s/%s", distDirectory, filename);
    file = fopen(name, "w");
    if (!file) {
        printf("Error: could not open file <%s>\n", name);
        exit(1);
    }
    for (i = 0; i < distances->row_dim; i++) {
        fprintf(file, "%s\t%.16e\n", names[i], ME(distances, i, col));
//        cout<<ME(distances, i, col)<<endl;
    }
    fclose(file);
}


/* ===========================================================================
 MAIN
 
 The arguments are processed and then the subspace and related information is
 read from the training file written by csuSubspaceTrain.  The subspace basis
 is read into a matrix. If the basis vectors are for a PCA subspace, then the 
 basis vectors are tested for orthonormality. While this should not be 
 necessary, it is a prudent check to see that nothing has gone wrong either 
 in the training phase or in the transcription of the subspace basis from the
 training code to the testing code. 
 
 Once the training information is read, then the images specified in the 
 imageNamesFile are read into the images matrix. This matrix is then mean 
 centered using the mean, or centroid, associated with the training data. 
 Next, the images are projected into subspace and the distances between all
 pairs of images are computed. Finally, these distances are written to files,
 one per image.
*/

string lda() {
    int i;
    //Arguments args;
    int numImages;
    ImageList *srt;
    Matrix subspims, distances;
    Subspace subspace;
    char **nameByIndex;
//    DistDirNode* ddn;
    double dist = DBL_MAX;
    char* trainingFile = "/Users/sachi/Documents/development/OpenCVTestApp/OpenCVTestApp/train/scraps/LDATrain.txt";
    char* imageNamesFile = "/Users/sachi/Documents/development/OpenCVTestApp/OpenCVTestApp/imagelists/scrap_all_x4.srt";
    char* imageDirectory = "/Users/sachi/Documents/development/OpenCVTestApp/OpenCVTestApp/data/csuScrapShots/normSep2002sfi";
    char* distDirectory = "/Users/sachi/Documents/development/OpenCVTestApp/OpenCVTestApp/distances/scraps/LDA_ldaSoft";
    char* distName = "ldaSoft";
    int whiten = 0;
    char name[MAX_FILENAME_LENGTH];
    int size;
    double value = DBL_MAX;
    string LDAresult;
    char chars[] = "0123456789_";//characters that need to be removed from LDAresult
    std::tr1::unordered_map<std::string, double> LDAdist;//hash for storing person name with corresponding computed LDA distances

    
    MESSAGE(OPENING);
    MESSAGE(VERSION);

    readSubspace (&subspace, trainingFile, quiet);

    SAVE_MATRIX(subspace.values);
    MESSAGE1ARG("Reading image data from directory %s and projecting onto the new basis.", imageDirectory);

    subspims = readAndProjectImages(&subspace, imageNamesFile, imageDirectory, &numImages, &srt);
    
    MESSAGE1ARG("Computing distances with distance measure %s.", distName);

    distances = computeDistances(subspims, subspace.values, numImages, distName);
        

//    MESSAGE2ARG("Writing distance files for %d test images to directory %s", 1, distDirectory);
    nameByIndex = getNameByIndex(&srt, numImages);

    for (i = 0; i < numImages; i++) 
    {
        LDAdist[nameByIndex[i]]= ME(distances, distances->row_dim-1, i);        
    }

    //finding minimum LDAsoft distance
    for (std::tr1::unordered_map<std::string, double>::const_iterator iter = LDAdist.begin();iter != LDAdist.end(); ++iter)
    {
        cout<<iter->first<<endl;
        cout<<iter->second<<endl;
        if (value>iter->second && iter->second != 0.0)
        {
            value = iter->second;
            LDAresult = iter->first;
        }
    }
//    cout<<LDAresult<<" "<<value<<endl;

    for (unsigned int i = 0; i < sizeof(chars); ++i)
    {
        LDAresult.erase (std::remove(LDAresult.begin(), LDAresult.end(), chars[i]), LDAresult.end());
    }
    LDAresult.replace(LDAresult.find(".sfi"),4,"");
//    for (i = 0; i < numImages; i++) {
//        writeProgress("Writing distance files", i,numImages);
//        writeDistancesForImage(distDirectory, nameByIndex[i], distances, i, nameByIndex);
//    }
    freeMatrix(distances);
    return LDAresult;
}

