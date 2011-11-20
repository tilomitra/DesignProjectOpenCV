/*
 *  csuCommonCommandLine.c
 *  csuEvalFaceRec
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

#include "csuCommon.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * Display a standard usage parameters or help if there is a problem with the
 * command line.
 */
void usage(const char* name)
{
    printf ("Usage: %s [OPTIONS] training_images.srt training_output_prefix\n", name);
    printf ("  Parameters\n");
    printf ("    training_images.srt:   Input image names.\n");
    printf ("    training_output_prefix Prefix which is used to save the subspaces\n");
    printf ("  Options\n");
    printf ("    -imDir <dir>:          Image directory.\n"
            "        DEFAULT = \".\"\n");
    printf ("    -ni <n>:               Total number of interpersonal images.\n"
            "        DEFAULT = 100\n");
    printf ("    -ne <n>:               Total number of extrapersonal images.\n"
            "        DEFAULT = 100\n");
    printf ("    -distances <file>:     Sort images using this distance matrix.\n"
            "        DEFAULT = \"Random\"\n");
    printf ("    -maxRank n             When sorting, take these many top-ranked images\n");
    printf("     -dropNVectors <int>: Drops the first N vectors which are normally lighting effects.\n        DEFAULT = 0\n");
    printf("     -cutOffMode <mode>:  Selects the method for eigen vector selection after PCA trianing\n");
    printf("                          and droping the first vectors.  DEFAULT = SIMPLE\n");
    printf("          NONE            Retain eigenvectors.\n");
    printf("          SIMPLE          Retain a percentage of the eigenvectors. \n");
    printf("                          Expects value between 0.0 and 100.0, DEFAULT PERCENT = %f \n", DEFAULT_CUTOFF_PERCENT_SIMPLE);
    printf("          ENERGY          Retain  eigenvectors accounting for a percentage of the total energy.\n");
    printf("                          Expects value between 0.0 and 100.0, DEFAULT PERCENT = %f\n", DEFAULT_CUTOFF_PERCENT_ENERGY);
    printf("          STRETCH         Retains all eigenvectors greater than a percentage of the largest eigenvector.\n");
    printf("                          Expects value between 0.0 and 100.0, DEFAULT PERCENT = %f\n", DEFAULT_CUTOFF_PERCENT_STRETCH);
    printf("          CLASSES         Retains as many eigenvectors as there LDA Classes: use only with LDA.\n");
    printf("                          Ignores cutOff value and uses number of classes instead\n");
    printf("     -cutOff <percent>:   Percentage of eigen vectors to retain (see cutOffMode).\n        DEFAULT = (See cutoff mode)\n");
    printf ("    -quiet:                Turn off all messages.\n"
            "        DEFAULT = messages on\n");
    printf ("    -debuglevel <int>:     Level of debug information to display"
            "        (automatically sets quiet to no).\n"
            "        DEFAULT = 0\n");
    exit (1);
}


/* returns true if opt is parsed and i+=0 */
int readOption(int argc, char** argv, int *i, const char* opt){
    if((*i) < argc){
        if(strcasecmp(argv[(*i)],opt) == 0){
            return 1;
        }
    }
    else{
        clParseError(argc, argv, (*i), "Error parsing command line.");
    }
    
    return 0;
}

/* returns true if opt is parsed and sets arg to point to location in argv that is after flag (the next string) i+=1 */
int readOptionString(int argc, char** argv, int *i, const char* opt, char** arg){
    if((*i) < argc){
        if(strcasecmp(argv[(*i)],opt) == 0){
            (*i) += 1;
            if(*i < argc){
                (*arg) = argv[(*i)];
            } else {
                clParseError(argc, argv, (*i), "Option expects one argument.");
            }

            return 1;
        }
    }
    else{
        clParseError(argc, argv, (*i), "Error parsing command line.");
    }

    return 0;    
}

/* returns true if opt is parsed and sets arg to point to location in argv that is after flag (the next string) i+=1 */
int readOptionYesNo(int argc, char** argv, int *i, const char* opt, int* flag){
    if((*i) < argc){
        if(strcasecmp(argv[*i],opt) == 0){
            (*i) += 1;
            if((*i) < argc){
                if(strcasecmp(argv[(*i)], CL_YES_STRING) == 0){
                    (*flag) = CL_YES;
                    return 1;
                }
                else if (strcasecmp(argv[(*i)], CL_NO_STRING) == 0){
                    (*flag) = CL_NO;
                    return 1;
                }
                else{
                    clParseError(argc, argv, (*i), "Option expects YES or NO.");
                }
            } else {
                clParseError(argc, argv, (*i), "Option expects YES or NO.");
            }

            return 1;
        }
    }
    else{
        clParseError(argc, argv, (*i), "Error parsing command line.");
    }

    return 0;
    
}

/* returns true only if opt is parsed and following argument string matches arg
* This is used to test for options like -hist PRE -hist POST -hist NONE i+=1*/
int readOptionMatch(int argc, char** argv, int *i, const char* opt, const char* arg){
    if((*i) < argc){
        if(strcasecmp(argv[*i],opt) == 0){
            if((*i) < argc){
                if(strcasecmp(argv[(*i)+1], arg) == 0){
                    /* option parsed inc and return */
                    (*i) += 1;
                    return 1;
                }
            } else {
                clParseError(argc, argv, (*i), "Option expects one argument");
            }

            return 0;
        }
    }
    else{
        clParseError(argc, argv, (*i), "Error parsing command line.");
    }

    return 0;    
}

/* this reads in an option and an int. i+=1 */
int readOptionInt(int argc, char** argv, int *i, const char* opt, int *arg){
    if((*i) < argc){
        if(strcasecmp(argv[(*i)],opt) == 0){
            (*i)+=1;
            if((*i) < argc){
                (*arg) = atoi(argv[(*i)]);
                return 1;
            } else {
                clParseError(argc, argv, (*i), "Option expects one argument");
            }

            return 0;
        }
    }
    else{
        clParseError(argc, argv, (*i), "Error parsing command line.");
    }

    return 0;
}

/* this reads in an option and two ints. i+=2 */
int readOptionInt2(int argc, char** argv, int *i, const char* opt, int *arg1, int *arg2){
    if((*i) < argc){
        if(strcasecmp(argv[(*i)],opt) == 0){
            /* Try to read first int */
            (*i)+=1;
            if((*i) < argc){
                (*arg1) = atoi(argv[(*i)]);
            } else {
                clParseError(argc, argv, (*i), "Option expects two arguments");
            }

            /* Try to read second int */
            (*i)+=1;
            if((*i) < argc){
                (*arg2) = atoi(argv[(*i)]);
                return 1;
            } else {
                clParseError(argc, argv, (*i), "Option expects two arguments");
            }

            return 0;
        }
    }
    else{
        clParseError(argc, argv, (*i), "Error parsing command line.");
    }

    return 0;
}

/* this reads in an option and a double. i+=1 */
int readOptionDouble(int argc, char** argv, int *i, const char* opt, double *arg){
    if((*i) < argc){
        if(strcasecmp(argv[(*i)],opt) == 0){
            (*i)+=1;
            if((*i) < argc){
                (*arg) = atof(argv[(*i)]);
                return 1;
            } else {
                clParseError(argc, argv, (*i), "Option expects one argument");
            }

            return 0;
        }
    }
    else{
        clParseError(argc, argv, (*i), "Error parsing command line.");
    }

    return 0;    
}

/* this reads in an option and four doubles. i+=4 */
int readOptionDouble2(int argc, char** argv, int *i, const char* opt, double *arg1, double *arg2){
    if((*i) < argc){
        if(strcasecmp(argv[(*i)],opt) == 0){
            /* Try to read first double */
            (*i)+=1;
            if((*i) < argc){
                (*arg1) = atof(argv[(*i)]);
            } else {
                clParseError(argc, argv, (*i), "Option expects two arguments");
            }

            /* Try to read second double */
            (*i)+=1;
            if((*i) < argc){
                (*arg2) = atof(argv[(*i)]);
                return 1;
            } else {
                clParseError(argc, argv, (*i), "Option expects two arguments");
            }

            return 0;
        }
    }
    else{
        clParseError(argc, argv, (*i), "Error parsing command line.");
    }

    return 0;
    
}

/* this reads in an option and four doubles. i+=4 */
int readOptionDouble4(int argc, char** argv, int *i, const char* opt, double *arg1, double *arg2, double *arg3, double *arg4){
    if((*i) < argc){
        if(strcasecmp(argv[(*i)],opt) == 0){
            /* Try to read first double */
            (*i)+=1;
            if((*i) < argc){
                (*arg1) = atof(argv[(*i)]);
            } else {
                clParseError(argc, argv, (*i), "Option expects four arguments");
            }

            /* Try to read second double */
            (*i)+=1;
            if((*i) < argc){
                (*arg2) = atof(argv[(*i)]);
            } else {
                clParseError(argc, argv, (*i), "Option expects four arguments");
            }

            /* Try to read third double */
            (*i)+=1;
            if((*i) < argc){
                (*arg3) = atof(argv[(*i)]);
            } else {
                clParseError(argc, argv, (*i), "Option expects four arguments");
            }

            /* Try to read fourth double */
            (*i)+=1;
            if((*i) < argc){
                (*arg4) = atof(argv[(*i)]);
                return 1;
            } else {
                clParseError(argc, argv, (*i), "Option expects four arguments");
            }

            return 0;
        }
    }
    else{
        clParseError(argc, argv, (*i), "Error parsing command line.");
    }

    return 0;    
}

/* Reads in a required argument, only if num == arg_num i += 0*/
int readRequiredString(int argc, char** argv, int *i, int *num, int arg_num, char** arg);

/* Reads in a required argument, only if num == arg_num i += 0*/
int readRequiredInt(int argc, char** argv, int *i, int *num, int arg_num, int* arg);

/* check to see if current argument starts with a dash and if it does output an error and exit. otherwize return 0 */
int checkBadOption(int argc, char** argv, int *i){
    if((*i) < argc){
        if(argv[(*i)][0] == '-'){
            clParseError(argc, argv, (*i), "Unrecognized option.");
        }
    }
    else{
        clParseError(argc, argv, (*i), "Error parsing command line.");
    }

    return 0;
}

/* Output a command line parsing error */
void clParseError(int argc, char **argv, int i, char* message){
    fprintf(stdout, "Error: %s\n", message);
    fprintf(stdout, "       for command at command line argument <%d: %s >\n", i, (i < argc) ? argv[i] : "(Error: passed end of line)");
    usage((0 < argc) ? argv[0] : "(Error: No program name)");
    exit(1);
}
