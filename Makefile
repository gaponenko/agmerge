# Makefile to build Andrei's merge library.
# Adopted from the twtools suite.
# Requires GNU make.
#
# A.Gaponenko, 2003, 2004
#

default: agmerge
.PHONY: default

TOPDIR = .
OBJDIR = $(TOPDIR)
DEPDIR = $(TOPDIR)

vpath %.o $(OBJDIR)
vpath %.d $(DEPDIR)

################################################################
##include $(TWTCTOP)/BuildConf.mk
CXX = g++
CXXFLAGS = -O -Wall
LD  = g++
LDFLAGS = 

# set var for the included Module.mk file
AGMERGETOP = $(TOPDIR)
include $(AGMERGETOP)/Module.mk

################################################################
AGMERGEPROGSRCS 	:= agmerge.cxx
AGMERGEPROGOBJS 	:= $(AGMERGEPROGSRCS:%.cxx=%.o)
AGMERGEPROGDEPENDS := $(AGMERGEPROGSRCS:%.cxx=$(DEPDIR)/%.d)

AGMERGEPROGCXXFLAGS := $(CXXFLAGS) $(shell root-config --cflags)

agmerge: $(AGMERGEPROGOBJS) $(AGMERGELIB)
	$(LD) -o $@ $(AGMERGEPROGCXXFLAGS) $(LDFLAGS) \
	$(AGMERGEPROGOBJS:%.o=$(OBJDIR)/%.o) \
	$(AGMERGELIB) $(shell root-config --libs) \
	-Xlinker -rpath -Xlinker $(shell root-config --libdir)

cleanall: clean
.PHONY: cleanall

clean: cleanagmergelib cleanagmergeprog
.PHONY: cleanagmergelib cleanagmergeprog
cleanagmergeprog:
	$(RM) $(TOPDIR)/agmergeprog
	$(RM) $(OBJDIR)/*.o $(DEPDIR)/*.d	

ifeq ($(findstring clean,$(MAKECMDGOALS)),)
include $(AGMERGEPROGDEPENDS)
endif

$(AGMERGEPROGDEPENDS): $(DEPDIR)/%.d: %.cxx
	$(SHELL) -ec '$(CXX) -MM $(AGMERGEPROGCXXFLAGS) $< \
                      | sed s#$*.o#$*.o\ $*.d# > $@ ;\
	[ -s $@ ] || rm -f $@; test -r $@ || exit 1'

$(AGMERGEPROGOBJS): %.o : %.cxx
	$(CXX) $(AGMERGEPROGCXXFLAGS) -c $< -o $(OBJDIR)/$(*F).o


.DELETE_ON_ERROR:

#EOF
