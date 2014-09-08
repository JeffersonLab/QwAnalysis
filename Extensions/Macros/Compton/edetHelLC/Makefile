Target := edetLC_new
CPP_FILES := asymFit.C evaluateAsym.C fileReadDraw.C infoDAQ.C qNormVariables.C writeToFile.C expAsym.C getEBeamLasCuts.C polCERunRange.C rhoToX.C weightedMean.C
OBJ_FILES := $(subst .C,.o,$(CPP_FILES))

ROOTFLAGS = $(shell root-config --cflags)
ROOTLIBS  = $(shell root-config --libs --ldflags)

CXX :="g++"
CPPFLAGS := "-g"

all:$(Target)

$(Target): $(Target).cpp $(OBJ_FILES)
	$(CXX) -o $@ $(Target).cpp $(OBJ_FILES) $(ROOTFLAGS) $(ROOTLIBS)

%.o: %.C
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(ROOTFLAGS) -c -o $@ $<

clean:
	rm -rf *.o
