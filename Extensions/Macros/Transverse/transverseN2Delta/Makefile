# Makefile for transverseN2Delta  analysis
# (modified version of $(ROOTSYS)/test/Makefile)
#
# August 04,  2012         Nuruzzaman

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

EXE := nurTransverseMD plotTransverseN2Delta compare calcTransverseN2Delta finalTransverseN2Delta mdQuery mdQueryModified dsLumiQuery usLumiQuery senQuery diffQuery bkgMDQuery asymDBRunlet bkgDBRunlet senDBRunlet diffDBRunlet cutDependence cutDependenceAsym octantPlot plotNoCFitTransverseN2Delta beamlineBkg nonLinearity  #clean

# Linux with egcs	
#ifeq ($(MYOS),Linux)
	CXX           = g++ 
	CXXFLAGS      = -O2 -Wall -fPIC 
#-I$(G0ANALYSIS)/Analysis/include
	LD            = g++
	LDFLAGS       = -O
	SOFLAGS       = -shared

	CXXFLAGS     += $(ROOTCFLAGS)
	LIBS          = $(ROOTLIBS)
#-L$(G0ANALYSIS)/lib -lG0 -lmyevio -L$(CODA_LIB) -let
	GLIBS         = $(ROOTGLIBS)
#endif

all:  $(EXE)
	@echo "---------------------> $(EXE) done <---------------------- "


nurTransverseMD :nurTransverseMD.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

plotTransverseN2Delta :plotTransverseN2Delta.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

compare :compare.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

calcTransverseN2Delta :calcTransverseN2Delta.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

finalTransverseN2Delta :finalTransverseN2Delta.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

mdQuery :mdQuery.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

mdQueryModified :mdQueryModified.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

dsLumiQuery :dsLumiQuery.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

usLumiQuery :usLumiQuery.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

bkgMDQuery :bkgMDQuery.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

senQuery :senQuery.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

diffQuery :diffQuery.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

asymDBRunlet :asymDBRunlet.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

bkgDBRunlet :bkgDBRunlet.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

senDBRunlet :senDBRunlet.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

diffDBRunlet :diffDBRunlet.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

cutDependence :cutDependence.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

cutDependenceAsym :cutDependenceAsym.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

octantPlot :octantPlot.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

plotNoCFitTransverseN2Delta :plotNoCFitTransverseN2Delta.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

beamlineBkg :beamlineBkg.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

nonLinearity :nonLinearity.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<


# diagonastics :diagonastics.o
# 	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<



clean:
	@rm -rf *.o
	@rm -rf *~
	@rm -rf  core

cleanall:
	@rm -f $(OBJS) $(EXE) *~  core

.PHONY : clean cleanall all
