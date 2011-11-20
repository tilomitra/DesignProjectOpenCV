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

#include <csuCommon.h>

#define OPENING  "csuToolsPGM2SFI convert PGM images to SFI format."

void usage(const char* name){
    printf("Usage: %s pgmfiles\n\n",name);
    printf( "    This program converts pgm images to sfi format.\n");
    exit(1);
}

void removeExtention(char* filename){
    int i;

    for(i = strlen(filename); i >= 0; i--){
        if(filename[i] == '.'){
            filename[i] = 0;
            break;
        }
        if(filename[i] == '/'){
            break;
        }
    }
}

int main(int argc, char** argv){
    Image im;
    char outfilename[MAX_FILENAME_LENGTH];
    int i = 1;

    printf("%s\n%s\n",OPENING,VERSION);
    if(argc == 1 || strcmp(argv[1],"-help") == 0 || strcmp(argv[1],"--help") == 0){
        usage(argv[0]);
    }


    for(;i < argc; i++){
        printf("Converting: %s\n",argv[i]);
        im = readPGMImage(argv[i]);
        
        removeExtention(argv[i]);

        sprintf(outfilename,"%s.sfi",argv[i]);
        printf("    Writing: %s\n",outfilename);
        writeRawImage(im,outfilename);
    }
    printf("done.\n");

    return 0;
}


