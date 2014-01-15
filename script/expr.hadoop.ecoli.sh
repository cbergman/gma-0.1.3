#!/bin/bash

cd ../example/ecoli/output/hadoop/

for o in 0; do
  for th in 20; do
    for l in 100; do   #  for l in 100 125 150; do
      for s in 0.01; do   #  for e in 0.00 0.01 0.02 0.05; do
        for i in 0; do   #  for e in 0.00 0.01 0.02 0.05; do
          for d in 0; do   #  for e in 0.00 0.01 0.02 0.05; do
            for x in is; do  # is, bwtsw, c 

              # log time
              date >> timestamp.txt
              echo "l:$l" >> ../timestamp.txt
              echo "q:$q" >> ../timestamp.txt
              echo "s:$s" >> ../timestamp.txt
              echo "i:$i" >> ../timestamp.txt
              echo "d:$d" >> ../timestamp.txt
              echo "o:$o" >> ../timestamp.txt
              echo "t:$t" >> ../timestamp.txt
              echo "f:$f" >> ../timestamp.txt
              echo "b:$b" >> ../timestamp.txt
              echo "x:$x" >> ../timestamp.txt
              echo "m:$m" >> ../timestamp.txt
              echo "p:$p" >> ../timestamp.txt
              echo "--" >> timestamp.txt
              # map 
              hadoop fs -rmr [TODO: hadoop output path on HDFS]
      
              hadoop jar /usr/lib/hadoop/contrib/streaming/hadoop-streaming-0.20.2-cdh3u3.jar \
              -D mapred.job.name="GMA_ecoli_o$o-t$th-l$l-s$s-i$i-d$d-qA" \
              -D mapred.job.arg0="24" \
              -D mapred.job.arg1="runall" \
              -D mapred.job.arg2="-l" \
              -D mapred.job.arg3="$l" \
              -D mapred.job.arg4="-q" \
              -D mapred.job.arg5="A" \
              -D mapred.job.arg6="-s" \
              -D mapred.job.arg7="$s" \
              -D mapred.job.arg8="-i" \
              -D mapred.job.arg9="$i" \
              -D mapred.job.arg10="-d" \
              -D mapred.job.arg11="$d" \
              -D mapred.job.arg12="-o" \
              -D mapred.job.arg13="$o" \
              -D mapred.job.arg14="-t" \
              -D mapred.job.arg15="$th" \
              -D mapred.job.arg16="-f" \
              -D mapred.job.arg17="ref.fa" \
              -D mapred.job.arg18="-b" \
              -D mapred.job.arg19="70" \
              -D mapred.job.arg20="-x" \
              -D mapred.job.arg21="/bluearc/data/schatz/hlee/genome/ecoli/index.$x/NC_000913.fna" \
              -D mapred.job.arg22="-p" \
              -D mapred.job.arg23="/bluearc/data/schatz/hlee/program" \
              -D mapred.map.tasks=1 \
              -D mapred.reduce.tasks=1 \
              -D map.output.key.field.separator="|" \
              -D num.key.fields.for.partition=1 \
              -partitioner org.apache.hadoop.mapred.lib.KeyFieldBasedPartitioner \
              -input [TODO: hadoop input path on HDFS] \
              -output [TODO: hadoop output path on HDFS] \
              -mapper  mapper  -file ../../../../../bin/mapper \
              -reducer reducer -file ../../../../../bin/reducer
      
              rm -rf gms_ecoli-l$l-s$s-i$i-d$d-o$o
              hadoop fs -get /user/hlee/gma/ecoli/output gms_ecoli-l$l-s$s-i$i-d$d-o$o
      
              cd gms_ecoli-l$l-s$s-i$i-d$d-o$o
              ../../../../../script/postpro.div.py part-*
              cd ..

            done; 
          done;
        done;
      done; #e
    done; #l
  done; #th
done; #o


