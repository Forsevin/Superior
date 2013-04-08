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
void     get( char file[] );                         // Get a file from repository
char    *getfn(  char *url );                       // Return the filename from url
char    *homedir( const char *file );               // Return the home directory with file


/*
 *  Code licensed under GPL (GNU General Public License) 
 *  See: http://www.gnu.org/licenses/gpl.html
*/

int main( int args, char *argv[] )
{

    curl_global_init(CURL_GLOBAL_ALL);
    
    if( args < 2 )
        printUsage();
    
    else if( !strcmp(ARG, "get") )
        get( argv[2] );
    
    else if( !strcmp(ARG, "update") )
        update();

    else if( !strcmp(ARG, "lf") )
        list("index");

    else if( !strcmp(ARG, "ls") )
        list("sources");

    else if( !strcmp(ARG, "help") )
        printUsage();
    
    else if( !strcmp(ARG, "setup") )
        setup();
    
    else if( !strcmp(ARG, "add-src") )
        addSrc( argv[2] );

    else printUsage();

    return 0;

}



/* Return home directory with file */
char *homedir(const char *file)
{

    char *dir;
    dir = getenv("HOME");
    char *superiorPath = "/Superior/";
    char *full = malloc( strlen(file) + strlen(superiorPath) + strlen(dir) + 1 );
    strcpy(full, dir);
    strcat(full, superiorPath);
    strcat(full, file);

    return full;
}



/* Add a source to sources */
void addSrc( char url[] )
{ 
    // Make sure url is set
    if( url == NULL )
        Error("Missing argument for add-src", -1);

    FILE *sources;
    // Open sources
    if( (sources = fopen(homedir("sources"), "a")) == NULL )
        Error("Couldn't open sources, maybe the file is missing?", -1);

    // Update sources
    fputs(url, sources);    // Put URL
    fputs("\n", sources);   // Put a newline as well
    fclose( sources );

    return;

}



/* List a files content */
void list(char ofile[])
{

    FILE    *lfile;
    int     lines;
    char    line[251];
    char    description[51];
    char    *cont;

    if( !(lfile = fopen( homedir(ofile), "r") ) )
            Error("Could not open file, maybe the file is missing?", -1);

    for( lines=0; fgets( line, 250, lfile  ); lines++ ){
    
        cont = strtok(line, " " );

        // Make sure it's not a file description
        if(cont[0] == '\t')
            continue;

        // Remove newlines
        removeNl(cont);    

        printf("| %s\n", cont);
    }

    printf("Total: %i\n", lines);
   
    return;

}



/* Create files */
void setup()
{

    int dir, ddir;
    FILE *file;
   
    printf("Settings up Superior...\n");
    
    // Create directory
    dir  = mkdir(homedir(""),           S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
    ddir = mkdir(homedir("downloads"),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );

    // Check if folder were created sucessfully
    if(dir < 0)
        printf("\t | Could not create directory at %s (maybe it already exist?)\n", homedir(""));
    else
        printf("\t | Directory created at %s\n", homedir(""));


    if(ddir < 0)
        printf("\t | Could not create directory at %s (maybe it already exist?)\n", homedir("downloads"));
    else
        printf("\t | Directory created at %s\n", homedir("downloads"));


    // Create index  & source
    if( (file = fopen(homedir("index"), "a" ) ) == NULL )
        printf("\t | Could not create '%s'\n", homedir("index"));
    else
        printf("\t | Index created in '%s'\n", homedir("index")); 
    

    fclose(file);

    if( (file = fopen(homedir("sources"), "a")) == NULL )
        printf("\t | Could not create '%s'\n", homedir("sources"));
    else
        printf("\t | Sources created in '%s'\n", homedir("sources"));
    
    fclose(file);

    return;
}



/* Update local index from sources */
void update()
{



}



/* Remove newline from string */
void removeNl( char *string  ){

    int i;
    for(i=0; i < strlen(string); i++ ) 
        if(string[i] == '\n')
            string[i] = '\0';

    return;
}



/* Get a file from index */
void get( char file[] )
{

    FILE *index;
    char line[250];
    char description[51];

    if( file == NULL )
        Error("Missing argument for get", -1);

    if( !(index = fopen( homedir("index"), "r") ) )
        Error("Could not open index, maybe the file is missing?", -1);

    printf("Looking for '%s'... ", file);

    while( fgets(line, 250, index) != NULL )
    {
    
        char* name = strtok( line, " ");

        if( !strcmp(name, file) )
        {
            
            printf(" found it!\n");

            // File information
            char *url       = strtok( NULL, " " );
            //char *version   = strtok( NULL, " " );    Currently removed since it never really served any purpose
            char *file      = getfn(url); 

            // Get description
            if(fgets(description, 50, index) == NULL){
                printf("[Warning] Missing description\n");
                strcpy(description, " (missing)");

            }else if(description[0] != '\t'){
                
                printf("[Warning] Missing description or wrongly formated\n");
                strcpy(description, " (missing)");
            }

            if(url == NULL || file == NULL){
                Error("Information was missing about file", -1);
            }
            // Remove newlines
            removeNl(description);
            removeNl(url);

            //Print info
            printf(" -name:\t%s\n -url:\t%s\n -desc:%s\n -file:\t%s\n", name, url, description, file);
            printf("Continue? [Y/n]");

            // Download
            char prompt = getchar(); 
            if( prompt == 'Y' || prompt == 'y' )
            {
                printf("downloading file...");
                download(url, file);
                printf(" Done!\n");
                return;
            
            } else {

                puts("Aborting...");
                return;
            }
        }
    }

    puts("Couldn't find file");
    return;
}



/* Return filename from url */
char* getfn( char *url)
{

    char *turl = malloc (1 + strlen(url));
    strcpy(turl, url);
    char * lastocr;
    lastocr =  strrchr(turl, '/');

    if(lastocr == NULL)
        Error("Invalid URL", -1);

    if(lastocr[0] == '/')
        memmove( lastocr, lastocr+1, strlen(lastocr) );
 
    return lastocr;
}



/* Print error message and exit */
void Error( char message[], int status )
{
    printf("[Failure] %s\n", message);
    exit( status );
}



/* Print commands */
void printUsage()
{

    printf("usage: \n Setup\t\t Setup Superior\n get \t\t Get a file from a repository \n update \t\t Update index from sources\n add-src\t Add new repository\n ls\t\t list all sources\n lf \t\t List all files avaible in index\n" );
    exit( 0 );

} 

/* Write to file */
size_t writef( void *ptr, size_t size, size_t nmemb, FILE *stream )
{

    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}



/* Download  a file*/
void download(char *url, char out[])
{
    
    // Set variables
    CURL *curl;
    FILE *src;
    CURLcode res;
    curl = curl_easy_init();

    // Download
    src = fopen( homedir(out), "wb");
    curl_easy_setopt( curl, CURLOPT_URL,            url     );
    curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION,  writef  );
    curl_easy_setopt( curl, CURLOPT_WRITEDATA,      src     );
    res = curl_easy_perform( curl );

    // Finish
    curl_easy_cleanup( curl );
    fclose( src );

}

