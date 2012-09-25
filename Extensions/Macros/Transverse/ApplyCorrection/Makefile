# (modified version of $(ROOTSYS)/test/Makefile)
#
# April 25, 2000 	 E. Beise, modify to work with Alpha DUX4.0
# March 16, 2010         B. Waidyawansa, modify to work for bpm_bcm_calibration.0
# July 20th,2012         B. Waidyawansa, coppied from Transverse to work on correction scripts

ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs)   -lNew  -lMinuit -lSpectrum
ROOTGLIBS     = $(shell root-config --glibs)


CXX           = g++ 
CXXFLAGS      = -O2 -Wall -fPIC 

CXXFLAGS     += $(ROOTCFLAGS)
LIBS          = $(ROOTLIBS)
GLIBS         = $(ROOTGLIBS)
# DEPS          = functions.h
# OBJ           = ResidualTransverseDistributions.o functions.o 

# # First, the .o file depends upon the .c version of the file and the .h files included in the DEPS macro
# # Next,to generate the .o file, make needs to compile the .c file using the compiler defined in the CXX macro.
# # The -c flag says to generate the object file, the -o $@ says to put the output of the compilation in the file named 
# # on the left side of the :, the $< is the first item in the dependencies list, 
# # and the CFLAGS macro is defined as above.
# # special macros $@ and $^, which are the left and right sides of the :, 
# from http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/
# %.o: %.c $(DEPS)
# 	$(CXX) -c -o $@ $< $(CXXLAGS) $(LIBS) $(GLIBS)

# residual_transverse_distributions : $(OBJ)
# 		$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS) $(GLIBS)


 EXE := correct_bars residual_transverse_distributions

 all:  $(EXE)
	@echo "---------------------> $(EXE) done <---------------------- "

 correct_bars: correct_bars.o 
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<


residual_transverse_distributions : ResidualTransverseDistributions.o 
	$(CXX) $(CXXFLAGS)  $(LIBS) $(GLIBS) -o $@ $<


clean:
	@rm -rf *.o
	@rm -rf *~
	@rm -rf  core

cleanall:
	@rm -f $(OBJS) $(EXE) *~  core

.PHONY : clean cleanall all
