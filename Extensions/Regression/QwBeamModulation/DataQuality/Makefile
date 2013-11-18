# Makefile to compile qwdataquality
# 	Joshua hoskins
#	  July 2011
#


ifndef EXTRAFLAGS
  EXTRAFLAGS := 
  $(warning Verbose MPS error messages have been disabled.  Recompile with 'make EXTRAFLAGS=-D__VERBOSE_ERRORS' to enable them.)
endif

ifdef NO_MYSQL
 $(warning Compiling code without the use of mysql libraries.  This may cause some code not to compile correctly.  Currently this should only be used for the MpsOnly)
endif


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

