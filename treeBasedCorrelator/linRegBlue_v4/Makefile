
HDRS     =  LinReg_Bevington_Pebay.h JbCorrelator.h JbLeafTransform.h JbBlueTree.h
SRCS     =  LinReg_Bevington_Pebay.cxx JbCorrelator.cxx JbLeafTransform.cxx JbBlueTree.cxx

# 

CXX    = g++
LD     = g++
AR     = ar rul

DEBUG  =-g -O 
DEFINES=
#

OBJS   = $(notdir $(patsubst %.cxx,%.o,$(SRCS)))

# -shared
LDFLAGS  = 
CFLAGS   =  -Wall $(DEBUG) $(DEFINES)  $(INCL)
ROOT_LIB  = `root-config --libs`
ROOT_INC  = -I `root-config --incdir`



all:  linRegBlue

doGen: doToy6Deve.o $(OBJS) 
	$(LD) $(LDFLAGS) -o $@  $^  -l m  -l Minuit $(ROOT_LIB)


doCor: doCorrelation.o $(OBJS)
	$(LD) $(LDFLAGS) -o $@  $^  -l m  -l Minuit $(ROOT_LIB)



linRegBlue: linRegBlue.o $(OBJS)
	$(LD) $(LDFLAGS) -o $@  $^  -l m  -l Minuit $(ROOT_LIB)


clean:
	-rm -f *.o $(LINK) linRegBlue


depend .depend : $(SRCS) 
	$(CXX) -M $(CFLAGS)  $(ROOT_INC) $^ > ./.depend


%.o: %.cxx
	$(CXX) $(CFLAGS)  $(ROOT_INC) -c $< -o $@


#
-include .depend
