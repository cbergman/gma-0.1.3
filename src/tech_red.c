#include <fcntl.h>
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <time.h>
#include <getopt.h>

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

int my_argc = 17;

char len[16] = { 0, };
char sub[16] = { 0, };
char ins[16] = { 0, };
char del[16] = { 0, };
char prg[16] = { 0, };

char idx[256] = { 0, };
char bpl[256] = { 0, };
char prg_path[256] = { 0,};

char *my_argv[17] = { "analyzer",
   "-l", "0", "-q", "A", "-s", "0", "-i", "0", "-d", "0" , "-o", "0", "-t", "20", "-f", "ref.fa" 
   };

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
    //while( ( c = getopt_long( argc, argv, "istrocb:x:m:p:", long_options, &option_index )) >= 0 ) 
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
                  strcpy( sub, "0.04" ); my_argv[SUB] = sub;
                  strcpy( ins, "0.01" ); my_argv[INS] = ins;
                  strcpy( del, "0.95" ); my_argv[DEL] = del;
                  break;
        case 'r': 
                  fprintf( stderr, "Roche/454-like simulation is applied\n" );
                  strcpy( len, "800" ); my_argv[LEN] = len;
                  strcpy( sub, "0.18" ); my_argv[SUB] = sub;
                  strcpy( ins, "0.54" ); my_argv[INS] = ins;
                  strcpy( del, "0.36" ); my_argv[DEL] = del;
                  break;
        case 'o':
                  fprintf( stderr, "PacBio-like simulation is applied\n" );
                  strcpy( len, "2000" ); my_argv[LEN] = len;
                  strcpy( sub, "1.4" ); my_argv[SUB] = sub;
                  strcpy( ins, "11.47" ); my_argv[INS] = ins;
                  strcpy( del, "3.43" ); my_argv[DEL] = del;
                  break;
        case 'c': 
                  fprintf( stderr, "PacBio(error corrected)-like simulation is applied\n" );
                  strcpy( len, "2000" ); my_argv[LEN] = len;
                  strcpy( sub, "0.33" ); my_argv[SUB] = sub;
                  strcpy( ins, "0.33" ); my_argv[INS] = ins;
                  strcpy( del, "0.33" ); my_argv[DEL] = del;
                  break;
        case 'b': break; 
        case 'x': break; 
        case 'm': break; 
        case 'p': break; 
/*
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
*/
        default : fprintf( stderr, "%s: WRONG OPTION \'%c\'\n", __FILE__, c ); 
                  ret = -1;
                  break;
        }
    }
    optind = 1;
    return ret;
}

int analyzer_by_tech(int argc, char *argv[])  
{  
    int hadoop = FALSE;
    int i = 0;
    char *ARGV[ARGC_MAX];
    int ARGC = 0;  

 
    if( argc == 0 ) // HADOOP
    {
        fprintf( stderr, "It is a Hadoop\n");

        ARGV[0] = getenv("mapred_job_arg0"); 
        ARGV[1] = "analyzer";
        ARGV[2] = getenv("mapred_job_arg2"); 
        ARGV[3] = getenv("mapred_job_arg3"); 
        ARGV[4] = getenv("mapred_job_arg4"); 
        ARGV[5] = getenv("mapred_job_arg5"); 
        ARGV[6] = getenv("mapred_job_arg6"); 
        ARGV[7] = getenv("mapred_job_arg7"); 
        ARGV[8] = getenv("mapred_job_arg8"); 
        ARGV[9] = getenv("mapred_job_arg9"); 
        ARGV[10] = getenv("mapred_job_arg10"); 
        ARGV[11] = getenv("mapred_job_arg11"); 
        ARGV[12] = getenv("mapred_job_arg12"); 
        ARGV[13] = getenv("mapred_job_arg13"); 
        ARGV[14] = getenv("mapred_job_arg14"); 
        ARGV[15] = getenv("mapred_job_arg15"); 
        ARGV[16] = getenv("mapred_job_arg16"); 
        ARGV[17] = getenv("mapred_job_arg17"); 
        ARGV[18] = getenv("mapred_job_arg18"); 
        ARGV[19] = getenv("mapred_job_arg19"); 
        ARGV[20] = getenv("mapred_job_arg20"); 
        ARGV[21] = getenv("mapred_job_arg21"); 
        ARGV[22] = getenv("mapred_job_arg22"); 
        ARGV[23] = getenv("mapred_job_arg23"); 

        ARGC = atoi( ARGV[0] );
 
        parse_args( ARGC, ARGV, 
                    my_argc, my_argv,
                    &hadoop );

        for( i = 0; i < my_argc; i++ )
        {
            fprintf( stderr, "%d. %s\n", i, my_argv[i] );
        }
        fprintf( stderr, "-------------------------------------------\n");
    
        analyzer(my_argc, my_argv);

    }
    else 
    {
        fprintf( stderr, "-------------------------------------------\n");
    
        parse_args( argc, argv, 
                    my_argc, my_argv,
                    &hadoop );
    
        for( i = 0; i < my_argc; i++ )
        {
            fprintf( stderr, "%d. %s\n", i, my_argv[i] );
        }
        fprintf( stderr, "-------------------------------------------\n");
    
        analyzer(my_argc, my_argv);
    }

    return EXIT_SUCCESS;  
}  


