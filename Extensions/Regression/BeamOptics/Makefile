# Makefile to compile optics_plot
# 	Joshua hoskins
#	December 2011
#


ROOTLIBS  = $(shell root-config --libs )
ROOTGLIBS = $(shell root-config --glibs)
INCLUDES  = -I$(shell root-config --incdir)
CC        = g++
DEPS      = headers.h beammod.hh
CFLAGS    = -O -Wall $(INCLUDES)

all: optics_plot 

%.o: %.cc $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

optics_plot : optics_plot.o
	$(CC) -o $@  $(CFLAGS) $^ $(ROOTLIBS) $(ROOTGLIBS)
clean: 
	rm -f *.o ~* optics_plot 