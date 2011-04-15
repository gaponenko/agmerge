#!/usr/bin/python

import sys
import os
import glob
import re
import subprocess
import string

#targetLumi = 35000. # inb
#targetLumi = 30010.6 # inb
#refh = "ex20_45/exmu20/cuts_p"
targetLumi = 1000000 # inb = 1ifb
refh = "rc20_45/exmu30000/cuts_p"
refbin = 1



dbfile = sys.argv[1]
datasets = sys.argv[2:]

comment = r"^\s*#"

#================================================================
def getDSNum(fn):
    pat = r"^.*user\.andr\.(\d{6})\..*$"
    m = re.match(pat, fn)
    if m:
        return int(m.group(1))
    
    raise Exception('ERROR parsing file name for (project,dsnum,gentag): "'+fn+'"')

#================================================================

print "Reading dbfile ",dbfile
xsec = {}
geneff = {}
f = open(dbfile, 'r')
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

for ds in datasets:
    outfile = ds + "/" + os.path.basename(ds).replace("_AGRS","") + ".ms.root"
    print "Producing ",outfile
    infiles = glob.glob(ds + "/*/*.mh.root")

    if len(infiles) == 0:
        infiles = glob.glob(ds + "/*.mh.root")

        if len(infiles) == 0:
            print >>sys.stderr, "ERROR: no input files for ", ds
            sys.exit(1)
            pass

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
                                   '--targetLumi',  str(targetLumi),
                                   '--refh', refh,
                                   '--refbin', str(refbin),
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
