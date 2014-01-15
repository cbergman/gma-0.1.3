#include <fcntl.h>
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <time.h>
#include <getopt.h>

#include "runall.h"
#include "reducer.h"

#define FILE_OPT_O (O_WRONLY | O_CREAT)
#define FILE_OPT_S (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

#define FALSE 0 
#define TRUE  1


static struct option long_options[] = 
{
    {"illumina",   no_argument, 0, 'i'},
    {"solid",      no_argument, 0, 's'},
    {"iontorrent", no_argument, 0, 't'},
    {"roche",      no_argument, 0, 'r'},
    {"pacbio",     no_argument, 0, 'o'},
    {"pacbio-ec",  no_argument, 0, 'c'},
    {0, 0, 0, 0}
};

int my_argc = 23;

char len[16] = { 0, };
char sub[16] = { 0, };
char ins[16] = { 0, };
char del[16] = { 0, };
char prg[16] = { 0, };

char idx[256] = { 0, };
char bpl[256] = { 0, };
char prg_path[256] = { 0,};

char *my_argv[23] = { "runall",
   "-l", "0", "-q", "A", "-s", "0", "-i", "0", "-d", "0" , "-o", "0", "-t", "20", "-f", "ref.fa", 
   "-b", "0", "-x", NULL , "  ", NULL };

#define LEN 2
#define SUB 6
#define INS 8
#define DEL 10
#define BPL 18
#define IDX 20
#define PRG 22

extern int parse_args( int argc, char *argv[], 
                       int my_argc, char *my_argv[],
                       int *hadoop )
{
    int ret = 0;
    char c;
    int option_index = 0;

    // all options need their value
    while( ( c = getopt_long( argc, argv, "istrocb:x:m:p:", long_options, &option_index )) >= 0 ) 
    {
       // fprintf( stderr, "optind : %d, %c, %s\n", optind, c, optarg );

        switch(c)
        {
        case 0: fprintf( stderr, "here case 0\n" );
        case 'i': 
                  fprintf( stderr, "Illumina-like simulation is applied\n" );
                  strcpy( len, "100" ); my_argv[LEN] = len;
                  strcpy( sub, "0.01" ); my_argv[SUB] = sub;
                  strcpy( ins, "0" ); my_argv[INS] = ins;
                  strcpy( del, "0" ); my_argv[DEL] = del;
                  break;
        case 's': 
                  fprintf( stderr, "Solid-like simulation is applied\n" );
                  strcpy( len, "75" ); my_argv[LEN] = len;
                  strcpy( sub, "0.01" ); my_argv[SUB] = sub;
                  strcpy( ins, "0" ); my_argv[INS] = ins;
                  strcpy( del, "0" ); my_argv[DEL] = del;
                  break;
        case 't': 
                  fprintf( stderr, "Ion Torrent-like simulation is applied\n" );
                  strcpy( len, "200" ); my_argv[LEN] = len;
                  strcpy( sub, "0.0004" ); my_argv[SUB] = sub;
                  strcpy( ins, "0.0001" ); my_argv[INS] = ins;
                  strcpy( del, "0.0095" ); my_argv[DEL] = del;
                  break;
        case 'r': 
                  fprintf( stderr, "Roche/454-like simulation is applied\n" );
                  strcpy( len, "800" ); my_argv[LEN] = len;
                  strcpy( sub, "0.0018" ); my_argv[SUB] = sub;
                  strcpy( ins, "0.0054" ); my_argv[INS] = ins;
                  strcpy( del, "0.0036" ); my_argv[DEL] = del;
                  break;
        case 'o':
                  fprintf( stderr, "PacBio-like simulation is applied\n" );
                  strcpy( len, "2000" ); my_argv[LEN] = len;
                  strcpy( sub, "0.014" ); my_argv[SUB] = sub;
                  strcpy( ins, "0.1147" ); my_argv[INS] = ins;
                  strcpy( del, "0.0343" ); my_argv[DEL] = del;
                  break;
        case 'c': 
                  fprintf( stderr, "PacBio(error corrected)-like simulation is applied\n" );
                  strcpy( len, "2000" ); my_argv[LEN] = len;
                  strcpy( sub, "0.0033" ); my_argv[SUB] = sub;
                  strcpy( ins, "0.0033" ); my_argv[INS] = ins;
                  strcpy( del, "0.0033" ); my_argv[DEL] = del;
                  break;
        case 'b':
                  strcpy( bpl, optarg );
                  my_argv[BPL] = bpl;
                  break;
        case 'x': 
                  strcpy( idx, optarg );
                  my_argv[IDX] = idx;
                  break;
        case 'm': break; 
        case 'p': (*hadoop) = TRUE;
                  strcpy( prg, "-p" );
                  my_argv[PRG-1] = prg;
                  strcpy( prg_path, optarg );
                  my_argv[PRG] = prg_path;
                  break;
        default : fprintf( stderr, "%s: WRONG OPTION \'%c\'\n", __FILE__, c ); 
                  ret = -1;
                  break;
        }
    }
    optind = 1;
    return ret;
}

int runall_by_tech(int argc, char *argv[])  
{  
    int hadoop = FALSE;
    //int i = 0;
    
    //fprintf( stderr, "-------------------------------------------\n");
    //fprintf( stderr, "1\n" );

    parse_args( argc, argv, 
                my_argc, my_argv,
                &hadoop );

    //fprintf( stderr, "-------------------------------------------\n");
    //for( i = 0; i < my_argc; i++ )
    //{
    //    fprintf( stderr, "%d. %s\n", i, my_argv[i] );
    //}
    fprintf( stderr, "-------------------------------------------\n");

    runall(my_argc, my_argv);


    return EXIT_SUCCESS;  
}  


