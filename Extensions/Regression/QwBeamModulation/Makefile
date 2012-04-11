# Makefile to compile qwbeammod
# 	Joshua hoskins
#	  July 2011
#


ifndef EXTRAFLAGS
  EXTRAFLAGS := 
  $(warning Verbose MPS error messages have been disabled.  Recompile with 'make EXTRAFLAGS=-D__VERBOSE_ERRORS' to enable them.)
endif


ROOTLIBS   = $(shell root-config --libs )
ROOTGLIBS  = $(shell root-config --glibs)
INCLUDES   = -I$(shell root-config --incdir) -Iinclude/ 
CC         = g++ ${INCLUDES}
SRC        = src
DEPS       = QwModulation.hh QwData.hh QwChargeSensitivity.hh
CFLAGS     = -O -Wall ${INCLUDES}

all: qwbeammod qwlibra qwcharge 

%.o: %.cc
	${CC} ${CFLAGS} ${EXTRAFLAGS} -c -o $@ $< 

qwbeammod : qwbeammod.o ${SRC}/QwModulation.o
	${CC} ${INCLUDES} -o $@  ${CFLAGS} $^ ${ROOTLIBS} ${ROOTGLIBS}
qwlibra : ${SRC}/qwlibra.o ${SRC}/QwData.o ${SRC}/QwDiagnostic.o
	${CC} ${INCLUDES} -o $@  ${CFLAGS} $^ ${ROOTLIBS} ${ROOTGLIBS}
qwcharge: ${SRC}/qwcharge.o ${SRC}/QwChargeSensitivity.o
	${CC} ${INCLUDES} -o $@  ${CFLAGS} $^ ${ROOTLIBS} ${ROOTGLIBS}
clean: 
	rm -f *.o ~* src/*.o src/~* qwbeammod qwlibra qwcharge
