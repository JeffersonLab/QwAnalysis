

CFLAGS = -g -Wall
SRCS = main.cc qdata.cc
CC = g++
LD = g++
LDFLAGS =
CLIBS = -ldl -lm
RM = /bin/rm -f
OBJS = main.o qdata.o
PROG = prog
ROOTLIBS = -L$(ROOTSYS)/lib -lCint -lCore -lGpad -lGraf -lGraf3d -lHist -lHtml -lMatrix -lMinuit -lNew -lPostscript -lProof -lThread -lTree -lTreePlayer 
INCLUDE = -I$(ROOTSYS)/include -I/home/stokes/hermes/hrc/mathlib
LIBS = -L/home/stokes/hermes/hrc/mathlib -lwmath

all: $(PROG)

%.o: %.cc %.h
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

#main.o: main.cc
#	$(CC) $(CFLAGS) -c main.cc
# The above commands are equivalent to the following:
#main.o: main.cc
#	g++ -g -Wall -c main.cc

#qdata.o: qdata.cc qdata.h
#	$(CC) $(CFLAGS) -c qdata.cc

#qdata.o: qdata.cc qdata.h
#	g++ -g -Wall -c qdata.cc

#$(PROG): $(OBJS)
#	$(LD) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $(PROG)

# The above commands are equivalent to the following:
#prog: main.o qdata.o
#	g++ -g -Wall main.o qdata.o -o prog

#The following did not work as advertised.  I should try
# to figure out how to fix it and understand what it's
# supposed to do.
#
#depend:
#	$(RM) .depend
#	makedepend -f- -- $(CFLAGS) -- $(SRCS) > .depend
#
#include .depend
#





clean:
	$(RM) $(PROG) $(OBJS) 
#clean: 
#	/bin/rm -f prog main.o qdata.o




# NOTES
# '-o' is for 'optimization'
# '-g' is for debugging
#
# using the '%' character will match the target to the dependencies, i.e. : "%.o: %.cc"
#			"$(CC) $(CFLAGS) -c $<"
# where the $< is all the dependencies that matched.





OBJS = tree.o main.o treesort.o treesearch.o treematch.o treecombine.o treedo.o Qset.o Qevent.o Qoptions.o

main.o: main.cc
	$(CC) $(CFLAGS) $(INCLUDE) -c main.cc


prog: $(OBJS)
	$(CC) $(CFLAGS) $(CLIBS) $(ROOTLIBS) $(LIBS) $(OBJS) -o prog


OBJS2 = test.o

test.o: test.cc
	$(CC) $(CFLAGS) $(INCLUDE) -c test.cc


prog2: $(OBJS2)
	$(CC) $(CFLAGS) $(CLIBS) $(ROOTLIBS) $(OBJS2) -o prog2



