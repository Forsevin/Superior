#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<curl/curl.h>
#include<curl/easy.h>
#include<sys/stat.h>

#define ARG argv[1]


void    printUsage();                               // Print usage of Superior
void    Error(char message[], int status);          // Show message and exit with status
void    checkFiles();                               // Make sure all files exist, create them if necessary
void    update();                                   // Update index
void    list( char ofile[] );                       // List the content of a file
void    download(char *url, char out[]);            // Download a file directly form url
void    addSrc(char url[]);                         // Add a source
void    setup();                                    // Setup Superior files and folders
void    removeNl( char *string );                   // Remove newline from string
void    get( char file[] );                         // Get a file from repository
char    *getfn(  char *url );                       // Return the filename from url
char    *homedir( const char *file );               // Return the home directory with file

