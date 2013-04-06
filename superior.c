#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<curl/curl.h>
#include<curl/easy.h>

#define ARG argv[1]

void    printUsage();                               // Print usage of Superior
void    Error(char message[], int status);          // Show message and exit with status
void    checkFiles();                               // Make sure all files exist, create them if necessary
void    pull();                                     // Update index
void    list( char ofile[] );                       // List the content of a file
void    download(char *url, char out[]);            // Download a file directly form url
void    addSrc(char url[]);
int     get( char file[] );                         // Get a file from repository
char    *getfn(  char *url );                       // Return the filename from url

int main( int args, char *argv[] ){

    curl_global_init(CURL_GLOBAL_ALL);
    
    if( args < 2 )
        printUsage();
    
    else if( !strcmp(ARG, "get") )
        get( argv[2] );
    
    else if( !strcmp(ARG, "pull") )
        pull();

    else if( !strcmp(ARG, "lf") )
        list("index");

    else if( !strcmp(ARG, "ls") )
        list("sources");

    else if( !strcmp(ARG, "help") )
        printUsage();
    
    else if( !strcmp(ARG, "add-src") )
        addSrc( argv[2] );

    else printUsage();

    return 0;

}

/*{{{*/
void addSrc( char url[] ){

    // Make sure url is set
    if( url == NULL )
        Error("Missing argument for add-src", -1);

    FILE *sources;
    // Open sources
    if( (sources = fopen("sources", "a")) == NULL )
        Error("Couldn't open sources, maybe the file is missing?", -1);

    // Update sources
    fputs(url, sources);    // Put URL
    fputs("\n", sources);   // Put a newline as well
    fclose( sources );

    return;

}
 /*}}}*/

/*{{{*/
void list(char ofile[]){

    FILE    *lfile;
    int     lines;
    char    line[250];
    char    *cont;

    if( !(lfile = fopen(ofile, "r") ) )
            Error("Could not open file, maybe the file is missing?", -1);

    for( lines=0; fgets( line, 250, lfile  ) != NULL; lines++ ){
    
        cont = strtok(line, " " );

        int i;
        // Remove newlines
        for( i = 0; i < strlen(cont); i++ )
            if(cont[i] == '\n')
                cont[i] = '\0';

        printf("%i - %s\n", lines, cont);
    }

    printf("Total: %i\n", lines);
   
    return;

}
/*}}}*/ 

/*{{{*/
void pull(){

    FILE *sources = fopen("sources", "wb");
    char *source;

    while(fgets(source, 250, sources) != NULL){
    
        printf("Found %s\n", source);
        printf("\t| Retrieving index... \t");
        
    }
}
/*}}}*/ 

/*{{{*/    
int get(char file[]){

    FILE *index;
    char line[250];

    if( file == NULL )
        Error("Missing argument for get", -1);

    if( !(index = fopen("index", "r") ) )
        Error("Could not open index, maybe the file is missing?", -1);

    printf("Looking for '%s'... ", file);

    while( fgets(line, 250, index) != NULL )
    {
    
        char* name = strtok( line, " ");

        if( !strcmp(name, file) )
        {
            
            printf(" found it!\n");

            // Package information
            char *url       = strtok( NULL, " " );
            char *version   = strtok( NULL, " " );
            char *file      = getfn(url);
            
            // Remove \n from version
            int i;
            for(i=0; i < strlen(version); i++ )
                if(version[i] == '\n')
                    version[i] = '\0';

            //Print info
            printf(" -name:\t%s\n -url:\t%s\n -vers:\t%s\n -file:\t%s\n", name, url, version, file);
            printf("Continue? [Y/n]\n");

            // Download
            char prompt = getchar(); 
            if( prompt == 'Y' || prompt == 'y' )
            {
                printf("downloading file...");
                download(url, file);
                printf(" Done!\n");
                return 0;
            
            } else {

                puts("Aborting...");
                return 0;
            }
        }
    }

    puts("Couldn't find file");
}
/*}}}*/

/*{{{*/
char* getfn( char *url){

    char *turl = malloc (1 + strlen(url));
    strcpy(turl, url);
    char * lastocr;
    lastocr =  strrchr(turl, '/');

    if(lastocr[0] == '/')
        memmove( lastocr, lastocr+1, strlen(lastocr) );
 
    return lastocr;
}
/*}}}*/

/*{{{*/
void Error( char message[], int status ){
    printf("%s\n", message);
    exit( status );
}
/*}}}*/

/*{{{*/
void printUsage(){

    printf("usage: \n download\t Download file directly\n get \t\t Get a file from a repository \n pull \t\t Update index from sources\n add-repo\t Add new repository\n ls\t\t list all sources\n lf \t\t List all files avaible in index\n" );
    exit( 0 );

} 
/*}}}*/

/*{{{*/
size_t writef( void *ptr, size_t size, size_t nmemb, FILE *stream ){

    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}
/*}}}*/

/*{{{*/

void download(char *url, char out[]){

    
    // Set variables
    CURL *curl;
    FILE *src;
    CURLcode res;
    curl = curl_easy_init();

    // Download
    src = fopen(out, "wb");
    curl_easy_setopt( curl, CURLOPT_URL,            url     );
    curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION,  writef  );
    curl_easy_setopt( curl, CURLOPT_WRITEDATA,      src     );
    res = curl_easy_perform( curl );

    // Finish
    curl_easy_cleanup( curl );
    fclose( src );

}

/*}}}*/
