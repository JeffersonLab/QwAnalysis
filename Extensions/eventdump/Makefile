HedSuffix = hh
ObjSuffix = o
SrcSuffix = cxx
ExePrefix = x
ExeSuffix = 
DepSuffix = d

EXENAME 	:= eventdump
EXECUTABLE	:= $(ExePrefix)$(EXENAME)$(ExeSuffix)	

ROOTCONFIG   	:= root-config

ROOTCFLAGS	:= $(shell $(ROOTCONFIG) --cflags)
ROOTINCLUDE	:= $(shell $(ROOTCONFIG) --incdir)
ROOTLDFLAGS   	:= $(shell $(ROOTCONFIG) --ldflags)
ROOTLIBS     	:= $(shell $(ROOTCONFIG) --libs)
ROOTGLIBS 	:= $(shell $(ROOTCONFIG) --glibs)

EXPLLINKLIBS  = $(ROOTLIBS) $(ROOTGLIBS)





CXX       	= g++
CXXFLAGS  	= -O2 -Wall  -fPIC -g
INCLUDE 	=  -I$(QWANALYSIS)/coda/include


CXXFLAGS 	+= $(ROOTCFLAGS)  

LD              = $(CXX)
LDFLAGS   	= -O 
SOFLAGS 	= -shared

GLIBS		= $(EXPLLINKLIBS)
CODA_LIB	= $(QWLIB)/libcoda.so
ALL_LIBS	= $(GLIBS)


ifdef CODA
LIBET = $(CODA)/Linux/lib/libet.so
ALL_LIBS += $(LIBET)
endif

SRCS		= src/xEventDump.$(SrcSuffix) src/CodaAnalysis.$(SrcSuffix) src/TextColor.$(SrcSuffix)

HEAD 		= $(SRCS:.$(SrcSuffix)=.$(HedSuffix))
OBJS 		= $(SRCS:.$(SrcSuffix)=.$(ObjSuffix))



.SUFFIXES: .$(SrcSuffix) .$(ObjSuffix)

all: $(EXECUTABLE) $(SRCS)


$(EXECUTABLE) : $(OBJS)
	$(LD)  $(LDFLAGS) $(ALL_LIBS) $(OBJS) -o $@  $(CODA_LIB)

.$(SrcSuffix).$(ObjSuffix):
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

# -c : only compile, $< is  all source files,
# -o : the destination of the object files $@ name and directory

clean:
	@echo Cleaning up ...
	@rm -f src/*.$(ObjSuffix) core
	@rm -f *~
	@rm -f $(EXECUTABLE)
