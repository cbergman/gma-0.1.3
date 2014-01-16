#!/bin/bash

cd ../example/ecoli/output/local/

for tech in illumina; do  #solid iontorrent roche pacbio pacbio-ed

    rm -rf ecoli.$tech
    mkdir ecoli.$tech
    cd ecoli.$tech
    rm *
    date >> ../timestamp.txt
    echo "tech:$tech" >> ../timestamp.txt

    cat ../../../input/ppd/NC_000913.fa.ppd | ../../../../../bin/mapper tech --$tech -b 70 -x ../../../input/index/NC_000913.fna -p ../../../../../bin 1> map.txt

    echo "sorting..."
    cat map.txt | sort -k1,1 -t"|" > mapsort.txt

    echo "complete"
    echo "====================================="

    echo "analyzing..."
    cat mapsort.txt | ../../../../../bin/reducer tech --$tech 1> mapred.txt

    echo "====================================="
    echo "reducer is done"
    echo "====================================="

done;


cd ../../../../../script/


