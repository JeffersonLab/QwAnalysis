# Makefile to compile nur_script_killer
# 	Joshua hoskins
#	December 2011
#


ROOTLIBS   = $(shell root-config --libs )
ROOTGLIBS  = $(shell root-config --glibs)
INCLUDES   = -I$(shell root-config --incdir) -Iinclude/ 
CC         = g++ ${INCLUDES}
#SRC        = 
DEPS       = MiddleEarth.hh
CFLAGS     = -O -Wall ${INCLUDES}

all: moria

%.o: %.cc
	${CC} ${CFLAGS} -c -o $@ $< 

moria : GandalfTheGrey.o MiddleEarth.o
	${CC} ${INCLUDES} -o $@  ${CFLAGS} $^ ${ROOTLIBS} ${ROOTGLIBS}
clean: 
	rm -f *.o ~* src/*.o src/~* moria 
