# This is the make file to compile the QwGUI program
#
# It is derived form the file in $ROOTSYS/test/Makefile, which
# was written by the "root" people....

#-include ../VERSION

LIBDIR        = 
#../QwGUILib


LIBINCLUDE    = 
#$(LIBDIR)/include
RDATALIB      = 
#$(LIBDIR)/QwGUIRootLib-$(VERS).a
PRG_INCLUDE   = ./include
PRG_SOURCE    = ./src

# Here we list the object files that require a CINT dictionary.
OBJECTS	      =
OBJECTS       += QwGUIMain.o
OBJECTS       += QwGUISubSystem.o
OBJECTS       += QwGUIMainDetector.o
OBJECTS       += QwGUILumiDetector.o
OBJECTS       += QwGUIInjector.o
OBJECTS       += QwGUICorrelationPlots.o
OBJECTS       += QwGUIHallCBeamline.o
OBJECTS       += QwGUISummary.o

QWTRSOURCE = $(QWANALYSIS)/Tracking/src
QWANSOURCE = $(QWANALYSIS)/Analysis/src

# Have this after all the object files are listed so that dictionaries
# will be made automatically.
DICTS		= $(OBJECTS:.o=Dict.o)

APPLIC        = QwRealTimeGUI
MFILE         = Makefile

# For Linux with egcs, gcc 2.9x, gcc 3.x (>= RedHat 5.2)
# the correct flag settings are:

DEBUGFLAGS    = #-DQWGUI_DEBUG
CXX           = g++
CXXFLAGS      = -Wall -fPIC  -O2 #-g -O2
LD            = g++
LDFLAGS       = #-O2
SOFLAGS       = -shared
SYSLIBS       =
GEOMLIB       = -lGeom
GUILIB        = -lGuiHtml

#ROOTCFLAGS   := $(shell root-config --cflags)
#ROOTLIBS     := $(shell root-config --libs)
ROOTGLIBS    := $(shell root-config --glibs)
QWEAKLIBS     := $(shell ${QWANALYSIS}/bin/qweak-config --libs)
QWEAKLDFLAGS  := $(shell ${QWANALYSIS}/bin/qweak-config --ldflags)
#  When we get the QwAnalysis include paths, turn paths like "./*" into "${QWANALYSIS}/*".
REGEXP_QWANA  := $(shell echo ${QWANALYSIS} | sed 's/\//\\\//g')
QWEAKCPPFLAGS := $(shell ${QWANALYSIS}/bin/qweak-config --cppflags | sed 's/I\./I${REGEXP_QWANA}/g' )


QWTRINCLUDE    = -I$(QWANALYSIS)/Tracking/include
QWANINCLUDE    = -I$(QWANALYSIS)/Analysis/include
QWPRINCLUDE    = -I$(QWANALYSIS)/Parity/include

QwLIBS =

CXXFLAGS     += $(DEBUGFLAGS) $(QWEAKCPPFLAGS) -Wno-deprecated
LIBS          = -lNew $(QWEAKLIBS) $(SYSLIBS) $(QWLIBS)
GLIBS         = -lNew $(ROOTGLIBS) $(SYSLIBS) $(QWLIBS) 

ENSCRIPT      = @enscript -2Gjr -o



#------------------------------------------------------------------------------------


all: $(APPLIC)
$(QWOBJECT)
$(APPLIC): $(OBJECTS) $(NODICTOBJECTS) $(DICTS) $(QWDICTOBJECTS) $(RDATALIB)
	$(CXX) -o $@ $(LDFLAGS) $(QWEAKLDFLAGS) $(LIBS) $(GLIBS) $(GEOMLIB) $(GUILIB) $^
	ln -sf QwRealTimeGUIView/$@ ../QwRealTimeGUI
	@echo "$@ done"

%.o : $(PRG_SOURCE)/%.cc
	$(CXX) -c $(CXXFLAGS) $< -I$(PRG_INCLUDE) -I$(LIBINCLUDE)   $(QWTRINCLUDE) $(QWANINCLUDE) $(QWPRINCLUDE)

%.o : $(QWTRSOURCE)/%.cc
	$(CXX) -c $(CXXFLAGS) $<  $(QWTRINCLUDE) $(QWPRINCLUDE) $(QWANINCLUDE) -I$(PRG_INCLUDE) -I$(LIBINCLUDE)

%Dict.o : %Dict.cc
	$(CXX) -c $(CXXFLAGS) $< -I$(PRG_INCLUDE) -I$(LIBINCLUDE)  $(QWTRINCLUDE) $(QWPRINCLUDE) $(QWANINCLUDE)

%Dict.cc : $(PRG_INCLUDE)/%.h
	@echo "Generating dictionary $@..."
	rootcint -f $@ -c -I$(LIBINCLUDE) $(QWTRINCLUDE) $(QWPRINCLUDE) $(QWANINCLUDE) $^

clean:
	rm -f ./*.o ./core ./*~ ./*Dict.cc ./*Dict.h ./~* ./G__*.* source/*~ \
	include/*~

QwGUIMain.o :
