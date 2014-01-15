#include <stdio.h> 
#include <string.h>

#include "../inc/sam.h"  
#include "reducer.h"
#include "prob_cal.h"

void get_info( char * input, 
               char *org_chr, int *org_pos, 
               char *fnd_chr, int *fnd_pos,
               char *base, unsigned int *flag, int *qual, int *ret )
{  
    char * tmp;
//    fprintf( stderr, "%s", input );

    tmp = (strtok( input, "|:\t\n" ) );
    strcpy( org_chr, tmp );

    tmp = strtok( NULL, ":\t\n" );
    if( tmp != NULL )
    {
        *org_pos = atoi( tmp );
    }else
    {   *org_pos = -1;
    }

    tmp = (strtok( NULL, ":\t\n" ) );
    strcpy( fnd_chr, tmp );

    tmp = strtok( NULL, ":\t\n" );
    if( tmp != NULL )
    {
        *fnd_pos = atoi( tmp );
    }else
    {   *fnd_pos = -1;
    }

    tmp = (strtok( NULL, ":\t\n" ) );
    if( tmp != NULL )
    {   *base = (*tmp);
    }
    else
    {
        *base = 'N';
    }

    tmp = strtok( NULL, ":\t\n" );
    if( tmp != NULL )
    {
        //*flag = atoi( &tmp[2] ); // 0x remove
        *flag = atoi( tmp ); // 0x remove
    }else
    {   *flag = -1;
    }

    tmp = strtok( NULL, ":\t\n" );
    if( tmp != NULL )
    {
        *qual = atoi( tmp );
    }else
    {
        *qual = -1;
    }

    tmp = strtok( NULL, ":\t\n" );
    if( tmp != NULL )
    {
        *ret = atoi( tmp );
    }else
    {
        *ret = -1;
    }

/*
    fprintf( stdout, "%d:", *pos );
    fprintf( stdout, "%s:", chr );
    fprintf( stdout, "%d\t", pos_chr );
    fprintf( stdout, "%c:", base );
    fprintf( stdout, "%d:", flag );
    fprintf( stdout, "%d:", qual );
    fprintf( stdout, "%d\n", ret );
*/
}

static int parse_args( int argc, char *argv[],
                int *length, int *threshold, int *range )
{
    int ret = 0;
    char c;

    // all options need their value
    while( ( c = getopt( argc, argv, "l:q:s:i:d:o:t:f:b:x:m:p:" )) >= 0 )
    {
        //fprintf( stderr, "optind : %d\n", optind );

        switch(c)
        {
        case 'l': (*length) = atoi( optarg ); break;
        case 'q': break; //(*qual) = optarg[0]; break;
        case 's': break; //(*err_rate) = atof( optarg ); break;
        case 'i': break;
        case 'd': break; //(*err_rate) = atof( optarg ); break;
        case 'o': (*range) = atoi( optarg); break; //(*range) = atoi( optarg); break;
        case 't': (*threshold) = atoi( optarg); break;
        case 'f': break; //strcpy( fasta, optarg); break;
        case 'b': break; //strcpy( fasta, optarg); break;
        case 'x': break; //strcpy( fasta, optarg); break;
        case 'm': break;
        case 'p': break; //strcpy( fasta, optarg); break;
        default : fprintf( stderr, "%s: WRONG OPTION \'%c\'\n", __FILE__, c );
                  ret = -1;
                  break;
        }
    }
    optind = 1;
    return ret;
}


#define BUF_SIZE 24000

void analyzer(int argc, char *argv[])  
{  
    char input[BUF_SIZE] = { 0, };
    int length = 0;
    int threshold = 0;
    int range = 0;
//    FILE *output dd= NULL;

    char org_chr[8] = { 0, };
    char prev_chr[8] = { 0, };
    int  org_pos;
    int  prev_pos = 1;
    int  is_new_pos = 0;  // HERE
    char fnd_chr[8] = { 0, };
    int  fnd_pos;
    char base;
    uint32_t  flag;
    int  qual;
    int  ret;
    char *ARGV[ARGC_MAX];
    int ARGC = 0;
    int i = 0;

    if( argc == 0 ) // HADOOP
    {
        ARGV[0] = getenv("mapred_job_arg0"); // e.g. 50
        ARGV[1] = "analyzer";
        ARGV[2] = getenv("mapred_job_arg2"); // e.g. 50
        ARGV[3] = getenv("mapred_job_arg3"); // e.g. -e
        ARGV[4] = getenv("mapred_job_arg4"); // e.g. 0.02
        ARGV[5] = getenv("mapred_job_arg5"); // e.g. -q
        ARGV[6] = getenv("mapred_job_arg6"); // e.g. A
        ARGV[7] = getenv("mapred_job_arg7"); // e.g. -r
        ARGV[8] = getenv("mapred_job_arg8"); // e.g. 200
        ARGV[9] = getenv("mapred_job_arg9"); // e.g. -t
        ARGV[10] = getenv("mapred_job_arg10"); // e.g. 20
        ARGV[11] = getenv("mapred_job_arg11"); // e.g. -f
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

        fprintf( stderr, "0. %s\n", ARGV[0] );
        fprintf( stderr, "1. %s\n", ARGV[1] );
        fprintf( stderr, "2. %s\n", ARGV[2] );
        fprintf( stderr, "3. %s\n", ARGV[3] );
        fprintf( stderr, "4. %s\n", ARGV[4] );
        fprintf( stderr, "5. %s\n", ARGV[5] );
        fprintf( stderr, "6. %s\n", ARGV[6] );
        fprintf( stderr, "7. %s\n", ARGV[7] );
        fprintf( stderr, "8. %s\n", ARGV[8] );
        fprintf( stderr, "9. %s\n", ARGV[9] );
        fprintf( stderr, "10. %s\n", ARGV[10] );
        fprintf( stderr, "11. %s\n", ARGV[11] );
        fprintf( stderr, "12. %s\n", ARGV[12] );
        fprintf( stderr, "13. %s\n", ARGV[13] );
        fprintf( stderr, "14. %s\n", ARGV[14] );
        fprintf( stderr, "15. %s\n", ARGV[15] );
        fprintf( stderr, "16. %s\n", ARGV[16] );
        fprintf( stderr, "17. %s\n", ARGV[17] );
        fprintf( stderr, "18. %s\n", ARGV[18] );
        fprintf( stderr, "19. %s\n", ARGV[19] );
        fprintf( stderr, "20. %s\n", ARGV[20] );
        fprintf( stderr, "21. %s\n", ARGV[21] );
        fprintf( stderr, "22. %s\n", ARGV[22] );
        fprintf( stderr, "23. %s\n", ARGV[23] );

        ARGC = atoi( ARGV[0] );
 
        parse_args( ARGC, ARGV, &length, &threshold, &range );
    }
    else
    {
        parse_args( argc, argv, &length, &threshold, &range );
    }

    fprintf( stderr, "==================================================\n" );
    fprintf( stderr, "length : %d\n", length );
    fprintf( stderr, "threshold : %d\n", threshold );
    fprintf( stderr, "library distance : %d\n", range );
    fprintf( stderr, "==================================================\n" );

    fprintf( stdout, "#chr\tpos\t\tbase\tmaq\ttotal = >= + <(um)\tGMS\n");


    init_prob_window( length );
//    output = fopen( "sort.gma", "w+" );

    while( fgets( input, BUF_SIZE-1, stdin ) )
    {
        get_info( input, org_chr, &org_pos, 
                         fnd_chr, &fnd_pos, &base, &flag, &qual, &ret ); 
/*
        fprintf( stderr, "%s", input );
*/
/*
        fprintf( stderr, "%s:", org_chr );
        fprintf( stderr, "%d-", org_pos );
        fprintf( stderr, "%s:", fnd_chr );
        fprintf( stderr, "%d\t", fnd_pos );

        fprintf( stderr, "%c:", base );
        fprintf( stderr, "%d(0x%x):", flag, flag );
        fprintf( stderr, "%d:", qual );
        fprintf( stderr, "%d\n", ret );
*/

        if( strcmp( prev_chr, "" ) == 0 )
        {
            //fprintf( stderr, "none -> %s +\n", org_chr );
            strcpy( prev_chr, org_chr );
        }
        else if( strcmp( prev_chr, org_chr ) == 0 )
        {
            //  in the same chromosome
            // fprintf( stderr, "org_pos: %d, prev_pos: %d\n", org_pos, prev_pos );

            if( prev_pos != org_pos )
            {
                for( i = 1; i < ( org_pos - prev_pos ) ; i++ )
                {
                    //fprintf( stdout, "prev_pos + i : %d\n", prev_pos + i );
                    move_window( NULL,
                                 0, // range
                                 org_chr, 
                                 prev_pos+i, //org_pos,
                                 '*', //base,
                                 0,   //flag,
                                 -1,  //qual, 
                                 threshold );

                }
 
                prev_pos = org_pos;
                is_new_pos = 1;
            }
            else
            {
                is_new_pos = 0;
            }
        }
        else // new chromosome
        {
            fprintf( stderr, "clear windows: prv_chr(%s), prv_pos(%d+1)\n", prev_chr, prev_pos );
            fprintf( stderr, "clear windows: org_chr(%s), org_pos(%d)\n", org_chr, org_pos );

            clean_windows( NULL, prev_chr, prev_pos, length, threshold); 
            destroy_prob_window();
            init_prob_window( length );

            fprintf( stderr, "%s -> %s *\n", prev_chr, org_chr );
            is_new_pos = 0;
            strcpy( prev_chr, org_chr );
        }

        // prg_chr starts with "chr", e.g. chrX, chr1
        // fnd_chr depends on how BWA and SAM tools work

        if( org_pos == fnd_pos )
        {
            //fprintf( stderr, "%9d:%9d:%d\n", org_pos, fnd_pos, qual );
        }
        else
        {
            qual = 0;
        }

        //fprintf( stdout, "%9d:%09d:%09d:%d\n", prev_pos, org_pos, fnd_pos, qual );

        if( ( strcmp( &org_chr[3],  fnd_chr )    == 0 ) ||
            ( strcmp( &org_chr[3], &fnd_chr[3] ) == 0 ) ||
            ( strcmp(  org_chr   ,  fnd_chr    ) == 0 ) ||
            ( strcmp(  org_chr   , &fnd_chr[3] ) == 0 ) )
        {
            //fprintf( stderr, "same %s == %s\n", org_chr, fnd_chr );

            if( is_new_pos == 0 )
            {
                

                fill_window( NULL,
                             range,
                             org_chr, 
                             org_pos,
                             base,
                             flag,
                             qual, 
                             threshold );
            }
            else
            {
//               fprintf( stderr, "\n" );
                move_window( NULL,
                             range,
                             org_chr, 
                             org_pos,
                             base,
                             flag,
                             qual, 
                             threshold );
            }
        }
        else
        {
            //fprintf( stderr, "diff %s != %s\n", org_chr, fnd_chr );
            if( is_new_pos == 0 )
            {
                fill_window( NULL,
                             range,
                             org_chr, 
                             org_pos,
                             base,
                             flag,
                             -1, 
                             threshold );
            }
            else
            {
//                fprintf( stderr, "\n" );
                move_window( NULL,
                             range,
                             org_chr, 
                             org_pos,
                             base,
                             flag,
                             -1, 
                             threshold );
            }
        }
    } // while

    //clean_windows( NULL, org_chr, org_pos+1, length); 
    clean_windows( NULL, org_chr, org_pos, length, threshold); 
    destroy_prob_window();
}



