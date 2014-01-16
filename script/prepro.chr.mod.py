#!/usr/bin/env python
import sys
import os

i = 0
ovlpstr = ''
ovlpln = 10  #20 , 2 for test
chnkln = 100 #200, 5 for test
start = 0
end = 0
chr = ''
dna = ''
filenum = 0
chr_num = 1;

# This module can handle multiple files 
for arg in sys.argv[1:] :
    filename = arg
    print filename
    file = os.path.splitext(os.path.basename(filename))[0]
#   file = filename.split('/')[len(filename.split('/'))-1].split(".")[0]
    fa_in  = open( filename, 'r' )
    #fa_out  = open( filename, 'r' )
    fa_out = open( file + '.ppd', 'w' ) # in case of T.v, there'll too many files
    
    ovlpstr = ''
    dna = ''
    start = 0
    end = 0
    i = 0

    for line in fa_in :    # read hg19.fa, multi chromosomes in one file
        if i in range(chnkln) :
            if line[0] == '>': # new chromosome
                if i != 0 :  # there is a previous chromosome
                    fa_out.write(chr + ':' + str(start) + '-' + str(end) + '>\t' + dna +'\n')
                    #fa_out.write("(1)")
                else :
                    #fa_out.write("(2)")
                    pass
                ovlpstr = ''
                dna = ''
                start = 0
                end = 0
                i = 0
                chr = line.split()[0].split('|')[0].strip('>') 
 
#                if chr.isdigit() == True or chr == "X" or chr == "Y" :
#                    chr = 'chr' + chr
#                else :
#                    if ( "MT" in line ) or ( "mito" in line ) :
#                       chr = 'chrMT'
#                    elif ( "chr" in line ) or ( "CHR" in line ) :
#                       #print chr
#                       chr = 'chr' + chr
                       #chr = 'chr' + str(chr_num)
                       #chr_num = chr_num + 1
#                    elif "|" in line  :
#                       print line.split("|")[1]
#                       print "T. vaginalis case"
#                    else : 
#                chr = filename.split('/')[len(filename.split('/'))-1].split(".")[0]

#                fa_out.close()
#                fa_out = open( chr + '.fna.ppd', 'w' )
                print str(chr), "is being processed"

                #fa_out.write("(3)")
            else :
                if i >= (chnkln - ovlpln) :
                    ovlpstr = ovlpstr + line.strip()
                    #fa_out.write("(4)")
                elif i == 0 :
                    if ovlpstr == '' :        # The first line
                        start = 1
                        end = 0
                        #fa_out.write("(5)")
                    else :                 # From second line, consider overlap
                        dna = ovlpstr
                        start = end - len(ovlpstr) + 1
                        i = ovlpln  
                        ovlpstr = ''
                        #fa_out.write("(6)")
                else :
                    #fa_out.write("(7)")
                    pass

                #fa_out.write("(8)")
                dna = dna + line.strip()
                #fa_out.write(line)
                end = end + len(line.strip())
                if i == (chnkln - 1) :
                    fa_out.write(chr + ':' + str(start) + '-' + str(end) + '>\t' + dna +'\n')
                    i = 0
                else :
                    i = i + 1

    #flush buffer
    fa_out.write(chr + ':' + str(start) + '-' + str(end) + '>\t' + dna +'\n')

    # clean up
    fa_out.close()
    fa_in.close()