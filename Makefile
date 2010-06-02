agmerge: agmerge.cxx InputFileGetter.h InputFileGetter.cxx RootUtils.h RootUtils.cxx FileMergeObjects.h FileMergeObjects.cxx
	g++ -o $@ $(shell root-config --cflags) $^ $(shell root-config --libs)

clean:
	$(RM) agmerge

.PHONY: clean
.DELETE_ON_ERROR:

#EOF

