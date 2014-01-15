#!/usr/bin/env python
import sys

cur_chr = '' 
prv_chr = '' 

# This module can handle multiple files 
for arg in sys.argv[1:] :
    filename = arg
    print "Processing ", filename
    fa_in  = open( filename, 'r' )
    
    for line in fa_in :    # read hg19.fa, multi chromosomes in one file
     
        if line.startswith("#") :
            pass
        else : 
            cur_chr = line.split('\t')[0]

            if  prv_chr != cur_chr :
                if  prv_chr == '' :  # 1st file and chromosome
                    pass
                else : 
                    fa_out.close()

                fa_out = open( cur_chr + '.gms', 'w' )
                prv_chr = cur_chr
                print cur_chr

            else : 
                pass

            fa_out.write( line )
 
    # clean up
    try: 
        fa_out.close()
    except NameError:
        pass

    fa_in.close()

