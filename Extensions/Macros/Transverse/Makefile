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

EXE :=  md_sensitivity_correlations md_sensitivity_pull_plot md_sensitivity_distribution compare_md_bkgd transverse_bkgdetectors_fit transverse_md_sensitivities transverse_lumi_fit_slugs transverse_md_fit_slugs transverse_compare_reg_schemes sim_15_effect_on_correction md_correlation_plots 
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
compare_md_bkgd: compare_md_bkgd.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

md_sensitivity_correlations: md_sensitivity_correlations.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

md_sensitivity_distribution: md_sensitivity_distribution.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

md_sensitivity_pull_plot: md_sensitivity_pull_plot.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

transverse_bkgdetectors_fit: transverse_bkgdetectors_fit.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

transverse_md_sensitivities :transverse_md_sensitivities.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

transverse_md_fit_slugs :transverse_md_fit_slugs.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

transverse_lumi_fit_slugs :transverse_lumi_fit_slugs.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

transverse_compare_reg_schemes :transverse_compare_reg_schemes.o
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

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
