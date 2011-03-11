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


EXE := injector_pedestals position_differnces hallc_beam_motion hallc_bcm_calib hallc_bpm_calib get_bpm_weights project_to_Wplug 

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

injector_pedestals : injector_beamline_calibration.o 
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

position_differnces : PositionDifferences.o 
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

hallc_beam_motion : hallc_beam_motion.o 
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

IA_Scan : ia_scan.o 
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<

hallc_bcm_calib : hallc_bcm_calib.o 
	$(CXX) $(CXXFLAGS) $(LIBS) $(GLIBS) -o $@ $<

hallc_bpm_calib : hallc_bpm_calib.o 
	$(CXX) $(CXXFLAGS) $(LIBS) $(GLIBS) -o $@ $<

get_bpm_weights : GetBPMWeights.o
	$(CXX) $(CXXFLAGS) $(LIBS) $(GLIBS) -o $@ $<

project_to_Wplug : w_plug_projection.o
	$(CXX) $(CXXFLAGS) $(LIBS) $(GLIBS) -o $@ $<

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
