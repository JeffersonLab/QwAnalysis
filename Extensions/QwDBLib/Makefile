CPP 	 = g++
#CPP 	 = clang
CXXFLAGS = -Wall -fPIC -ggdb -c -pthread -m64 -I/usr/include/root -Iinclude -I$(shell root-config --incdir)
LDFLAGS  = $(shell root-config --glibs)

PROG_FILES := \
	mdallbars_y mdallpmtavg bcmdd bcm uslumi uslumi_y dslumi

LIB_FILES := \
	src/QwTreeBranch src/QwTreeBranchPair src/QwTreeBranchPlot

LIB_O_FILES := $(addsuffix .o,$(LIB_FILES))

all: $(PROG_FILES)

uslumi: uslumi.o libQwDB.so
	$(CPP) -Wall -ggdb -o $@ uslumi.o $(LDFLAGS) -L. -lQwDB

dslumi: dslumi.o libQwDB.so
	$(CPP) -Wall -ggdb -o $@ dslumi.o $(LDFLAGS) -L. -lQwDB

mdallpmtavg: mdallpmtavg.o libQwDB.so
	$(CPP) -Wall -ggdb -o $@ mdallpmtavg.o $(LDFLAGS) -L. -lQwDB

mdallbars_y: mdallbars_y.o libQwDB.so
	$(CPP) -Wall -ggdb -o $@ mdallbars_y.o $(LDFLAGS) -L. -lQwDB

uslumi_y: uslumi_y.o libQwDB.so
	$(CPP) -Wall -ggdb -o $@ uslumi_y.o $(LDFLAGS) -L. -lQwDB

bcmdd: bcmdd.o libQwDB.so
	$(CPP) -Wall -ggdb -o $@ bcmdd.o $(LDFLAGS) -L. -lQwDB

bcm: bcm.o libQwDB.so
	$(CPP) -Wall -ggdb -o $@ bcm.o $(LDFLAGS) -L. -lQwDB

libQwDB.so: $(LIB_O_FILES)
	$(CPP) -shared -Wl,-soname,libQwDB.so -o $@ $^

%.o: %.cpp
	$(CPP) -Wall -ggdb -c $(CXXFLAGS) -o $@ $^

clean:
	rm -f libQwDB.so *.o src/*.o lumi mdallbars_y mdallpmtavg bcmdd lumi example

install:
	install libQwDB.so $(QWANALYSIS)/lib/
