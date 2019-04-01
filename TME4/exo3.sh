#!/bin/bash
# My first script

python3 exo1.py
./tme4 graph00.txt labels00.txt
sort -s -n -k 1 labels00.txt >> graph00_labels.txt
python3 exo2_tests.py graph00_labels.txt

rm -f labels00.txt
rm -f graph00_labels.txt

./convert -i ~/10MARS_DEVOIRS/CPA/TME4/graph00.txt -o graph00.bin
./louvain graph00.bin -l -1 -v -q id_qual > graph00.tree

