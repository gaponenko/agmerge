#!/usr/bin/python

import sys
import os
import glob
import re
import subprocess
import string
import argparse


parser = argparse.ArgumentParser(description='Combine ROOT files according to the process cross section.')

parser.add_argument('--dbfile', required=True)
#targetLumi = 30010.6 # inb
parser.add_argument('--targetLumi', type=float, required=True, help='Target lumi for scaling, in the units corresponding to the dbfile')
parser.add_argument('--refh', default='rc20_45/exmu30000/cuts_p', help='A cuts_p histogram')
parser.add_argument('--refbin', default=1, type=int, help='N before cuts bin in the cuts_p histogram')
parser.add_argument('datasets', nargs='+', help='List of *output* dataset dirs - input components are looked for underneath each of the dirs')

opt = parser.parse_args()
#print 'got: ',opt

comment = r"^\s*#"

#================================================================
def getDSNum(fn):
    pat = r"^.*user\.andr\.(\d{6})\..*$"
    m = re.match(pat, fn)
    if m:
        return int(m.group(1))
    
    raise Exception('ERROR parsing file name for (project,dsnum,gentag): "'+fn+'"')

#================================================================

print "Reading dbfile ",opt.dbfile
xsec = {}
geneff = {}
f = open(opt.dbfile, 'r')
for line in f:

    m = re.match(comment, line)
    if not m:
        line = line.rstrip()
        if line != "":
            (dsnum, xs, eff, name) = string.split(line, None, 4)
            dsnum = int(dsnum)
            xsec[dsnum] = float(xs)
            geneff[dsnum] = float(eff)
            #print "Got: dsnum = ",dsnum," xsec = ", xsec[dsnum], ", geneff = ",geneff[dsnum]
            pass
        pass
f.close()
del(f)

#print "xsec = ",xsec

for ds in opt.datasets:
    outfile = ds + "/" + os.path.basename(ds).replace("_AGRS","") + ".ms.root"
    print "Producing ",outfile
    infiles = glob.glob(ds + "/*/*.mh.root")

    xsfilepairs = []

    for fn in infiles:
        dsnum = getDSNum(fn)
        effxs = xsec[dsnum] * geneff[dsnum]

        #print "dsnum = ",dsnum,", effxs = ", effxs, ", for ",fn
        xsfilepairs += [ str(effxs), fn ]
        
        pass

    print "Got xsfilepairs = ",xsfilepairs

    try:
        os.unlink(outfile)
    except OSError as (errno, strerror):
        pass
    
    
    try:
        retcode = subprocess.call([#'/bin/echo',
                                   '/home/f14/andr/scripts/agxmerge',
                                   '--targetLumi',  str(opt.targetLumi),
                                   '--refh', opt.refh,
                                   '--refbin', str(opt.refbin),
                                   '--out', outfile
                                   ] + xsfilepairs)
        if retcode != 0:
            print >>sys.stderr, "Child return code ", retcode
            sys.exit(1)
            pass
        
    except OSError, e:
        print >>sys.stderr, "Execution failed:", e
        pass # try-subprocess
    
    pass # ds in datasets


#for ds in list:
#
#
#    m = re.match(comment, ds)
#    if not m:
#        ds = ds.rstrip()
#        if ds != "":
#            outfile = ds + "/" + ds.rstrip("/").replace("_AGRS","") + ".mh.root"
#            #print "Merging dataset: ",ds," out = ",outfile
#            infiles = glob.glob(ds + "/*/*.root*")
#
#            if len(infiles) > 0:
#                print "Preparing ", outfile
#                try:
#                    #print "Removing old merged file"
#                    os.unlink(outfile)
#                # Requiring  os.access(outfile, os.F_OK) 
#                # would leave hanging symlinks not removed.
#                # Use the try/catch to do the "rm -f" 
#                except OSError as (errno, strerror):
#                    #print "OS error({0}): {1}".format(errno, strerror)
#                    pass
#
#                if len(infiles) == 1:
#                    #print "Linking"
#                    os.link(infiles[0], outfile)
#                else:
#                    #print "merging out=",outfile,", in = ",infiles
#                    try:
#                        retcode = subprocess.call([ # '/bin/echo',
#                                                   '/home/f14/andr/scripts/agmerge',
#                                                   outfile ] + infiles)
#                        if retcode != 0:
#                            print >>sys.stderr, "Child return code ", retcode
#                            sys.exit(1)
#                            pass
#
#                    except OSError, e:
#                        print >>sys.stderr, "Execution failed:", e
#                        pass # try-subprocess
#                    
#                    pass # len(infiles)
#            else:
#                print "WARNING: no input files for ",ds
#                pass
#
#            pass
#        pass
#    pass
#
#print "Done"
##EOF
