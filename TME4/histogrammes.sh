#!/bin/bash
# My first script

for i in {1..1000}
do
    ./tme4 com-youtube.ungraph.txt youtubelabels$i.txt
    sort -s -n -k 1 youtubelabels$i.txt >> youtube_labels$i.txt
    printf "$i " >> histogrammes.txt
    python3 exo2_tests.py youtube_labels$i.txt >>  histogrammes.txt
done