/*
$RCSfile$
$Author: bolme $
$Date: 2007-01-22 21:25:49 -0700 (Mon, 22 Jan 2007) $
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

#ifndef CSU_GABOR_INCLUDED
#define CSU_GABOR_INCLUDED

#include <csuCommon.h>

#define LABELMAX 100

#define GFTYPE FTYPE

#define NO_WEIGHTING 1      //! version 5.0 method (low frequency)
#define WISKOTT_WEIGHTING 2 //! same method as Wiskott (high frequency)
#define SCALE_WEIGHTING 3   //! wavelets are normalized across scales (no frequency bias)

/* Set the type of weighting that should be used for the gabor kernels. */
#define WEIGHT_TYPE SCALE_WEIGHTING //! change this to change the frequency sensitivety

/*************************** Gabor Parameters ******************************/

/*! \brief A datastructure used to describe the
 *         parameters of each wavelet mask.
 *
 * These parameters hold on to the information
 * needed for the gabor kernel.  For many
 * applications, it is assumed that every other
 * gabor kernel is identical but out of phase by
 * 90 degrees (even and odd).  Jets assume this
 * structure and store complex values.  Therefore
 * a jet stores length/2 complex values.
 *
 * \sa GaborJetParams */
typedef struct{
    /*! \brief The total number of wavelet masks. */
    int length;

    /*! \brief The wavelength of the sinusoid */
    GFTYPE* wavelength;
    /*! \brief The oreintation of the sinusoid */
    GFTYPE* angle;
    /*! \brief The phase of the sinusoid */
    GFTYPE* phase;
    /*! \brief The aspect ratio of the Gaussian */
    GFTYPE* aspect;
    /*! \brief The radius of the Gaussian */
    GFTYPE* radius;
    /*! \brief The size of the mask */
    int*    size;

    /*! \brief The frequency in the x directions used for displacement estimation*/
    GFTYPE* kx;
     /*! \brief The frequency in the y directions used for displacement estimation*/
   GFTYPE* ky;
} gabor_jet_params;

typedef gabor_jet_params *GaborJetParams;

/*! \brief  Allocate space for the Jet Params */
GaborJetParams makeJetParams(int size);
/*! \brief  Free space for the Jet Params */
void freeJetParams(GaborJetParams params);


/******************************** Gabor Mask *******************************/

/*! \brief This structure holds on to images of gabor kernels (masks) that
 * are used in convolution to determine the gabor coeffecents
 * \sa JetMasks
 */
typedef struct{
    /*! \brief the number of masks */
    int size;

    /*! \brief The masks themselves */
    Image *masks;

    /*! \brief The parameters used to create the masks. */
    GaborJetParams params;
} jet_masks;

typedef jet_masks* JetMasks;

/*! \brief Allocate space for the Jet Masks */
JetMasks makeJetMasks(int size);
/*! \brief Free space for the Jet Masks */
void     freeJetMasks(JetMasks masks);

/*! \brief Creates an image that is used as a gabor mask for computing wavelet convolutions */
Image makeGaborMask(FTYPE wavelength, FTYPE angle, FTYPE phase, FTYPE aspect, FTYPE radius, int size);

/*! \brief Create gabor masks based on parameters in the file */
JetMasks readMasksFile(const char* filename);

/******************************* Gabor Jets ********************************/

/*! \brief The structure used to store the gabor jet information.
*
* This structure uses containes the convolution values for a set
* of gabor filters at a paticular location.  Structure elements
* include the location x,y and length of the jet, and the values.
* see Wiskott (Sec. 2.1.1)
* \sa GaborJet
*/
typedef struct{
    /*! \brief the number of coefficients in the jet. */
    int length;
    /*! \brief the location the jet was extracted from. */
    GFTYPE x, y;

    /*! \brief Complex specification for the coefficients */
    GFTYPE* realPart;
    /*! \brief Complex specification for the coefficients */
    GFTYPE* imagPart;

    /*! \brief Polar transformation of the above complex numbers used for displacement estimation */
    GFTYPE* mag;
    /*! \brief Polar transformation of the above complex numbers used for displacement estimation */
    GFTYPE* ang;

    /*! \brief Parameters used to generate gabor kernels */
    GaborJetParams params;
} gabor_jet;

typedef gabor_jet *GaborJet;

/*! \brief Allocate memory for the jet. */
GaborJet makeGaborJet(int length);

/*! \brief Free the memory for the jet. */
void     freeGaborJet(GaborJet jet);

/*! \brief Extract a jet from a given location in an image. This
*          function returns a GaborJet that will need to be freed */
GaborJet extractJet(GFTYPE x, GFTYPE y, Image im, JetMasks masks);

/******************************* Jet Bunch *********************************/

/*! \brief Structure that contains multiple gabor jets corresponding to a
single landmark.

A Jet bunch is a collection of gabor jets that have been extracted from the
same facidual point on the face.  The idea is to form a "bunch" (collection)
of example jets that represent that point.  When trying to localize a point
in a novel image the code can choose a jet that is more similar to the image
being matched.

\sa JetBunch
*/

typedef struct{
    /*! \brief The size of the array allocated that is used for dynamic resizing */
    int allocsize;

    /*! \brief The number of jets stored in the array.  */
    int size;

    /*! \brief The jets. */
    GaborJet* jets;
} jet_bunch;

typedef jet_bunch *JetBunch;

/*! \brief Allocate memory for the jet bunch. */
JetBunch makeJetBunch();

/*! \brief Free the memory for the jet bunch */
void freeJetBunch(JetBunch bunch);

/*! \brief Add a jet to the bunch */
int addJetToBunch(JetBunch bunch, GaborJet jet);


/************************* Gabor Graph Discription *************************/

/*! \brief Simple datastructure that describes a graph edge.  The structure
 *         contains the vertex numbers for the edge connections. */
typedef struct{
    int vert1, vert2;
} Edge;

/*! \brief A simple datastructure to describe the location of a vertex. */
typedef struct{
    FTYPE x, y;
} Vert;

/*! \brief  This data structure is used to describe the geometry of a face, or
*           it can be used as a bunch graph by storing jets in the bunch.
*
* This structure has a dual purpose. It is used to contian the geometry information
* for individual images. The geometry information is stored in the graph as well as
* connections between the nodes.  Simple functions are provided for storing this
* information to disk.
*
* The structure can also be used as a bunch graph. In addition to the geometry
* information each node has an associated bunch. When used to only describe the
* geometry, the bunch field is not used. When used as a bunch graph the verts
* typically take the average value of the node location, and are used to compute
* an initail estimate of the landmarks location. (See David Bolme's thesis).
*
* \sa GraphDiscription
*/
typedef struct{
    int numVert;
    int numEdge;
    Vert* verts;
    Edge* edges;
    char** vertLabels;

    JetBunch* bunch;
} graph_discription;

typedef graph_discription *GraphDiscription;

/*! \brief Read geometry information from a file.
 *
 * This function reads in a discription of a graph and loads it into
 * a graph discription structure.  The file format is asci were the
 * first token is the number of vericies followed by verticy labels
 * and guess x, y coordinates.  This is followed by the number of edges
 * and the indexes of the vericies that they connect.
 */
GraphDiscription readGraphDiscription(const char* filename);

/* \brief Save geometry information to a file. */
void             saveGraphDiscription(const char* filename,GraphDiscription);

/* \brief Release any memory used by the GraphDiscription */
void             freeGraphDiscription(GraphDiscription gd);


/******************************* Face Graph ********************************/
/*! \brief This structure represents a face.  It includes Gabor jets and
*          their extraction locations.  The similarity of faces is a function
*          based on the similarity of the facegraph.
*/
typedef struct{
    /* \brief Number of landmark points. */
    int geosize;
    /* \brief Number of jets including landmark and interpolated jets. */
    int totalsize;
    /* \brief Parameters used to create the wavelets. Needed for similarity measurement. */
    GaborJetParams params;
    /* \brief The gabor jets. Geometry information is stored with the Gabor jets as the 
    *         extraction location. */
    GaborJet* jets;
} face_graph;

typedef face_graph* FaceGraph;

/*! \brief Allocate memory for the face graph */
FaceGraph makeFaceGraph( int geosize, int totalsize );

/*! \brief Free the memory for the face graph. */
void freeFaceGraph( FaceGraph );

/*! \brief load a face graph from a file. */
FaceGraph loadFaceGraph(char *filename);

/*! \brief save a face graph to disk. */
void saveFaceGraph(char *filename, FaceGraph);



/*********************** Extra Utility Functions ***************************/




#endif



