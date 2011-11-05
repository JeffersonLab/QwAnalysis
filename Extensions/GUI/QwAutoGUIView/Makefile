# This is the make file to compile the QwGUI program
#
# It is derived form the file in $ROOTSYS/test/Makefile, which
# was written by the "root" people....

-include ../VERSION

LIBDIR        = #../QwGUILib
LIBINCLUDE    = #$(LIBDIR)/include
RDATALIB      = #$(LIBDIR)/QwGUIRootLib-$(VERS).a
PRG_INCLUDE   = ./include
PRG_SOURCE    = ./src

# Here we list the object files that require a CINT dictionary.
OBJECTS	      =
OBJECTS       += QwAutoGUI.o

# Have this after all the object files are listed so that dictionaries
# will be made automatically.
DICTS		= $(OBJECTS:.o=Dict.o)

APPLIC        = QwAutoGUI-$(VERS)
MFILE         = Makefile

# For Linux with egcs, gcc 2.9x, gcc 3.x (>= RedHat 5.2)
# the correct flag settings are:

DEBUGFLAGS    = #-DQWGUI_DEBUG
CXX           = g++
CXXFLAGS      = -O2 -Wall -fPIC -DVERS="\"$(VERS)\"" #-g #-O2
LD            = g++
LDFLAGS       = #-O
SOFLAGS       = -shared
SYSLIBS       =
GEOMLIB       = -lGeom
GUILIB        = -lGuiHtml

ROOTGLIBS    := $(shell root-config --glibs)
QWEAKLIBS     := $(shell ${QWANALYSIS}/bin/qweak-config --libs)
QWEAKLDFLAGS  := $(shell ${QWANALYSIS}/bin/qweak-config --ldflags)
#  When we get the QwAnalysis include paths, turn paths like "./*" into "${QWANALYSIS}/*".
REGEXP_QWANA  := $(shell echo ${QWANALYSIS} | sed 's/\//\\\//g')
QWEAKCPPFLAGS := $(shell ${QWANALYSIS}/bin/qweak-config --cppflags | sed 's/I\./I${REGEXP_QWANA}/g' )

#QwLIBS = -l

CXXFLAGS     += $(DEBUGFLAGS) $(QWEAKCPPFLAGS) -Wno-deprecated
LIBS          = $(QWEAKLIBS) $(SYSLIBS) $(QWLIBS) -lNew
GLIBS         = $(ROOTGLIBS) $(SYSLIBS) $(QWLIBS) -lNew

ENSCRIPT      = @enscript -2Gjr -o
#------------------------------------------------------------------------------------


all: $(APPLIC)
$(QWOBJECT)
$(APPLIC): $(OBJECTS) $(NODICTOBJECTS) $(DICTS) $(QWDICTOBJECTS) $(RDATALIB)
	$(CXX) -o $@ $(LDFLAGS) $(QWEAKLDFLAGS) $(LIBS) $(GLIBS) $(GEOMLIB) $(GUILIB) $^
	ln -sf QwAutoGUIView/$@ ../QwAutoGUI
	@echo "$@ done"

%.o : $(PRG_SOURCE)/%.cc
	$(CXX) -c $(CXXFLAGS) $< -I$(PRG_INCLUDE)

%Dict.cc : $(PRG_INCLUDE)/%.h
	@echo "Generating dictionary $@..."
	@rootcint -f $@ -c -DROOTCINTMODE $^

clean:
	rm -f ./*.o ./core ./*~ ./*Dict.cc ./*Dict.h ./~* ./G__*.* source/*~ \
	include/*~

QwGUIMain.o : ../VERSION
