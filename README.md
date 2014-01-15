Introduction
------------

Name
:   GMA - Genome Mappability Analyzer

Latest Version
:   The latest version is 0.1.3
:   You can download gma-0.1.3.tar.gz on [download
    page](https://sourceforge.net/projects/gma-bio/files/).

System Requirement
:   GMA is developed under Linux environment. Details are described
    below, and thus it will work in similar environments that we
    support. If you're Windows user, you may need
    [cygwin](http://www.cygwin.com/).

<!-- -->

    $ uname -a
    Linux mshadoop1.cshl.edu 2.6.32-220.el6.x86_64 #1 SMP Tue Dec 6 19:48:22 GMT 2011 x86_64 x86_64 x86_64 GNU/Linux

GMS(Genome Mappability Score) File Format
-----------------------------------------

GMA outputs ".gms" extension files, which consists of 6 columns. Detail
description of each column is as follows:

  ------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  chr     | This shows chromosomes if it is species that have multi chromosomes. Otherwise it means the species.
  pos     | Position. Note that it begins with "1" not "0".
  base    | Base from aligning results. Note that if it's not a,c,g,t/A,C,G,T, then the alignment tool failed to map reads to this position. In this case, "\*" will be shown.
  maq     | By design, each position can have 4 mapping quality scores at maximum. All slots are initialized with "-1". If any mapping quality scores are assigned, non-negative numbers should be shown.
  total   | This will give you rough idea of GMS. Total means how many reads are mapped to the position. The first number after "=" means the number of reads more than threshold that a user can set as a GMA parameters. Thus they are considered reliable reads. However the second number means the number of reads that is less than threshold so they are unreliable reads. Based on such information guide you rough GMS.
  GMS     | This is the score that shows how reliably each base of the genome can be mapped.
  ------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

This is a snippet of *E.coli* gms file.

***E.coli***

    #chr            pos             base      maq          total = >= + <(um)   GMS
    NC_000913       000000001       A       (60,-1)(-1,-1)  1 = 1 + 0(0)    99.9999
    NC_000913       000000002       G       (60,-1)(-1,-1)  2 = 2 + 0(0)    99.9999
    NC_000913       000000003       C       (60,-1)(-1,-1)  3 = 3 + 0(0)    99.9999
    NC_000913       000000004       T       (60,-1)(-1,-1)  4 = 4 + 0(0)    99.9999
    NC_000913       000000005       T       (60,-1)(-1,-1)  5 = 5 + 0(0)    99.9999
    NC_000913       000000006       T       (60,-1)(-1,-1)  6 = 6 + 0(0)    99.9999
    NC_000913       000000007       T       (60,-1)(-1,-1)  7 = 7 + 0(0)    99.9999
    NC_000913       000000008       C       (60,-1)(-1,-1)  8 = 8 + 0(0)    99.9999
    NC_000913       000000009       A       (60,-1)(-1,-1)  9 = 9 + 0(0)    99.9999
    NC_000913       000000010       T       (60,-1)(-1,-1)  10 = 10 + 0(0)  99.9999
    NC_000913       000000011       T       (60,-1)(-1,-1)  11 = 11 + 0(0)  99.9999
    NC_000913       000000012       C       (60,-1)(-1,-1)  12 = 12 + 0(0)  99.9999
    NC_000913       000000013       T       (60,-1)(-1,-1)  13 = 13 + 0(0)  99.9999
    NC_000913       000000014       G       (60,-1)(-1,-1)  14 = 14 + 0(0)  99.9999
    NC_000913       000000015       A       (60,-1)(-1,-1)  15 = 15 + 0(0)  99.9999
    NC_000913       000000016       C       (60,-1)(-1,-1)  16 = 16 + 0(0)  99.9999
    NC_000913       000000017       T       (60,-1)(-1,-1)  17 = 17 + 0(0)  99.9999
    NC_000913       000000018       G       (60,-1)(-1,-1)  18 = 18 + 0(0)  99.9999
    NC_000913       000000019       C       (60,-1)(-1,-1)  19 = 19 + 0(0)  99.9999
    NC_000913       000000020       A       (60,-1)(-1,-1)  20 = 20 + 0(0)  99.9999

This is a snippet of *hg19* gms file.

*'*hg19 (Homo Sapiens ver.19) *'*


    chrX    000093411       G       (20,-1)(37,-1)  200 = 47 + 153(0)       38.7653
    chrX    000093412       G       (20,-1)(37,-1)  200 = 49 + 151(0)       39.7602
    chrX    000093413       C       (20,-1)(37,-1)  200 = 51 + 149(0)       40.7551
    chrX    000093414       A       (20,-1)(37,-1)  200 = 53 + 147(0)       41.7500
    chrX    000093415       G       (20,-1)(43,-1)  200 = 55 + 145(0)       42.7450
    chrX    000093416       T       (20,-1)(43,-1)  200 = 57 + 143(0)       43.7399
    chrX    000093417       T       (20,-1)(43,-1)  200 = 59 + 141(0)       44.7349
    chrX    000093418       A       (20,-1)(43,-1)  200 = 61 + 139(0)       45.7299
    chrX    000093419       G       (20,-1)(43,-1)  200 = 63 + 137(0)       46.7249
    chrX    000093420       A       (18,-1)(41,-1)  200 = 64 + 136(0)       47.7169
    chrX    000093421       G       (18,-1)(41,-1)  200 = 65 + 135(0)       48.7089
    chrX    000093422       A       (18,-1)(41,-1)  200 = 66 + 134(0)       49.7010
    chrX    000093423       G       (18,-1)(41,-1)  200 = 67 + 133(0)       50.6930
    chrX    000093424       C       (18,-1)(41,-1)  200 = 68 + 132(0)       51.6850
    chrX    000093425       A       (18,-1)(41,-1)  200 = 69 + 131(0)       52.6771
    chrX    000093426       G       (18,-1)(41,-1)  200 = 70 + 130(0)       53.6691
    chrX    000093427       A       (18,-1)(41,-1)  200 = 71 + 129(0)       54.6612
    chrX    000093428       C       (18,-1)(41,-1)  200 = 72 + 128(0)       55.6532
    chrX    000093429       T       (18,-1)(41,-1)  200 = 73 + 127(0)       56.6452
    chrX    000093430       G       (18,-1)(41,-1)  200 = 74 + 126(0)       57.6373
    chrX    000093431       T       (18,-1)(41,-1)  200 = 75 + 125(0)       58.6293
    chrX    000093432       G       (18,-1)(41,-1)  200 = 76 + 124(0)       59.6213
    chrX    000093433       C       (18,-1)(41,-1)  200 = 77 + 123(0)       60.6134
    chrX    000093434       A       (18,-1)(41,-1)  200 = 78 + 122(0)       61.6054
    chrX    000093435       A       (0,-1)(23,-1)   200 = 79 + 121(0)       62.1029
    chrX    000093436       C       (0,-1)(23,-1)   200 = 80 + 120(0)       62.6004
    chrX    000093437       C       (0,-1)(23,-1)   200 = 81 + 119(0)       63.0979
    chrX    000093438       T       (0,-1)(23,-1)   200 = 82 + 118(0)       63.5954
    chrX    000093439       T       (0,-1)(23,-1)   200 = 83 + 117(0)       64.0929
    chrX    000093440       T       (0,-1)(23,-1)   200 = 84 + 116(0)       64.5904
    chrX    000093441       A       (0,-1)(23,-1)   200 = 85 + 115(0)       65.0879
    chrX    000093442       G       (0,-1)(23,-1)   200 = 86 + 114(0)       65.5854
    chrX    000093443       A       (0,-1)(23,-1)   200 = 87 + 113(0)       66.0829
    chrX    000093444       G       (0,-1)(23,-1)   200 = 88 + 112(0)       66.5803
    chrX    000093445       T       (0,-1)(23,-1)   200 = 89 + 111(0)       67.0778
    chrX    000093446       C       (0,-1)(23,-1)   200 = 90 + 110(0)       67.5753
    chrX    000093447       T       (0,-1)(0,-1)    200 = 89 + 111(0)       67.0785
    chrX    000093448       G       (0,-1)(0,-1)    200 = 88 + 112(0)       66.5816

Installation
------------

​1. Download the latest version of GMA
[here](https://sourceforge.net/projects/gma-bio/files/).

​2. Extract files.

    $ tar xvfz gma-0.1.3.tar.gz 

If you go to "gma-0.1.3" directory, there are 6 directories.

    $ cd gma-0.1.3/
    bin  example  inc  lib  script  src

​3. Now you have two options to run GMA

​(1) Pre-compiled binary files

You use pre-compiled binary file in bin directory. Note that **mapper**
and **reducer** are our responsibility. Since "bwa" and "samtools" are
needed to run GMA, we included the latest version of them into GMA
package. They fix bugs and improve their programs, so it is your
responsibility to update those programs. If they are updated, you can
always change them with new versions.

    $ cd bin
    $ ls
    bwa mapper reducer samtools

​(2) Compiling source code.

You may want to get binary files from compiling source code. Then go to
src/ directory and "make".

    $ cd src
    $ make

:   If you get errors below, you need to intall *libz* library.

<!-- -->

    $ make
    gcc -g -Wall  -L../lib main_mapper.o genfa.o mapper.o genfq.o runall.o  -o mapper -lz -lbam -lm     
    /usr/bin/ld: cannot find -lz
    collect2: ld returned 1 exit status
    make: *** [mapper] Error 1

:   If compiling works smoothly, you can find new binary files in bin/
    directory. Note that there will be no binary files newly generated
    in src/ directory. Makefile script moves them to bin/ automatically.

Commands and Options
--------------------

Options of GMA are designed for users to use them intuitively. However,
it may confuse users because it is implemented for large scale data and
cloud computing such as [Hadoop](http://hadoop.apache.org/). Hence, we
add simple script files to give you ideas how to run GMA.

### **Quick Start**

For quick start, there are two important directories that you must know.
One is example and the other is script. In example, there is
sub-directory called "ecoli", then we have pre-computed index/ and
pre-processed fasta file, named *NC\_000913.fa.ppd*.

    gma-0.1.3$ ls
    bin  example  inc  lib  script  src
    gma-0.1.3$ ls example 
    ecoli
    gma-0.1.3$ ls example/ecoli 
    index  NC_000913.fa.ppd

Now, all you need is to run script in script/ directory.

    gma-0.1.3$ cd script/
    gma-0.1.3/script$ ./expr.local.ecoli.sh 
    .
    .
    .

Now, you can see a new directory, named "ecoli.l100.o0.qA.s0.02.i0.d0 "
is generated with "timestamp.txt". The result file name is "mapred.txt".

    gma-0.1.3/script$ cd ../example/ecoli/output/local
    gma-0.1.3/example/ecoli/output/local$ ls
    ecoli.l100.o0.qA.s0.02.i0.d0 
    gma-0.1.3/example/ecoli$ cd ecoli.l100.o0.qA.s0.02.i0.d0 
    gma-0.1.3/example/ecoli/ecoli.l100.o0.qA.s0.02.i0.d0$ less mapred.txt

Details of the script file and options will be followed.

### **Local version**

GMA provides two main binary files;mapper and reducer whose names are
more intuitive under
[MapReduce](http://en.wikipedia.org/wiki/MapReduce). For this reason,
GMA for local version can be achieve by the combination of two programs.
Since this may not be trivial for users, here we explain how it works.

#### **Run by technology**

For a quick run, we provide 5 kinds of the most popular sequencing
technologies; illumin, solid, iontorrent, roche and pacbio. You can use
one of them instead of illumina in below command. We also support
pacbio-ec, which assumes PacBio error corrected case.

    cat ../../../input/ppd/NC_000913.fa.ppd | /local2/work/hlee/schatzlab-public/gma/bin/mapper tech --illumina -b 70 -x 
    ../../../input/index/NC_000913.fna -p ../../../../../bin 1> map.txt

  ------------------ --------------------------------------------- -----------------------
  command            option                                        Description
  tech               **--illumina**                                Settings for Illumina
  **--solid**        Settings for Solid
  **--iontorrent**   Settings for Ion Torrent
  **--roche**        Settings for Roche/454
  **--pacbio**       Settings for PacBio
  **--pacbio-ec**    Settings for PacBio error corrected version
  ------------------ --------------------------------------------- -----------------------

#### **Run by your own settings**

Basically, *expr.local.ecoli.pe.sh* follows Hadoop steps, that is *map
-\> sort/shuffle -\> reduce*. Hence there are 3 important lines in the
file. You may need to get .ppd file before running map step. Such files
are included for e.coli and yeast, but you may need to prepare for
different species. This is how you can generate pre-processed file for
map step.

Step0 preprocess fasta files

<!-- -->

    $ cd {example directory}/yeast/input/ppd
    $ ../../../../script/prepro.chr.py ../index/yeast.fa
    chr1.fa.ppd is being processed
    chr2.fa.ppd is being processed
    chr3.fa.ppd is being processed
    chr4.fa.ppd is being processed
    chr5.fa.ppd is being processed
    chr6.fa.ppd is being processed
    chr7.fa.ppd is being processed
    chr8.fa.ppd is being processed
    chr9.fa.ppd is being processed
    chr10.fa.ppd is being processed
    chr11.fa.ppd is being processed
    chr12.fa.ppd is being processed
    chr13.fa.ppd is being processed
    chr14.fa.ppd is being processed
    chr15.fa.ppd is being processed
    chr16.fa.ppd is being processed
    chrMT.fa.ppd is being processed

Step1 map

<!-- -->

    cat ../../../input/ppd/NC_000913.fa.ppd | /local2/work/hlee/schatzlab-public/gma/bin/mapper runall -l 100 -q A -s 0.02
     -i 0 -d 0 -o 0 -t 20 -f ref.fa -b 70 -x ../../../input/index/NC_000913.fna  -p ../../../../../bin 1> map.txt

In this command, we can learn that *mapper* takes its input as standard
input(*stdin*). The format of command is as follows:

    mapper <command> [options] > map.txt

Commands, options and description for *mapper* is described in below
table.

  --------- ------------------------------------------------------------------------------------------------------------------------------------------------------- -------------
  command   option                                                                                                                                                  Description
  runnall   **-l**                                                                                                                                                  Read length
  **-q**    Phred-scaled quality value for each base
  **-s**    Error rate of substitution
  **-i**    Error rate of insertion
  **-d**    Error rate of deletion
  **-o**    Expected distance between two reads if paired-end.
  **-t**    Threshold
  **-f**    File path of a reference fasta
  **-b**    The number of bases in a line when re-building fasta file from pre-processed file. It is important to use the same number of the original fasta file.
  **-x**    File path of index files
  **-m**    File path of bam file
  **-p**    File path for programs such as BWA and SAMtools
  --------- ------------------------------------------------------------------------------------------------------------------------------------------------------- -------------

Step2 sort/shuffle

L/Unix commond "sort" is properly used to this purpose.

    cat map.txt | sort > mapsort.txt

Step3 Reduce

For reduce step, "analyzer" command is used with only 3 options.

    cat mapsort.txt | ../../../bin/reducer analyzer -l 100 -t 20 -o 0 1> mapred.txt 2> log.txt

  ---------- ---------------------------------------------------- -------------
  command    option                                               Description
  analyzer   **-l**                                               Read length
  **-t**     Threshold
  **-o**     Expected distance between two reads if paired-end.
  ---------- ---------------------------------------------------- -------------

### **Hadoop version**

In the *script* directory there are 4 script files;

-   expr.hadoop.ecoli.sh
-   expr.hadoop.yeast.sh
-   expr.local.ecoli.sh
-   expr.local.yeast.sh
-   expr.local.ecoli.tech.sh
-   expr.local.yeast.tech.sh

Once you have cloud computing system on your local machine or from
commercial services such as EC2, you can use expr.hadoop.\* script file
for your convenience. This is a simple guideline. Since we have no idea
about your system environment, you need to modify the given scripts
along to your system. There are explicitly specified with keyword "TODO"
so that users easily modify 3 TODOs in the scripts.

    [TODO: index file path]
    [TODO: hadoop input path on HDFS] 
    [TODO: hadoop output path on HDFS] 

Then, you can run the script in your script directory.

    script$ ./expr.hadoop.yeast.sh 

See Also
--------

-   [GMA sourceforge project
    page](https://sourceforge.net/projects/gma-bio/)

