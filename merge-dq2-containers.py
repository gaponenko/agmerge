#!/usr/bin/python

import sys
import os
import glob
import re
import subprocess

comment = r"^\s*#"

list=sys.stdin

print "Reading stdin..."


for ds in list:
    m = re.match(comment, ds)
    if not m:
        ds = ds.rstrip()
        if ds != "":
            outfile = ds + "/" + ds.rstrip("/").replace("_AGRS","") + ".mh.root"
            #print "Merging dataset: ",ds," out = ",outfile
            infiles = glob.glob(ds + "/*/*.root*")

            if len(infiles) > 0:
                print "Preparing ", outfile
                try:
                    #print "Removing old merged file"
                    os.unlink(outfile)
                # Requiring  os.access(outfile, os.F_OK) 
                # would leave hanging symlinks not removed.
                # Use the try/catch to do the "rm -f" 
                except OSError as (errno, strerror):
                    #print "OS error({0}): {1}".format(errno, strerror)
                    pass

                if len(infiles) == 1:
                    #print "Linking"
                    os.link(infiles[0], outfile)
                else:
                    #print "merging out=",outfile,", in = ",infiles
                    try:
                        retcode = subprocess.call([ # '/bin/echo',
                                                   '/home/f14/andr/scripts/agmerge',
                                                   outfile ] + infiles)
                        if retcode != 0:
                            print >>sys.stderr, "Child return code ", retcode
                            sys.exit(1)
                            pass

                    except OSError, e:
                        print >>sys.stderr, "Execution failed:", e
                        pass # try-subprocess
                    
                    pass # len(infiles)
            else:
                print "WARNING: no input files for ",ds
                pass

            pass
        pass
    pass

print "Done"
#EOF

    
