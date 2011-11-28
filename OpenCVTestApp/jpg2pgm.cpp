//
//  jpg2pgm.cpp
//  OpenCVTestApp
//
//  Created by Sachi on 11-11-21.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//


#include "jpg2pgm.h"

void getfilenames()

{
    //char index[100]; //Not used at this stage - redundant?
    FILE *fr;
    int depth,l,w,q;
    char *material,code;
    char directory[]="/Users/sachi/Documents/development/OpenCVTestApp/OpenCVTestApp/data/csuScrapShots/source/pgm";
    char file[25];//not used - redundant?
    char str[1000]; //maybe make this larger
    string array[100];
    struct dirent *dp;
    string line ;
    int i;
    string * start = array;
    
    
    DIR * dfd = opendir(directory);
    
    //   if(dfd != NULL) 
    //{
    while((dp = readdir(dfd)) != NULL) 
    {
        line =  strcat(str,dp->d_name);
        line = strcat(str,"\n");  
    }
    //printf("%s\n",str);
    
    closedir(dfd);
    //   }		
    
    FILE * pFile;
    
    pFile = fopen ("/Users/sachi/Documents/development/OpenCVTestApp/OpenCVTestApp/data/csuScrapShots/source/filenames.txt","w");
    fputs(str,pFile);
    fclose(pFile);
    
}

bool replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

void jpg2pgm()

{
    char myfile[256] = "/Users/sachi/Documents/development/OpenCVTestApp/OpenCVTestApp/data/csuScrapShots/source/filenames.txt";
    char directory[]="/Users/sachi/Documents/development/OpenCVTestApp/OpenCVTestApp/data/csuScrapShots/source/pgm";
    int a = 0;
    string fileline[100];
    size_t found;
    IplImage * image = 0;
    char imagename[256];
    
    ifstream file(myfile);
    getfilenames();
    while(!file.eof())
    {
        getline(file,fileline[a]);
        found = fileline[a].find(".jpg");
        
        if(found!=string::npos)
        {
            replace(fileline[a],".jpg",".pgm");
            sprintf(imagename, "%s%s%s", directory,"/",fileline[a].c_str());
            image = cvLoadImage(imagename, CV_LOAD_IMAGE_UNCHANGED);
            cvSaveImage(imagename, image);
            a++;
            
            // clean up and release resources
            cvReleaseImage(&image);
        }

    }
}