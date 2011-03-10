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
ROOTLIBS      = $(shell root-config --libs)   -lNew  -lMinuit -lSpectrum
ROOTGLIBS     = $(shell root-config --glibs)


MYOS := $(subst -,,$(shell uname))

EXE := md_correlation_plots TransverseFit slug_average plot_regressed_lumi slug_average_regressed

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

md_correlation_plots : md_correlations.o 
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

TransverseFit : TransverseFit.o 
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

slug_average : slug_average_fits.o 
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

slug_average_regressed :slug_avrage_fits_regressed.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

plot_regressed_lumi : Regressed_LUMI.o 
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

%: %.C
	$(CXX) $(CXXFLAGS) $(LIBS) $(GLIBS) -o $@ $<

%.o: %.C
	$(CXX) -c  $(CXXFLAGS)   -o $@ $<



#depend .depend : $(SRCS)
#	$(CXX) $(CXXFLAGS) -M $(MAIN)  >./.depend

clean:
	@rm -rf *.o
	@rm -rf *~
	@rm -rf  core

cleanall:
	@rm -f $(OBJS) $(EXE) *~  core

.PHONY : clean cleanall all
