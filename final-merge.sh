#!/bin/bash
# 
# Final (second stage) merging: combining output of grid histo merge jobs
#
# Andrei Gaponenko, 2010

set -e

run=${1:?Usage:$0 dirname}
cd $run

glb1='*-m*0001.out.root'
#glb='*.root'

declare -a part1arr
part1arr=($glb1)

#echo "Size of part1arr = ${#part1arr[@]}"
if [ ${#part1arr[@]} -gt 1 ]; then
    echo "ERROR: multiple matches for $glb1"
    exit 1;
fi

part1="${part1arr[0]}"
if [ -e "$part1" ]; then
    out="$(echo $part1 | sed -e 's/-m.*0001\.out\.root$//').all.root";
    glb="$(echo $glb1|sed -e 's/0001/*/')"
    echo "Merging files in $run, outfile = $out"
    echo agmerge $out $glb
    /usr/bin/time agmerge $out $glb
else 
    echo "ERROR: nothing matches $glb1 in $run"
    exit 1
fi

exit 0
#EOF
