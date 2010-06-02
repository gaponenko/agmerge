# Makefile to build Andrei's merge library.
# Adopted from the twtools suite.
# Requires GNU make.
#
# A.Gaponenko, 2003, 2004
#

default: agmerge agwmerge
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

#================================================================
# agmerge:
AGMERGEPROGSRCS 	:= agmerge.cxx
AGMERGEPROGOBJS 	:= $(AGMERGEPROGSRCS:%.cxx=%.o)
AGMERGEPROGDEPENDS := $(AGMERGEPROGSRCS:%.cxx=$(DEPDIR)/%.d)

AGMERGEPROGCXXFLAGS := $(CXXFLAGS) $(shell root-config --cflags)

agmerge: $(AGMERGEPROGOBJS) $(AGMERGELIB)
	$(LD) -o $@ $(AGMERGEPROGCXXFLAGS) $(LDFLAGS) \
	$(AGMERGEPROGOBJS:%.o=$(OBJDIR)/%.o) \
	$(AGMERGELIB) $(shell root-config --libs) \
	-Xlinker -rpath -Xlinker $(shell root-config --libdir)

clean: cleanagmergeprog 
.PHONY: cleanagmergeprog 
cleanagmergeprog:
	$(RM) $(TOPDIR)/agmerge

ifeq ($(findstring clean,$(MAKECMDGOALS)),)
include $(AGMERGEPROGDEPENDS)
endif

$(AGMERGEPROGDEPENDS): $(DEPDIR)/%.d: %.cxx
	$(SHELL) -ec '$(CXX) -MM $(AGMERGEPROGCXXFLAGS) $< \
                      | sed s#$*.o#$*.o\ $*.d# > $@ ;\
	[ -s $@ ] || rm -f $@; test -r $@ || exit 1'

$(AGMERGEPROGOBJS): %.o : %.cxx
	$(CXX) $(AGMERGEPROGCXXFLAGS) -c $< -o $(OBJDIR)/$(*F).o


#================================================================
# agwmerge:
AGWMERGEPROGSRCS 	:= agwmerge.cxx
AGWMERGEPROGOBJS 	:= $(AGWMERGEPROGSRCS:%.cxx=%.o)
AGWMERGEPROGDEPENDS := $(AGWMERGEPROGSRCS:%.cxx=$(DEPDIR)/%.d)

AGWMERGEPROGCXXFLAGS := $(CXXFLAGS) $(shell root-config --cflags)

agwmerge: $(AGWMERGEPROGOBJS) $(AGMERGELIB)
	$(LD) -o $@ $(AGWMERGEPROGCXXFLAGS) $(LDFLAGS) \
	$(AGWMERGEPROGOBJS:%.o=$(OBJDIR)/%.o) \
	$(AGMERGELIB) $(shell root-config --libs) \
	-Xlinker -rpath -Xlinker $(shell root-config --libdir)

clean: cleanagwmergeprog 
.PHONY: cleanagwmergeprog 
cleanagwmergeprog:
	$(RM) $(TOPDIR)/agwmerge

ifeq ($(findstring clean,$(MAKECMDGOALS)),)
include $(AGWMERGEPROGDEPENDS)
endif

$(AGWMERGEPROGDEPENDS): $(DEPDIR)/%.d: %.cxx
	$(SHELL) -ec '$(CXX) -MM $(AGWMERGEPROGCXXFLAGS) $< \
                      | sed s#$*.o#$*.o\ $*.d# > $@ ;\
	[ -s $@ ] || rm -f $@; test -r $@ || exit 1'

$(AGWMERGEPROGOBJS): %.o : %.cxx
	$(CXX) $(AGWMERGEPROGCXXFLAGS) -c $< -o $(OBJDIR)/$(*F).o


################################################################


#also clean up the library by default
clean: cleanagmergelib
	$(RM) $(OBJDIR)/*.o $(DEPDIR)/*.d	

cleanall: clean
.PHONY: cleanall clean

.DELETE_ON_ERROR:

#EOF
