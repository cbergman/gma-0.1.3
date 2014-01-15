#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <zlib.h>
#include <time.h>
#include <string.h>

#include "main_mapper.h"
#include "genfa.h"
#include "genfq.h"

#define BASES_PER_LINE      60   // default 60, Min 10, Max 100
#define BASES_PER_LINE_MAX  100   // default 60, Min 10, Max 100
#define NUM_LINE            100
#define OVERLAY             10
#define BUF_SIZE            16384 //(BASES_PER_LINE*(NUM_LINE+1))


static int parse_args( int argc, char *argv[], char *fasta, int *bpl )
{
    int ret = 0;
    char c;

    while( (c = getopt( argc, argv, "l:q:s:i:d:o:t:f:b:x:m:p:" )) >= 0 )
    {
//        fprintf( stderr, "%c %s\n", c, optarg );
        switch(c)
        {
        case 'l': break;
        case 'q': break;
        case 's': break;
        case 'i': break;
        case 'd': break;
        case 'o': break;
        case 't': break;
        case 'f': strcpy( fasta, optarg ); 
                  break;
        case 'b': (*bpl) = atoi(optarg);
                  break;
        case 'x': break;
        case 'm': break;
        case 'p': break;
        default : usage();
                  break;
        }
    }        
    optind = 1; // very imp, for the next getopt

    return ret;
}

int genfa(int argc, char *argv[] ) //, int length)
{
    int i = 0;//, k = 0;
    char line[BUF_SIZE] = { 0, };
    //char dna[BUF_SIZE] = { 0, };
    char oneline[BASES_PER_LINE_MAX] = {0,};
    char *chr = NULL; 
    char *pDNA = NULL;
    FILE * ref_fa = NULL;
    int start = 0, end = 0;
    int start_base_pos = 0;
    int linecount = 0;
    int lastpos = 0;
    int bpl = BASES_PER_LINE;

    char fasta[64] = { 0, };

    parse_args( argc, argv, fasta, &bpl );

    fprintf( stderr, "fasta file : %s\n", fasta );
    fprintf( stderr, "bases per line : %d\n", bpl );
    fprintf( stderr, "-------------------------------------------\n");


    ref_fa = fopen( fasta, "w" );

    if( ref_fa == NULL )
    {   fprintf( stderr, "%s open fail!\n", fasta );
        return (-1);
    }

    while( fgets( line, BUF_SIZE, stdin ) )
    {
//        fprintf( stderr, "%s\n", line );

        chr = strtok( line, ":\t\n " );
        start = atoi( strtok( NULL, "-\t\n ") );
        end = atoi( strtok( NULL, ">\t\n ") );
        pDNA = strtok( NULL, "\t\n " );
       
        if( ( linecount == 0)  || ( start == 1 ) )
        { 
            fprintf( ref_fa, ">%s %d\n", chr, start );
            start_base_pos = start;
            lastpos = 0;
            linecount++;
        }

        for( i = start; i < end; i = i+bpl )
        {   
            if( i < lastpos )
            {   // do nothing
            }
            else
            {
                memset( oneline, 0, bpl );
                memcpy( oneline, &pDNA[i-start], bpl );

                fprintf( ref_fa, "%s\n", oneline );
                //fprintf( ref_fa, "[%d: %d]%s\n", i-start, i, oneline );
            }
        }
        lastpos = i;
       //fprintf( ref_fa, "lastpos %d\n", lastpos );
    }

    //printf("Total %d bases, error rate:%.2f%%\n", seq->seq.l, error*100 );
    fclose( ref_fa );
    return start_base_pos;
}


