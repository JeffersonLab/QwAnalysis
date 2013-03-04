CXXFLAGS = $(shell root-config --cflags --glibs)
LIBS = $(shell root-config --glibs)

SRCS := \
	main.cpp \
	tree_fill.cpp \
	detector.cpp \
	parse.cpp \
	runlet.cpp \
	parse.h

OFILES := $(addsuffix .o,$(basename $(SRCS)))

all: tree_fill
	@echo ""
	@echo "ATTENTION!!"
	@echo "This program should *ONLY* be run with permission from Wade Duvall (wsduvall@jlab.org)."
	@echo "Running this will create massive database load, and is done automatically once per day."

tree_fill: $(OFILES)
		g++ -Wall -ggdb $(CXXFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
		g++ -Wall -ggdb -c $(CXXFLAGS) -o $@ $^

clean:
	rm -f tree_fill *.o
