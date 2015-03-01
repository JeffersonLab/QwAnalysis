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
OBJECTS       += QwGUIComboBox.o
OBJECTS       += QwGUIProgressDialog.o
OBJECTS       += QwGUIPlotOptionsDialog.o
OBJECTS       += QwGUIDataWindow.o
OBJECTS       += QwGUISubSystem.o
# OBJECTS       += QwGUIMainDetector.o
# OBJECTS       += QwGUIScanner.o
# OBJECTS       += QwGUIBeamModulation.o			 #Nuruzzaman
# OBJECTS       += QwGUILumiDetector.o
# OBJECTS       += QwGUIInjector.o
# OBJECTS	+= QwGUIHallCBeamline.o
# OBJECTS       += QwGUITrackFinding.o
# OBJECTS       += QwGUIEventDisplay.o
OBJECTS       += QwGUISuperCanvas.o
OBJECTS       += QwGUIDatabaseContainer.o
OBJECTS       += QwGUIDatabaseSelectionDialog.o
# OBJECTS       += QwGUIEventWindowSelectionDialog.o
OBJECTS       += QwGUIDatabase.o
OBJECTS       += QwGUIDatabaseSummary.o
OBJECTS       += QwGUIDBCorrelations.o


# Now append the object files that Don't require a CINT dictionary.
#NODICTOBJECTS   = QwHit.o
#NODICTOBJECTS  += QwHitRootContainer.o
#NODICTOBJECTS  += QwHitContainer.o
#NODICTOBJECTS  += QwParameterFile.o
#NODICTOBJECTS  += $(QWANALYSIS)/Analysis/src/QwDatabase.o

#QWDICTOBJECTS    += $(QWANALYSIS)/Tracking/dictionary/QwHitDict.o
#QWDICTOBJECTS    += $(QWANALYSIS)/Tracking/dictionary/QwHitContainerDict.o
#QWDICTOBJECTS    += $(QWANALYSIS)/Tracking/dictionary/QwHitRootContainerDict.o

QWTRSOURCE = $(QWANALYSIS)/Tracking/src
QWANSOURCE = $(QWANALYSIS)/Analysis/src

#QWOBJECTS    += $(QWANALYSIS)/Tracking/src/QwEvent.o
##QWOBJECTS    += $(QWANALYSIS)/Tracking/src/QwEventHeader.o
#QWOBJECTS    += $(QWANALYSIS)/Tracking/src/QwTreeLine.o
#QWOBJECTS    += $(QWANALYSIS)/Tracking/src/VQwTrackingElement.o
#QWOBJECTS    += $(QWANALYSIS)/Tracking/src/QwPartialTrack.o
#QWOBJECTS    += $(QWANALYSIS)/Tracking/src/Det.o  # for QwPartialTrack

#QWOBJECTS    += $(QWANALYSIS)/Tracking/dictionary/QwEventDict.o
#QWOBJECTS    += $(QWANALYSIS)/Tracking/dictionary/QwEventHeaderDict.o
#QWOBJECTS    += $(QWANALYSIS)/Tracking/dictionary/QwTreeLineDict.o
#QWOBJECTS    += $(QWANALYSIS)/Tracking/dictionary/VQwTrackingElementDict.o
#QWOBJECTS    += $(QWANALYSIS)/Tracking/dictionary/QwPartialTrackDict.o


# Have this after all the object files are listed so that dictionaries
# will be made automatically.
DICTS		= $(OBJECTS:.o=Dict.o)

APPLIC        = QwDataBaseGUI-$(VERS)
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

QWTRINCLUDE     = -I$(QWANALYSIS)/Tracking/include
QWANINCLUDE     = -I$(QWANALYSIS)/Analysis/include
QWPRINCLUDE     = -I$(QWANALYSIS)/Parity/include

#QWEVINCLUDE     = -I$(QWANALYSIS)/evio/include
#QWMSQLPPINCLUDE = -I/usr/include/mysql++
#QWMSQLINCLUDE   = -I/usr/include/mysql

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
	ln -sf QwDataBaseGUIView/$@ ../QwDBGUI
	@echo "$@ done"

%.o : $(PRG_SOURCE)/%.cc
	$(CXX) -c $(CXXFLAGS) $< -I$(PRG_INCLUDE) -I$(LIBINCLUDE)

%.o : $(QWTRSOURCE)/%.cc
	$(CXX) -c $(CXXFLAGS) $<  $(QWTRINCLUDE) $(QWPRINCLUDE) $(QWANINCLUDE) -I$(PRG_INCLUDE) -I$(LIBINCLUDE)

%Dict.o : %Dict.cc
	$(CXX) -c $(CXXFLAGS) $< -I$(PRG_INCLUDE) -I$(LIBINCLUDE)  $(QWTRINCLUDE) $(QWPRINCLUDE) $(QWANINCLUDE)

%Dict.cc : $(PRG_INCLUDE)/%.h
	@echo "Generating dictionary $@..."
	rootcint -f $@ -c -DROOTCINTMODE -I$(LIBINCLUDE) $(QWTRINCLUDE) $(QWANINCLUDE) $(QWPRINCLUDE) $^

clean:
	rm -f ./*.o ./core ./*~ ./*Dict.cc ./*Dict.h ./~* ./G__*.* source/*~ \
	include/*~

QwGUIMain.o : ../VERSION
