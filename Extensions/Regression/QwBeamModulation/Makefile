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
INCLUDES   = -I$(shell root-config --incdir) -Iinclude/ -I/net/cdaqfs/apps/local/RHEL5/include/mysql++ -I/usr/include/mysql
LIB        = -L/net/cdaqfs/apps/local/RHEL5/lib -l mysqlpp -L/usr/lib/mysql  -lmysqlclient
CC         = g++ ${INCLUDES}
SRC        = src
DEPS       = QwModulation.hh QwData.hh QwChargeSensitivity.hh QwDBConnection.hh QwDataContainer.hh QwPlotHelper.hh
CFLAGS     = -O -Wall ${INCLUDES}
MYSQL      = 

all: qwbeammod qwlibra qwcharge qwdbplot qw_ndelta

%.o: %.cc
	${CC} ${CFLAGS} ${EXTRAFLAGS} -c -o $@ $< 

qwbeammod : qwbeammod.o ${SRC}/QwModulation.o
	${CC} ${INCLUDES} -o $@  ${CFLAGS} $^ ${ROOTLIBS} ${ROOTGLIBS}
qwlibra : ${SRC}/qwlibra.o ${SRC}/QwData.o ${SRC}/QwDiagnostic.o
	${CC} ${INCLUDES} -o $@  ${CFLAGS} $^ ${ROOTLIBS} ${ROOTGLIBS}
qwcharge: ${SRC}/qwcharge.o ${SRC}/QwChargeSensitivity.o
	${CC} ${INCLUDES} -o $@  ${CFLAGS} $^ ${ROOTLIBS} ${ROOTGLIBS}
qwdbplot: ${SRC}/qwdbplot.o ${SRC}/QwDBConnection.o ${SRC}/QwDataContainer.o
	${CC} ${INCLUDES} -o $@  ${CFLAGS} $^ ${ROOTLIBS} ${ROOTGLIBS}  ${LIB}
qw_ndelta: ${SRC}/qw_ndelta.o ${SRC}/QwDBConnection.o ${SRC}/QwDataContainer.o ${SRC}/QwPlotHelper.o
	${CC} ${INCLUDES} -o $@  ${CFLAGS} $^ ${ROOTLIBS} ${ROOTGLIBS}  ${LIB}

clean: 
	rm -f *.o ~* src/*.o src/~* qwbeammod qwlibra qwcharge qwdbplot qw_ndelta

