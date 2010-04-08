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
OBJECTS       += QwGUIHelpBrowser.o
OBJECTS       += QwGUIProgressDialog.o
OBJECTS       += QwGUIPlotOptionsDialog.o
OBJECTS       += QwGUIDataWindow.o
OBJECTS       += QwGUISubSystem.o
OBJECTS       += QwGUIMainDetector.o
OBJECTS       += QwGUILumiDetector.o
OBJECTS       += QwGUIInjector.o
OBJECTS       += QwGUIEventDisplay.o
OBJECTS       += QwGUISuperCanvas.o

# Now append the object files that Don't require a CINT dictionary.
#NODICTOBJECTS   = QwHit.o
#NODICTOBJECTS  += QwHitRootContainer.o
#NODICTOBJECTS  += QwHitContainer.o
#NODICTOBJECTS  += QwParameterFile.o

#QWDICTOBJECTS    += $(QWANALYSIS)/Tracking/dictionary/QwHitDict.o
#QWDICTOBJECTS    += $(QWANALYSIS)/Tracking/dictionary/QwHitContainerDict.o
#QWDICTOBJECTS    += $(QWANALYSIS)/Tracking/dictionary/QwHitRootContainerDict.o

QWTRSOURCE = $(QWANALYSIS)/Tracking/src
QWANSOURCE = $(QWANALYSIS)/Analysis/src

#QWOBJECTS    += $(QWANALYSIS)/Tracking/src/QwEvent.o
##QWOBJECTS    += $(QWANALYSIS)/Tracking/src/QwEventHeader.o
#QWOBJECTS    += $(QWANALYSIS)/Tracking/src/QwTrackingTreeLine.o
#QWOBJECTS    += $(QWANALYSIS)/Tracking/src/VQwTrackingElement.o
#QWOBJECTS    += $(QWANALYSIS)/Tracking/src/QwPartialTrack.o
#QWOBJECTS    += $(QWANALYSIS)/Tracking/src/Det.o  # for QwPartialTrack

#QWOBJECTS    += $(QWANALYSIS)/Tracking/dictionary/QwEventDict.o
#QWOBJECTS    += $(QWANALYSIS)/Tracking/dictionary/QwEventHeaderDict.o
#QWOBJECTS    += $(QWANALYSIS)/Tracking/dictionary/QwTrackingTreeLineDict.o
#QWOBJECTS    += $(QWANALYSIS)/Tracking/dictionary/VQwTrackingElementDict.o
#QWOBJECTS    += $(QWANALYSIS)/Tracking/dictionary/QwPartialTrackDict.o


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
GUILIB        = -lGuiHtml

ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLIBS     := $(shell root-config --libs)
ROOTGLIBS    := $(shell root-config --glibs)

QWTRINCLUDE    = -I$(QWANALYSIS)/Tracking/include
QWANINCLUDE    = -I$(QWANALYSIS)/Analysis/include
QWPRINCLUDE    = -I$(QWANALYSIS)/Parity/include

QwLIBS =

CXXFLAGS     += $(DEBUGFLAGS) $(ROOTCFLAGS) -Wno-deprecated
LIBS          = $(ROOTLIBS) $(SYSLIBS) $(QWLIBS)
GLIBS         = $(ROOTGLIBS) $(SYSLIBS) $(QWLIBS)

ENSCRIPT      = @enscript -2Gjr -o
#------------------------------------------------------------------------------------


all: $(APPLIC)
$(QWOBJECT)
$(APPLIC): $(OBJECTS) $(NODICTOBJECTS) $(DICTS) $(QWDICTOBJECTS) $(RDATALIB)
	$(CXX) -o $@ $(LDFLAGS) $(GLIBS) $(GEOMLIB) $(GUILIB) $^
	ln -sf QwGUIView/$@ ../QwGUI
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

QwGUIMain.o : ../VERSION
