# Makefile to compile qwbeammod
# 	Joshua hoskins
#	  July 2011
#


ROOTLIBS   = $(shell root-config --libs )
ROOTGLIBS  = $(shell root-config --glibs)
INCLUDES   = -I$(ROOTSYS)/include -Iinclude/ 
CC         = g++ ${INCLUDES}
SRC        = src
DEPS       = QwModulation.hh
CFLAGS     = -O -Wall ${INCLUDES}

all: qwbeammod qwlibra

%.o: %.cc
	${CC} ${CFLAGS} -c -o $@ $< 

qwbeammod : qwbeammod.o ${SRC}/QwModulation.o
	${CC} ${INCLUDES} -o $@  ${CFLAGS} $^ ${ROOTLIBS} ${ROOTGLIBS}
qwlibra : ${SRC}/qwlibra.o ${SRC}/QwData.o ${SRC}/QwDiagnostic.o
	${CC} ${INCLUDES} -o $@  ${CFLAGS} $^ ${ROOTLIBS} ${ROOTGLIBS}
clean: 
	rm -f *.o ~* src/*.o src/~* qwbeammod qwlibra 
