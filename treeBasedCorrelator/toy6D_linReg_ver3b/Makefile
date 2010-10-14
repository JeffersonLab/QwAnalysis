
HDRS     = JbToy6DGen.h  LinReg_Bevington_Pebay.h Toy6DEvent.h JbCorrelator.h
SRCS     = JbToy6DGen.cxx LinReg_Bevington_Pebay.cxx JbCorrelator.cxx

# 

CXX    = g++
LD     = g++
AR     = ar rul

DEBUG  =-g -O 
DEFINES=
#

OBJS   = $(notdir $(patsubst %.cxx,%.o,$(SRCS)))

# -shared
LDFLAGS  = -m32
CFLAGS   = -m32 -Wall $(DEBUG) $(DEFINES)  $(INCL)
ROOT_LIB  = `root-config --libs`
ROOT_INC  = -I `root-config --incdir`



all: doGen doCor

doGen: doToy6Deve.o $(OBJS) 
	$(LD) $(LDFLAGS) -o $@  $^  -l m  -l Minuit $(ROOT_LIB)


doCor: doCorrelation.o $(OBJS)
	$(LD) $(LDFLAGS) -o $@  $^  -l m  -l Minuit $(ROOT_LIB)

doCorAvr: doCorAvr.o $(OBJS)
	$(LD) $(LDFLAGS) -o $@  $^  -l m  -l Minuit $(ROOT_LIB)


clean:
	-rm -f *.o $(LINK) doGen doCor


depend .depend : $(SRCS) 
	$(CXX) -M $(CFLAGS)  $(ROOT_INC) $^ > ./.depend


%.o: %.cxx
	$(CXX) $(CFLAGS)  $(ROOT_INC) -c $< -o $@


#
-include .depend
