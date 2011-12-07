# This is the make file to compile some useful classes for
# data handling and visualization into a basic library.
# The classes herein are derived from the ROOT data analysis
# library.

-include ../VERSION

PRG_INCLUDE   = ./include
PRG_SOURCE    = ./source

OBJECTS		 = 
NODICTOBJECTS	 =
# Here we list the object files that require a CINT dictionary.
OBJECTS		+= RDataContainer.o
OBJECTS		+= RDataWindow.o
OBJECTS		+= RSDataWindow.o
OBJECTS		+= RDataFit.o
OBJECTS		+= RPlotList.o
OBJECTS		+= RDataFitEntry.o
OBJECTS		+= RProcessLog.o
OBJECTS		+= RComboBoxDialog.o
OBJECTS		+= RPlotContainer.o
OBJECTS		+= RNumberEntryDialog.o
OBJECTS		+= RStringEntryDialog.o
OBJECTS         += RMsgBox.o
# Now append the object files that Don't require a CINT dictionary.
NODICTOBJECTS	+= RVegas.o

# Have this after all the object files are listed so that dictionaries
# will be made automatically.
DICTS		= $(OBJECTS:.o=Dict.o)

APPLIC        = QwGUIRootLib-$(VERS).a
MFILE         = Makefile

OutPutOpt     = -o 

# For Linux with egcs, gcc 2.9x, gcc 3.x (>= RedHat 5.2)
# the correct flag settings for root are: 

CXX           = g++
CXXFLAGS      = -O2 -Wall -fPIC # -g
AR            = ar
ARFLAGS       = -r
LD            = g++
LDFLAGS       = -O
SOFLAGS       = -shared
SYSLIBS       = 

ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLIBS     := $(shell root-config --libs)
ROOTGLIBS    := $(shell root-config --glibs)

CXXFLAGS     += $(ROOTCFLAGS)
LIBS          = -lNew $(ROOTLIBS) $(SYSLIBS) 
GLIBS         = -lNew $(ROOTGLIBS) $(SYSLIBS)

ENSCRIPT	= @enscript -2Gjr -o
#------------------------------------------------------------------------------------


all: $(APPLIC)

$(APPLIC): $(OBJECTS) $(NODICTOBJECTS) $(DICTS)
	$(AR) $(ARFLAGS) $@ $^
	@echo "$@ done"

%.o : $(PRG_SOURCE)/%.cxx
	$(CXX) -c $(CXXFLAGS) $< -I$(PRG_INCLUDE)

%Dict.o : %Dict.cxx
	$(CXX) -c $(CXXFLAGS) $< -I$(PRG_INCLUDE)

%Dict.cxx : $(PRG_INCLUDE)/%.h
	@echo "Generating dictionary $@..."
	@rootcint -f $@ -c $^

clean:
	rm -f ./*.o ./core ./*~ $(PRG_SOURCE)/*~ $(PRG_INCLUDE)/*~ \
	./*Dict.cxx ./*Dict.h ./~* ./G__*.*

