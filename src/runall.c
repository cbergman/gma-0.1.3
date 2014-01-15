#include <fcntl.h>
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <time.h>

#include "main_mapper.h"
#include "genfa.h"
#include "genfq.h"
#include "mapper.h"

#define FILE_OPT_O (O_WRONLY | O_CREAT)
#define FILE_OPT_S (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

#define FALSE 0 
#define TRUE  1

void timestamp()
{
    time_t ltime; /* calendar time */
    ltime=time(NULL); /* get current cal time */
    fprintf( stderr, "\n------------------------------------------\n" );
    fprintf( stderr, "[INFO] %s\n", asctime( localtime(&ltime) ) );
}

void run_exc_ls( char * dir )
{
    pid_t pid;
    int error;
    int status;

    pid = fork();
    if( pid == 0 )
    {   
        error = execl("/bin/ls", "ls", "-al", dir, (char *) 0);
        if( error < 0 )
        {
            fprintf( stderr, "ls - FAIL!\n\n");
        }else
        {
            fprintf( stderr, "ls - DONE!\n\n");
        }
        _exit(1);
    }
    else
    {
        wait(&status);
    }
}

void run_exc_cp( char *src, char *dst )
{
    pid_t pid;
    int error;
    int status;

    fprintf( stderr, "Copying %s to %s\n", src, dst);

    pid = fork();
    if( pid == 0 )
    {   // copy indices  
        error = execl("/bin/cp", "cp", src, dst ,(char *) 0);
        if( error < 0 )
        {
            fprintf( stderr, "cp - FAIL!\n\n");
        }else
        {
            fprintf( stderr, "cp - DONE!\n\n");
        }
        _exit(1);
    }
    else
    {
        wait(&status);
    }
}

void run_exc_rm( char *filepath )
{
    pid_t pid;
    int error;
    int status;

    fprintf( stderr, "remove %s.....\n", filepath );

    pid = fork();
    if( pid == 0 )
    {   // copy indices  
        error = execl("/bin/rm", "rm", filepath ,(char *) 0);
        if( error < 0 )
        {
            fprintf( stderr, "rm - FAIL!\n\n");
        }else
        {
            fprintf( stderr, "rm - DONE!\n\n");
        }
        _exit(1);
    }
    else
    {
        wait(&status);
    }
}

void run_exc_wc( char *filepath )
{
    pid_t pid;
    int error;
    int status;

    fprintf( stderr, "counting lines %s\n", filepath );

    pid = fork();
    if( pid == 0 )
    {   // output will be to stdout
        error = execl("/usr/bin/wc", "wc", "-l",  filepath ,(char *) 0);

        if( error < 0 )
        {
            fprintf( stderr, "wc - FAIL!\n\n");
        }else
        {
            fprintf( stderr, "wc - DONE!\n\n");
        }
        _exit(1);
    }
    else
    {
        wait(&status);
    }
}


static int parse_args( int argc, char *argv[], 
                int *length, char *qual, 
                float *error_sub_rate, float *error_ins_rate, float *error_del_rate, 
                int *range, int *threshold, char *fasta, char *index_path,
                int *hadoop, char *prog_path )
{
    int ret = 0;
    char c;

    // all options need their value
    while( ( c = getopt( argc, argv, "l:q:s:i:d:o:t:f:b:x:m:p:" )) >= 0 ) 
    {
        //fprintf( stderr, "optind : %d, %c, %s\n", optind, c, optarg );
        switch(c)
        {
        case 'l': (*length) = atoi( optarg ); break;
        case 'q': (*qual) = optarg[0]; break;
        case 's': (*error_sub_rate) = atof( optarg ); break;
        case 'i': (*error_ins_rate) = atof( optarg ); break;
        case 'd': (*error_del_rate) = atof( optarg ); break;
        case 'o': (*range) = atoi( optarg ); break;
        case 't': (*threshold) = atoi( optarg ); break;
        case 'f': strcpy( fasta, optarg ); break;
        case 'b': 
                  break;
        case 'x': strcpy( index_path, optarg ); break;
        case 'm': break;
        case 'p': (*hadoop) = TRUE; 
                  strcpy( prog_path, optarg );
                  break;
        default : fprintf( stderr, "%s: WRONG OPTION \'%c\'\n", __FILE__, c ); 
                  usage();
                  ret = -1;
                  break;
        }
    }
    optind = 1;
    return ret;
}


static int run_bwa( char *fasta, char *fastq_se, char *fastq_pe, int hadoop, char *bwa_path, int length ) //, int length  )
{
    pid_t pid;
    int fd = 0;
    int error = 0;
    int status = 0;

   

    if( length > 200 )
    {
        fprintf( stderr, "Since chosen read length is more than 200bp, bwasw algorithm will be used for BWA.\n" );

        timestamp();
    
        fprintf( stderr, "BWA - Generating SAM file for single end reads ......\n");
        fprintf( stderr, "[command] bwa bwasw %s %s > aln_se.sam\n", fasta, fastq_se );
        //run_exc_wc( fastq_se );
    
        pid = fork();
        if( pid == 0 )
        {   // child process   
            fd = open("aln_se.sam", FILE_OPT_O, FILE_OPT_S );
            dup2(fd, STDOUT_FILENO);
            close(fd);

            if( hadoop == TRUE )
            {
                error = execl( bwa_path, "bwa", "bwasw", fasta, fastq_se, (char *) 0);
            }
            else
            {
                error = execl("../../../../../bin/bwa", "bwa", "bwasw", fasta, fastq_se, (char *) 0);     
            }

            if( error < 0 )
            {
                fprintf( stderr, "BWA bwasw - FAIL!\n\n");
            }
            else
            {
                fprintf( stderr, "BWA bwasw - DONE!\n\n");
            }
            _exit(1);
        }
        else
        {   // parent process
            wait(&status);
        }
    }
    else
    {
        //run_exc_wc( fastq_se );
        timestamp();
        fprintf( stderr, "BWA - Aligning for single-end reads................\n");
        fprintf( stderr, "[command] bwa aln %s %s > aln_se.sai\n", fasta, fastq_se );
          
        pid = fork();
        if( pid == 0 )
        {   // child process   
            fd = open("aln_se.sai", FILE_OPT_O, FILE_OPT_S ); 
    
            dup2(fd, STDOUT_FILENO);
            close(fd);
    
            if( hadoop == TRUE )
            {
                error = execl( bwa_path, "bwa", "aln", fasta, fastq_se, (char *) 0);
            }
            else
            {
                error = execl("../../../../../bin/bwa", "bwa", "aln", fasta, fastq_se, (char *) 0);
            }

            if( error < 0 )
            {
                fprintf( stderr, "BWA aln - fails\n\n");
            }else
            {
                fprintf( stderr, "BWA aln - DONE!\n\n");
            }
            _exit(1);
        }
        else
        {   // parent process
          wait(&status);
        }

        timestamp();
    
        fprintf( stderr, "BWA - Generating SAM file for single end reads ......\n");
        fprintf( stderr, "[command] bwa samse %s aln_se.sai %s > aln_se.sam\n", fasta, fastq_se );
    
        pid = fork();
        if( pid == 0 )
        {   // child process   
            fd = open("aln_se.sam", FILE_OPT_O, FILE_OPT_S );
    
            dup2(fd, STDOUT_FILENO);
    
            close(fd);
            // bwa samse
            if( hadoop == TRUE )
            {
                error = execl( bwa_path, "bwa", "samse", fasta, "aln_se.sai", fastq_se, (char *) 0);
            }
            else
            {
                error = execl("../../../../../bin/bwa", "bwa", "samse", fasta, "aln_se.sai", fastq_se, (char *) 0);     
            }
    
            if( error < 0 )
            {
                fprintf( stderr, "BWA samse - FAIL!\n\n");
            }else
            {
                fprintf( stderr, "BWA samse - DONE!\n\n");
            }
            _exit(1);
        }
        else
        {   // parent process
            wait(&status);
        }
    }

    //////////////////////////////////////////////////////////////////
    // pair-end
    //////////////////////////////////////////////////////////////////
   
    if( fastq_pe == NULL )
    {
        // do nothing
    }
    else  // for pair-end, 1 more fastq and sai are needed
    { 
        timestamp();
        fprintf( stderr, "BWA - Aligning for paired-end reads with %s ...\n", fastq_pe );
        fprintf( stderr, "[command] bwa aln %s %s > aln_pe.sai\n", fasta, fastq_pe );
        //run_exc_wc( fastq_se );

        pid = fork();
        if( pid == 0 )
        {   // child process   
            fd = open("aln_pe.sai", FILE_OPT_O, FILE_OPT_S );
    
            dup2(fd, STDOUT_FILENO);
    
            close(fd);
#ifdef NERSC
            error = execl("./bwa", "bwa", "aln", fasta, fastq_pe, (char *) 0);
#else
        if( hadoop == TRUE )
        {
            error = execl( bwa_path, "bwa", "aln", fasta, fastq_pe, (char *) 0);
        }
        else
        {
            error = execl("../../../../../bin/bwa", "bwa", "aln", fasta, fastq_pe, (char *) 0);
        }
#endif
            if( error < 0 )
            {
                fprintf( stderr, "BWA aln - FAIL!\n\n");
            }else
            {
                fprintf( stderr, "BWA aln - DONE!\n\n");
            }
            _exit(1);
        }
        else
        {   // parent process
            wait(&status);
        }
    
        timestamp();
        fprintf( stderr, "BWA - Generating SAM file for pair end reads.........\n");
        fprintf( stderr, "[command] bwa sampe %s aln_se.sai aln_pe.sai %s %s > aln_pe.sam\n", fasta, fastq_se, fastq_pe );

        pid = fork();
        if( pid == 0 )
        {   // child process   
            fd = open("aln_pe.sam", FILE_OPT_O, FILE_OPT_S );
    
            dup2(fd, STDOUT_FILENO);
    
            close(fd);
#ifdef NERSC
            error = execl("./bwa", "bwa", "sampe", fasta, "aln_se.sai", "aln_pe.sai", fastq_se, fastq_pe, (char *) 0);
#else
            if( hadoop == TRUE )
            {
                error = execl( bwa_path, "bwa", "sampe", fasta, "aln_se.sai", "aln_pe.sai", fastq_se, fastq_pe, (char *) 0);
            }
            else
            {
                error = execl("../../../../../bin/bwa", "bwa", "sampe", fasta, "aln_se.sai", "aln_pe.sai", fastq_se, fastq_pe, (char *) 0);
            }
#endif
            if( error < 0 )
            {
                fprintf( stderr, "BWA sampe - FAIL!\n\n");
            }else
            {
                fprintf( stderr, "BWA sampe - DONE!\n\n");
            }
            _exit(1);
        }
        else
        {   // parent process
            wait(&status);
        }
    }   
    return error; 
}


static int run_samtools( char *fasta, const char *aln_sam, 
                                      const char *aln_bam,
                                      const char *aln_sort, 
                                      const int   hadoop, 
                                      const char *samtools_path )
{
    char fai[64] = { 0, };
    pid_t pid;
    int status = 0;
    int error = 0;

    //////////////////////////////////////////////////////////////////
    timestamp();
    fprintf( stderr, "Samtools is indexing reference genome................\n");
    fprintf( stderr, "[command] samtools faidx %s\n", fasta );

    pid = fork();
    if( pid == 0 )
    {   // child process 
#ifdef NERSC
        error = execl("./samtools", "samtools", "faidx", fasta, (char *) 0);
#else  
        if( hadoop == TRUE )
        {
            error = execl( samtools_path, "samtools", "faidx", fasta, (char *) 0);
        }
        else
        {
            error = execl("../../../../../bin/samtools", "samtools", "faidx", fasta, (char *) 0);
        }
#endif
        if( error < 0 )
        {
            fprintf( stderr, "sam faidx - FAIL!\n\n");
        }else
        {
            fprintf( stderr, "sam faidx - DONE!\n\n");
        }
        _exit(1);
    }
    else
    {   // parent process
        wait(&status);
    }

    //////////////////////////////////////////////////////////////////
    timestamp();

    strcpy( fai, fasta );
       
    strcat( fai, ".fai" );
    fprintf( stderr, "Samtools is converting SAM to BAM \n");
    fprintf( stderr, "[command] samtools import %s %s %s\n", fasta, aln_sam, aln_bam );

    pid = fork();
    if( pid == 0 )
    {   // child process   
#ifdef NERSC
        error = execl("./samtools", "samtools", "import", fai, aln_sam, aln_bam, (char *) 0);
#else
        if( hadoop == TRUE )
        {
            error = execl( samtools_path, "samtools", "import", fai, aln_sam, aln_bam, (char *) 0);
        }
        else
        {
            error = execl("../../../../../bin/samtools", "samtools", "import", fai, aln_sam, aln_bam, (char *) 0); 
        } 
#endif
        if( error < 0 )
        {
            fprintf( stderr, "sam import - FAIL!\n\n");
        }else
        {
            fprintf( stderr, "sam import - DONE!\n\n");
        }
        _exit(1);
    }
    else
    {   // parent process
        wait(&status);
    }


    
    fprintf( stderr, "\nSamtools is sorting BAM .......\n");
    fprintf( stderr, "[command] samtools sort %s %s\n", aln_bam, aln_sort );
    pid = fork();
    if( pid == 0 )
    {   // child process   
#ifdef NERSC
        error = execl("./samtools", "samtools", "sort", aln_bam, aln_sort,  (char *) 0);
#else
        if( hadoop == TRUE )
        {
            error = execl( samtools_path, "samtools", "sort", aln_bam, aln_sort, (char *) 0);
        }
        else
        {
            error = execl("../../../../../bin/samtools", "samtools", "sort", aln_bam, aln_sort, (char *) 0);
        }
#endif
        if( error < 0 )
        {
            fprintf( stderr, "sam sort - fails\n");
        }else
        {
            fprintf( stderr, "sam sort - DONE!\n\n");
        }
        _exit(1);
    }
    else
    {   // parent process
        wait(&status);
    }
    return error;
}

//#define ARGC 13

int runall(int argc, char *argv[])  
{  
    char genfqse[] = "genfqse";
    char genfqpe[] = "genfqpe";
    char fastq_se[64] = { 0, };
    char fastq_pe[64] = { 0, };
    char fasta[64] = { 0, };
    char index_path[256] = { 0, };
    char index_file[256] = { 0, };
    char prog_path[256] = { 0, };
    char bwa_path[256] = { 0, };
    char samtools_path[256] = { 0, };

    const char aln_se_sam[] = "aln_se.sam";
    const char aln_se_bam[] = "aln_se.bam";
    const char aln_se_sort[] = "aln_se.sort";
    const char aln_se_sort_bam[] = "aln_se.sort.bam";
    const char aln_pe_sam[] = "aln_pe.sam";
    const char aln_pe_bam[] = "aln_pe.bam";
    const char aln_pe_sort[] = "aln_pe.sort";
    const char aln_pe_sort_bam[] = "aln_pe.sort.bam";

    int length = 0, ob_range = 0, threshold = 0;
    float error_sub_rate = 0.0;
    float error_ins_rate = 0.0;
    float error_del_rate = 0.0;
    int hadoop = FALSE;
    int ib_range = 0;
    char qual;

    int start_base_pos = 1;

    /////////////////////////////////////////////////////////////////
    // Verify and parse arguments 
    /////////////////////////////////////////////////////////////////

    //fprintf( stderr, "-------------------------------------------\n");

    
    parse_args( argc, argv, 
                &length, &qual, &error_sub_rate, &error_ins_rate, &error_del_rate, 
                &ob_range, &threshold, fasta, index_path, &hadoop, prog_path );

    //fprintf( stderr, "-------------------------------------------\n");
    ib_range = ob_range - 2 * length ;

    if ( ( ob_range > 0 ) && ( ib_range < 0 ) )
    {
        fprintf( stderr, "[ERROR:002] expected distance is %d and read length is %d. Two reads are overlapped with %d base.\n\n", ob_range, length, ib_range*(-1) );
        return EXIT_FAILURE;
    }

    fprintf( stderr, "read length : %d\n", length );
    fprintf( stderr, "average quality value : %c\n", qual );
    fprintf( stderr, "error rate(substitution) : %.4f\n", error_sub_rate );
    fprintf( stderr, "error rate(insertion) : %.4f\n", error_ins_rate );
    fprintf( stderr, "error rate(deletion) : %.4f\n", error_del_rate );
    fprintf( stderr, "library distance : %d\n", ob_range );
    fprintf( stderr, "threshold : %d\n", threshold );
    fprintf( stderr, "fasta file : %s\n", fasta );
    fprintf( stderr, "index path : %s\n", index_path );
    fprintf( stderr, "is hadoop : %d\n", hadoop );
    fprintf( stderr, "program path : %s\n", prog_path );
    fprintf( stderr, "-------------------------------------------\n");

    /////////////////////////////////////////////////////////////////
    // Generate FASTA
    /////////////////////////////////////////////////////////////////
 

    timestamp();

    fprintf( stderr, "FASTA file is being generated.\n");

    start_base_pos = genfa(argc, argv ); //, atoi(argv[2]));

    if( start_base_pos < 0 )
    {
        fprintf( stderr, "FAIL to generate fasta file.\n");
        return EXIT_FAILURE;
    }

    //run_exc_cp( "ref.fa", "/local2/work/hlee/gma_test/local_fly_pair/" );
    fprintf( stderr, "start position is %d.\n", start_base_pos );
    fprintf( stderr, "-------------------------------------------\n");

    /////////////////////////////////////////////////////////////////
    // Generate FASTQ
    /////////////////////////////////////////////////////////////////
    timestamp();
    
    fprintf( stderr, "FASTQ files are being generated.\n");

    argv[0] = genfqse;
    genfq( argc, argv );
    get_output_name( fastq_se );
    fprintf( stderr, "\t%s is generated. DONE!\n\n", fastq_se );

    if( ib_range > 0 )
    {
         argv[0] = genfqpe;
         genfq( argc, argv );
         get_output_name( fastq_pe );
         fprintf( stderr, "\t%s is generated. DONE!\n\n", fastq_pe );
         fprintf( stderr, "Pair-end alignment: outbound range %d\n", ob_range );
    }
    else
    { 
         fprintf( stderr, "Single-end alignment\n" );
    }

    /////////////////////////////////////////////////////////////////
    // Copying pre-calculated index for entire human genome
    /////////////////////////////////////////////////////////////////

    fprintf( stderr, "\n");
    fprintf( stderr, "-------------------------------------------\n");
#ifdef NERSC
    fprintf( stderr, "Assuming index has been generated in /global/u2/h/hlee/index/hg19/\n");
#else
    fprintf( stderr, "Assuming index has been generated in %s\n", index_path );
#endif

    fprintf( stderr, "\n" );
    fprintf( stderr, "Copying pre-generated index to current directory.....\n");

#ifdef NERSC
/*
    run_exc_cp( "/global/u2/h/hlee/index/hg19/hg19.fa", "ref.fa" );
    run_exc_cp( "/global/u2/h/hlee/index/hg19/hg19.fa.ann", "ref.fa.ann" );
    run_exc_cp( "/global/u2/h/hlee/index/hg19/hg19.fa.amb", "ref.fa.amb" );
    run_exc_cp( "/global/u2/h/hlee/index/hg19/hg19.fa.bwt", "ref.fa.bwt" );
    run_exc_cp( "/global/u2/h/hlee/index/hg19/hg19.fa.pac", "ref.fa.pac" );
    run_exc_cp( "/global/u2/h/hlee/index/hg19/hg19.fa.rbwt", "ref.fa.rbwt" );
    run_exc_cp( "/global/u2/h/hlee/index/hg19/hg19.fa.rpac", "ref.fa.rpac" );
    run_exc_cp( "/global/u2/h/hlee/index/hg19/hg19.fa.sa", "ref.fa.sa" );
    run_exc_cp( "/global/u2/h/hlee/index/hg19/hg19.fa.rsa", "ref.fa.rsa" );
*/
#else
    run_exc_cp( index_path, "ref.fa" );

    sprintf( index_file, "%s.%s", index_path, "sa" );
    run_exc_cp( index_file, "ref.fa.sa" );

    sprintf( index_file, "%s.%s", index_path, "ann" );
    run_exc_cp( index_file, "ref.fa.ann" );
    sprintf( index_file, "%s.%s", index_path, "amb" );
    run_exc_cp( index_file, "ref.fa.amb" );
    sprintf( index_file, "%s.%s", index_path, "bwt" );
    run_exc_cp( index_file, "ref.fa.bwt" );
    sprintf( index_file, "%s.%s", index_path, "pac" );
    run_exc_cp( index_file, "ref.fa.pac" );
    sprintf( index_file, "%s.%s", index_path, "rsa" );
    run_exc_cp( index_file, "ref.fa.rsa" );
    sprintf( index_file, "%s.%s", index_path, "rbwt" );
    run_exc_cp( index_file, "ref.fa.rbwt" );
    sprintf( index_file, "%s.%s", index_path, "rpac" );
    run_exc_cp( index_file, "ref.fa.rpac" );

#endif
    /////////////////////////////////////////////////////////////////
    fprintf( stderr, "\n");
//    fprintf( stderr, "ls -al .\n");
//    run_exc_ls( "." );

    ////////////////////////////////////////////////////////////////// 
    // BWA
    ////////////////////////////////////////////////////////////////// 
    if( hadoop == TRUE )
    {   
        sprintf( bwa_path, "%s/bwa", prog_path );
        fprintf( stderr, "bwa path : %s\n\n", bwa_path );
    }

    if( ( length >=200 ) && ( ib_range > 0 ) )
    {
        fprintf( stderr, "[ERROR:001] read length is %d. Long read(>=200) cannot be combined with paired alignment option\n", length );    
        return EXIT_FAILURE; 
    } 
    else if( ib_range > 0 )
    {
        run_bwa( fasta, fastq_se, fastq_pe, hadoop, bwa_path, length );
    }
    else
    {
        run_bwa( fasta, fastq_se, NULL, hadoop, bwa_path, length );
    }

    //////////////////////////////////////////////////////////////////
    // SAMTOOLS 
    ////////////////////////////////////////////////////////////////// 
    if( hadoop == TRUE )
    {   
        sprintf( samtools_path, "%s/samtools", prog_path );
        fprintf( stderr, "samtools path : %s\n", samtools_path );
    }
    
    if( ib_range > 0 )
    {   
        run_samtools( fasta, aln_pe_sam, aln_pe_bam, aln_pe_sort, hadoop, samtools_path );
    }
    else
    {
        run_samtools( fasta, aln_se_sam, aln_se_bam, aln_se_sort, hadoop, samtools_path );
    }

    //////////////////////////////////////////////////////////////////
    // Extract info from sam file
    //////////////////////////////////////////////////////////////////

    timestamp();

    fprintf( stderr, "Generating mapper output %d, %s\n", length, fasta );

    if( ib_range > 0 ) 
    {
        mapper( fasta, length, start_base_pos, aln_pe_sort_bam );
    }
    else
    {
        mapper( fasta, length, start_base_pos, aln_se_sort_bam );
    }

    fprintf( stderr, "=====================================\n");
    fprintf( stderr, "Complete Mapper!\n");
    fprintf( stderr, "=====================================\n");
 
    //////////////////////////////////////////////////////////////////
    // CLEANUP 
    ////////////////////////////////////////////////////////////////// 

    //run_exc_ls( "." );
//    run_exc_cp( "ref.fa.p-100-A-0.00.fq", "/bluearc/data/schatz/hlee/hdwork/" );
//    run_exc_cp( "ref.fa.s-100-A-0.00.fq", "/bluearc/data/schatz/hlee/hdwork/" );
//    run_exc_cp( "aln_pe.bam", "/bluearc/data/schatz/hlee/hdwork/" );
//    run_exc_cp( "aln_pe.sort.bam", "/bluearc/data/schatz/hlee/hdwork/" );


/*
    run_exc_rm( "ref.fa" );
    run_exc_rm( "ref.fa.ann" );
    run_exc_rm( "ref.fa.amb" );
    run_exc_rm( "ref.fa.bwt" );
    run_exc_rm( "ref.fa.pac" );
    run_exc_rm( "ref.fa.rbwt" );
    run_exc_rm( "ref.fa.rpac" );
    run_exc_rm( "ref.fa.sa" );
    run_exc_rm( "ref.fa.rsa" );
*/
    return EXIT_SUCCESS;  
}  



