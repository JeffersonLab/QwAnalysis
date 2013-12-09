#CPP 	 = g++
CPP 	 = clang
CXXFLAGS = -Wall -fPIC -ggdb -c -pthread -m64 -I/usr/include/root -Iinclude
LDFLAGS  = $(shell root-config --glibs)

PROG_FILES := \
	example

LIB_FILES := \
	src/QwTreeBranch src/QwTreeBranchPair src/QwTreeBranchPlot

LIB_O_FILES := $(addsuffix .o,$(LIB_FILES))

PROG_O_FILES := $(addsuffix .o,$(PROG_FILES))

$(PROG_FILES): $(PROG_O_FILES) libQwDB.so
	$(CPP) -Wall -ggdb -o $@ $(PROG_O_FILES) $(LDFLAGS) -L. -lQwDB

libQwDB.so: $(LIB_O_FILES)
	$(CPP) -shared -Wl,-soname,libQwDB.so -o $@ $^

%.o: %.cpp
	$(CPP) -Wall -ggdb -c $(CXXFLAGS) -o $@ $^

clean:
	rm -f libQwDB.so *.o src/*.o example

install:
	install libQwDB.so $(QWANALYSIS)/lib/
