# Makefile for QwEventDisplay3D : make required
# Based (....copied...) from the makefiles for QwAnalysis and QwEventDisplay3D
#
# Changelog:
# * Sat Jul 17 2010 Juan Carlos Cornejo <cornejo@jlab.org>
# - Initial creation

############################
############################
# User's compilation and linkage flags, plus other customization variables :
############################
############################

DEBUG := -g
# Add -g if you need to debug (but you'd better
# first type 'make distclean' to enable full
# recompilation with this flag).
# Add -O0 if you find that the compiler optimizes
# away some of the variables you are interested in.

OPTIM  := -O
# Default, safe, optimization flag understood by most compliers. Set
# optmization flag(s) in the architecture dependent sections instead.

DEFAULTADD = $(ADD)
# Should contain extra flag to pass to the precompiler
# ADD is a variable you should not set in this Makefile but either
# in your.cshrc file or from the prompt line when you type
# "make [config] 'ADD=-D__PADDLES'" instead of simply "make [config]"
# Use DEFAULTADD to set extra flags you always want to have for your
# customized compiling e.g replace above line "DEFAULTADD := $(ADD)" by
# "DEFAULTADD := $(ADD) -D__PADDLES"


############################
############################
# Extensions for files :
# Ought not to be modified, thus your nomenclature should abide by
# these standards unless everybody agrees on changing them.
# (Which sure would be painful).
############################
############################

ObjSuf  := .o
SrcSuf  := .cc
ExeSuf  :=
DllSuf  := .so
IncSuf  := .h


############################
############################
MYRELATIVEPATH:=/Extensions/EventDisplay3D
############################
############################

############################
############################
# Shell commands called :
# You might have to modify default flags and/or choice of command to port
# the Makefile.
############################
############################

AWK      := awk
BASENAME := basename
CAT      := cat
CD       := cd
CHMOD    := chmod
DIRNAME  := dirname
ECHO     := echo
FIND     := find
GCC      := g++
      # This must be the GNU compiler collection : explicit use of
      # flag '-M' for automatic search for dependencies
      # It is not correlated to $(CXX) and $(LD) which depend on $(ARCH)
GREP     := grep
LS       := ls
MAKE     := make
RM       := \rm -f
      # 'rm' is often aliases as 'rm -i', so '\rm' instead
CP       := \cp
ROOTCINT := rootcint
SED      := sed -e
TOUCH    := touch



############################
############################
# Environment :
############################
############################

ifeq ($(QwSHELL),)
SHELL := bash
else
SHELL := $(QwSHELL)
endif
      # Warning : Explicit use of statements "for in; do ;done;"
      # and "if []; then ; fi;
      # Should be bash shell or alike (ksh on ifarms1)
ARCH  := $(shell uname)
      # Operating system

#  If an OS and hardware specific subdirectory is present,
#  we will use the "bin" and "lib" directories in it,
#  instead of the base "bin" and "lib" directories.
OS_HW_NAME  := $(uname -s -m | sed 's/ /_/g')
INSTALL_DIR := $(strip $(shell $(ECHO) $(QWANALYSIS)$(MYRELATIVEPATH)/$$( (if [ -d $(OS_HW_NAME)]; then $(ECHO) "/"$(OS_HW_NAME); fi))))

############################
############################
# Cern ROOT package related variables :
############################
############################


ifndef ROOTSYS
  ROOTSYS := $(shell root-config --prefix)
  ifndef ROOTSYS
    $(warning ROOTSYS variable is not defined and root-config did not return a location.)
    $(warning Use the script SetupFiles/SET_ME_UP.csh or SetupFiles/SET_ME_UP.bash first.)
    $(warning See the Qweak Wiki for installation and compilation instructions.)
    $(warning ->   http://qweak.jlab.org/wiki/index.php/Software)
    $(warning )
    $(error   Error: No ROOT installation could be found)
  endif
endif
ROOTCONFIG   := $(ROOTSYS)/bin/root-config
## ROOTDEFINE   := $(shell $(ROOTCONFIG) --features | $(SED) 's/\(\s*\)\([a-zA-Z0-9_]*\)/\1-D__ROOT_HAS_\2/g;y/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/')
ROOTCFLAGS   := $(shell $(ROOTCONFIG) --cflags)
ROOTLIBS     := $(shell $(ROOTCONFIG) --new --libs --evelibs) -lTreePlayer -lGui
ROOTGLIBS    := $(shell $(ROOTCONFIG) --glibs)

# -lMathMore : for using ROOT advanced math library
ifeq ($(shell $(ROOTCONFIG) --has-mathmore),yes)
  ROOTCFLAGS += -D__ROOT_HAS_MATHMORE
  ROOTLIBS += -lMathMore
else
  $(warning The ROOT MathMore plugin was not found on your system.  Expect reduced functionality.)
endif


############################
############################
# Qw Paths :
# They are set when $(QWANALYSIS)/SetupFiles/.Qwcshrc (or .bash)
# is sourced prior to the call for this Makefile.
# A priori they won't be modified. They are (don't uncomment) :
# QWANALYSIS := /home/lenoble/QwAnalysis
# Not the actual value, but $(MAKE) is run from
# this directory
# QW_BIN      := $(QWANALYSIS)/bin
# QW_LIB      := $(QWANALYSIS)/lib
############################
############################
#  These next lines check the paths and exit if there is a problem.
############################
############################

ifndef QWANALYSIS
  $(warning Warning : QWANALYSIS variable is not defined.  Setting to current directory.)
  QWANALYSIS := $(shell pwd)
endif
ifeq ($(strip $(QWANALYSIS)),)
  $(error Aborting : QWANALYSIS variable is not set.  Source the SetupFiles/.Qwcshrc script first)
endif
ifneq ($(shell test $(QWANALYSIS)$(MYRELATIVEPATH) -ef $(shell pwd) || echo false),)
  $(warning Target directory is $(QWANALYSIS)$(MYRELATIVEPATH).)
  $(error Aborting : QWANALYSIS variable disagrees with the working directory.  Source the SetupFiles/.Qwcshrc script first)
endif

ifndef QW_BIN
  $(warning Warning : QW_BIN variable is not defined.  Setting to QWANALSYIS/bin.)
  QW_BIN := $(QWANALYSIS)/bin
endif

EVIO := $(QWANALYSIS)/evio




MYSRCDIR = ./src
MYINCDIR = ./include
MYDICTDIR= ./dictionary

VPATH    = ./main:./src


##################
###  Get the flags from the QWANALYSIS build
##################

QWEAKLIBS     := $(shell ${QWANALYSIS}/bin/qweak-config --libs)
QWEAKLDFLAGS  := $(shell ${QWANALYSIS}/bin/qweak-config --ldflags)
#  When we get the QwAnalysis include paths, turn paths like "./*" into "${QWANALYSIS}/*".
REGEXP_QWANA  := $(shell ${ECHO} ${QWANALYSIS} | ${SED} 's/\//\\\//g')
QWEAKCPPFLAGS := $(shell ${QWANALYSIS}/bin/qweak-config --cppflags | sed 's/I\./I${REGEXP_QWANA}/g' )

##################

############################
############################
# JLab CODA package related variables :
############################
############################

ifdef CODA
CODACFLAGS   := -I$(CODA)/common/include -D__CODA_ET
CODALIBS     := -L$(CODA_LIB) -let
endif
CODACFLAGS   += -I$(EVIO)/include
CODALIBS     += -L$(QW_LIB) -lmyevio
      # -lmyevio : now integrated in our distribution (April 19 2001) ;
      # Regenerated if necessary ; I had to rewrite CODA
      # group's Makefile in $(EVIO)



############################
############################
# Some set-up for the Boost library use
############################
############################
ifndef BOOST_INC_DIR
  ifneq ($(strip $(shell $(FIND) /usr/include -maxdepth 1 -name boost)),/usr/include/boost)
    $(warning Install the Boost library on your system, or set the environment)
    $(warning variables BOOST_INC_DIR and BOOST_LIB_DIR to the directory with)
    $(warning the Boost headers and libraries, respectively. The headers should)
    $(warning be in BOOST_INC_DIR/boost, and the library files in BOOST_LIB_DIR.)
    $(warning See the Qweak Wiki for installation and compilation instructions.)
    $(warning ->   http://qweak.jlab.org/wiki/index.php/Software)
    $(warning )
    $(error   Error: Could not find the Boost library)
  endif
  BOOST_INC_DIR = /usr/include
  BOOST_LIB_DIR = /usr/lib
  BOOST_INC  =
  BOOST_LIBS =
else
  ifneq ($(strip $(shell $(FIND) $(BOOST_INC_DIR)/boost -maxdepth 1 -name version.hpp)),$(BOOST_INC_DIR)/boost/version.hpp)
    $(warning Set the environment variables BOOST_INC_DIR and BOOST_LIB_DIR to)
    $(warning the directory with the Boost headers and libraries, respectively.)
    $(warning The Boost header files should be in BOOST_INC_DIR/boost, and the)
    $(warning library files in BOOST_LIB_DIR.)
    $(warning See the Qweak Wiki for installation and compilation instructions.)
    $(warning ->   http://qweak.jlab.org/wiki/index.php/Software)
    $(warning )
    $(error   Error: Could not find the Boost library)
  else
    BOOST_INC  = -I${BOOST_INC_DIR}
    BOOST_LIBS = -L${BOOST_LIB_DIR}
  endif
endif

#  Check to see if BOOST_INC_DIR is equal to /usr/include;
#  if so, clear the BOOST_INC flag, but leave BOOST_LIBS unchanged
ifeq ($(shell test $(BOOST_INC_DIR) -ef /usr/include || echo false),)
  BOOST_INC  =
endif

#  We should also put a test on the boost version number here.
BOOST_VERSION = $(shell perl -ane "print /\#define\s+BOOST_VERSION\s+(\S+)/" ${BOOST_INC_DIR}/boost/version.hpp)
ifeq ($(BOOST_VERSION),)
  $(error   Error: Could not determine Boost version)
endif
ifneq ($(shell test $(BOOST_VERSION) -lt "103300" && echo "boost_not_supported"),)
  $(warning Warning: Boost libraries probably too old)
  $(warning Boost library support only for versions 1.33.0 and up)
endif

#  List the Boost libraries to be linked to the analyzer.
ifeq ($(strip $(shell $(FIND) $(BOOST_LIB_DIR) -maxdepth 1 -name libboost_filesystem-mt.so)),$(BOOST_LIB_DIR)/libboost_filesystem-mt.so)
  # If multi-threaded libraries exist, use them instead of single threaded libraries
  BOOST_LIBS += -lboost_filesystem-mt -lboost_program_options-mt
else
  # Otherwise use the single threaded libraries
  BOOST_LIBS += -lboost_filesystem -lboost_program_options
endif

#  Before boost::filesystem 1.35.0 the system functionality is inside filesystem
ifeq ($(shell test $(BOOST_VERSION) -lt "103500" && echo "boost_system_in_filesystem"),)
  ifeq ($(strip $(shell $(FIND) $(BOOST_LIB_DIR) -maxdepth 1 -name libboost_system-mt.so)),$(BOOST_LIB_DIR)/libboost_system-mt.so)
    BOOST_LIBS += -lboost_system-mt
  else
    BOOST_LIBS += -lboost_system
  endif
endif

BOOST_LIBS += -ldl



############################
############################
# Platform dependent variables :
############################
############################
LIBTOOL = $(LD)

ifeq ($(ARCH),Linux)

CXX            := $(GCC)
CXXFLAGS       := -Wall -fPIC
OPTIM          := $(OPTIM)
LD             = $(GCC)
LDFLAGS	       = -Wl,-rpath,$(QW_LIB)
LDLIBS         =
SOFLAGS        = -shared
LINKER        :=$(CXX)
ROOTCFLAGS   := $(ROOTCFLAGS) -D_REENTRANT
        # -D_REENTRANT : '$(ROOTCONFIG) --cflags' gives incomplete result
        #                on some environment

ROOTLIBS     := $(ROOTLIBS) -lpthread  -lThread
        # -lpthread : because '$(ROOTCONFIG) --libs' gives incomplete result
        #             on gzero and libet.so requires it
        # -lThread:   Required for compilation on Linux systems with
        #             ROOT 4.04/02 or 5.08/00 (first noted by J-S Real
        #             on 3FEB2006)
endif

ifeq ($(ARCH),SunOS)
CXX            = CC
CXXFLAGS       = -KPIC
OPTIM         := $(subst "-","-x",$(OPTIM))
LD             = CC
LDFLAGS        =
LDLIBS         = -lm -lposix4  -lsocket -lnsl -lresolv -ldl
# These flags were suggested by Carl Timmer on 30May2001 to link properly the
# code on jlabs1
# 2002Feb07, PMK; transfered the flags from "LDFLAGS" to "LDLIBS".
SOFLAGS        = -G
endif

ifeq ($(ARCH),Darwin)

## MacOS 10.5 shipped with g++-4.0 (default) and g++-4.2.
## The older g++ doesn't properly support "covariant returns," so
## specify the newer one in this section.
## Override on the command line with "make GCC=g++-whatever"

GCC            := g++-4.2
CXX            := $(GCC)
CXXFLAGS       := -Wall -fPIC
OPTIM          := $(OPTIM)
LD             = $(GCC)
LIBTOOL 	   = libtool
LDFLAGS        =
LDLIBS         = -lSystemStubs
SOFLAGS        =
DllSuf        := .dylib

ROOTCFLAGS   := $(shell $(ROOTCONFIG) --cflags)
ROOTLIBS     := $(ROOTLIBS) -lTreePlayer -lGX11 -lpthread -lThread 
# --new give a runtime error on darwin and root 4.04 :
# <CustomReAlloc2>: passed oldsize 64, should be 0
# Fatal in <CustomReAlloc2>: storage area overwritten
# aborting

ifeq ($(strip $(shell $(FIND) $(BOOST_LIB_DIR) -maxdepth 1 -name libboost_filesystem-mt.so)),$(BOOST_LIB_DIR)/libboost_filesystem-mt.so)
  BOOST_LIBS += -lboost_system-mt
else
  BOOST_LIBS += -lboost_system
endif


endif

#####

##############


############################
############################
# Some set-up for the MySQL library
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
    $(warning Setting MYSQL_INC_DIR to /usr/include/mysql)
    MYSQL_INC_DIR = /usr/include/mysql
    MYSQL_LIB_DIR = /usr/lib/mysql
  endif
endif

MYSQL_INC  = -I${MYSQL_INC_DIR}
MYSQL_LIBS = -L${MYSQL_LIB_DIR} -lmysqlclient

############################
############################
# Some set-up for the MySQL++ library
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
    $(warning Setting MYSQLPP_INC_DIR to /usr/include/mysql++)
    MYSQLPP_INC_DIR = /usr/include/mysql++
    MYSQLPP_LIB_DIR = /usr/lib
  endif
endif

MYSQLPP_INC  = -I${MYSQLPP_INC_DIR}
MYSQLPP_LIBS = -L${MYSQLPP_LIB_DIR} -lmysqlpp

############################
############################
# A few fixes :
############################
############################

ifeq ($(strip $(shell $(QWANALYSIS)/SetupFiles/checkrootversion | $(GREP) WARNING | $(SED) 's/\*//g')),WARNING)
$(error Aborting : ROOT version 3.01/02 or later required)
# With version 3.77 or earlier of make, the message is simply 'missing separator.  Stop.'
endif

ifeq ($(strip $(shell $(QWANALYSIS)/SetupFiles/checkrootversion | $(GREP) older)),but older than 3.01/06)
DEFAULTADD += -DTGFILEDIALOG_FIX
endif

ifeq ($(CXX),)
$(error $(ARCH) invalid architecture)
# With version 3.77 or earlier of make, the message is simply 'missing separator.  Stop.'
endif



############################
############################
# Various 'merged' flags for $(CXX) and co :
############################
############################

INCFLAGS =  $(patsubst %,-I%,$(sort $(dir $(shell $(FIND) $(QWANALYSIS) | $(GREP) '\$(IncSuf)' | $(SED) '/\$(IncSuf)./d' | $(FILTER_OUT_TRASH) | $(INTO_RELATIVE_PATH) |  $(FILTER_OUT_LIBRARYDIR_DEPS)))))

# Qw include paths : /SomePath/QwAnalysis/Analysis/include/Foo.h -> -I./Analysis/include/

INCFLAGS += $(MYSQL_INC) $(MYSQLPP_INC)
INCFLAGS += $(BOOST_INC) -I./

# Necessary for dictionary files where include files are quoted with relative
# path appended (default behaviour for root-cint)

CPPFLAGS = $(INCFLAGS) $(ROOTCFLAGS) $(CODACFLAGS) $(sort $(DEFAULTADD) $(ADD)) $(QWEAKCPPFLAGS) $(DEBUG)
# ADD should be contained in DEFAULTADD, but DEFAULTADD may be tempered with...

CXXFLAGS += $(OPTIM) $(DEBUG)


ifneq ($(CXX),CC)
  LDLIBS      += -lstdc++
  LDLIBS      += -lz
endif
LIBS =  -L$(QW_LIB) -lQw
LIBS +=  $(ROOTLIBS) $(ROOTGLIBS) $(CODALIBS)
LIBS +=  $(MYSQL_LIBS) $(MYSQLPP_LIBS)
LIBS +=  $(BOOST_LIBS) $(LDLIBS)

############################
############################
# Miscellaneous
############################
############################

TAB   = '	'# <--- This is a tab character, for clarity in rules
                 # Don't touch !

SPACE = ' '# <--- Space character, for clarity


############################
############################
# Tricky variables (type 1 to prevent premature interpretation of special
# characters)
############################
############################

FILTER_OUT_TRASH    = $(SED) '/~$$/d' | $(SED) '/\#/d' | $(SED) '/JUNK/d'
# FILTER_OUT_TRASH pipes stream and filters out '~', '.#' and '#*#'
# typical editor backup file names

FILTER_OUT_DOXYGEN    = $(SED) '/Doxygen/d'
# FILTER_OUT_DOXYGEN pipes stream and filters out 'Doxygen'
# where the html documentation tree is stored

INTO_RELATIVE_PATH  := $(SED) 's/\//xxqqqqqxx/g' | $(SED) 's/$(subst /,xxqqqqqxx,$(QWANALYSIS)$(MYRELATIVEPATH))/./g' | $(SED) 's/xxqqqqqxx/\//g'
# To be piped in
# The special meaning of '/' in regular expressions is painful
# This pipe is a trick to encapsulate the conversion from $(QWANALYSIS) to .


APPEND_BIN_PATH  = $(SED) 's/\//xxqqqqqxx/g' | $(SED) 's/\([a-z0-9_]* \)/$(subst /,xxqqqqqxx,$(QW_BIN))xxqqqqqxx\1/g' | $(SED) 's/xxqqqqqxx/\//g'
# To be piped in, all letters to lower already...
# The special meaning of '/' in regular expressions is painful...

TO_LINE = $(AWK) 'BEGIN {RS="\\"};{print $$0}' | $(AWK) 'BEGIN {RS=" "};{print $$0}'

ADD_ANTISLASH = $(SED) 's/$$/ \\/'

REMOVE_-D = $(SED) 's/-D//g'

FILTER_OUT_FOREIGN_DEPS =  $(SED) 's/^\([A-Za-z_]\)/\.\/\1/' | $(GREP) "\./"
# To be piped in
# To filter out non Qw include file names generated by $(GCC) -M
# Expects paths to be already relative, but preceded by './' : the call
# to $(SED) corrects potential 'QwROOT/src/Foo.C' into './QwROOT/src/Foo.C'
# After the 3 $(AWK) calls, all piped in names are on disctinct lines
# $(GREP) keeps Qw related lines

FILTER_OUT_LIBRARYDIR_DEPS = $(SED) '$(patsubst %,/^.\/%/d;,$(EXCLUDEDIRS))'

#PROCESS_GCC_ERROR_MESSAGE =  $(SED) 's/In file/xxqqqqqxx/' | $(AWK) 'BEGIN {RS="qqqxx"};{print $$0}' | $(SED) '/included/d'
# Obsolete : needed On_ONE_LINE, but incompatible with SunOS
# To be piped in
# 'In file' is first words of the error message when $(GCC) -MM failed
# Encapsulate controled removal of this error message and leaves xxqq tags


# ON_ONE_LINE = $(SED) 's/$$/ \\/' | $(SED) ':b;/\\$$/{;N;s/\\\n//;bb;}'
# Incompatible with 'sed' on SunOS
# To be piped in
# Gets everything on one line

#############
##############

MYSRCDIR = ./src
MYINCDIR = ./include

VPATH    = ./main:./src



MYSRCFILES= $(shell find $(MYSRCDIR) -name \*.cc -or -name \*.c)
MYINCFILES= $(shell find $(MYINCDIR) -name \*.h)
MYDICTFILES= $(shell find $(MYDICTDIR) -name \*.cc)

MYOBJFILES= $(shell echo $(MYSRCFILES)|sed -e 's/\.cc/\.o/g' -e 's/\.c/\.o/g'| $(GREP) -v dictionary/)
MYDICTOBJFILES= $(shell echo $(MYDICTFILES)|sed -e 's/\.cc/\.o/g' -e 's/\.c/\.o/g')

MYINCFLAGS =  $(patsubst %,-I%,$(sort $(dir $(shell $(FIND) $(MYINCDIR) | $(GREP) '\$(IncSuf)' | $(SED) '/\$(IncSuf)./d' | $(FILTER_OUT_TRASH) | $(INTO_RELATIVE_PATH) |  $(FILTER_OUT_LIBRARYDIR_DEPS)))))

CPPFLAGS += $(MYINCFLAGS)
##############

all:  dictionary qweventdisplay3d

qweventdisplay3d: $(MYDICTOBJFILES) main/QwEventDisplay3D.o $(MYOBJFILES)
	@$(ECHO) "Making Executable QwEventDisplay3D" $@
	${LINKER} ${CFLAGS}  ${filter-out %.a %.so dictionaries, $^}  ${LDFLAGS} ${LIBS} -o  $@   

.auxDictFiles:
	#Find the files that require directories built
	@mkdir -p dictionary
	@$(ECHO) "Will build dictionary files for:"
	@for file in $(MYSRCFILES);\
	do \
	if [ "`$(GREP) 'ClassImp' $$file | $(SED) '/^ClassImp/!d;s/\(^ClassImp(\)\(.*\)\()\)/\2/'`" != "" ]; \
	then \
	$(ECHO) "	$$file";\
	$(ECHO) $$file >> .auxDictFiles; \
	fi \
	done
	@touch .auxDictFiles

dictionary: .auxDictFiles 
	@$(ECHO) "################################### Building Dictionary Files"
	@for file in `cat .auxDictFiles`; \
	do \
	$(ECHO) `basename $$file .cc`; \
	$(ECHO) $(ROOTCINT) -f dictionary/`basename $$file` -c $(INCFLAGS) include/`basename $$file .cc`.h; \
	$(ROOTCINT) -f dictionary/`basename $$file` -c $(INCFLAGS) include/`basename $$file .cc`.h; \
	done
	@$(ECHO) "################################### Success! (dictionaries)"

dictionary/%.o: dictionary/%.cc dictionary/%.h
	@echo  "###### Building dictionaries (again)"
	$(CXX)  ${CFLAGS}  ${CPPFLAGS} -o $@ -c $< 
	@echo  "######"

main/%.o: main/%.cc $(MYINCFILES)
	@echo  "###### Building files under main/"
	$(CXX)  ${CFLAGS}  ${CPPFLAGS} -o $@ -c $<
	@echo  "######"

src/%.o: src/%.cc include/%.h
	@echo  "###### Building files under src/" 
	$(CXX)  ${CFLAGS}  ${CPPFLAGS} -o $@ -c $< 
	@echo  "######"

.auxSrcFile:
	@$(ECHO) "Building source files"

clean: clean.auxfiles
	@$(ECHO) "Cleaning old binary files"
	$(RM) -rf *.o */*.o core qweventdisplay3d *~ */*~ dictionary/*

clean.auxfiles:
	@$(ECHO) "Cleaning old auxilary files"
	$(RM) -rf .auxDictFiles .auxSrcFiles .EXES

############################
############################
# Built-in targets :
############################
############################

.PHONY : all clean.auxfiles all

.SUFFIXES :
.SUFFIXES : $(SrcSuf) $(IncSuf) $(ObjSuf)

