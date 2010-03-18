# Makefile for bpm_bcm_calibration.cc  Code
# (modified version of $(ROOTSYS)/test/Makefile)
#

# April 25, 2000 	 E. Beise, modify to work with Alpha DUX4.0
# March 16, 2010         B. Waidyawansa, modify to work for bpm_bcm_calibration.0

ObjSuf        = o
SrcSuf        = C
ExeSuf        =
DllSuf        = so
EVENTLIB      = $(EVENTO)
OutPutOpt     = -o  

ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs)   -lNew  -lMinuit
ROOTGLIBS     = $(shell root-config --glibs)


MYOS := $(subst -,,$(shell uname))


# Linux with egcs	
#ifeq ($(MYOS),Linux)
	CXX           = g++ 
	CXXFLAGS      = -O2 -Wall -fPIC 
#-I$(G0ANALYSIS)/Analysis/include
	LD            = g++
	LDFLAGS       = -g
	SOFLAGS       = -shared

	CXXFLAGS     += $(ROOTCFLAGS)
	LIBS          = $(ROOTLIBS)
#-L$(G0ANALYSIS)/lib -lG0 -lmyevio -L$(CODA_LIB) -let
	GLIBS         = $(ROOTGLIBS)
#endif


pedestal_extractor : bpm_bcm_calibration.o 
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<


%.o: %.C
	$(CXX) -c  $(CXXFLAGS)   -o $@ $<


all: depend $(EXE)

	@echo "---------------------> $(EXE) done <---------------------- "

depend .depend : $(SRCS)
	$(CXX) $(CXXFLAGS) -M $(MAIN)  >./.depend

clean:
		@rm -f $(OBJS) *~  core

cleanall:
		@rm -f $(OBJS) $(EXE) *~  core
