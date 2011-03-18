#include "QwRTGUISubSystem.h"


QwRTGUISubSystem(const TGWindow *main, TGCompositeFrame *parent, UInt_t w, UInt_t h)
{ 

  dHistoReset = 0;
  dHistoAccum = 0;
  dHistoPause = 0;
  dMapFile    = NULL;

LIBINCLUDE    = 
PRG_INCLUDE   = ./include
PRG_SOURCE    = ./src

# Here we list the object files that require a CINT dictionary.
OBJECTS	      =
OBJECTS       += QwRTGUIMain.o
OBJECTS       += QwRTGUIShutter.o
#OBJECTS       += QwRTGUISubSystem.o
#OBJECTS       += QwGUIMainDetector.o
#OBJECTS       += QwGUILumiDetector.o
#OBJECTS       += QwGUIInjector.o
#OBJECTS       += QwGUICorrelationPlots.o
#OBJECTS       += QwRTGUIHallCBeamline.o

}

# Have this after all the object files are listed so that dictionaries
# will be made automatically.
DICTS		= #$(OBJECTS:.o=Dict.o)

}



void 
QwRTGUISubSystem::SetMapFile(TMapFile *file)
{
  if(file) {
    dMapFile = file;
    dMapFileFlag = true;
    std::cout << "Subsystem " << std::setw(22) << this->GetName()
	      << " now can access the map file at the address "
	      << dMapFile << std::endl;
  }
  else {
    dMapFileFlag = false;
  }
  return;
};



void 
QwRTGUISubSystem::SummaryHist(TH1 *in)
{

  Double_t out[4] = {0.0};
  Double_t test   = 0.0;


#------------------------------------------------------------------------------------


all: $(APPLIC)
$(QWOBJECT)
$(APPLIC): $(OBJECTS) $(NODICTOBJECTS) $(DICTS) $(QWDICTOBJECTS)
	$(CXX) -o $@ $(LDFLAGS) $(QWEAKLDFLAGS) $(LIBS) $(GLIBS) $(GEOMLIB) $(GUILIB) $^
	ln -sf QwRealTimeGUIView/$@ ../$(APPLIC)
	@echo "$@ done"

%.o : $(PRG_SOURCE)/%.cc
	$(CXX) -c $(CXXFLAGS) $< -I$(PRG_INCLUDE) -I$(LIBINCLUDE)   $(QWTRINCLUDE) $(QWANINCLUDE) $(QWPRINCLUDE)

%.o : $(QWTRSOURCE)/%.cc
	$(CXX) -c $(CXXFLAGS) $<  $(QWTRINCLUDE) $(QWPRINCLUDE) $(QWANINCLUDE) -I$(PRG_INCLUDE) -I$(LIBINCLUDE)

#%Dict.o : %Dict.cc
#	$(CXX) -c $(CXXFLAGS) $< -I$(PRG_INCLUDE) -I$(LIBINCLUDE)  $(QWTRINCLUDE) $(QWPRINCLUDE) $(QWANINCLUDE)

#%Dict.cc : $(PRG_INCLUDE)/%.h
#	@echo "Generating dictionary $@..."
#	rootcint -f $@ -c -I$(LIBINCLUDE) $(QWTRINCLUDE) $(QWPRINCLUDE) $(QWANINCLUDE) $^

clean:
	rm -f ./*.o ./core ./*~ ./*Dict.cc ./*Dict.h ./~* ./G__*.* source/*~ \
	include/*~ $(APPLIC)

QwRTGUIMain.o : ../VERSION
