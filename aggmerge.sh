#!/bin/bash
#
# Merge histogram files on the grid.
#
#    prun --exec "agmerge.sh %IN"  --outputs out.root \
#         --noBuild --match="*AGRS*.root" --athenaTag=15.6.8 \
#         --nFilesPerJob=200 \
#         --inDS user10.AndreiGaponenko.105011.J2_pythia_jetjet.e468_s766_s767_r1251_20100527-a.v1  \
#         --outDS user10.AndreiGaponenko.105011.J2_pythia_jetjet.e468_s766_s767_r1251_20100527-a.v1-m
# 
# Andrei Gaponenko, 2010

echo "AG: Starting on $(date)"
#ls
#echo "================================================================"
#printenv
#echo "================================================================"

/usr/bin/time make

echo "$1"| sed -e 's/,/ /g' | /usr/bin/time ./agmerge out.root --stdin

echo "AG: Finished on $(date)"
exit 0
