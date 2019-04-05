#! /bin/bash

make
for fic in $1/*.txt
do
  ./tme6 $fic $2
done
