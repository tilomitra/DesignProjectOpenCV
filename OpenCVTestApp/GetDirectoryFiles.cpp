//
//  GetDirectoryFiles.cpp
//  OpenCVTestApp
//
//  Created by Sachi on 11-11-05.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//
#include "GetDirectoryFiles.h"


void GetDirectoryFiles(char * folder)

{
    //char index[100]; //Not used at this stage - redundant?
    FILE *fr;
    int depth,l,w,q;
    char *material,code;
    char directory[]="/Users/sachi/Downloads/haarcascades/";
    char file[25];//not used - redundant?
    char str[50]; //maybe make this larger
    string array[100];
    struct dirent *dp;
    string line ;
    int i;
    string * start = array;

    sprintf(directory, "%s%s",directory, folder);
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
    
    pFile = fopen ("/Users/sachi/Documents/development/OpenCVTestApp/OpenCVTestApp/myfile.txt","w");
    fputs(str,pFile);
    fclose(pFile);
   
}