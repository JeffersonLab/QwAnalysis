# Makefile for Qweak analysis code : make required
# P. M. King
# 2006-11-07
#
# Modifications:
#----------------------------------------------------------------------
#
#
#----------------------------------------------------------------------

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
GCC      := gcc
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



############################
############################
# Modularity :
#
#   The "EXCLUDEDIRS" list should contain directories which are not part
#   of the QwAnalysis standard package, and which should not be built
#   by this Makefile.  If they should be built automatically, a call
#   to their own Makefile will be made from a specfic target.
#   See the "coda_lib" target, as an example.
#
############################
############################

EXCLUDEDIRS = coda Extensions

ifeq ($(strip $(shell $(ECHO) $$(if [ -e .EXES ]; then $(CAT) .EXES; fi))),)
 ifneq ($(CODA),)
  #  The realtime executables should be added in this section.
  EXES := qwtracking qwsimtracking qwanalysis_adc qwanalysis_beamline
 else
  EXES := qwtracking qwsimtracking qwanalysis_adc qwanalysis_beamline
 endif
else
 EXES := $(shell $(ECHO) $$(if [ -e .EXES ]; then $(CAT) .EXES; fi))
endif
ifeq ($(filter config,$(MAKECMDGOALS)),config)
 ifneq ($(CODA),)
  #  The realtime executables should be added in this section.
  EXES := qwtracking qwsimtracking qwanalysis_adc qwanalysis_beamline
 else
  EXES := qwtracking qwsimtracking qwanalysis_adc qwanalysis_beamline
 endif
endif
# overridden by "make 'EXES=exe1 exe2 ...'"

ifneq ($(filter qwrealtime,$(EXES)),)
 ifeq ($(CODA),)
  $(error qwrealtime requires CODA)
  # With version 3.77 or earlier of make, the message is simply 'missing separator.  Stop.'
 endif
endif



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
ROOTLIBS     := $(shell $(ROOTCONFIG) --new --libs) -lTreePlayer -lGX11
        # -lNew : for map file capability
        # -lTreePlayer -lProof : for user loops calling tree
        #                        variables under conditions
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
# They are set when $(QWANALYSIS)/SetupFiles/.QwSetup.csh (or .bash)
# is sourced prior to the call for this Makefile.
# A priori they won't be modified. They are (don't uncomment) :
# QWANALYSIS := /home/lenoble/QwAnalysis
# Not the actual value, but $(MAKE) is run from
# this directory
# QWBIN      := $(QWANALYSIS)/bin
# QWLIB      := $(QWANALYSIS)/lib
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
  $(error Aborting : QWANALYSIS variable is not set.  Source the SetupFiles/.QwSetup.csh script first)
endif
ifneq ($(strip $(QWANALYSIS)),$(strip $(shell pwd)))
  $(error Aborting : QWANALYSIS variable disagrees with the working directory.  Source the SetupFiles/.QwSetup.csh script first)
endif

ifndef QWBIN
  $(warning Warning : QWBIN variable is not defined.  Setting to QWANALSYIS/bin.)
  QWBIN := $(QWANALYSIS)/bin
endif
ifneq ($(strip $(QWBIN)),$(strip $(shell $(FIND) $(QWANALYSIS) -name bin)))
  $(error Aborting : QWBIN variable is not set properly  Source the SetupFiles/.QwSetup.csh script first)
endif

ifndef QWLIB
  $(warning Warning : QWLIB variable is not defined.  Setting to QWANALSYIS/lib.)
  QWLIB := $(QWANALYSIS)/lib
endif
ifneq ($(strip $(QWLIB)),$(strip $(shell $(FIND) $(QWANALYSIS) -name lib)))
  $(error Aborting : QWLIB variable is not set properly  Source the SetupFiles/.QwSetup.csh script first)
endif

ifndef QWEVIO
  $(warning Warning : QWEVIO variable is not defined.  Setting to QWANALSYIS/coda.)
  QWEVIO := $(QWANALYSIS)/coda
endif
ifneq ($(strip $(QWEVIO)),$(strip $(shell $(FIND) $(QWANALYSIS) -name coda)))
  $(error Aborting : QWEVIO variable is not set properly  Source the SetupFiles/.QwSetup.csh script first)
endif



############################
############################
# JLab CODA package related variables :
############################
############################

ifdef CODA
CODACFLAGS   := -I$(CODA)/common/include -D__CODA_ET
CODALIBS     := -L$(CODA_LIB) -let
endif
CODACFLAGS   += -I$(QWANALYSIS)/coda/include
CODALIBS     += -L$(QWANALYSIS)/lib -lcoda
      # -lmyevio : now integrated in our distribution (April 19 2001) ;
      # Regenerated if necessary ; I had to rewrite CODA
      # group's Makefile in $(QWEVIO)



############################
############################
# Platform dependent variables :
############################
############################
LIBTOOL = $(LD)

ifeq ($(ARCH),Linux)

CXX            := gcc
CXXFLAGS       := -Wall -fPIC
OPTIM          := -O2
LD             = gcc
LDFLAGS	       = -Wl,-rpath,$(QWLIB)
LDLIBS         =
SOFLAGS        = -shared

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
OPTIM         := -xO2
LD             = CC
LDFLAGS        =
LDLIBS         = -lm -lposix4  -lsocket -lnsl -lresolv -ldl
# These flags were suggested by Carl Timmer on 30May2001 to link properly the
# code on jlabs1
# 2002Feb07, PMK; transfered the flags from "LDFLAGS" to "LDLIBS".
SOFLAGS        = -G
endif

ifeq ($(ARCH),Darwin)

CXX            := gcc-3.3
CXXFLAGS       := -Wall -fPIC
OPTIM          := -O2
LD             = gcc-3.3
LIBTOOL 	   = libtool
LDFLAGS        = -bind_at_load
LDLIBS         = -lSystemStubs
SOFLAGS        =
DllSuf        := .dylib

ROOTCFLAGS   := $(shell $(ROOTCONFIG) --cflags)
ROOTLIBS     := $(shell $(ROOTCONFIG) --libs) -lTreePlayer -lGX11 -lpthread -lThread
# --new give a runtime error on darwin and root 4.04 :
# <CustomReAlloc2>: passed oldsize 64, should be 0
# Fatal in <CustomReAlloc2>: storage area overwritten
# aborting

endif


############################
############################
# Some set-up for the Boost library use
############################
############################
ifndef BOOST_INC_DIR
  ifneq ($(strip $(shell $(FIND) /usr/include -maxdepth 1 -name boost)),/usr/include/boost)
    $(warning Install the Boost library on your system, or set the environment)
    $(warning variables BOOST_INC_DIR and BOOST_LIB_DIR to the directory with)
    $(warning the Boost headers and libraries, respectively.)
    $(warning See the Qweak Wiki for installation and compilation instructions.)
    $(warning ->   http://qweak.jlab.org/wiki/index.php/Software)
    $(warning )
    $(error   Error: Could not find the Boost library)
  endif
  BOOST_INC_DIR = /usr/include/boost
  BOOST_LIB_DIR = /usr/lib
  BOOST_VERSION = $(shell perl -ane "print /\#define\s+BOOST_LIB_VERSION\s+\"(\S+)\"/" $(BOOST_INC_DIR)/version.hpp)
  BOOST_INC  =
  BOOST_LIBS =
else
  BOOST_VERSION = $(shell perl -ane "print /\#define\s+BOOST_LIB_VERSION\s+\"(\S+)\"/" ${BOOST_INC_DIR}/version.hpp)
  BOOST_INC  = -I${BOOST_INC_DIR}
  BOOST_LIBS = -L${BOOST_LIB_DIR}
endif

#  We should also put a test on the boost version number here.
ifeq ($(BOOST_VERSION),)
  $(error   Error: Could not determine Boost version)
endif

#  List the Boost libraries to be linked to the analyzer.
ifeq ($(strip $(shell $(FIND) $(BOOST_LIB_DIR) -maxdepth 1 -name libboost_filesystem-mt.so)),$(BOOST_LIB_DIR)/libboost_filesystem-mt.so)
  BOOST_LIBS += -lboost_filesystem-mt -lboost_program_options-mt
else
  BOOST_LIBS += -lboost_filesystem -lboost_program_options
endif

BOOST_LIBS += -ldl

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


INCFLAGS += $(BOOST_INC) -I./
# Necessary for dictionary files where include files are quoted with relative
# path appended (default behaviour for root-cint)

CPPFLAGS = $(INCFLAGS) $(ROOTCFLAGS) $(CODACFLAGS) $(sort $(DEFAULTADD) $(ADD))
# ADD should be contained in DEFAULTADD, but DEFAULTADD may be tempered with...

CXXFLAGS += $(OPTIM) $(DEBUG)


ifneq ($(CXX),CC)
  LDLIBS      += -lstdc++
  LDLIBS      += -lz
endif
LIBS =  -L$(QWLIB) -lQw
LIBS +=  $(ROOTLIBS) $(ROOTGLIBS) $(CODALIBS)
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

INTO_RELATIVE_PATH  = $(SED) 's/\//xxqqqqqxx/g' | $(SED) 's/$(subst /,xxqqqqqxx,$(QWANALYSIS))/./g' | $(SED) 's/xxqqqqqxx/\//g'
# To be piped in
# The special meaning of '/' in regular expressions is painful
# This pipe is a trick to encapsulate the conversion from $(QWANALYSIS) to .


APPEND_BIN_PATH  = $(SED) 's/\//xxqqqqqxx/g' | $(SED) 's/\([a-z0-9_]* \)/$(subst /,xxqqqqqxx,$(QWBIN))xxqqqqqxx\1/g' | $(SED) 's/xxqqqqqxx/\//g'
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



############################
############################
# Main targets :
############################
############################

export

all: .ADD .EXES .auxDepends
ifneq ($(strip $(ADD)),)
	@if [ "$(strip $(sort $(shell $(CAT) .ADD)))" != "$(strip $(sort $(ADD)))" ]; \
	then \
	$(ECHO) ; \
	$(ECHO) \*\*\* ADD options have changed since last config; \
	$(ECHO) \*\*\* Removing involved object files... ; \
	$(ECHO) \*\*\* Rerun \'make config\' with your new options; \
	$(ECHO) \*\*\* Then rerun \'make\' \(you can omit your new options\); \
	$(ECHO) ; \
	for wd in xxxdummyxxx $(sort $(shell $(ECHO) $(filter-out $(shell $(CAT) .ADD),$(ADD)) $(filter-out $(ADD),$(shell $(CAT) .ADD)) | $(REMOVE_-D))); \
	do \
	$(RM) `$(GREP) $$wd */*/*$(IncSuf) */*/*$(SrcSuf) | $(SED) 's/^\([A-Za-z0-9\/\._]*\):.*/\1/g;s/\$(IncSuf)/\$(ObjSuf)/g;s/\$(SrcSuf)/\$(ObjSuf)/g'`; \
	done; \
	exit 1; \
	fi
endif
ifneq ($(strip $(EXES)),)
	@if [ "$(strip $(sort $(shell $(CAT) .EXES)))" != "$(strip $(sort $(EXES)))" ]; \
	then \
	$(ECHO) ; \
	$(ECHO) \*\*\* EXES choice has changed since last config; \
	$(ECHO) \*\*\* Removing involved object files... ; \
	$(ECHO) \*\*\* Rerun \'make config\' with your new options; \
	$(ECHO) \*\*\* Then rerun \'make\' \(you can omit your new options\); \
	$(ECHO) ; \
	for wd in xxxdummyxxx $(sort $(shell $(ECHO) $(filter-out $(shell $(CAT) .EXES),$(EXES)) $(filter-out $(EXES),$(shell $(CAT) .EXES)) | $(REMOVE_-D))); \
	do \
	$(RM) `$(CAT) .auxDepends | $(SED) "/$$wd/!d;s/.*$$wd: \([A-Za-z0-9\/\._]*\$(ObjSuf)\) .*/\1/"` $(QWLIB)/libQw$(DllSuf); \
	done; \
	exit 1; \
	fi
endif
####	@$(MAKE) -f $(QWEVIO)/Makefile libcoda.so
	@$(MAKE) coda_lib
#ifneq ($(CODA),)
#ifneq ($(ARCH),SunOS)
#	@cd $(VISU);$(MAKE) -f GNUmakefile
#endif
#endif
	@$(MAKE) -f .auxDepends `$(CAT) .auxExeFiles | $(SED) 's/$$/ /g' | $(APPEND_BIN_PATH) | $(INTO_RELATIVE_PATH)`


config: .ADD .EXES clean.auxfiles .auxDepends
	@for wd in xxxdummyxxx $(sort $(shell $(ECHO) $(filter-out $(shell $(CAT) .ADD),$(ADD)) $(filter-out $(ADD),$(shell $(CAT) .ADD)) | $(REMOVE_-D))); \
	do \
	$(RM) `$(GREP) $$wd */*/*$(IncSuf) */*/*$(SrcSuf) | $(SED) 's/^\([A-Za-z0-9\/\._]*\):.*/\1/g;s/\$(IncSuf)/\$(ObjSuf)/g;s/\$(SrcSuf)/\$(ObjSuf)/g'`; \
	done
	@for wd in xxxdummyxxx $(sort $(shell $(ECHO) $(filter-out $(shell $(CAT) .EXES),$(EXES)) $(filter-out $(EXES),$(shell $(CAT) .EXES)) | $(REMOVE_-D))); \
	do \
	cd $(QWBIN);$(RM) `$(LS) $(QWBIN) | $(SED) 's/CVS//g' | $(SED) 's/SunWS_cache//g'` $(QWLIB)/libQw$(DllSuf); \
	done
	@$(ECHO) $(ADD)  | $(TO_LINE) > .ADD
	@$(ECHO) $(EXES)  | $(TO_LINE) > .EXES

coda_lib:
	cd $(QWEVIO); $(MAKE) libcoda$(DllSuf)
	$(CP) $(QWEVIO)/libcoda$(DllSuf) $(QWLIB)/libcoda$(DllSuf)

.auxDepends: .auxLibFiles
	@$(ECHO) Generating .auxLibFiles
	@$(RM) .auxLibFiles
	@$(ECHO) $(QWLIB)/libQw$(DllSuf) | $(INTO_RELATIVE_PATH) > .auxLibFiles
	@$(ECHO) $(QWLIB)/libQw$(DllSuf): `$(CAT) .auxSrcFiles` `$(CAT) .auxDictFiles` \
		| $(TO_LINE) \
		| $(INTO_RELATIVE_PATH) \
		| $(SED) 's/\$(SrcSuf)/\$(ObjSuf)/g' \
		| $(ADD_ANTISLASH) \
		| $(FILTER_OUT_FOREIGN_DEPS) >> .auxDepends
	@$(ECHO) >> .auxDepends
	@$(ECHO) $(TAB)$(LIBTOOL) $(SOFLAGS) $(LDFLAGS) '$$^' -o $(QWLIB)/libQw$(DllSuf) | $(INTO_RELATIVE_PATH) >> .auxDepends
	@$(ECHO) $(TAB)@$(ECHO) >> .auxDepends
	@$(ECHO) >> .auxDepends
	@for file in `$(CAT) 2>&1 .auxMainFiles`; \
	do \
	$(ECHO) $(QWBIN)/`$(ECHO) $$file | $(SED) 's/.*\/\([A-Za-z0-9_]*\)\$(SrcSuf)/\1/;y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/'`: `$(ECHO) $$file | $(SED) 's/\$(SrcSuf)/\$(ObjSuf)/'` `$(CAT) .auxLibFiles`  | $(INTO_RELATIVE_PATH) >> .auxDepends; \
	$(ECHO) $(TAB)$(LD) $(CXXFLAGS) '$$<' $(LIBS) $(LDFLAGS) -o '$$@' | $(INTO_RELATIVE_PATH) >> .auxDepends; \
	$(ECHO) $(TAB)@$(ECHO) >> .auxDepends; \
	$(ECHO) >> .auxDepends; \
	done


.auxLibFiles: .auxLinkDefFiles
	@for file in `$(CAT) 2>&1 .auxDictFiles`; \
	do \
	$(ECHO) Writing dependencies for file $$file...; \
	$(ECHO) $$file | $(SED) 's/\(.*\/\)dictionary\(\/.*\)Dict\$(SrcSuf)/&: \1include\2\$(IncSuf)/' >> .auxDepends; \
	$(ECHO) $(TAB)@$(ROOTCINT) -f '$$@' -c $(INCFLAGS) $(CODACFLAGS) -DROOTCINTFIX "\`""$(CAT) .auxLinkDefFiles | $(GREP)" '$$<'"\`" >> .auxDepends; \
	$(ECHO) $(TAB)@$(ECHO) $(ROOTCINT) -f '$$@' -c $(INCFLAGS) $(CODACFLAGS) -DROOTCINTFIX "\`""$(CAT) .auxLinkDefFiles | $(GREP)" '$$<'"\`" >> .auxDepends; \
	$(ECHO) $(TAB)@$(ECHO) >> .auxDepends; \
	$(ECHO) >> .auxDepends; \
	done
	@for file in `$(CAT) 2>&1 .auxDictFiles | $(SED) 's/\$(SrcSuf)/\$(ObjSuf)/'`; \
	do \
	$(ECHO) Writing dependencies for file $$file...; \
	$(ECHO) $$file | $(SED) 's/\(\.[A-Za-z0-9\/_]*\)\$(ObjSuf)/&: \1\$(SrcSuf)/' >> .auxDepends; \
	$(ECHO) $(TAB)$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o '$$@' -c '$$<' >> .auxDepends; \
	$(ECHO) $(TAB)@$(ECHO) >> .auxDepends; \
	$(ECHO) >> .auxDepends; \
	done


.auxLinkDefFiles : .auxDictFiles
	@$(RM) .tmp1 .tmp2
	@$(ECHO) Generating $@
	@$(FIND) $(QWANALYSIS) | $(GREP) '\$(IncSuf)' | $(INTO_RELATIVE_PATH) | $(FILTER_OUT_LIBRARYDIR_DEPS) | $(FILTER_OUT_DOXYGEN) | $(SED) '/\.svn/d;/LinkDef/d;/Dict/d;s/\$(IncSuf)//' > .tmp1
	@$(FIND) $(QWANALYSIS) | $(GREP) LinkDef | $(INTO_RELATIVE_PATH) | $(FILTER_OUT_LIBRARYDIR_DEPS) | $(FILTER_OUT_DOXYGEN) | $(SED) '/\.svn/d;s/LinkDef\$(IncSuf)//'> .tmp2
	@for file in `$(CAT) .tmp1`; \
	do \
	if [ "`$(GREP) $$file .tmp2`" != "" ]; \
	then \
	$(ECHO) $$file$(IncSuf) $$file\LinkDef$(IncSuf) >> $@; \
	else \
	$(ECHO) $$file$(IncSuf) >> $@; \
	fi; \
	done
	@$(RM) .tmp1 .tmp2


.auxDictFiles: .auxSrcFiles
	@stem () { \
	$(BASENAME) $$1 | $(SED) 's/\$(SrcSuf)//'; \
	}; \
	$(RM) .aux; \
	for file in `$(CAT) 2>&1 .auxSrcFiles`; \
	do \
	$(RM) .tmp; \
	$(RM) .tmperror; \
	$(ECHO) Writing dependencies for file $$file...; \
	$(ECHO) `$(DIRNAME) $$file`/`$(GCC) $(CPPFLAGS) 2>.tmperror -MM $$file` \
	| $(TO_LINE) \
	| $(INTO_RELATIVE_PATH) \
	| $(ADD_ANTISLASH) \
	| $(FILTER_OUT_FOREIGN_DEPS) \
	>> .tmp; \
	if [ ! -e .tmperror ];\
	then \
	$(ECHO) "Aborting:  Unable to locate a file in the include paths:"; \
	$(CAT) .tmperror; \
	exit 1; \
	fi;\
	$(RM) .tmperror; \
	$(CAT) .tmp | $(GREP) -v "\/$$(stem $$file)\." | $(SED) "s/\/include\//\/src\//g;s/\$(IncSuf)/\$(SrcSuf)/g;s/\\\//g" >> .aux; \
	if [ "`$(CAT) .tmp | $(GREP) /In | $(SED) '/In[A-Za-z0-9\/_]/d'`" = "" ]; \
	then \
	$(ECHO)  >> .tmp; \
	$(ECHO) $(TAB)$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o '$$@' -c '$$<' >> .tmp; \
	$(ECHO) $(TAB)@$(ECHO)>> .tmp; \
	$(ECHO) >> .tmp; \
	$(CAT) .tmp | $(INTO_RELATIVE_PATH) >> .auxDepends; \
	fi; \
	done; \
	$(RM) .auxSrcFiles; \
	$(TOUCH) .auxSrcFiles; \
	$(ECHO) Checking for nested dependencies...; \
	for file in `$(CAT) 2>&1 .aux`; \
	do \
	if [ "`$(GREP) $$file .auxSrcFiles`" = "" ]; \
	then \
	if [ -f $$file ]; \
	then \
	$(ECHO) $$file | $(FILTER_OUT_LIBRARYDIR_DEPS) >> .auxSrcFiles; \
	$(ECHO) `$(GCC) $(CPPFLAGS) 2>.tmperror -MM $$file` \
	| $(TO_LINE) \
	| $(INTO_RELATIVE_PATH) \
	| $(FILTER_OUT_FOREIGN_DEPS) \
	| $(GREP) -v "\/$$(stem $$file)\." \
	| $(SED) "s/\/include\//\/src\//g;s/\$(IncSuf)/\$(SrcSuf)/g;s/\\\//g" \
	>> .aux; \
	if [ ! -e .tmperror ];\
	then \
	$(ECHO) "Aborting:  Unable to locate a file in the include paths:"; \
	$(CAT) .tmperror; \
	exit 1; \
	fi;\
	$(RM) .tmperror; \
	fi; \
	fi; \
	done; \
	$(ECHO) Checking for double-nested dependencies...; \
	for file in `$(CAT) 2>&1 .aux`; \
	do \
	if [ "`$(GREP) $$file .auxSrcFiles`" = "" ]; \
	then \
	if [ -f $$file ]; \
	then \
	$(ECHO) $$file | $(FILTER_OUT_LIBRARYDIR_DEPS) >> .auxSrcFiles; \
	$(ECHO) `$(GCC) $(CPPFLAGS) 2>&1 -MM $$file` \
	| $(TO_LINE) \
	| $(INTO_RELATIVE_PATH) \
	| $(FILTER_OUT_FOREIGN_DEPS) \
	| $(GREP) -v "\/$$(stem $$file)\." \
	| $(SED) "s/\/include\//\/src\//g;s/\$(IncSuf)/\$(SrcSuf)/g;s/\\\//g" \
	>> .aux; \
	fi; \
	fi; \
	done; \
	$(ECHO) Building list of source files...; \
	for file in `$(CAT) 2>&1 .aux`; \
	do \
	if [ "`$(GREP) $$file .auxSrcFiles`" = "" ]; \
	then \
	if [ -f  $$file ]; \
	then \
	$(ECHO) $$file | $(FILTER_OUT_LIBRARYDIR_DEPS) >> .auxSrcFiles; \
	fi; \
	fi; \
	done
	@for file in `$(CAT) 2>&1 .auxSrcFiles`; \
	do \
	$(RM) .tmp; \
	$(ECHO) Writing dependencies for file $$file...; \
	$(ECHO) `$(DIRNAME) $$file`/`$(GCC) $(CPPFLAGS) 2>&1 -MM $$file` \
	| $(TO_LINE) \
	| $(INTO_RELATIVE_PATH) \
	| $(ADD_ANTISLASH) \
	| $(FILTER_OUT_FOREIGN_DEPS) \
	>> .tmp; \
	if [ "`$(CAT) .tmp | $(GREP) /In | $(SED) '/In[A-Za-z0-9\/_]/d'`" = "" ]; \
	then \
	$(ECHO)  >> .tmp; \
	$(ECHO) $(TAB)$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o '$$@' -c '$$<' >> .tmp; \
	$(ECHO) $(TAB)@$(ECHO)>> .tmp; \
	$(ECHO) >> .tmp; \
	$(CAT) .tmp | $(INTO_RELATIVE_PATH) >> .auxDepends; \
	fi; \
	done;
	@$(RM) .tmp .aux
	@$(ECHO) Generating .auxDictFiles
	@$(RM) .auxDictFiles
	@$(TOUCH) .auxDictFiles
	@for file in `$(CAT) .auxDepends | $(SED) '/:/!d;s/\(.*\):\(.*\)/\1/;s/\$(ObjSuf)/\$(SrcSuf)/'`; \
	do \
	if [ "`$(GREP) 'ClassImp' $$file | $(SED) '/^ClassImp/!d;s/\(^ClassImp(\)\(.*\)\()\)/\2/'`" != "" ]; \
	then \
	$(ECHO) $$file | $(SED) 's/\(.*\/\)src\(\/.*\)\$(SrcSuf)/\1dictionary\2Dict\$(SrcSuf)/' | $(INTO_RELATIVE_PATH) >> .auxDictFiles; \
	fi; \
	done



.auxSrcFiles: .auxExeFiles
	@$(ECHO) Generating $@
	@for file in $(shell $(FIND) $(QWANALYSIS) | $(SED) '/\/main\//!d;/\.svn/d;/CVS/d;/\$(SrcSuf)/!d' | $(FILTER_OUT_TRASH)); \
	do \
	case `$(ECHO) $$file | $(SED) 's/.*\/\([A-Za-z0-9_]*\)\$(SrcSuf)/\1/;y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/'` in ${foreach exe,$(shell $(CAT) $<),$(exe) ${shell ${ECHO} ")"} $(ECHO) $$file | $(INTO_RELATIVE_PATH) >> $@;;} \
	esac; \
	done
	@$(CAT) .auxSrcFiles > .auxMainFiles


.auxExeFiles:
	@$(ECHO) Generating $@
	@$(ECHO) $(filter $(shell $(FIND) $(QWANALYSIS) | $(SED) '/\/main\//!d;/\.svn/d;/CVS/d;/\$(SrcSuf)/!d' | $(FILTER_OUT_TRASH) | $(SED) 's/.*\/\([A-Za-z0-9_]*\)\$(SrcSuf)/\1/;y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/'),$(EXES)) > $@


.ADD:
	@$(ECHO) $(ADD)  | $(TO_LINE) > .ADD

.EXES:
	@$(ECHO) $(EXES)  | $(TO_LINE) > .EXES

qweak-config: qweak-config.in
	@$(CAT) $< | $(SED) 's!%QWANALYSIS%!$(QWANALYSIS)!' | $(SED) 's!%LIBS%!$(LIBS)!'   \
	           | $(SED) 's!%QWLIB%!$(QWLIB)!' | $(SED) 's!%QWBIN%!$(QWBIN)!'           \
	           | $(SED) 's!%LDFLAGS%!$(LDFLAGS)!' | $(SED) 's!%CPPFLAGS%!$(CPPFLAGS)!' \
	           > bin/$@

############################
############################
# Specific clean targets :
############################
############################

clean.auxfiles:
# Removes auxiliary config files
	@$(ECHO) Removing '.aux*' files
	@$(RM) .aux*


clean.dictfiles:
# Removes all dict files '*Dict*'
	@$(ECHO) Removing *Dict$(SrcSuf), *Dict$(IncSuf) files
	@$(RM) `$(FIND) $(QWANALYSIS) | $(GREP) 'Dict\$(SrcSuf)' | $(SED) '/\$(SrcSuf)./d'`
	@$(RM) `$(FIND) $(QWANALYSIS) | $(GREP) 'Dict\$(IncSuf)' | $(SED) '/\$(IncSuf)./d'`


clean.libs:
# Removes libraries
	@$(ECHO) Removing '*$(DllSuf)' files
	@$(RM) $(QWLIB)/lib*$(DllSuf)


clean: clean.coda
# Removes all object files, '*~', '#*#' and '.#*' files
	@$(ECHO) Removing '*$(ObjSuf)' files
	@$(RM) `$(FIND) $(QWANALYSIS) | $(GREP) '\$(ObjSuf)' | $(SED) '/\$(ObjSuf)./d'`
	@$(ECHO) Removing *~, "#*#, .#*" files
	@$(RM) `$(FIND) $(QWANALYSIS) | $(GREP) '~'`
	@$(RM) `$(FIND) $(QWANALYSIS) | $(GREP) '#'`

clean.exes:
# Removes executables
	@$(ECHO) Removing executables
	@$(RM) `$(ECHO) $(QWBIN)/* | $(SED) 's/[A-Za-z0-9\/_]*CVS//'`

clean.olddotfiles:
	@$(RM) .dirs .libdepend .libdepend2 .exedepend .exedepend2 .mains .dictdepend .exes .objdepend .dicts .incdirs .srcdirs $(SETUP)/.MapFileBaseAddress



cleanSunWS_cache :
	@$(RM) -r $(filter %SunWS_cache/,$(sort $(dir $(shell $(FIND) $(QWANALYSIS)))))
	@$(RM) -r $(filter %SunWS_cache,$(sort $(shell $(FIND) $(QWANALYSIS))))

clean.coda:
	cd $(QWEVIO); $(MAKE) realclean

distclean: cleanSunWS_cache clean.dictfiles clean clean.libs clean.exes clean.auxfiles clean.olddotfiles clean.coda
# Removes all files that can be regenerated
	@$(RM) .ADD .EXES



############################
############################
# Built-in targets :
############################
############################

.PHONY : clean clean.dictfiles clean.exes clean.libs distclean clean.auxfiles config all clean.olddotfiles cleanSunWS_cache

.SUFFIXES :
.SUFFIXES : $(SrcSuf) $(IncSuf) $(ObjSuf)


