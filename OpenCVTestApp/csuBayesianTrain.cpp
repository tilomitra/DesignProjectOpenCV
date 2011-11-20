/**
File:    csuBayesianTrain.c                                                            
Authors: Marcio L. Teixeira                      
Date:    May 30, 2002                                                
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

/* 
  This version is being modified by Ross Beveridge, July 1005
*/

#define OPENING  "Trains the Bayesian Interpersonal/Extrapersonal Classifier."

#define EACH_SUBJECT(srt, subject)           (subject = srt; subject != NULL; subject = subject->next_subject)
#define EACH_REPLICATE(srt, replicate)       (replicate = srt; replicate != NULL; replicate = replicate->next_replicate)

/******************************************************************************
*                               INCLUDES                                      *
******************************************************************************/

#include "csuCommon.h"
#include "csuBayesianTrain.h"

/******************************************************************************
*                               GLOBALS                                       *
******************************************************************************/

typedef struct
{
  char *imageList;
  char *imageDirectory;
  char *trainingFilename;

  char *distanceMatrix;
  int maxRank;

  int nExtrapersonal;
  int nIntrapersonal;

  CutOffMode cutOffMode;
  double cutOff;
  int dropNVectors;

  int argc;
  char **argv;
}
Arguments;

/**
 * Process the command line and initialize the variables
 *
 * @param argc The number of arguments
 * @param argv The arguments
 */
void
process_commandtrain (int argc, char **argv, Arguments * args)
{
  int i;
  int param_num = 0;
  int cutOffSet = 0;

  /******* Set up default values *******/

  args->argc = argc;
  args->argv = argv;

  args->nIntrapersonal = 100;
  args->nExtrapersonal = 100;
  args->distanceMatrix = NULL;
  args->maxRank        = -1;

  args->cutOffMode      = CUTOFF_SIMPLE;
  args->cutOff          = DEFAULT_CUTOFF_PERCENT_SIMPLE;
  args->dropNVectors    = 0;

  debuglevel = 0;

  /******* Read command line arguments *******/

  for (i = 1; i < argc; i++) {

    /* Catch common help requests */
    if      (readOption       (argc, argv, &i, "-help" )) { usage(argv[0]); }
    else if (readOption       (argc, argv, &i, "--help")) { usage(argv[0]); }

    /* Read in input directories */
    else if (readOptionString (argc, argv, &i, "-imDir",     &(args->imageDirectory)));
    else if (readOptionString (argc, argv, &i, "-distances", &(args->distanceMatrix)));

    /* Read in number of images to generate */
    else if (readOptionInt    (argc, argv, &i, "-ni", &(args->nIntrapersonal)));
    else if (readOptionInt    (argc, argv, &i, "-ne", &(args->nExtrapersonal)));

    /* Read in maxRank */
    else if (readOptionInt    (argc, argv, &i, "-maxRank", &(args->maxRank)));

    /* Read in PCA training options */
    else if (readOptionInt    (argc, argv, &i, "-dropNVectors", &(args->dropNVectors)));

    else if (readOptionMatch(argc, argv, &i, "-cutOffMode", "NONE"))
      { args->cutOffMode = CUTOFF_NONE; }
    else if (readOptionMatch(argc, argv, &i, "-cutOffMode", "SIMPLE"))
      { args->cutOffMode = CUTOFF_SIMPLE; }
    else if (readOptionMatch(argc, argv, &i, "-cutOffMode", "ENERGY"))
      { args->cutOffMode = CUTOFF_ENERGY; }
    else if (readOptionMatch(argc, argv, &i, "-cutOffMode", "STRETCH"))
      { args->cutOffMode = CUTOFF_STRETCH; }
    else if (readOptionMatch(argc, argv, &i, "-cutOffMode", "CLASSES"))
      { args->cutOffMode = CUTOFF_CLASSES; }

    else if (readOptionDouble (argc, argv, &i, "-cutOff", &(args->cutOff)))
      {
	if ((args->cutOff <= 1.0) && (args->cutOff >= 0.000001)) {
	  printf("WARNING: cutOff value is %f, expects percentage "
		 "between 0.0 and 100.0", args->cutOff);
	  printf("         will proceed assuming value is as desired.");
	}
	cutOffSet = 1;
      }   

    /* other flags */
    else if (readOption       (argc, argv, &i, "-quiet")) { quiet = 1; }
    else if (readOptionInt    (argc, argv, &i, "-debuglevel", &debuglevel));

    /* check if the current argument is an unparsed option */
    else if (checkBadOption(argc,argv,&i));

    /* read required arguments */ 
    else if (param_num == 0) {
      args->imageList = argv[i];
      param_num++;
    }
    else if (param_num == 1) {
      args->trainingFilename = strdup (argv[i]);
      param_num++;
    } 
  }

  /* make sure that there are the proper number of required arguments */

  if (param_num != 2){ clParseError(argc,argv,i,"Wrong number of required arguments"); }

  /* Print out the program parameters for appropriate debug level */

  DEBUG_INT (1, "Debuging enabled", debuglevel);
  if(debuglevel > 0){
    printf("***************** Program Parameters *********************\n");
    printf ("Image directory:  %s\n", args->imageDirectory);
    printf ("Image list:       %s\n", args->imageList);
    printf ("nIntrapersonal:   %d\n", args->nIntrapersonal);
    printf ("nExtrapersonal:   %d\n", args->nExtrapersonal);
    printf ("distanceMatrix:   %s\n ", args->distanceMatrix);
  }
}

/******************************************************************************
*                             DIFFERENCE IMAGE ROUTINES                       *
******************************************************************************/

void
makeDifferenceImages (char *imageDirectory, char *imageList, char *distanceMatrix, int maxRank, int reqNIntra, int reqNExtra, Matrix *intrapersonal, Matrix *extrapersonal)
{
  ImageList*  imlist;
  void       *nameList = NULL;
  char      **nameArray;
  void       *subjList = NULL;
  int        *subjArray, *shuffledIndices, **sortedBySimilarityToProbe;
  int         subjId, probeIdx, galleryIdx, idx, nIntrapersonal, nExtrapersonal, rank, i, numPixels;
  int         nImages;
  /* size_t trash; */
  Matrix sourceImages, intraImages, extraImages;
  ImageList *replicate, *subject;
  char *subjName;

  /* Read in a list of all the images

     The doubly nesteed loops steps through the image list, which has the 
     standard srt doubly linked list structure. Thus, the outer loop 
     iterates through unique subjects (people) and the inner loop iterates
     through the images for that subject.  File names and subject IDs are
     first added to a growing list, and at the end copied to arrays. The
     result is two arrays:

        The name array (nameArray) accumlates file names.
        The subjet array (subjArray) accumlates subject IDs.  
          These are not FERET subject IDs, but simply unique integers, 
          in sequence, starting at zero. 

 */

  imlist = getImageNames (imageList, &nImages);
  subjId = 0;
  for EACH_SUBJECT (imlist, subject) {
      for EACH_REPLICATE (subject, replicate) {
        subjName = strdup (replicate->filename);  /* Now have file name for this replicate */
	listAccumulate (&nameList, &subjName, sizeof (char *));
	listAccumulate (&subjList, &subjId,   sizeof (int));
	writeProgress ("Reading subjects list", subjId, 0);
      }
      subjId++;
  }
  nameArray = (char**)listToNullTerminatedArray (&nameList, sizeof (char *), NULL);
  subjArray = (int*)listToNullTerminatedArray (&subjList, sizeof (int),    NULL);

  /* Allocate storage for source images and difference images */

  numPixels = autoFileLength (makePath (imageDirectory, nameArray[0]));

  sourceImages = makeMatrix (numPixels, nImages);
  intraImages  = makeMatrix (numPixels, reqNIntra);
  extraImages  = makeMatrix (numPixels, reqNExtra);

  DEBUG_CHECK (sourceImages != NULL, "Not enough memory to allocate matrix");
  DEBUG_CHECK (intraImages  != NULL, "Not enough memory to allocate matrix");
  DEBUG_CHECK (extraImages  != NULL, "Not enough memory to allocate matrix");

  /* Load in all the source images */

  for (i = 0; i < nImages; i++) {
    readFile (makePath (imageDirectory, nameArray[i]), i, sourceImages);
    writeProgress ("Loading source images", i, nImages);
  }

  /* First, for each image generate a list of every other image
     sorted by similarity to that image. Note that by default the 
     distance matrix character variable is NULL and this causes the 
     sort to be a random shuffle of the images.

     When this loop is finished, sortedBySimilarityToProbe is a square
     matrix. Each row is a list of all the integer image indices representing
     the position of each image in the now loaded matrix of images called
     sourceImages. Again, recall, if  distanceMatrix is NULL, order is
     random. There is one row per source image, and the row index matches
     that of both the nameArray array and the sourceImages matrix.
 */

  sortedBySimilarityToProbe = (int**) malloc  (nImages * sizeof (int*));
  for (probeIdx = 0; probeIdx < nImages; probeIdx++) {
    sortedBySimilarityToProbe[probeIdx] = (int*) malloc  (nImages * sizeof (int));
    if (sortedBySimilarityToProbe[probeIdx] == 0L)
      {
	fprintf (stderr, "Not enough memory to continue\n");
	exit (1);	
      }
    sortSubjectsBySimilarityToProbe (nameArray[probeIdx], nameArray, distanceMatrix, sortedBySimilarityToProbe[probeIdx]);
    writeProgress ("Sorting images", probeIdx, nImages);
  }

  /* Now write out the difference images */

  idx            = 0;
  rank           = 0;
  nIntrapersonal = 0;
  nExtrapersonal = 0;
  shuffledIndices = shuffledNumbers (nImages);

  if (maxRank == -1)
      maxRank = nImages;

  while (nIntrapersonal < reqNIntra || nExtrapersonal < reqNExtra)
    {
      probeIdx = shuffledIndices[idx];

      idx++;
      if (idx == nImages)
	{
	  idx = 0;
	  rank++;

	  if (rank == maxRank)
	    {
	      fprintf (stderr, "\n");
	      fprintf (stderr, "WARNING: Unable to generate enough images. Try increasing maxRank or adding more subjects to the list.\n");
	      break;
	    }
	}
      
      galleryIdx = sortedBySimilarityToProbe[probeIdx][rank];
	  
      if (galleryIdx == probeIdx || !strcmp (nameArray[probeIdx], nameArray[galleryIdx]))
	continue;

      /* Compute the difference image */

      if (subjArray[probeIdx] == subjArray[galleryIdx])
	{
	  if(nIntrapersonal < reqNIntra) {
	    for (i = 0; i < sourceImages->row_dim; i++)
	      ME (intraImages, i, nIntrapersonal) = ME (sourceImages, i, probeIdx) - ME (sourceImages, i, galleryIdx);

	    nIntrapersonal++;
	  }
	}
      else
	{
	  if( nExtrapersonal < reqNExtra ) {
	    for (i = 0; i < sourceImages->row_dim; i++)
	      ME (extraImages, i, nExtrapersonal) = ME (sourceImages, i, probeIdx) - ME (sourceImages, i, galleryIdx);

	    nExtrapersonal++;
	  }
	}
      
      writeProgress ("Computing difference images", nIntrapersonal + nExtrapersonal, reqNIntra + reqNExtra);
    }

  fprintf (stdout, "Generated %d extrapersonal images\n", nExtrapersonal);
  fprintf (stdout, "Generated %d intrapersonal images\n", nIntrapersonal);

  /* Clean up */

  for (probeIdx = 0; probeIdx < nImages; probeIdx++)
    free (sortedBySimilarityToProbe[probeIdx]);
  free (sortedBySimilarityToProbe);

  freeMatrix (sourceImages);
  freeListOfStrings (nameArray);
  free (subjArray);
  free (shuffledIndices);

  *extrapersonal = extraImages;
  *intrapersonal = intraImages;
}

/******************************************************************************
*                               MAIN PROGRAM                                  *
******************************************************************************/

/*
    main()
 */
int
csuBayesianTrain (int argc, char *argv[])
{
  Arguments args;
  Matrix intraImages, extraImages;
  Subspace intraSubspace, extraSubspace;
  char filename[256];
    ImageList *srt;
  process_commandtrain (argc, argv, &args);
  MESSAGE (OPENING);
  MESSAGE (VERSION);

  /* Sanity check */

  checkReadableDirectory (args.imageDirectory, "%s is not a readable directory");
  checkReadableFile (args.imageList, "Cannot read subject replicates list %s");

  makeDifferenceImages (args.imageDirectory,
			args.imageList,
			args.distanceMatrix,
			args.maxRank,
			args.nIntrapersonal,
			args.nExtrapersonal,
			&intraImages,
			&extraImages
			);

  MESSAGE("Training intrapersonal subspace");
    
  subspaceTrain(&intraSubspace, intraImages, NULL, args.nIntrapersonal, args.dropNVectors, args.cutOffMode, args.cutOff, 0, 0);

  MESSAGE("Training extrapersonal subspace");

  subspaceTrain(&extraSubspace, extraImages, NULL, args.nExtrapersonal, args.dropNVectors,args.cutOffMode, args.cutOff, 0, 0);

  MESSAGE("Saving intrapersonal training file");

  sprintf (filename, "%s.intra", args.trainingFilename);
  writeSubspace (&intraSubspace, filename, args.imageList, args.argc, args.argv);

  MESSAGE("Saving extrapersonal training file");

  sprintf (filename, "%s.extra", args.trainingFilename);
  writeSubspace (&extraSubspace, filename, args.imageList, args.argc, args.argv);

  MESSAGE("Finished Training.");
  
  return 0;

}
