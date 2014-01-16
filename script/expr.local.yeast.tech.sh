#!/bin/bash

cd ../example/yeast/output/local/

for tech in illumina; do  #solid iontorrent roche pacbio pacbio-ed

    rm -rf yeast.$tech
    mkdir yeast.$tech
    cd yeast.$tech
    rm *
    date >> ../timestamp.txt
    echo "tech:$tech" >> ../timestamp.txt

    cat ../../../input/ppd/*.ppd | ../../../../../bin/mapper tech --$tech -b 70 -x ../../../input/index/yeast.fa -p ../../../../../bin 1> map.txt

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


