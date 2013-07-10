# Makefile to compile qwbeammod at UVA
# 	Don Jones
#	June 2013
#





ROOTLIBS   = $(shell root-config --libs --cflags --ldflags --glibs)
INCLUDES   = -I$(shell root-config --incdir) -Iinclude 
LIBS       = ${ROOTLIBS} -lSpectrum
CC         = g++
SRC        = src
CFLAGS     = -O -Wall 

all: QwMpsOnly runMpsOnly


QwMpsOnly: 
	   ${CC} ${CFLAGS} ${LIBS} ${INCLUDES} ${SRC}/$@.cc -c -o ${SRC}/$@.o

runMpsOnly: ${SRC}/QwMpsOnly.o 
	   ${CC} ${CFLAGS} ${LIBS} ${INCLUDES} $^ ${SRC}/$@.cc -o $@

clean: 
	rm -f *.o ~* src/*.o src/*.cc~ include/*.hh~ 
