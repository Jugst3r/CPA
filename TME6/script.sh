#! /bin/bash

make
for fic in $1/*.txt
do
  echo $fic
  ./tme6 $fic $2
done
