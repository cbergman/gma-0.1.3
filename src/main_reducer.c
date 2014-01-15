/*
    intmap -p file
    

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <time.h>

//#include "genfq.h"
//#include "runall.h"
#include "reducer.h"
#include "tech_red.h"

#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define VERSION_BUILD 1

static void usage()
{
    printf( "Program : GMA (Genome Mappability Analyzer) REDUCER\n" );
    printf( "Version : %d.%d.%d\n", VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD );
    printf( "Usage : reducer [COMMAND] [OPTION]\n");
    printf( "[COMMAND] analyzer \n" );
    printf( "analyzer : anlayze map results, calculate genome mappabilityr.\n" );
    printf( "    [OPTION] l | t \n" );
    printf( "    -l   Length [1-200]\n" ); 
    printf( "    -t   Threshold of map quality from sam file\n" );
    printf( "\n" );
}

int main( int argc, char *argv[] )
{

    if( (argc == 2 ) && (strcmp( "-h", argv[1] ) == 0) ) // HELP
        usage();        
    else if( argc < 2 ) // HADOOP
    {    //analyzer( argc-1, argv+1 );
        analyzer_by_tech( argc-1, argv+1 );
    }else if( strcmp( argv[1], "analyzer") == 0 )
    {    analyzer( argc-1, argv+1 );
    }else if( strcmp( argv[1], "tech") == 0 )
    {    analyzer_by_tech( argc-1, argv+1 );
    }else
    {    usage();
    }

    return EXIT_SUCCESS;  // must return 0(=EXIT_SUCCESS)
                          // non-zero means errors, EXIT_FAILURE
}


