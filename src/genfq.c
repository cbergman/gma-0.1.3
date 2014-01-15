#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <zlib.h>
#include <time.h>
#include <math.h>


#include "main_mapper.h"
#include "genfq.h"
#include "kseq.h"


#define FQ_MAX_LEN 4096

KSEQ_INIT( gzFile, gzread )

char random_convert( char base )
{
    int choice = 0;
    char result;

    choice = rand() % 3;

    switch(base)
    {
    case 'A': 
            if( choice == 0 ) result = 'T';
            else if( choice == 1 ) result = 'C';
                 else result = 'G';
            break;
    case 'a': 
            if( choice == 0 ) result = 't';
            else if( choice == 1 ) result = 'c';
                 else result = 'g';
            break;
    case 'T':
            if( choice == 0 ) result = 'A';
            else if( choice == 1 ) result = 'C';
                 else result = 'G';
            break;
    case 't':
            if( choice == 0 ) result = 'a';
            else if( choice == 1 ) result = 'c';
                 else result = 'G';
            break;
    case 'C':
            if( choice == 0 ) result = 'T';
            else if( choice == 1 ) result = 'A';
                 else result = 'G';
            break;
    case 'c':
            if( choice == 0 ) result = 't';
            else if( choice == 1 ) result = 'a';
                 else result = 'g';
            break;
    case 'G':
            if( choice == 0 ) result = 'T';
            else if( choice == 1 ) result = 'C';
                 else result = 'A';
            break;
    case 'g':
            if( choice == 0 ) result = 't';
            else if( choice == 1 ) result = 'c';
                 else result = 'a';
            break;
    case 'N':
            choice = rand() % 4;
            if( choice == 0 ) result = 'A';
            else if( choice == 1 ) result = 'T';
                 else if( choice == 2 ) result = 'C';
                      else result = 'G';
            break;
    case 'n':
            choice = rand() % 4;
            if( choice == 0 ) result = 'a';
            else if( choice == 1 ) result = 't';
                 else if( choice == 2 ) result = 'c';
                      else result = 'g';
            break;
    default :
            result = 'n';
            fprintf( stderr, "ERROR: base(%c, %d, 0x%x) is not normal\n", base, base, base );
            break;
    }
    return result;
}




void make_complement_reverse( char *rev, char *seq, int length )
{
    int i = 0;

    for( i = 0; i < length; i++ )
    {
        switch(seq[i])
        {
            case 'T': rev[length - i - 1] = 'A'; break;
            case 't': rev[length - i - 1] = 'a'; break;
            case 'C': rev[length - i - 1] = 'G'; break;
            case 'c': rev[length - i - 1] = 'g'; break;
            case 'A': rev[length - i - 1] = 'T'; break;
            case 'a': rev[length - i - 1] = 't'; break;
            case 'G': rev[length - i - 1] = 'C'; break;
            case 'g': rev[length - i - 1] = 'c'; break;
            case 'N': rev[length - i - 1] = 'N'; break;
            case 'n': rev[length - i - 1] = 'n'; break;
            default : break;
        }
    }
}


static char output_name[64] = { 0, };

void get_output_name( char * buf )
{
    strcpy( buf, output_name );
}

/*
void get_length(int * plength)
{
    (*plength) = length;
}
*/


static int parse_args( int argc, char *argv[], 
                       int *length, char *qual, 
                       float *error_sub_rate, float *error_ins_rate, float * error_del_rate, 
                       int * ob_range, char *fasta ) 
{
    char c;
    int ret = 0;

    while( ( c = getopt( argc, argv, "l:q:s:i:d:o:t:f:b:x:m:p:")) >= 0 )
    {
        switch(c)
        {
        case 'l': (*length) = atoi(optarg); break;
        case 'q': (*qual) = optarg[0];      break;
        case 's': (*error_sub_rate) = atof(optarg);  break;
        case 'i': (*error_ins_rate) = atof(optarg);  break;
        case 'd': (*error_del_rate) = atof(optarg);  break;
        case 'o': (*ob_range) = atoi(optarg);  break;
        case 'f': strcpy( fasta, optarg );  break;  
        case 't': break;  // ignore
        case 'b': break;  // ignore
        case 'x': break;  // ignore
        case 'm': break;  // ignore
        case 'p': break;  // ignore
        default : fprintf( stderr, "%s:  WRONG OPTION \'%c\'\n", __FILE__, c );
                  usage();
                  ret = -1;
                  break;
        }
    }
    optind = 1;
    return ret;
}


void genfq(int argc, char *argv[])
{
    //float error;
    int start = 0, end_offset = 0, revcomp = 0;
    //int pos = 0;
    //char c;
    int l = 0, i = 0, j = 0, cn = 0;
    int r = 0;
    int offset = 0;
    kseq_t *seq;
    gzFile input;
    FILE *output;
    char seqbuf[FQ_MAX_LEN] = { 0, };
    char revbuf[FQ_MAX_LEN] = { 0, };
    char qvbuf[FQ_MAX_LEN] = { 0, };

    int  length = 0;
    int  read_length = 0;
    char qual;
    float error_sub = 0.0;
    float error_ins = 0.0;
    float error_del = 0.0;
    int  ob_range = 0;
    char fasta[64] = { 0, };
    char err_qual = '!';
    int sampling = 1;

    parse_args( argc, argv, &length, &qual, &error_sub, &error_ins, &error_del, &ob_range, fasta );

    if( error_sub == (float)0.0 )
    {    err_qual = '?';
         fprintf( stderr, "no errors will be involved.\n" );
    }
    else
    {
         //err_qual = 30 + '!';
         err_qual = (char)((-10) * log10(error_sub)) + '!';
    }

    fprintf( stderr, "length : %d\n", length );
    fprintf( stderr, "qual : %c\n", qual );
    fprintf( stderr, "error rate(substitution) : %.4f (quality value : %c)\n", error_sub, err_qual );
    fprintf( stderr, "error rate(insertion) : %.4f (quality value : %c)\n", error_ins, err_qual );
    fprintf( stderr, "error rate(deletion) : %.4f (quality value : %c)\n", error_del, err_qual );
    fprintf( stderr, "outbound length : %d\n", ob_range );
    fprintf( stderr, "fasta file : %s\n", fasta );
    fprintf( stderr, "-------------------------------------------\n");


    fprintf( stderr, "Generating fastq file, randomly tainted........\n");

    // prepare qual string
    for( j = 0; j < length; j++ )
    {
        qvbuf[j] = qual;
    }

    // generate error
    srand( (unsigned) time(NULL) );
    //srand(rand()*GetTickCount());

    // real routine
    input = gzopen( fasta, "r");
    //printf( "%s\n", fasta );

    if( input == NULL ) fprintf( stderr, "File open failed.\n");

    if( strcmp( argv[0], "genfqse" ) == 0 ) 
    {
        sprintf( output_name, "%s.s-%d-%c-%.4f-%.4f-%.4f.fq", fasta, length, qual, error_sub, error_ins, error_del );
        start = 0;
        if( ob_range > 0 )
        {
            end_offset = ob_range;
        }
        else
        {
            end_offset = length;
        }
    }else
    {   sprintf( output_name, "%s.p-%d-%c-%.4f-%.4f-%.4f.fq", fasta, length, qual, error_sub, error_ins, error_del );
        start = ob_range - length; 
        end_offset = length;
        revcomp = 1;
    }
    output = fopen( output_name, "w" );
    if( output == NULL ) fprintf( stderr, "File open failed.\n");


    seq = kseq_init(input);

    while( ( l = kseq_read( seq ) ) >= 0 )
    {
        //fprintf( output, "*> %s\n", seq->name.s );
        offset = atoi( seq->comment.s );
        offset += start;

        //fprintf( output, "*> %d\n", offset );

        if( length >= 200 )
        {
            sampling = length/100;
            fprintf( stderr, "sampling frequency : %d\n", sampling );
        }
     
        for( i = start; i <= (seq->seq.l - end_offset); i = i+sampling )
        {
            memset( seqbuf, 0, FQ_MAX_LEN );
            strncpy( seqbuf, (const char *)&(seq->seq.s[i]), length );
      
            //for( cn = 0; cn < 100; cn++ )  // copy number
            for( cn = 0; cn < 1; cn++ )  // copy number
            {
                fprintf( output, ">%s|%09d(%d) (L%d-%d)\n", seq->name.s, offset, cn+1, offset, offset+length-1 );
                //fprintf( stderr, ">%s|%09d(%d) (L%d-%d)\n", seq->name.s, offset, cn+1, offset, offset+length-1 );
                read_length = length;
 
                //for( j = 0; j < read_length; j=j+sampling )
                for( j = 0; j < read_length; j++ )
                {   
                    r = (rand() % 10000);
                    //fprintf( stderr, "random: %d\n", r );

                    if( r < (error_sub*10000) ) 
                    {
                        //fprintf( stderr, "substitution\n" );
                        //seqbuf[j] = random_convert(seqbuf[pos]);    
                        seqbuf[j] = random_convert(seqbuf[j]);    
                        qvbuf[j] = err_qual;
                    }
                    else if( r < ( (error_sub + error_ins)*10000) ) // insertion
                    {
                        //if( r % 2 == 0 ) // insert
                        //{
                            //fprintf( stderr, "in %d, %d\n", j, read_length );
                            //fprintf( stderr, "%s\n", &seqbuf );
                            
                            memcpy( &seqbuf[j+1], &seqbuf[j], read_length-j );
                            qvbuf[read_length] = qual;

                            //seqbuf[j] = random_convert(seqbuf[pos]);  
                            seqbuf[j] = random_convert(seqbuf[j]);  
                            qvbuf[j] = err_qual - 1;
  
                            /*if( seqbuf[j] == 'n' )
                            {
                                qvbuf[j] = err_qual - 1;
                            }*/

                            //fprintf( stderr, "%s\n", seqbuf );
                            //fprintf( stderr, "%s\n", qvbuf );

                            read_length++;
                            j++;
                    }
                    else if( r < ( (error_sub + error_ins + error_del)*10000) ) 
                    {//    else  // delete
                            //fprintf( stderr, "del\n" );
                            //fprintf( stderr, "del %d, %d\n", j, read_length );
                            //fprintf( stderr, "%s\n", &seqbuf[j] );
                            memcpy( &seqbuf[j], &seqbuf[j+1], read_length-j-1 );
                            seqbuf[read_length-1] = '\0';
                            qvbuf[read_length-1] = '\0';
                            //fprintf( stderr, "%s\n", seqbuf );
                            //fprintf( stderr, "%s\n", qvbuf );

                            read_length--;
                            j--;
                    }
                }

                if( revcomp == 0 )
                {
                    fprintf( output, "%s\n", seqbuf );
                    //fprintf( stderr, "%s\n", seqbuf );
                }else
                {
                    //fprintf( output, "%s\n", seqbuf );
                    make_complement_reverse( revbuf, seqbuf, length );
    
                    fprintf( output, "%s\n", revbuf );
                }
     
                
                if( i < (seq->seq.l - length) )
                {
                    fprintf( output, "+\n%s\n", qvbuf );
                    //fprintf( stderr, "+\n%s\n", qvbuf );
                }
                else
                {   for( j = 0; j < (seq->seq.l - i); j++ )
                    {
                        fprintf( output, "%c", qual ); 
                        //fprintf( stderr, "%c", qual ); 
                    }
                    fprintf( output, "\n"); 
                    //fprintf( stderr, "\n"); 
                }         
    
                memset( qvbuf, 0, FQ_MAX_LEN );
                for( j = 0; j < length; j++ )
                {    qvbuf[j] = qual;
                }
            }
            //offset++;
            offset += sampling;
 
        } 
    }

    if( strcmp( argv[0], "genfqse" ) == 0 ) 
    {
        fprintf( stderr, "\tGenerated single end reads \n");
     	fprintf( stderr, "\tread length : %d, qual : %c, error rate(sub,in,del): %.2f%%, %.2f%%, %.2f%%\n", length, qual, error_sub*100, error_ins*100, error_del*100 );
    }else
    {   fprintf( stderr, "\tGenerated pair end reads \n");
     	fprintf( stderr, "\tread length : %d, qual : %c, error rate(sub,in,del): %.2f%%, %.2f%%, %.2f%%, library distance: %d bp\n", length, qual, error_sub*100, error_ins*100, error_del*100, ob_range );
    }

    kseq_destroy( seq );
    fclose(output);        
    gzclose(input);   
    return;
}


