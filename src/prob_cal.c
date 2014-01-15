#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define READ1 0
#define READ2 1

#define FST   0
#define SND   1

typedef struct _prob_base
{
    // store raw information
    char base;
    int  pos;
    int  maq[2][2];
    int  flag[2][2];
    // calculate through window list
    int  total;
    int  good;
    int  bad;
    int  unmapped;
    double qual;
    struct _prob_base * next;

} prob_base;

typedef struct _prob_window
{
    prob_base * head;
    prob_base * tail;
    int length;
    int prev_pos;
    int next_pos;
} prob_window;

#ifndef min
    #define min( a, b ) ( ((a) < (b)) ? (a) : (b) ) 
#endif
#ifndef max
    #define max( a, b ) ( ((a) > (b)) ? (a) : (b) ) 
#endif




prob_window pw;


void init_prob_base( prob_base * pb )
{
    pb->base = '*';
    pb->pos = 0;
    pb->maq[READ1][FST] = -1;
    pb->maq[READ1][SND] = -1;
    pb->maq[READ2][FST] = -1;
    pb->maq[READ2][SND] = -1;
    pb->flag[READ1][FST] = -1;
    pb->flag[READ1][SND] = -1;
    pb->flag[READ2][FST] = -1;
    pb->flag[READ2][SND] = -1;

    // calculate through window list
    pb->total = 0;
    pb->good = 0;
    pb->bad = 0;
    pb->unmapped = 0;
    pb->qual = 0.0;
}


void init_prob_window( int length )
{
    int i = 0;
    prob_base * tmp;

    pw.length = length;
    pw.prev_pos = 0; // the position that we expect
     
    pw.head = malloc( sizeof( prob_base ) );
    memset( pw.head, 0, sizeof( prob_base ) );
    init_prob_base( pw.head );

    tmp = pw.head;

    for( i = 1; i < length; i++ )
    {
        tmp->next = malloc( sizeof( prob_base ) );
        tmp = tmp->next;
        tmp->next = NULL;

        init_prob_base( tmp );
    }    

    pw.tail = tmp;

//fprintf( stderr, "length : %d\n", length );

//    printf("[head] %p\n", pw.head );
//    printf("[tail] %p\n", pw.tail );

//    tmp = pw.head;

//    for( i = 0; i < length; i++ )
//    {
//        printf("[%d] %p->%p\n", i, tmp, tmp->next );
//        tmp = tmp->next;
//    }

}


void destroy_prob_window( )
{
    int i = 0;
    prob_base * tmp2 = pw.head;
    prob_base * tmp1 = pw.head;

    for( i = 0; i < pw.length; i++ )
    {
        free( tmp2 );
        tmp2 = tmp1->next;
        tmp1 = tmp1->next;
    }
   
    pw.length = 0;
    pw.head = NULL;
    pw.tail = NULL; 
}


void min_maq( prob_base * head, int read, int *qual, int *flag )
{
    int maq = -1;
    int flg = -1;

    
    if( head->maq[read][FST] > -1 ) 
    {
        if( head->maq[read][SND] > -1 ) 
        {
            maq = min( head->maq[read][FST],  head->maq[read][SND] ); // [N,N]

            if( maq == head->maq[read][FST] ) 
            {
                flg = head->flag[read][FST];
            }
            else
            {
                flg = head->flag[read][SND];
            }
        }
        else
        {
            maq = head->maq[read][FST];   // [N,-1]
            flg = head->flag[read][FST];   // [N,-1]
        }
    }
    else
    {
        if( head->maq[read][SND] > -1 ) 
        {
            maq = head->maq[read][SND];  // [-1,N]
            flg = head->flag[read][SND];   // [N,-1]
        }
        else
        {
            flg = head->flag[read][FST];   // [N,-1]
            // [-1,-1]
        }
    }

    (*qual) = maq;
    (*flag) = flg;
}
     


// void circulate_window_list( prob_base * head, int flag, int qual, int threshold )
void circulate_window_list( prob_base * head, int read, int threshold )
{
    int i = 0;
    int qual = 0;
    int flag = 0;

    double p_err = 0.0;
    
    prob_base * tmp = head;

    min_maq( head, read, &qual, &flag );

    //fprintf( stdout, "min_maq : %d\n", qual );

    if( qual <= -1 )
    {
        p_err = pow( (double)0.1, (0/10.0) );
    }
    else 
    {
        p_err = pow( (double)0.1, (qual/10.0) );

        for( i = 0; i < pw.length; i++ )
        {
            if( qual >= 0 )
            {
                if( qual >= threshold ) tmp->good++; else tmp->bad++;

                if( flag & 0x0004 ) 
                {
                    tmp->unmapped++;
                }
       

                tmp->total++;        

                tmp->qual += 1.0 - p_err;
            }
            tmp = tmp->next;
        }
    }
}



void compute_gms( prob_base * head, int threshold )
{
    circulate_window_list( head, READ1, threshold);
    circulate_window_list( head, READ2, threshold);
}






void fill_qual_flag( prob_base *pb, int read, int num, int qual, int flag )
{

//    fprintf( stderr, "flag : 0x%x\n", flag );

    if( ( qual == -1 ) && ( flag == 0 ) )
    {
        pb->maq[read][num] = -1; 
        pb->flag[read][num] = 0; 
    }
    else if( qual == -1 ) 
    {
        pb->maq[read][num] = 0; 
        pb->flag[read][num] = flag; 
    }
    else 
    {
        pb->maq[read][num] = qual;
        pb->flag[read][num] = flag; 
    }
}

void move_next_prob_base( int pos )
{
    pw.tail->next = pw.head;
    pw.tail = pw.head;
    pw.head = pw.head->next; 
    memset( pw.tail, 0, sizeof( prob_base ) );
    init_prob_base( pw.tail );

    pw.prev_pos = pos;
}

void print_prob_base( char *chr, prob_base * pb, char *mark)
{
    if( pb->total == 0 )
    {
        fprintf( stdout, "%s\t%09d\t%c\t(%d,%d)(%d,%d)\t%d = %d + %d(%d)\t%.4f\n", 
                chr, pb->pos, pb->base, pb->maq[READ1][FST], 
                pb->maq[READ1][SND], pb->maq[READ2][FST], pb->maq[READ2][SND],
                pb->total, pb->good, pb->bad, pb->unmapped, 
                //pb->qual, 
                0.0000 );
    }
    else
    {
        fprintf( stdout, "%s\t%09d\t%c\t(%d,%d)(%d,%d)\t%d = %d + %d(%d)\t%.4f\n", 
                 chr, pb->pos, pb->base, pb->maq[READ1][FST], 
                 pb->maq[READ1][SND], pb->maq[READ2][FST], pb->maq[READ2][SND],
                 pb->total, pb->good, pb->bad, pb->unmapped, 
                 //pb->qual, 
                 (pb->qual)*(100.00/pb->total) );
    }
}

void fill_prob_base( prob_base * pb, int pos, char base, int read, int qual, int flag  )
{
    //fprintf( stdout, "[fill_prob_base]  (filled: %d, pq.head->pos: %d, pb->pos: %d, qual: %d, flqg: %d )\n", pos, pw.head->pos, pb->pos, qual, flag ); 

    pb->pos = pos; 
    pb->base = base; 

    if( ( flag & 0x0040 ) || ( flag & 0x0080 ) )
    {
        //pb->base = base; 

        if( read == READ1 ) 
        {
            if( pb->maq[READ1][FST] == -1 )
            { 
                pb->flag[READ1][FST] = flag;
                fill_qual_flag( pb, READ1, FST, qual, flag );
            }
            else
            {
                pb->flag[READ1][SND] = flag;
                fill_qual_flag( pb, READ1, SND, qual, flag );
            }
        }
        else if( read == READ2 ) 
        {
            if( pb->maq[READ2][FST] == -1 )

            { 
                pb->flag[READ2][FST] = flag;
                fill_qual_flag( pb, READ2, FST, qual, flag );
            }
            else
            {
                pb->flag[READ2][SND] = flag;
                fill_qual_flag( pb, READ2, SND, qual, flag );
            }
        }
        else
        {  
            fprintf( stderr, "[ERROR:005] check_fill_prob_base: read not first, not second\n" );
        }
    }
    else  // if flag == 0, it's single end read or cannot  know which read if comes from if paired-end read
    {
        //fprintf( stdout, "[fill_prob_base]  (filled: %d, pq.head->pos: %d, pb->pos: %d, qual: %d, flqg: %d )\n", pos, pw.head->pos, pb->pos, qual, flag ); 

        if( ( pb->flag[READ1][FST] == -1 ) || ( pw.prev_pos == pos ) )
        { 
            pb->flag[READ1][FST] = flag;
            fill_qual_flag( pb, READ1, FST,  qual, flag );
        }
        else if( pb->flag[READ2][FST] == -1 ) 
        {
            pb->flag[READ2][FST] = flag;
            fill_qual_flag( pb, READ2, FST, qual, flag );
        }
        else if( pb->flag[READ1][SND] == -1 )  
        { 
            pb->flag[READ1][SND] = flag;
            fill_qual_flag( pb, READ1, SND,  qual, flag );
        }
        else if( pb->flag[READ2][SND] == -1 ) 
        {
            pb->flag[READ2][SND] = flag;
            fill_qual_flag( pb, READ2, SND, qual, flag );
        }
        else
        {
            fprintf( stderr, "[ERROR:004] check_fill_prob_base: flag: not first, not second\n" );
        }
    }
}

void check_fill_prob_base( prob_base * pb, char *chr, int pos, char base, int read, int qual, int flag  )
{

    //fprintf( stdout, "pw.prev_pos : %d\n", pw.prev_pos );

    if( ( ( pos - pw.prev_pos ) >= 2 ) ||
        ( ( ( pos - pb->pos )     == 1 )  && ( pos != 1 ) ) )
    {
        print_prob_base( chr, pb, "f" );
        move_next_prob_base( pos );
    }

    fill_prob_base( pw.head, pos, base, read, qual, flag );
}


/*
void increase_count_prob_base( prob_base * pb )
{
    pb->count++;
}
*/

int fill_window( FILE * output, int range, char *chr, int pos, char base, int flag, int qual, int threshold )
{
    //fprintf( stdout, "[INFO][fill_window] chr: %s, pos:%d, base: %c, flag: 0x%x qual: %d\n", chr, pos, base, flag, qual );
/*
    fprintf( stderr, "[INFO] maq(%d, %d | %d, %d), flaq(%d, %d | %d, %d) -> ", 
                              pw.head->maq[READ1][FST], 
                              pw.head->maq[READ1][SND], 
                              pw.head->maq[READ2][FST], 
                              pw.head->maq[READ2][SND], 
                              pw.head->flag[READ1][FST], 
                              pw.head->flag[READ1][SND], 
                              pw.head->flag[READ2][FST],
                              pw.head->flag[READ2][SND] );
*/
    if( ( flag & 0x0040 ) && ( pw.head->maq[READ1][FST] == -1 ) ) // the first read1
    {
        check_fill_prob_base( pw.head, chr, pos, base, READ1, qual, flag );
    }
    else if( (flag & 0x0040) && ( pw.head->maq[READ1][FST] != -1 ) ) // the second read1
    {
        check_fill_prob_base( pw.head, chr, pos, base, READ1, qual, flag );
    }
    else if( ( flag & 0x0080 ) && ( pw.head->maq[READ2][FST] == -1 ) ) // the first read2
    {
        check_fill_prob_base( pw.head, chr, pos, base, READ2, qual, flag );
    }
    else if( (flag & 0x0080) && ( pw.head->maq[READ2][FST] != -1 ) ) // the second read2
    {
        check_fill_prob_base( pw.head, chr, pos, base, READ2, qual, flag );
    }
    else if( flag == 0 ) // single-end read
    {
        //fprintf( stderr, "pos:%d(%c), flag:0x%x qual: %d\n",  pos, base, flag, qual);
        check_fill_prob_base( pw.head, chr, pos, base, -1, qual, flag );
    }
    else if( flag & 0x10 ) // single-end read
    {
        check_fill_prob_base( pw.head, chr, pos, base, -1, qual, flag );
//        fprintf( stderr, "pos:%d(%c), flag:0x%x qual: %d\n",  pos, base, flag, qual);
    }
    else if( ( pos - pw.head->pos ) == 1 )
    {
        
        fprintf( stderr, "[ERROR] here\n" );
        check_fill_prob_base( pw.head, chr, pos, base, -1, qual, flag );  //flag
    }
    else
    {
        fprintf( stderr, "[ERROR:006] move_window: not first, not second\n" );
        fprintf( stderr, "[ERROR:007] (pos: %d, range: %d, flag: 0x%x, qual: %d, pw.prev_pos: %d, maq(%d, %d | %d, %d), flaq(%d, %d | %d, %d)\n", 
                          pos, range, flag, qual, pw.prev_pos, 
                          pw.head->maq[READ1][FST], 
                          pw.head->maq[READ1][SND], 
                          pw.head->maq[READ2][FST], 
                          pw.head->maq[READ2][SND], 
                          pw.head->flag[READ1][FST], 
                          pw.head->flag[READ1][SND], 
                          pw.head->flag[READ2][FST],
                          pw.head->flag[READ2][SND] );
    }

/*
    fprintf( stderr, "[INFO] maq(%d, %d | %d, %d), flaq(%d, %d | %d, %d)\n", 
                              pw.head->maq[READ1][FST], 
                              pw.head->maq[READ1][SND], 
                              pw.head->maq[READ2][FST], 
                              pw.head->maq[READ2][SND], 
                              pw.head->flag[READ1][FST], 
                              pw.head->flag[READ1][SND], 
                              pw.head->flag[READ2][FST],
                              pw.head->flag[READ2][SND] );
*/
    return 0;
}


    
int move_window( FILE * output, int range, char *chr, int pos, char base, int flag, int qual, int threshold )
{
    //fprintf( stdout, "[INFO][move_window] chr: %s, pos:%d, base: %c, flag: 0x%x qual: %d\n", chr, pos, base, flag, qual );

    compute_gms( pw.head, threshold );
    print_prob_base( chr, pw.head, "1" );

    move_next_prob_base( pos );

    fill_window( output, range, chr, pos, base, flag, qual, threshold );

    return 0;
}

void clean_windows( FILE * output, char * org_chr, int org_pos, int length, int threshold )
{
    int i = 0;
    
    //fprintf( stderr, "(%d, %d)\n", org_pos, pw.head->pos );

    compute_gms( pw.head, threshold );
    print_prob_base( org_chr, pw.head, "1" );
    move_next_prob_base( org_pos );
    org_pos++;

    for( i = 0; i < (length-1); i++ )
    {
        pw.head->pos = org_pos++;

        print_prob_base( org_chr, pw.head, "c" );
        move_next_prob_base( org_pos );
    }
}


