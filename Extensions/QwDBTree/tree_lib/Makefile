CXXFLAGS = -Wall -ggdb -c -pthread -m64 -I/usr/include/root
LIBS = $(shell root-config --glibs)

PROGRAM_FILES := \
	detector_plot

LIB_FILES := \
	QwTreeLib

detector_plot: $(PROGRAM_FILES).o libQwDB.so
	g++ -Wall -ggdb -o $@ $(PROGRAM_FILES).o $(LIBS) -L. -lQwDB

libQwDB.so: $(LIB_FILES).o
	g++ -shared -Wl,-soname,libQwDB.so -o $@ $^

$(LIB_FILES).o: $(LIB_FILES).cpp
	g++ -shared -Wall -ggdb -c -fPIC $(CXXFLAGS) -o $@ $^

$(PROGRAM_FILES).o: $(PROGRAM_FILES).cpp
	g++ -Wall -ggdb -c $(CXXFLAGS) -o $@ $^

clean:
	rm -f libQwDB.so *.o

install:
	install libQwDB.so $(QWANALYSIS)/lib/
	install detector_plot $(QWANALYSIS)/bin/
