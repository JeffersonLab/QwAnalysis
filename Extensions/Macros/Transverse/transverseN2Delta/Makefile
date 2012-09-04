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

EXE := nurTransverseMD plotTransverseN2Delta

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


# plotTransverseN2Delta : plotTransverseN2Delta.o%: %.C
#         $(CXX) $(CXXFLAGS) $(LIBS) $(GLIBS) -o $@ $<


clean:
	@rm -rf *.o
	@rm -rf *~
	@rm -rf  core

cleanall:
	@rm -f $(OBJS) $(EXE) *~  core

.PHONY : clean cleanall all
