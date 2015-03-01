# Makefile to compile qwbeammod
# 	Joshua hoskins
#	  July 2011
#


ifndef EXTRAFLAGS
  EXTRAFLAGS := 
  $(warning Verbose MPS error messages have been disabled.  Recompile with 'make EXTRAFLAGS=-D__VERBOSE_ERRORS' to enable them.)
endif

ifndef NO_MYSQL
 ############################
 ############################
 # Some set-up for the MySQL library (copied from $QWANALYSIS/GNUmakefile)
 ############################
 ############################
 ifndef MYSQL_INC_DIR
   ifneq ($(strip $(shell $(FIND) /usr/include -maxdepth 1 -name mysql)),/usr/include/mysql)
     $(warning Install the MySQL client library on your system, or set the environment)
     $(warning variables MYSQL_INC_DIR and MYSQL_LIB_DIR to the directory with)
     $(warning the MySQL headers and libraries, respectively.)
     $(warning See the Qweak Wiki for installation and compilation instructions.)
     $(warning ->   http://qweak.jlab.org/wiki/index.php/Software)
     $(warning )
     $(error   Error: Could not find the MySQL library)
   else
     $(info Setting MYSQL_INC_DIR to /usr/include/mysql)
     MYSQL_INC_DIR = /usr/include/mysql
     MYSQL_LIB_DIR = /usr/lib/mysql
   endif
 endif

 MYSQL_INC  = -I${MYSQL_INC_DIR}
 MYSQL_LIBS = -L${MYSQL_LIB_DIR} -lmysqlclient


 ############################
 ############################
 # Some set-up for the MySQL++ library (copied from $QWANALYSIS/GNUmakefile)
 ############################
 ############################
 ifndef MYSQLPP_INC_DIR
   ifneq ($(strip $(shell $(FIND) /usr/include -maxdepth 1 -name mysql++)),/usr/include/mysql++)
     ifneq ($(strip $(shell $(FIND) /usr/local/include -maxdepth 1 -name mysql++)),/usr/local/include/mysql++)
       $(warning Install the MySQL++ library on your system, or set the environment)
       $(warning variables MYSQLPP_INC_DIR and MYSQLPP_LIB_DIR to the directory with)
       $(warning the MySQL++ headers and libraries, respectively.)
       $(warning See the Qweak Wiki for installation and compilation instructions.)
       $(warning ->   http://qweak.jlab.org/wiki/index.php/Software)
       $(warning )
       $(error   Error: Could not find the MySQL++ library)
     else
       $(warning Setting MYSQLPP_INC_DIR to /usr/local/include/mysql++)
       MYSQLPP_INC_DIR = /usr/local/include/mysql++
       MYSQLPP_LIB_DIR = /usr/local/lib
     endif
   else
     $(info Setting MYSQLPP_INC_DIR to /usr/include/mysql++)
     MYSQLPP_INC_DIR = /usr/include/mysql++
     MYSQLPP_LIB_DIR = /usr/lib
   endif
 endif

 MYSQLPP_INC  = -I${MYSQLPP_INC_DIR}
 MYSQLPP_LIBS = -L${MYSQLPP_LIB_DIR} -lmysqlpp

 ############################
 ############################
endif

ifdef NO_MYSQL
 $(warning Compiling code without the use of mysql libraries.  This may cause some code not to compile correctly.  Currently this should only be used for the MpsOnly)
endif


ROOTLIBS   = $(shell root-config --libs ) -lSpectrum
ROOTGLIBS  = $(shell root-config --glibs)
INCLUDES   = -I$(shell root-config --incdir) -Iinclude/ ${MYSQL_INC} ${MYSQLPP_INC}
LIB        = ${MYSQL_LIBS} ${MYSQLPP_LIBS}
CC         = g++ ${INCLUDES}
SRC        = src
CFLAGS     = -O -Wall ${INCLUDES}
MYSQL      = 

all: qwbeammod qwlibra qwasymsplitter qwresidual qwmpsonly

%.o: %.cc
	${CC} ${CFLAGS} ${EXTRAFLAGS} -c -o $@ $< 

qwbeammod : ${SRC}/qwbeammod.o ${SRC}/QwModulation.o
	${CC} ${INCLUDES} -o $@  ${CFLAGS} $^ ${ROOTLIBS} ${ROOTGLIBS}
runMpsOnly : ${SRC}/runMpsOnly.o ${SRC}/QwMpsOnly.o
	${CC} ${INCLUDES} -o $@  ${CFLAGS} $^ ${ROOTLIBS} ${ROOTGLIBS}
qwlibra : ${SRC}/qwlibra.o ${SRC}/QwData.o ${SRC}/QwDiagnostic.o
	${CC} ${INCLUDES} -o $@  ${CFLAGS} $^ ${ROOTLIBS} ${ROOTGLIBS}
qwcharge: ${SRC}/qwcharge.o ${SRC}/QwChargeSensitivity.o
	${CC} ${INCLUDES} -o $@  ${CFLAGS} $^ ${ROOTLIBS} ${ROOTGLIBS}
qwdbplot: ${SRC}/qwdbplot.o ${SRC}/QwDBConnection.o ${SRC}/QwDataContainer.o
	${CC} ${INCLUDES} -o $@  ${CFLAGS} $^ ${ROOTLIBS} ${ROOTGLIBS}  ${LIB}
qwdboptics: ${SRC}/qwdboptics.o ${SRC}/QwDBConnection.o ${SRC}/QwDataContainer.o
	${CC} ${INCLUDES} -o $@  ${CFLAGS} $^ ${ROOTLIBS} ${ROOTGLIBS}  ${LIB}
qw_ndelta: ${SRC}/qw_ndelta.o ${SRC}/QwDBConnection.o ${SRC}/QwDataContainer.o ${SRC}/QwPlotHelper.o
	${CC} ${INCLUDES} -o $@  ${CFLAGS} $^ ${ROOTLIBS} ${ROOTGLIBS}  ${LIB}
ramp_pedestal: ${SRC}/ramp_pedestal.o 	
	${CC} ${INCLUDES} -o $@  ${CFLAGS} $^ ${ROOTLIBS} ${ROOTGLIBS}  ${LIB}
asymmetry_plotter: ${SRC}/asymmetry_plotter.o 	
	${CC} ${INCLUDES} -o $@  ${CFLAGS} $^ ${ROOTLIBS} ${ROOTGLIBS}  ${LIB}
qwslug: ${SRC}/qwslug.o ${SRC}/QwSlug.o 	
	${CC} ${INCLUDES} -o $@  ${CFLAGS} $^ ${ROOTLIBS} ${ROOTGLIBS}  ${LIB}
qwcorrelation: ${SRC}/qwcorrelation.o ${SRC}/QwSlug.o 	
	${CC} ${INCLUDES} -o $@  ${CFLAGS} $^ ${ROOTLIBS} ${ROOTGLIBS}  ${LIB}
qwregrun: ${SRC}/qwregrun.o ${SRC}/QwSlug.o 	
	${CC} ${INCLUDES} -o $@  ${CFLAGS} $^ ${ROOTLIBS} ${ROOTGLIBS}  ${LIB}
qwasymsplitter: ${SRC}/qwasymsplitter.o ${SRC}/QwModulation.o 	
	${CC} ${INCLUDES} -o $@  ${CFLAGS} $^ ${ROOTLIBS} ${ROOTGLIBS}  ${LIB}
qwresidual : ${SRC}/qwresidual.o ${SRC}/QwModulation.o
	${CC} ${INCLUDES} -o $@  ${CFLAGS} $^ ${ROOTLIBS} ${ROOTGLIBS}	
qwbpmplot: ${SRC}/qwbpmplot.o ${SRC}/QwDBConnection.o ${SRC}/QwDataContainer.o
	${CC} ${INCLUDES} -o $@  ${CFLAGS} $^ ${ROOTLIBS} ${ROOTGLIBS}  ${LIB}	
qwdetplot: ${SRC}/qwdetplot.o ${SRC}/QwDBConnection.o ${SRC}/QwDataContainer.o
	${CC} ${INCLUDES} -o $@  ${CFLAGS} $^ ${ROOTLIBS} ${ROOTGLIBS}  ${LIB}	
qwfileplot: ${SRC}/qwfileplot.o ${SRC}/QwDataFile.o 
	${CC} ${INCLUDES} -o $@  ${CFLAGS} $^ ${ROOTLIBS} ${ROOTGLIBS}  ${LIB}	
qwfilemplot: ${SRC}/qwfilemplot.o ${SRC}/QwDataFile.o 
	${CC} ${INCLUDES} -o $@  ${CFLAGS} $^ ${ROOTLIBS} ${ROOTGLIBS}  ${LIB}	
qwnormal: ${SRC}/qwnormal.o ${SRC}/QwModulation.o 
	${CC} ${INCLUDES} -o $@  ${CFLAGS} $^ ${ROOTLIBS} ${ROOTGLIBS}  ${LIB}	
qwcomposite: ${SRC}/qwcomposite.o ${SRC}/QwDataContainer.o ${SRC}/QwModulation.o ${SRC}/QwDBConnection.o
	${CC} ${INCLUDES} -o $@  ${CFLAGS} $^ ${ROOTLIBS} ${ROOTGLIBS}  ${LIB}	
clean: 
	rm -f *.o ~* src/*.o src/*.cc~ include/*.hh~ qwbeammod qwlibra qwcharge qwdbplot qw_ndelta qwslug qwcorrelation qwregrun qwasymsplitter qwresidual qwdboptics

