# Makefile for FullAnalysisCalculator  Code
# (modified version of $(ROOTSYS)/test/Makefile)
#
# March 8th, 2014 B.Waidyawansa
#


ObjSuf        = o
SrcSuf        = C
ExeSuf        =
DllSuf        = so
EVENTLIB      = $(EVENTO)
OutPutOpt     = -o  

ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs)   -lNew  -lMinuit -lSpectrum
ROOTGLIBS     = $(shell root-config --glibs)


MYOS := $(subst -,,$(shell uname))

EXE := fullanacalc

CXX           = g++ 
LD            = g++
LDFLAGS       = -O
SOFLAGS       = -shared
CXXFLAGS     += $(ROOTCFLAGS)
LIBS          = $(ROOTLIBS)
GLIBS         = $(ROOTGLIBS)


all:  $(EXE)
	@echo "---------------------> $(EXE) done <---------------------- "

fullanacacl: fullanacalc.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<
%: %.C
	$(CXX) $(CXXFLAGS) $(LIBS) $(GLIBS) -o $@ $<

%.o: %.C
	$(CXX) -c  $(CXXFLAGS)   -o $@ $<

clean:
	@rm -rf *.o
	@rm -rf *~
	@rm -rf  core

cleanall:
	@rm -f $(OBJS) $(EXE) *~  core

.PHONY : clean cleanall all
