# Makefile
# (modified version of $(ROOTSYS)/test/Makefile)
#

# April 25, 2000 	 E. Beise, modify to work with Alpha DUX4.0
# April4th, 2010         B. Waidyawansa, modify to work for correlation_plots.cc

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
#-I$(QWANALYSIS)/Analysis/include
	LD            = g++
	LDFLAGS       = -g
	SOFLAGS       = -shared

	CXXFLAGS     += $(ROOTCFLAGS)
	LIBS          = $(ROOTLIBS)
#-L$(QWANALYSIS)/lib -lG0 -lmyevio -L$(CODA_LIB) -let
	GLIBS         = $(ROOTGLIBS)
#endif


correlation_plots : correlation_plots.o 
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
