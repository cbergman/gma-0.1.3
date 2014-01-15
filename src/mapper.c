#include <stdio.h> 
#include <zlib.h>

#include "../inc/sam.h"  
#include "mapper.h"
#include "kseq.h"


KSEQ_INIT( gzFile, gzread );

typedef struct 
{  
    int beg, end;  
    samfile_t *in;  

} anal_t;  
  
// callback for bam_fetch()  
/*
static int fetch_func(const bam1_t *b, void *data)  
{  
    bam_plbuf_t *buf = (bam_plbuf_t*)data;  
    bam_plbuf_push(b, buf);  
    return 0;  
} 
*/ 
// callback for bam_plbuf_init()  


static int pileup_func(uint32_t tid, uint32_t pos, int n, const bam_pileup1_t *pl, void *data)  
{  
    // as a callback function
/*
    anal_t *input = (anal_t*)data; 
    if ((int)pos >= input->beg && (int)pos < input->end)  
        fprintf( stderr, "%s\t%d\t%d\n", input->in->header->target_name[tid], pos + 1, n);  
*/
    return 0;  
}  



int find_chr( char *name, kseq_t *seq, char *chr )
{
    int l;
    int ret = -1;

    while( (l=kseq_read( seq )) >= 0 )
    {
    
        fprintf( stderr, "name : %s, seq->name.s: %s\n", name, seq->name.s );

        if( strcmp( name, seq->name.s ) == 0 )
        {
            strcpy( chr, seq->name.s );
            ret = 0;
            break;
        }
    }

    return ret;
}



void mapper( char *ref, int length, int start_base_pos, const char *bam )
{  
    anal_t input;  
    gzFile pRef;
    kseq_t * seq = NULL;
    char chr[8] = { 0, };
    int ret;
    bam_plbuf_t *buf;
    bam1_t *b;

/*
    fprintf( stderr, "ref: %s\n", ref );
    fprintf( stderr, "length: %d\n", length );
    fprintf( stderr, "start_base_pos: %d\n", start_base_pos );
    fprintf( stderr, "bam: %s\n", bam );
*/
    input.beg = 0; input.end = 0x7fffffff;  
    input.in = samopen(bam, "rb", 0);
  
    if (input.in == 0) 
    {  
        fprintf(stderr, "Fail to open BAM file %s\n", bam);  
        return;  
    }  

    pRef = gzopen( ref, "r" );

        fprintf( stderr, "ref : %s\n", ref );
        fprintf( stderr, "pRef: %p\n", pRef );
    if( pRef == NULL )
    {
        fprintf( stderr, "ref : %s\n", ref );
        fprintf( stderr, "pRef: %p\n", pRef );

        return;
    }

    seq = kseq_init( pRef );

    b = bam_init1(); // alloc memory size of bam1_t
//fprintf( stderr, "%\pn", b );
    buf = bam_plbuf_init(pileup_func, &input); // alloc memory

    bam_plbuf_set_mask(buf, -1);
    
    while ((ret = samread( input.in, b)) >= 0)
    {   
        bam_plbuf_push(b, buf); 
        
//fprintf( stderr, "%x\n", b->core.flag );
        if( b->core.flag & 0x0004 ) // unmapped
        {    // do nothing
/*
            qname1 = strtok(bam1_qname(b), ":\t\n ");
            qname2 = strtok(NULL, ":\t\n ");
            qname3 = atoi(qname2);

            fprintf( stderr, "%s:%10d:%s:%d\t%c:%d:%d:%d\n", 
                qname1, qname3, "*", b->core.pos,
                '*', b->core.flag, b->core.qual, ret );
*/
            fprintf( stdout, "%s:%s:%d\t%c:0x%x:%d:%d\n", 
                bam1_qname(b), "*", b->core.pos+1,
                '*', b->core.flag, b->core.qual, ret );
/*
            fprintf( stderr, "%s:%s:%d\t%c:0x%x:%d:%d\n", 
                bam1_qname(b), "*", b->core.pos,
                '*', b->core.flag, b->core.qual, ret );
*/
        }
        else
        {
            // to find a base in the reference genome, seq.

            if( ( seq != NULL ) &&  
                ( strcmp( input.in->header->target_name[b->core.tid], chr ) == 0 ) )
            {
                // already found that 
                // fprintf( stderr, "found : %s\n", chr );
            }else
            {
                if( find_chr(input.in->header->target_name[b->core.tid], seq, chr) < 0 )
                {
                     fprintf( stderr, "ERROR : cannot find chromosome %s\n", \
                             input.in->header->target_name[b->core.tid] );
                }else
                {
                     fprintf( stderr, "FOUND CHR : %s\n", chr );
                }          
            }
            // remove not aligned to the chromosome

            fprintf( stdout, "%s:%s:%d\t%c:%d:%d:%d\n", 
                bam1_qname(b),
                input.in->header->target_name[b->core.tid], 
                b->core.pos+1,
                seq->seq.s[b->core.pos], b->core.flag, b->core.qual, ret );

/*
            fprintf( stderr, "%s:%s:%d\t%c:%d:%d:%d\n", 
                bam1_qname(b),
                input.in->header->target_name[b->core.tid], 
                b->core.pos,
                seq->seq.s[b->core.pos], b->core.flag, b->core.qual, ret );
*/
        }
    }

    // for the last bases...
  
//    printf("pos:%d(%c), flag:%d qual: %d(ret %d)\n", 
//           b->core.pos+1, seq->seq.s[b->core.pos], b->core.flag, b->core.qual, ret );

    bam_plbuf_push(0, buf); 

    bam_plbuf_destroy(buf); // release memory
    bam_destroy1(b);  // release memory size of bam1_t
     
    samclose(input.in);  
 
    kseq_destroy( seq );
    gzclose( pRef );

    return;  
}  

static int parse_args( int argc, char *argv[],
                char * ref,  
                int *length,
                int *start_base_pos, 
                char *bam )
{
    int ret = 0;
    char c;

    // all options need their value
    while( ( c = getopt( argc, argv, "l:q:s:i:d:o:t:f:b:x:m:p:" )) >= 0 )
    {
//        fprintf( stderr, "optind : %d\n", optind );
        switch(c)
        {
        case 'l': (*length) = atoi( optarg ); break;
        case 'q': break;
        case 's': break;
        case 'i': break;
        case 'd': break;
        case 'o': break;
        case 't': break;
        case 'f': strcpy( ref, optarg); break;
        case 'b': break;
        case 'x': break;
        case 'm': strcpy( bam, optarg); break;
        case 'p': break;
        default : fprintf( stderr, "%s:  WRONG OPTION \'%c\'\n", __FILE__, c );
                  ret = -1;
                  break;
        }
    }
    optind = 1;
    return ret;
}



int extractor( int argc, char *argv[] )
{

/*
    char *ref = argv[1];
    int length = atoi(argv[2]);
    int start_base_pos = atoi(argv[3]);
    char *bam = argv[4];
*/
    char ref[256] = { 0, };
    int length = 0;
    int start_base_pos = 1;
    char bam[256] = { 0, };

    parse_args( argc, argv, ref, &length, &start_base_pos, bam );

    fprintf( stderr, "extract only option\n" );
    fprintf( stderr, "ref: %s\n", ref );
    fprintf( stderr, "length: %d\n", length );
    fprintf( stderr, "start_base_pos: %d\n", start_base_pos );
    fprintf( stderr, "bam: %s\n", bam );
    fprintf( stderr, "=============================================\n" );
    
    mapper( ref, length, start_base_pos, bam );

    return EXIT_SUCCESS;
} 




