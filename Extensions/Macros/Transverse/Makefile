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

EXE := sim_15_effect_on_correction md_correlation_plots 100k_transverse_fit slug_average_fits plot_regressed_lumi slug_summary slug_average_fits_regressed unregressed_slug_plots slug_average_fits_regressed_run_range compare_regressed_unregressed fits_unregressed_run_range slug_average_fits_unregressed draw_correlations_runlets pass3_regressed sim_15_effect

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

100k_transverse_fit : 100k_transverse_fit.o 
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

slug_average_fits : slug_average_fits.o 
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

slug_average_fits_regressed :slug_avrage_fits_regressed.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<	

slug_average_fits_regressed_run_range :slug_average_fits_regressed_run_range.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

Regressed_lumi : Regressed_LUMI.o 
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

slug_summmary : slug_summary.o 
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

unregressed_slug_plots : unregressed_slug_plots.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

compare_regressed_unregressed : compare_regressed_unregressed.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

fits_unregressed_run_range : fits_unregressed_run_range.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

slug_average_fits_unregressed :slug_average_fits_unregressed.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

slug_average_fits_regressed :slug_avrage_fits_regressed.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

sim_15_effect :sim_15_effect.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

sim_15_effect_on_correction :sim_15_effect_on_correction.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

draw_correlations_runlets : draw_correlations_runlets.o

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
