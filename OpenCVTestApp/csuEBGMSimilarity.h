/*
$RCSfile$
$Author: lionelle $
$Date: 2004-01-05 09:28:11 -0700 (Mon, 05 Jan 2004) $
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

#ifndef EBGM_SIMILARITY
#define EBGM_SIMILARITY

#include <csuEBGMUtil.h>
/************************ Jet Similarity Functions *************************/

/*! \brief This is the basic structure of a function that computes the similarty of jets */
typedef FTYPE (*JetSimilarityMeasure)(GaborJet j1, GaborJet j2);

/*! \brief Magnitude only jet similarity */
FTYPE JetSimilarityMag(GaborJet j1, GaborJet j2);

/*! \brief Phase jet similarity. This does NOT include any displacement estimation */
FTYPE JetSimilarityPhase(GaborJet j1, GaborJet j2);

/*! \brief Phase jet similarity using DEGridSample method. */
FTYPE JetSimilarityDEGridSample(GaborJet j1, GaborJet j2);

/*! \brief Phase jet similarity using DEPredictiveStep method. */
FTYPE JetSimilarityDEPredictiveStep(GaborJet j1, GaborJet j2);

/*! \brief Phase jet similarity using DEPredictiveIter method.  */
FTYPE JetSimilarityDEPredictiveIter(GaborJet j1, GaborJet j2);

/*! \brief Phase jet similarity using DEFixedLocalSearch method.  */
FTYPE JetSimilarityDEFixedLocalSearch(GaborJet j1, GaborJet j2);

/*! \brief Phase jet similarity using DENarrowingLocalSearch method.  */
FTYPE JetSimilarityDENarrowingLocalSearch(GaborJet j1, GaborJet j2);

/*! \brief An alternative distance measure that computes L1 on the raw wavelet coeffecents. */
FTYPE JetSimilarityCityBlock(GaborJet j1, GaborJet j2);

/*! \brief An alternative distance measure that computes Correlation on the raw wavelet coeffecents. */
FTYPE JetSimilarityCorrelation(GaborJet j1, GaborJet j2);

/*! \brief An alternative distance measure that computes Covariance on the raw wavelet coeffecents. */
FTYPE JetSimilarityCovariance(GaborJet j1, GaborJet j2);


/*! \brief Similarity measure used to generate figures for David Bolme's thesis. Use with csuEBGMLocalization */
FTYPE JetSimilarityResponseMag(GaborJet j1, GaborJet j2);

/*! \brief Similarity measure used to generate figures for David Bolme's thesis. Use with csuEBGMLocalization */
FTYPE JetSimilarityResponsePhase(GaborJet j1, GaborJet j2);

/*! \brief Similarity measure used to generate figures for David Bolme's thesis. Use with csuEBGMLocalization */
FTYPE JetSimilarityDisplacementCheckMag(GaborJet j1, GaborJet j2);

/*! \brief Similarity measure used to generate figures for David Bolme's thesis. Use with csuEBGMLocalization */
FTYPE JetSimilarityDisplacementCheckPhase(GaborJet j1, GaborJet j2);

/* ********************* Jet Displacement Estimation *********************** */

/*! \brief This defines the basic structure of a displacmente estimation (DE) function
 *
 * \param j1 model jet
 * \param j2 novel jet
 * \param tdx the estimated x displacement of j1 based on the phase information of j2
 * \param tdy the estimated y displacement of j1 based on the phase information of j2
 */
typedef FTYPE (*JetDisplacementEstimator)(GaborJet j1, GaborJet j2, FTYPE *tdx, FTYPE *tdy);

/*! \brief This DE produces a psudo random displacement.
*          It can be useful for testing purposes.
* \sa JetDisplacementEstimator */
FTYPE estimateDisplacementRandom (GaborJet j1, GaborJet j2, FTYPE *tdx, FTYPE *tdy);

/*! \brief This DE searches a grid of displacements for the similarity optimum
* \sa JetDisplacementEstimator */
FTYPE DEGridSample(GaborJet j1, GaborJet j2, FTYPE *tdx, FTYPE *tdy);

/*! \brief This DE is based on the method used by Bochum/USC
* \sa JetDisplacementEstimator */
FTYPE DEPredictiveStep(GaborJet j1, GaborJet j2, FTYPE *tdx, FTYPE *tdy);

/*! \brief This DE is similar to DEPredictiveStep however it uses iteration to get better localization
* \sa JetDisplacementEstimator */
FTYPE DEPredictiveIter(GaborJet j1, GaborJet j2, FTYPE *tdx, FTYPE *tdy);

/*! \brief This DE implements a simple local search with a fixed step size.
* \sa JetDisplacementEstimator */
FTYPE DEFixedLocalSearch(GaborJet j1, GaborJet j2, FTYPE *tdx, FTYPE *tdy);

/*! \brief This DE is the same as DEFixedLocalSearch only with a narrowing step size
* \sa JetDisplacementEstimator */
FTYPE DENarrowingLocalSearch(GaborJet j1, GaborJet j2, FTYPE *dx, FTYPE *dy);

/********************** Face Graph Similarity ************************/

/*! \brief This defines the basic structure of functions used to
*          compute the distance of two face graphs
*
* These functions represent the "similarity" of faces in "distance" form.
* Smaller values indicate more similarity.  This is so that the fucntions
* will produce a distance matrix which can be analized by other tools
* included with the system.
*/
typedef FTYPE (*FaceGraphSimilarity)(FaceGraph g1, FaceGraph g2, double*);

/*! \brief Compute the geometry similarity based on the L2 distance
 *         measure */
FTYPE GeometrySimL2(FaceGraph, FaceGraph, double*);
/*! \brief Compute the geometry similarity after solving for a rotation
*          translation and scale transformation using a least squares method
*          that will minimize the difference in geometry */
FTYPE GeometrySimLeastSquares(FaceGraph, FaceGraph, double*);
/*! \brief Compute GeometrySimLeastSquares after correcting the geometry useing
*          the DEPredictiveStep method. */
FTYPE GeometrySimLeastSquaresPS(FaceGraph f1, FaceGraph f2, double*);
/*! \brief Compute GeometrySimLeastSquares after correcting the geometry useing
*          the DEPredictiveIter method. */
FTYPE GeometrySimLeastSquaresPI(FaceGraph f1, FaceGraph f2, double*);
/*! \brief Compute GeometrySimLeastSquares after correcting the geometry useing
*          the DEFixedLocalSearch method. */
FTYPE GeometrySimLeastSquaresFLS(FaceGraph f1, FaceGraph f2, double*);
/*! \brief Compute GeometrySimLeastSquares after correcting the geometry useing
*          the DENarrowingLocalSearch method. */
FTYPE GeometrySimLeastSquaresNLS(FaceGraph f1, FaceGraph f2, double*);

/*! \brief Compute jet based similarity using the magnitude only measure. */
FTYPE fgSimMagnitude(FaceGraph, FaceGraph, double*);

/*! \brief Compute jet based similarity using the phase measure. */
FTYPE fgSimPhase(FaceGraph, FaceGraph, double*);

/*! \brief Compute jet based similarity using the DEGridSample correction. */
FTYPE fgSimPhaseGridSample(FaceGraph, FaceGraph, double*);

/*! \brief Compute jet based similarity using the DEPredictiveStep correction. */
FTYPE fgSimPhasePredictiveStep(FaceGraph, FaceGraph, double*);

/*! \brief Compute jet based similarity using the DEPredictiveIter correction. */
FTYPE fgSimPhasePredictiveIter(FaceGraph, FaceGraph, double*);

/*! \brief Compute jet based similarity using the DEFixedLocalSearch correction. */
FTYPE fgSimPhaseFixedLocalSearch(FaceGraph, FaceGraph, double*);

/*! \brief Compute jet based similarity using the DENarrowingLocalSearch correction. */
FTYPE fgSimPhaseNarrowingLocalSearch(FaceGraph, FaceGraph, double*);
FTYPE fgSimGeoAndJet(FaceGraph, FaceGraph, double*);

#endif



