/*
$RCSfile$
$Author: bolme $
$Date: 2007-03-30 12:26:12 -0600 (Fri, 30 Mar 2007) $
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

#include <csuEBGMSimilarity.h>
#include <csuEBGMUtil.h>

/********************** Compute the similarity between jets *****************/


/*! Compute a phase invariant corrilation like similarity
*   that was suggested by Wiskott, et.al.
*/
FTYPE JetSimilarityMag(GaborJet j1, GaborJet j2){
    FTYPE j12 = 0;
    FTYPE j11 = 0;
    FTYPE j22 = 0;
    int i;

    assert(j1 && j2 && j1->length && j1->length==j2->length);
    
    for(i = 0; i < j1->length; i++){
        j12 += j1->mag[i]*j2->mag[i];
        j11 += j1->mag[i]*j1->mag[i];
        j22 += j2->mag[i]*j2->mag[i];
    }

    return j12/sqrt(j11*j22);
}

/*! Compute a similarity score as JetSimilarityMag only with a term
   added for the cos(Phase) */
FTYPE JetSimilarityPhase(GaborJet j1, GaborJet j2){
    FTYPE j12 = 0;
    FTYPE j11 = 0;
    FTYPE j22 = 0;
    int i;

    assert(j1 && j2 && j1->length && j1->length==j2->length);
    
    for(i = 0; i < j1->length; i++){
        j12 += j1->mag[i]*j2->mag[i]*cos(j1->ang[i] - j2->ang[i]);
        j11 += j1->mag[i]*j1->mag[i];
        j22 += j2->mag[i]*j2->mag[i];
    }

    return j12/sqrt(j11*j22);

}


/*! Compute a similarity score as JetSimilarityPhase only correcting
*   the phase angles using the DEGridSample method.
* \sa DEGridSample JetSimilarityPhase
*/
FTYPE JetSimilarityDEGridSample(GaborJet j1, GaborJet j2){
    FTYPE dx = 0, dy = 0;
    return DEGridSample(j1, j2, &dx, &dy);
}

/*! Compute a similarity score as JetSimilarityPhase only correcting
*   the phase angles using the DEPredictiveStep method.
* \sa DEPredictiveStep JetSimilarityPhase
*/
FTYPE JetSimilarityDEPredictiveStep(GaborJet j1, GaborJet j2){
    FTYPE dx = 0.0, dy = 0.0;
    return DEPredictiveStep(j1, j2, &dx, &dy);
}


/*! Compute a similarity score as JetSimilarityPhase only correcting
*   the phase angles using the DEPredictiveIter method.
* \sa DEPredictiveIter JetSimilarityPhase
*/
FTYPE JetSimilarityDEPredictiveIter(GaborJet j1, GaborJet j2){
    FTYPE dx = 0.0, dy = 0.0;
    return DEPredictiveIter(j1, j2, &dx, &dy);
}


FTYPE SIM_DISPLACE(GaborJet j1, GaborJet  j2, FTYPE dx, FTYPE dy) {
    FTYPE j12 = 0.0;                                                    
    FTYPE j11 = 0.0;                                                    
    FTYPE j22 = 0.0;                                                    
    int i;                                                              
    for(i = 0; i < j1->length; i++){                                    
        j12 += j1->mag[i]*j2->mag[i]*                                   
	       cos(j1->ang[i] - j2->ang[i] -                            
	       (dx * j1->params->kx[2*i] + dy * j1->params->ky[2*i]));  
        j11 += j1->mag[i]*j1->mag[i];                                   
        j22 += j2->mag[i]*j2->mag[i];                                   
    }                                                                   
    return j12/sqrt(j11*j22);                                            
}                                  


/*! Compute a similarity score as JetSimilarityPhase only correcting
*   the phase angles using the DEFixedLocalSearch method.
* \sa DEFixedLocalSearch JetSimilarityPhase
*/
FTYPE JetSimilarityDEFixedLocalSearch(GaborJet j1, GaborJet j2){
    FTYPE dx = 0.0, dy = 0.0;
    return DEFixedLocalSearch(j1, j2, &dx, &dy);
}


/*! Compute a similarity score as JetSimilarityPhase only correcting
*   the phase angles using the DENarrowingLocalSearch method.
* \sa DENarrowingLocalSearch JetSimilarityPhase
*/
FTYPE JetSimilarityDENarrowingLocalSearch(GaborJet j1, GaborJet j2){
    FTYPE dx = 0, dy = 0;
    return DENarrowingLocalSearch(j1, j2, &dx, &dy);
}


/*! Compute a similarity score based on the City Block
   distance measure */
FTYPE JetSimilarityCityBlock(GaborJet j1, GaborJet j2){
    FTYPE sim = 0;
    int i;

    assert(j1 && j2 && j1->length && j1->length==j2->length);

    for(i = 0; i < j1->length; i++){
        /* compute the negitive so that greater values are more simillar */
        sim -= ABS(j1->realPart[i]-j2->realPart[i]);
        sim -= ABS(j1->imagPart[i]-j2->imagPart[i]);
    }
    return sim;
}

/*! Compute a similarity score based on the Correlation
   distance measure */
FTYPE JetSimilarityCorrelation(GaborJet j1, GaborJet j2){
    int i;

    double corrilation = 0.0;
    double j1mean = 0.0, j2mean = 0.0;
    double j1scale = 0.0, j2scale = 0.0;

    assert(j1 && j2 && j1->length && j1->length == j2->length);
    
    /* Pass one: compute the pixel mean. */
    for(i = 0; i < j1->length; i++){
        j1mean += j1->realPart[i];
        j1mean += j1->imagPart[i];
        j2mean += j2->realPart[i];
        j2mean += j2->imagPart[i];
    }

    j1mean = j1mean/(2*j1->length);
    j2mean = j2mean/(2*j1->length);

    /* Pass two: compute the unscaled corrilation value and the associated scale parameter. */
    for(i = 0; i < j1->length; i++){
        corrilation += (j1->realPart[i]-j1mean)*(j2->realPart[i]-j2mean);
        corrilation += (j1->imagPart[i]-j1mean)*(j2->imagPart[i]-j2mean);
        j1scale     += SQR(j1->realPart[i]-j1mean);
        j1scale     += SQR(j1->imagPart[i]-j1mean);
        j2scale     += SQR(j2->realPart[i]-j2mean);
        j2scale     += SQR(j2->imagPart[i]-j2mean);
    }

    corrilation = corrilation / (sqrt(j1scale)*sqrt(j2scale));

    return corrilation;

}

/*! Compute a similarity score based on the Covariance
   distance measure */
FTYPE JetSimilarityCovariance(GaborJet j1, GaborJet j2){
    int i;
    FTYPE magi = 0.0,
          magj = 0.0,
          dot = 0.0;

    assert(j1 && j2 && j1->length && j1->length==j2->length);
    
    for (i = 0; i < j1->length; i++) {
        magi += SQR(j1->realPart[i]) + SQR(j1->imagPart[i]);
        magj += SQR(j2->realPart[i]) + SQR(j2->imagPart[i]);
        dot  += j1->realPart[i] * j2->realPart[i];
        dot  += j1->imagPart[i] * j2->imagPart[i];
    }
    return (dot / (sqrt(magi) * sqrt(magj)));
}

/* Distance measure used to create figures in David Bolme's thesis */
FTYPE JetSimilarityDisplacementCheckMag(GaborJet j1, GaborJet j2){
    FTYPE sim = 0.0;
    int i;

    for(i = 0; i < j1->length; i++){
        sim += exp(-0.1*SQR(j1->mag[i]-j2->mag[i]));
    }

    return sim;
}

/* Distance measure used to create figures in David Bolme's thesis */
FTYPE JetSimilarityDisplacementCheckPhase(GaborJet j1, GaborJet j2){
    FTYPE sim = 0.0;
    FTYPE dx = j1->x - j2->x;
    FTYPE dy = j1->y - j2->y;
    int i;

    for(i = 0; i < j1->length; i++){
        sim += cos(j1->ang[i] - j2->ang[i] - (dx * j1->params->kx[2*i] + dy * j1->params->ky[2*i]));
    }

    return sim;
}

/* Distance measure used to create figures in David Bolme's thesis */
FTYPE JetSimilarityResponseMag(GaborJet j1, GaborJet j2){
    FTYPE sim = 0.0;
    int i;

    for(i = 0; i < j1->length; i++){
        sim += j1->mag[i];
    }

    return sim;
}

/* Distance measure used to create figures in David Bolme's thesis */
FTYPE JetSimilarityResponsePhase(GaborJet j1, GaborJet j2){
    FTYPE sim = 0.0;
    int i;

    for(i = 0; i < j1->length; i++){
        sim += j1->ang[i];
    }

    return sim;
}

/*****************************************************************************
    Displacement estimators are used to find the displacement between two
    jets.  These functions optimize a similarity function for a displacement
    vector. For more info see DSB.
*****************************************************************************/


/*!
* This displacment estimator computes a psudo random displacment based
* on a gausian distribution. This method has been used for testing
* different parts of the algorithm because it has some nice predictable
* properties.
*/
double estimateDisplacementRandom(GaborJet j1, GaborJet j2, FTYPE *tdx, FTYPE *tdy){
    FTYPE j12 = 0;
    FTYPE j11 = 0;
    FTYPE j22 = 0;
    int i;
    FTYPE sim = 0.0;
    FTYPE dx  = randBM(),
          dy  = randBM();

    assert(j1 &&  j1->length && j2 && j1->length==j2->length);

    /* Compute the similarity with the estimated displacement. */
    j12 = 0.0;
    j11 = 0.0;
    j22 = 0.0;
    for(i = 0; i < j1->length; i++){
        j12 += j1->mag[i]*j2->mag[i]*cos(j1->ang[i] - j2->ang[i] - (dx * j1->params->kx[2*i] + dy * j1->params->ky[2*i]));
        j11 += j1->mag[i]*j1->mag[i];
        j22 += j2->mag[i]*j2->mag[i];
    }
    sim = j12/sqrt(j11*j22);

    /* Return the proper values */
    *tdx = dx;
    *tdy = dy;

    return sim;
}


/*! This displacement estimation method has been used as an ideal
*   baseline for comparing displacement estimation methods. The
*   method searches a regularly spaced grid around zero displacement.
*
*   Because the method searches the entire grid, it is garinteed to not
*   get stuck in a local maximum.  The method also search a much larger
*   area than is required for a displacement estimator.
*
*   Unfortuatly this method is very computationally expensive. It is
*   too slow to be used in a large experiment.
*/
double DEGridSample(GaborJet j1, GaborJet j2, FTYPE *tdx, FTYPE *tdy){
    FTYPE j12 = 0;
    FTYPE j11 = 0;
    FTYPE j22 = 0;
    int i;
    int first = 1;
    FTYPE sim = 0.0;
    FTYPE dx, dy;

    assert(j1 && j1->length && j2 && j1->length==j2->length);
    for(dx = -16.0; dx < 16.0; dx += 0.5){
        for(dy = -16.0; dy < 16.0; dy += 0.5){
            j12 = 0.0;
            j11 = 0.0;
            j22 = 0.0;
            for(i = 0; i < j1->length; i++){
                j12 += j1->mag[i]*j2->mag[i]*cos(j1->ang[i] - j2->ang[i] - (dx * j1->params->kx[2*i] + dy * j1->params->ky[2*i]));
                j11 += j1->mag[i]*j1->mag[i];
                j22 += j2->mag[i]*j2->mag[i];
            }
            if(first || sim < j12/sqrt(j11*j22)){
                sim = j12/sqrt(j11*j22);
                *tdx = dx;
                *tdy = dy;
                first = 0;
            }
        }
    }
    return sim;
}




/*! This displacement estimation method is based on the method used
*   in the Bochum/USC algorithm. Do to a lack of a good description
*   of this part of the algorithm, a step by step copy of the Bochum/USC
*   method was not created. This impementation replicates only the
*   displacement that would be produced by the equations found in
*   Wiskott(96).  There was no attempt to focus on different frequencies.
*/
double DEPredictiveStep(GaborJet j1, GaborJet j2, FTYPE *tdx, FTYPE *tdy){
    FTYPE j12 = 0;
    FTYPE j11 = 0;
    FTYPE j22 = 0;
    int i;
    FTYPE sim = 0.0;
    FTYPE dx  = 0.0,
          dy  = 0.0;
    FTYPE Gxx, Gxy, Gyx, Gyy;
    FTYPE Px, Py;

    assert(j1 &&  j1->length && j2 && j1->length==j2->length);

    Gxx = 0;
    Gxy = 0;
    Gyx = 0;
    Gyy = 0;

    Px  = 0;
    Py  = 0;

    for(i = 0; i < j1->length; i++){
        double ang = j1->ang[i] - j2->ang[i];

        /* Scale the angle such that it is closest to zero displacement. */
        while(ang >  PI){ ang -= 2*PI; }
        while(ang < -PI){ ang += 2*PI; }

        Gxx += j1->mag[i]*j2->mag[i]*j1->params->kx[2*i]*j1->params->kx[2*i];
        Gxy += j1->mag[i]*j2->mag[i]*j1->params->kx[2*i]*j1->params->ky[2*i];
        Gyx += j1->mag[i]*j2->mag[i]*j1->params->kx[2*i]*j1->params->ky[2*i];
        Gyy += j1->mag[i]*j2->mag[i]*j1->params->ky[2*i]*j1->params->ky[2*i];

        Px  += j1->mag[i]*j2->mag[i]*j1->params->kx[2*i]*(ang);
        Py  += j1->mag[i]*j2->mag[i]*j1->params->ky[2*i]*(ang);
    }

    /* Catch any divide by zero errors */
    if(Gxx*Gyy-Gxy*Gyx != 0.0){
        dx = (Gyy*Px-Gyx*Py)/(Gxx*Gyy-Gxy*Gyx);
        dy = (-Gxy*Px+Gxx*Py)/(Gxx*Gyy-Gxy*Gyx);
    } else {
        /* Divide by zero occured - display warning */
        DEBUG(-1, "Warning: Divide by zero -- Attempting to recover");
        dx = 0.0;
        dy = 0.0;
    }

    /* Compute the similarity with the estimated displacement. */
    j12 = 0.0;
    j11 = 0.0;
    j22 = 0.0;
    for(i = 0; i < j1->length; i++){
        j12 += j1->mag[i]*j2->mag[i]*cos(j1->ang[i] - j2->ang[i] - (dx * j1->params->kx[2*i] + dy * j1->params->ky[2*i]));
        j11 += j1->mag[i]*j1->mag[i];
        j22 += j2->mag[i]*j2->mag[i];
    }
    sim = j12/sqrt(j11*j22);

    /* Return the proper values */
    *tdx = dx;
    *tdy = dy;

    return sim;
}

/*! This displacement estimator might be functionally similar to that
*   implemented by Bochum/USC.  The function follows DEPredictiveStep to
*   determine the next predicted displacement, however now the process
*   is iterated until it converges.
*
*   \sa DEPredictiveStep
*/
double DEPredictiveIter(GaborJet j1, GaborJet j2, FTYPE *tdx, FTYPE *tdy){
    FTYPE j12 = 0;
    FTYPE j11 = 0;
    FTYPE j22 = 0;
    int i,n;
    FTYPE sim = 0.0;
    FTYPE dx  = 0.0,
          dy  = 0.0;
    FTYPE ddx, ddy;
    FTYPE Gxx, Gxy, Gyx, Gyy;
    FTYPE Px, Py;

    assert(j1 &&  j1->length && j2 && j1->length==j2->length);

    for(n = 0; n < 10; n++){

        Gxx = 0;
        Gxy = 0;
        Gyx = 0;
        Gyy = 0;

        Px  = 0;
        Py  = 0;
        
        for(i = 0; i < j1->length; i++){
            double ang = j1->ang[i] - j2->ang[i] - (dx * j1->params->kx[2*i] + dy * j1->params->ky[2*i]);

            /* Scale the angle such that it is closest to zero displacement. */
            while(ang >  PI){ ang -= 2*PI; }
            while(ang < -PI){ ang += 2*PI; }

            Gxx += j1->mag[i]*j2->mag[i]*j1->params->kx[2*i]*j1->params->kx[2*i];
            Gxy += j1->mag[i]*j2->mag[i]*j1->params->kx[2*i]*j1->params->ky[2*i];
            Gyx += j1->mag[i]*j2->mag[i]*j1->params->kx[2*i]*j1->params->ky[2*i];
            Gyy += j1->mag[i]*j2->mag[i]*j1->params->ky[2*i]*j1->params->ky[2*i];

            Px  += j1->mag[i]*j2->mag[i]*j1->params->kx[2*i]*(ang);
            Py  += j1->mag[i]*j2->mag[i]*j1->params->ky[2*i]*(ang);
        }

        /* Catch any divide by zero errors */
        if(Gxx*Gyy-Gxy*Gyx != 0.0){
            ddx = (Gyy*Px-Gyx*Py)/(Gxx*Gyy-Gxy*Gyx);
            ddy = (-Gxy*Px+Gxx*Py)/(Gxx*Gyy-Gxy*Gyx);
            dx += ddx;
            dy += ddy;
            if( (ddx*ddx+ddy*ddy < .001) ) break;
        } else {
            /* Divide by zero occured - display warning */
            DEBUG(-1, "Warning: Divide by zero -- Attempting to recover");
            dx += 0.0;
            dy += 0.0;
        }
    }
    
    /* Compute the similarity with the estimated displacement. */
    j12 = 0.0;
    j11 = 0.0;
    j22 = 0.0;
    for(i = 0; i < j1->length; i++){
        j12 += j1->mag[i]*j2->mag[i]*cos(j1->ang[i] - j2->ang[i] - (dx * j1->params->kx[2*i] + dy * j1->params->ky[2*i]));
        j11 += j1->mag[i]*j1->mag[i];
        j22 += j2->mag[i]*j2->mag[i];
    }
    sim = j12/sqrt(j11*j22);

    /* Return the proper values */
    *tdx = dx;
    *tdy = dy;

    return sim;
}





/*! This is a competly new method for solving for displacements.  Instead
*   of the predictive methods, this uses local search to walk threw the space
*   of displacements.  This method will search the same points as DEGridSample;
*   however, it will get stuck in any local optima.
*
*   The local search uses a neighborhood with half pixel spacing. The search
*   starts with a displacement of (0.0, 0.0) and evaluates the neighbors to
*   see if there is a better option.
*
*   The neighbor with the best evaluated similarity will be come the next point
*   searched.
*/
double DEFixedLocalSearch(GaborJet j1, GaborJet j2, FTYPE *tdx, FTYPE *tdy){
    FTYPE sim  = 0.0;
    FTYPE dx   = 0.0,
          dy   = 0.0;
    int change = 1,
        iter   = 50;
    FTYPE nextx, nexty;
    FTYPE bestsim = 0.0;

    assert(j1 && j1->length && j2 && j1->length==j2->length);


    sim = SIM_DISPLACE(j1, j2, dx,dy);
    bestsim = sim;

    nextx = dx;
    nexty = dy;
    while(change && iter--){
        dx = nextx;
        dy = nexty;
        change = 0;

        sim = SIM_DISPLACE(j1, j2, dx+0.5,dy+0.5);
        if( bestsim < sim ){
            bestsim = sim;
            nextx = dx+0.5;
            nexty = dy+0.5;
            change = 1;
        }

        sim = SIM_DISPLACE(j1, j2, dx+0.5,dy-0.5);
        if( bestsim < sim ){
            bestsim = sim;
            nextx = dx+0.5;
            nexty = dy-0.5;
            change = 1;
        }

        sim = SIM_DISPLACE(j1, j2, dx-0.5,dy-0.5);
        if( bestsim < sim ){
            bestsim = sim;
            nextx = dx-0.5;
            nexty = dy-0.5;
            change = 1;
        }

        sim = SIM_DISPLACE(j1, j2, dx-0.5,dy+0.5);
        if( bestsim < sim ){
            bestsim = sim;
            nextx = dx-0.5;
            nexty = dy+0.5;
            change = 1;
        }
        dx = nextx;
        dy = nexty;
    }

    *tdx = dx;
    *tdy = dy;

    return bestsim;
}


/*! This DE is very similary to DEFixedLocalSearch.  THe difference is that
*   this method starts with a step size of 1.0 and decreases the step size
*   by half if none of the neigbors offer an improvement.
*
*   \sa DEFixedLocalSearch
*/
double DENarrowingLocalSearch(GaborJet j1, GaborJet j2, FTYPE *tdx, FTYPE *tdy){
    FTYPE sim  = 0.0;
    FTYPE dx   = 0.0,
          dy   = 0.0;
    int change = 1,
        iter   = 50;
    FTYPE nextx, nexty;
    FTYPE bestsim = 0.0;
    FTYPE tol  = .2;
    FTYPE step = 1.0;


    assert(j1 && j1->length  && j2 && j1->length==j2->length);

    sim = SIM_DISPLACE(j1, j2, dx,dy);
    bestsim = sim;
    *tdx = 0.0;
    *tdy = 0.0;

    change = 1;
    sim = SIM_DISPLACE(j1, j2, dx,dy);
    bestsim = sim;

    nextx = dx;
    nexty = dy;
    while(change && iter--){
        dx = nextx;
        dy = nexty;
        change = 0;

        sim = SIM_DISPLACE(j1, j2, dx+step,dy+step);
        if( bestsim < sim ){
            bestsim = sim;
            nextx = dx+step;
            nexty = dy+step;
            change = 1;
        }

        sim = SIM_DISPLACE(j1, j2, dx+step,dy-step);
        if( bestsim < sim ){
            bestsim = sim;
            nextx = dx+step;
            nexty = dy-step;
            change = 1;
        }

        sim = SIM_DISPLACE(j1, j2, dx-step,dy-step);
        if( bestsim < sim ){
            bestsim = sim;
            nextx = dx-step;
            nexty = dy-step;
            change = 1;
        }

        sim = SIM_DISPLACE(j1, j2, dx-step,dy+step);
        if( bestsim < sim ){
            bestsim = sim;
            nextx = dx-step;
            nexty = dy+step;
            change = 1;
        }
        dx = nextx;
        dy = nexty;

        if(change == 0 && step > tol){
            change = 1;
            step = step * 0.5;
        }
    }

    *tdx = dx;
    *tdy = dy;
    
    /*printf("BestSim: %f\n",bestsim); */
    return bestsim;
}


/* This function performs a simple L2 distance
   measurement on the geometry of the face graph */
FTYPE GeometrySimL2(FaceGraph f1, FaceGraph f2, double* weights){
    FTYPE totalSim = 0.0;
    int i;

    for(i = 0; i < f1->geosize; i++){
        totalSim += sqrt(SQR(f1->jets[i]->x - f2->jets[i]->x)+SQR(f1->jets[i]->y - f2->jets[i]->y));
    }
    return totalSim;
}



/* This method performs an L2 based distance measure
after solving for a best fit transformation.  The
best fit transformation is a combination of a 2D
scale, rotation and translation.  The algorithm
solves for this transformation using a least squares
solution to a set of transformation aproximations. */
void TransformLeastSquares(Matrix g1, Matrix g2){
    int i;
    FTYPE dist = 0.0;

    Matrix  A = makeMatrix(g1->row_dim,4);
    Matrix v;
    FTYPE b, a, dx, dy;

    dist = 0.0;

    for (i = 0; i < g1->row_dim/2; i++) {
        ME(A,2*i,0  ) =  ME(g1,2*i,0);
        ME(A,2*i,1  ) = -ME(g1,2*i+1,0);
        ME(A,2*i,2  ) =  1;
        ME(A,2*i,3  ) =  0;

        ME(A,2*i+1,0) =  ME(g1,2*i+1,0);
        ME(A,2*i+1,1) =  ME(g1,2*i,0);
        ME(A,2*i+1,2) =  0;
        ME(A,2*i+1,3) =  1;
    }

    v = leastSquares(A, g2);

    a = ME(v,0,0);
    b = ME(v,1,0);
    dx    = ME(v,2,0);
    dy    = ME(v,3,0);

    dist = 0.0;
    for (i = 0; i < g1->row_dim/2; i++) {
        FTYPE x = ME(g1,2*i  ,0);
        FTYPE y = ME(g1,2*i+1,0);

        ME(g1,2*i  ,0) = a*x - b*y + dx;
        ME(g1,2*i+1,0) = b*x + a*y + dy;
    }


    freeMatrix(v);
    freeMatrix(A);
}

FTYPE L2Dist(Matrix g1, Matrix g2){
    FTYPE dist = 0.0;
    int i;

    for (i = 0; i < g1->row_dim/2; i++) {
        dist += SQR(ME(g1,2*i,0) - ME(g2,2*i,0));
        dist += SQR(ME(g1,2*i+1,0) - ME(g2,2*i+1,0));
    }

    return sqrt(dist);
}


/* This method performs an L2 based distance measure
after solving for a best fit transformation.  The
best fit transformation is a combination of a 2D
scale, rotation and translation.  The algorithm
solves for this transformation using a least squares
solution to a set of transformation aproximations. */
FTYPE GeometrySimLeastSquares(FaceGraph f1, FaceGraph f2, double* weights){
    int i;
    FTYPE dist = 0.0;

    Matrix g1 = makeMatrix(f1->geosize*2,1);
    Matrix g2 = makeMatrix(f1->geosize*2,1);

    for (i = 0; i < g1->row_dim; i++) {
        ME(g1,i,0) = (i%2) ? f1->jets[i/2]->y : f1->jets[i/2]->x ;
        ME(g2,i,0) = (i%2) ? f2->jets[i/2]->y : f2->jets[i/2]->x ;
    }

    TransformLeastSquares(g1,g2);

    dist = L2Dist(g1, g2);

    freeMatrix(g1);
    freeMatrix(g2);

    return dist;
}

/* This method performs an L2 based distance measure
after solving for a best fit transformation.  The
best fit transformation is a combination of a 2D
scale, rotation and translation.  The algorithm
solves for this transformation using a least squares
solution to a set of transformation aproximations. */
FTYPE GeometrySimLeastSquaresPS(FaceGraph f1, FaceGraph f2, double* weights){
    int i;
    FTYPE dist = 0.0;

    Matrix g1 = makeMatrix(f1->geosize*2,1);
    Matrix g2 = makeMatrix(f1->geosize*2,1);

    for (i = 0; i < f1->geosize; i++) {
        FTYPE dedx=0, dedy=0;
        DEPredictiveStep(f1->jets[i],f2->jets[i],&dedx,&dedy);

        ME(g1,2*i,0)   = f1->jets[i]->x ;
        ME(g1,2*i+1,0) = f1->jets[i]->y ;
        ME(g2,2*i,0)   = f2->jets[i]->x + dedx;
        ME(g2,2*i+1,0) = f2->jets[i]->y + dedy;
    }

    TransformLeastSquares(g1,g2);

    dist = L2Dist(g1, g2);

    freeMatrix(g1);
    freeMatrix(g2);

    return dist;
}


/* This method performs an L2 based distance measure
after solving for a best fit transformation.  The
best fit transformation is a combination of a 2D
scale, rotation and translation.  The algorithm
solves for this transformation using a least squares
solution to a set of transformation aproximations. */
FTYPE GeometrySimLeastSquaresPI(FaceGraph f1, FaceGraph f2, double* weights){
    int i;
    FTYPE dist = 0.0;

    Matrix g1 = makeMatrix(f1->geosize*2,1);
    Matrix g2 = makeMatrix(f1->geosize*2,1);

    for (i = 0; i < f1->geosize; i++) {
        FTYPE dedx=0, dedy=0;
        DEPredictiveIter(f1->jets[i],f2->jets[i],&dedx,&dedy);

        ME(g1,2*i,0)   = f1->jets[i]->x ;
        ME(g1,2*i+1,0) = f1->jets[i]->y ;
        ME(g2,2*i,0)   = f2->jets[i]->x + dedx;
        ME(g2,2*i+1,0) = f2->jets[i]->y + dedy;
    }

    TransformLeastSquares(g1,g2);

    dist = L2Dist(g1, g2);

    freeMatrix(g1);
    freeMatrix(g2);

    return dist;
}


/* This method performs an L2 based distance measure
after solving for a best fit transformation.  The
best fit transformation is a combination of a 2D
scale, rotation and translation.  The algorithm
solves for this transformation using a least squares
solution to a set of transformation aproximations. */
FTYPE GeometrySimLeastSquaresFLS(FaceGraph f1, FaceGraph f2, double* weights){
    int i;
    FTYPE dist = 0.0;

    Matrix g1 = makeMatrix(f1->geosize*2,1);
    Matrix g2 = makeMatrix(f1->geosize*2,1);

    for (i = 0; i < f1->geosize; i++) {
        FTYPE dedx=0, dedy=0;
        DEFixedLocalSearch(f1->jets[i],f2->jets[i],&dedx,&dedy);

        ME(g1,2*i,0)   = f1->jets[i]->x ;
        ME(g1,2*i+1,0) = f1->jets[i]->y ;
        ME(g2,2*i,0)   = f2->jets[i]->x + dedx;
        ME(g2,2*i+1,0) = f2->jets[i]->y + dedy;
    }

    TransformLeastSquares(g1,g2);

    dist = L2Dist(g1, g2);

    freeMatrix(g1);
    freeMatrix(g2);

    return dist;
}


/* This method performs an L2 based distance measure
after solving for a best fit transformation.  The
best fit transformation is a combination of a 2D
scale, rotation and translation.  The algorithm
solves for this transformation using a least squares
solution to a set of transformation aproximations. */
FTYPE GeometrySimLeastSquaresNLS(FaceGraph f1, FaceGraph f2, double* weights){
    int i;
    FTYPE dist = 0.0;

    Matrix g1 = makeMatrix(f1->geosize*2,1);
    Matrix g2 = makeMatrix(f1->geosize*2,1);

    for (i = 0; i < f1->geosize; i++) {
        FTYPE dedx=0, dedy=0;
        DENarrowingLocalSearch(f1->jets[i],f2->jets[i],&dedx,&dedy);

        ME(g1,2*i,0)   = f1->jets[i]->x ;
        ME(g1,2*i+1,0) = f1->jets[i]->y ;
        ME(g2,2*i,0)   = f2->jets[i]->x + dedx;
        ME(g2,2*i+1,0) = f2->jets[i]->y + dedy;
    }

    TransformLeastSquares(g1,g2);

    dist = L2Dist(g1, g2);

    freeMatrix(g1);
    freeMatrix(g2);

    return dist;
}




/*****************************************************************************
    Face graph similarity measures determine the similarity between face
    graphs.  The similarity measures are based on the geometry of the
    graph and the values of the Gabor Jets.
*****************************************************************************/

/* Magnitude only similarity measure */
FTYPE fgSimMagnitude(FaceGraph f1, FaceGraph f2, double* weights){
    FTYPE totalSim = 0.0;
    int i;

    for(i = 0; i < f1->totalsize; i++){
        totalSim += JetSimilarityMag( f1->jets[i], f2->jets[i] );
    }
    totalSim = totalSim / f1->totalsize;

    return -totalSim;
}

/* Phase based similarity measure */
FTYPE fgSimPhase01(FaceGraph f1, FaceGraph f2, double* weights){
    FTYPE totalSim = 0.0;
    int i;

    for(i = 0; i < f1->totalsize; i++){
        totalSim += JetSimilarityPhase( f1->jets[i], f2->jets[i] );
    }
    totalSim = totalSim / f1->totalsize;

    return -totalSim;
}

/* Phase with displacement estimation */
FTYPE fgSimPhase02(FaceGraph f1, FaceGraph f2, double* weights){
    FTYPE totalSim = 0.0;
    int i;

    for(i = 0; i < f1->totalsize; i++){
        totalSim += JetSimilarityDEPredictiveStep( f1->jets[i], f2->jets[i] );
    }
    totalSim = totalSim / f1->totalsize;

    return -totalSim;
}

/* Phase with displacement estimation */
FTYPE fgSimPhase03(FaceGraph f1, FaceGraph f2, double* weights){
    FTYPE totalSim = 0.0;
    int i;

    for(i = 0; i < f1->totalsize; i++){
        totalSim += JetSimilarityDEGridSample( f1->jets[i], f2->jets[i] );
    }
    totalSim = totalSim / f1->totalsize;

    return -totalSim;
}

/* Phase with displacement estimation */
FTYPE fgSimPhase04(FaceGraph f1, FaceGraph f2, double* weights){
    FTYPE totalSim = 0.0;
    int i;

    for(i = 0; i < f1->totalsize; i++){
        totalSim += JetSimilarityDEFixedLocalSearch( f1->jets[i], f2->jets[i] );
    }
    totalSim = totalSim / f1->totalsize;

    return -totalSim;
}

/* Phase with displacement estimation */
FTYPE fgSimPhase05(FaceGraph f1, FaceGraph f2, double* weights){
    FTYPE totalSim = 0.0;
    int i;

    for(i = 0; i < f1->totalsize; i++){
        totalSim += JetSimilarityDENarrowingLocalSearch( f1->jets[i], f2->jets[i] );
    }
    totalSim = totalSim / f1->totalsize;

    return -totalSim;
}


/* Phase with displacement estimation */
FTYPE fgSimPhase06(FaceGraph f1, FaceGraph f2, double* weights){
    FTYPE totalSim = 0.0;
    int i;

    for(i = 0; i < f1->totalsize; i++){
        totalSim += JetSimilarityDEPredictiveIter( f1->jets[i], f2->jets[i] );
    }
    totalSim = totalSim / f1->totalsize;

    return -totalSim;
}

FTYPE fgSimPhase(FaceGraph f1, FaceGraph f2, double* weights){
    FTYPE totalSim = 0.0;
    int i;

    for(i = 0; i < f1->totalsize; i++){
        totalSim += JetSimilarityPhase( f1->jets[i], f2->jets[i] );
    }
    totalSim = totalSim / f1->totalsize;

    return -totalSim;

}


FTYPE fgSimPhaseGridSample(FaceGraph f1, FaceGraph f2, double* weights){
    FTYPE totalSim = 0.0;
    int i;

    for(i = 0; i < f1->totalsize; i++){
        totalSim += JetSimilarityDEGridSample( f1->jets[i], f2->jets[i] );
    }
    totalSim = totalSim / f1->totalsize;

    return -totalSim;

}



FTYPE fgSimPhasePredictiveStep(FaceGraph f1, FaceGraph f2, double* weights){
    FTYPE totalSim = 0.0;
    int i;

    for(i = 0; i < f1->totalsize; i++){
        totalSim += JetSimilarityDEPredictiveStep( f1->jets[i], f2->jets[i] );
    }
    totalSim = totalSim / f1->totalsize;

    return -totalSim;

}



FTYPE fgSimPhasePredictiveIter(FaceGraph f1, FaceGraph f2, double* weights){
    FTYPE totalSim = 0.0;
    int i;

    for(i = 0; i < f1->totalsize; i++){
        totalSim += weights[i] * JetSimilarityDEPredictiveIter( f1->jets[i], f2->jets[i] );
    }
    totalSim = totalSim / f1->totalsize;

    return -totalSim;

}



FTYPE fgSimPhaseFixedLocalSearch(FaceGraph f1, FaceGraph f2, double* weights){
    FTYPE totalSim = 0.0;
    int i;

    for(i = 0; i < f1->totalsize; i++){
        totalSim += JetSimilarityDEFixedLocalSearch( f1->jets[i], f2->jets[i] );
    }
    totalSim = totalSim / f1->totalsize;

    return -totalSim;

}



FTYPE fgSimPhaseNarrowingLocalSearch(FaceGraph f1, FaceGraph f2, double* weights){
    FTYPE totalSim = 0.0;
    int i;

    for(i = 0; i < f1->totalsize; i++){
        totalSim += JetSimilarityDENarrowingLocalSearch( f1->jets[i], f2->jets[i] );
    }
    totalSim = totalSim / f1->totalsize;

    return -totalSim;

}


FTYPE fgSimGeoAndJet(FaceGraph f1, FaceGraph f2, double* weights){
    FTYPE alpha = 0.5;
    return (1-alpha) * GeometrySimLeastSquaresPI( f1,  f2, weights) + 
      (alpha) * fgSimPhasePredictiveIter( f1,  f2, weights);
}



