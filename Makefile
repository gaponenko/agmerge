agmerge: agmerge.cxx InputFileGetter.h InputFileGetter.cxx RootUtils.h RootUtils.cxx
	g++ -o $@ $(shell root-config --cflags) $^ $(shell root-config --libs)

clean:
	$(RM) agmerge

.PHONY: clean
.DELETE_ON_ERROR:

#EOF

