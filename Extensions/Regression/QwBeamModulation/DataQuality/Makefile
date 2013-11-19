# Makefile to compile qwdataquality
# 	Joshua hoskins
#	  July 2013
#

ROOTLIBS   = $(shell root-config --libs ) -lSpectrum
ROOTGLIBS  = $(shell root-config --glibs)
INCLUDES   = -I$(shell root-config --incdir) -Iinclude/
#LIB        = ${MYSQL_LIBS} ${MYSQLPP_LIBS}
CC         = g++ 
SRC        = src
CFLAGS     = -O -Wall ${INCLUDES}
MYSQL      = 

all: qwdataquality

%.o: %.cc
	${CC} ${CFLAGS} ${EXTRAFLAGS} -c -o $@ $< 

qwdataquality : ${SRC}/qwdataquality.o ${SRC}/QwDataQuality.o
	${CC} ${INCLUDES} -o $@  ${CFLAGS} $^ ${ROOTLIBS} ${ROOTGLIBS}
clean: 
	rm -f *.o ~* src/*.o src/*.cc~ include/*.hh~ qwdataquality

