# Makefile code to be included in makefiles using this library.
# Requires GNU make.  Adopted from the twtools suite.
#
# A.Gaponenko, 2003, 2010
#

# sources
AGMERGESDIR = $(AGMERGETOP)
# includes
AGMERGEIDIR = $(AGMERGETOP)
# objects
AGMERGEODIR = $(AGMERGETOP)
# library directory
AGMERGELDIR = $(AGMERGETOP)

vpath %.cxx $(AGMERGESDIR)
vpath %.h $(AGMERGEIDIR)
vpath %.o $(AGMERGEODIR)

AGMERGELIB = $(AGMERGELDIR)/libagmerge.a

################################################################
AGMERGELIBSRCS 	:= FileMergeObjects.cxx InputFileGetter.cxx RootUtils.cxx
AGMERGELIBOBJS 	:= $(AGMERGELIBSRCS:%.cxx=%.o)
AGMERGELIBDEPENDS := $(AGMERGELIBSRCS:%.cxx=$(DEPDIR)/%.d)

################################################################
LIBAGMERGEFLAGS := $(CXXFLAGS) -Wall $(shell root-config --cflags)

$(AGMERGELIB): $(AGMERGELIBOBJS)
	$(RM) $@
	ar cr $@ $(AGMERGELIBOBJS:%.o=$(AGMERGEODIR)/%.o)

cleanall: cleanagmergelib
.PHONY: cleanagmergelib cleanall

cleanagmergelib:
	$(RM) $(AGMERGELIBOBJS:%.o=$(AGMERGEODIR)/%.o)
	$(RM) $(AGMERGELIBDEPENDS)
	$(RM) $(AGMERGELIB)
	$(RM) $(AGMERGEROOTCHECKFILE)

ifeq ($(findstring clean,$(MAKECMDGOALS)),)
include $(AGMERGELIBDEPENDS)
endif

################################################################
AGMERGE_NEWROOTSTR := $(shell root-config --version) $(shell root-config --libdir)
AGMERGEROOTCHECKFILE = $(AGMERGEODIR)/AGMERGE.ROOTCHECK
$(AGMERGEROOTCHECKFILE):; echo 'AGMERGE_ROOTSTR='$(AGMERGE_NEWROOTSTR) > $@

$(AGMERGELIBDEPENDS) $(AGMERGELIBOBJS): $(AGMERGEROOTCHECKFILE)
ifeq ($(findstring clean,$(MAKECMDGOALS)),)
include $(AGMERGEROOTCHECKFILE) 
endif

ifneq ($(AGMERGE_ROOTSTR),$(AGMERGE_NEWROOTSTR))
ifdef AGMERGE_ROOTSTR
define errtext
ROOT version and/or lib dir has changed from "$(AGMERGE_ROOTSTR)" to "$(AGMERGE_NEWROOTSTR)".
Do 'make cleanall' first to get a consistent build
endef
$(error $(errtext))
endif
endif

################################################################
$(AGMERGELIBDEPENDS): $(DEPDIR)/%.d: %.cxx
	$(SHELL) -ec '$(CXX) -MM $(LIBAGMERGEFLAGS) $< \
                      | sed s#$*.o#$*.o\ $*.d# > $@ ;\
	[ -s $@ ] || rm -f $@; test -r $@ || exit 1'

$(AGMERGELIBOBJS): %.o : %.cxx
	$(CXX) $(LIBAGMERGEFLAGS) -c $< -o $(AGMERGEODIR)/$(*F).o

.DELETE_ON_ERROR:
#EOF
