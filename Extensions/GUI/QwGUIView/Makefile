# This is the make file to compile the QwGUI program
#
# It is derived form the file in $ROOTSYS/test/Makefile, which
# was written by the "root" people....

-include ../VERSION

LIBDIR        = ../QwGUILib
LIBINCLUDE    = $(LIBDIR)/include
RDATALIB      = $(LIBDIR)/QwGUIRootLib-$(VERS).a
PRG_INCLUDE   = ./include
PRG_SOURCE    = ./src

# Here we list the object files that require a CINT dictionary.
OBJECTS	      =
OBJECTS       += QwGUIMain.o
OBJECTS       += QwGUISubSystem.o
OBJECTS       += QwGUIMainDetector.o
# Now append the object files that Don't require a CINT dictionary.
NODICTOBJECTS =
#NODICTOBJECTS += NMpContainer.o

# Have this after all the object files are listed so that dictionaries
# will be made automatically.
DICTS		= $(OBJECTS:.o=Dict.o)

APPLIC        = QwGUI-$(VERS)
MFILE         = Makefile

# For Linux with egcs, gcc 2.9x, gcc 3.x (>= RedHat 5.2)
# the correct flag settings are: 

DEBUGFLAGS    = #-DQWGUI_DEBUG
CXX           = g++
CXXFLAGS      = -Wall -fPIC -DVERS="\"$(VERS)\"" -g #-O2
LD            = g++
LDFLAGS       = #-O2
SOFLAGS       = -shared
SYSLIBS       = 
GEOMLIB       = -lGeom

ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLIBS     := $(shell root-config --libs)
ROOTGLIBS    := $(shell root-config --glibs)

CXXFLAGS     += $(DEBUGFLAGS) $(ROOTCFLAGS) -Wno-deprecated
LIBS          = $(ROOTLIBS) $(SYSLIBS)
GLIBS         = $(ROOTGLIBS) $(SYSLIBS)

ENSCRIPT      = @enscript -2Gjr -o
#------------------------------------------------------------------------------------


all: $(APPLIC)

$(APPLIC): $(OBJECTS) $(NODICTOBJECTS) $(DICTS) $(RDATALIB)
	$(CXX) -o $@ $(LDFLAGS) $(GLIBS) $(GEOMLIB) $^
	ln -sf QwGUIView/$@ ../QwGUI
	@echo "$@ done"

%.o : $(PRG_SOURCE)/%.cc
	$(CXX) -c $(CXXFLAGS) $< -I$(PRG_INCLUDE) -I$(LIBINCLUDE)

%Dict.o : %Dict.cc
	$(CXX) -c $(CXXFLAGS) $< -I$(PRG_INCLUDE) -I$(LIBINCLUDE)

%Dict.cc : $(PRG_INCLUDE)/%.h
	@echo "Generating dictionary $@..."
	rootcint -f $@ -c -I$(LIBINCLUDE) $^

clean:
	rm -f ./*.o ./core ./*~ ./*Dict.cc ./*Dict.h ./~* ./G__*.* source/*~ \
	include/*~

QwGUIMain.o : ../VERSION
