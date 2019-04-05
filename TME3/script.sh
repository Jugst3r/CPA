#! /bin/bash

make
for fic in $1/*.txt
do
  ./tme3 $fic $2
done
