CPP 	 = g++
#CPP 	 = clang
CXXFLAGS = -Wall -fPIC -ggdb -c -pthread -m64 -I/usr/include/root -Iinclude -I$(shell root-config --incdir)
LDFLAGS  = $(shell root-config --glibs)

PROG_FILES := \
	example data_quality

LIB_FILES := \
	src/QwTreeBranch src/QwTreeBranchPair src/QwTreeBranchPlot

LIB_O_FILES := $(addsuffix .o,$(LIB_FILES))

all: $(PROG_FILES)

example: example.o libQwDB.so
	$(CPP) -Wall -ggdb -o $@ example.o $(LDFLAGS) -L. -lQwDB

data_quality: data_quality.o libQwDB.so
	$(CPP) -Wall -ggdb -o $@ data_quality.o $(LDFLAGS) -L. -lQwDB

libQwDB.so: $(LIB_O_FILES)
	$(CPP) -shared -Wl,-soname,libQwDB.so -o $@ $^

%.o: %.cpp
	$(CPP) -Wall -ggdb -c $(CXXFLAGS) -o $@ $^

clean:
	rm -f libQwDB.so *.o src/*.o example

install:
	install libQwDB.so $(QWANALYSIS)/lib/
