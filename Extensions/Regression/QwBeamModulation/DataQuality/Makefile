# Makefile to compile dataquality
# 	Joshua hoskins
#	  November 2013
#

ROOTLIBS   = $(shell root-config --libs ) -lSpectrum
ROOTGLIBS  = $(shell root-config --glibs)
INCLUDES   = -I$(shell root-config --incdir) -Iinclude/
#LIB        = ${MYSQL_LIBS} ${MYSQLPP_LIBS}
CC         = g++ 
SRC        = src
CFLAGS     = -O -Wall ${INCLUDES}

all: dataquality

%.o: %.cc
	${CC} ${CFLAGS} ${EXTRAFLAGS} -c -o $@ $< 

dataquality : ${SRC}/dataquality.o ${SRC}/QwDataQuality.o
	${CC} -o $@  ${CFLAGS} $^ ${ROOTLIBS} ${ROOTGLIBS}
avgposdiff : ${SRC}/avgposdiff.o ${SRC}/QwDataQuality.o
	${CC} -o $@  ${CFLAGS} $^ ${ROOTLIBS} ${ROOTGLIBS}
clean: 
	rm -f *.o ~* src/*.o src/*.cc~ include/*.hh~ dataquality avgposdiff

