# Regression/Dithering Analyzer redana
#  - B. Moffit and K. Paschke
# 
# Uncomment this if profiling (note: it slows down the code)
# export PROFILE = 1
#
# To compile optimized  (10% faster run time but very slow compilation)
# export OPTIMIZE =1
# 

# Choose the compiler.
GCC=g++
GLD=g++
ifdef INSURE
  GCC=insure
  GLD=insure
endif

ifdef PAN_SRC_PATH
	PANSRCPATH = $(PAN_SRC_PATH)
else
	PANSRCPATH    = ../../pan/src
endif
ROOTLIBS      = $(shell root-config --libs)
ROOTGLIBS     = $(shell root-config --glibs)
INCLUDES      = -I$(shell root-config --incdir)
CXX           = $(GCC)
CXXFLAGS      = -Wall -fno-exceptions -fPIC $(INCLUDES) -fpermissive
LD            = $(GLD)
LDFLAGS       = 
SOFLAGS       = -shared 
GLIBS         = $(ROOTGLIBS) -L/usr/X11R6/lib -lXpm -lX11
LIBPANCLASS   = $(PANSRCPATH)/libpan.a
LIBS          = $(GLIBS) $(ROOTLIBS) $(ROOTGLIBS) $(LIBPANCLASS)
INCLUDES     += -I$(PANSRCPATH)

MAKEDEPEND    = $(GCC)

ALL_LIBS = $(LIBS)


ifdef PROFILE
   CXXFLAGS += -pg
endif

ifdef OPTIMIZE
   CXXFLAGS += -O
else
   CXXFLAGS += -g
endif

# Grow the list of sources and pan objects here.
PANSRC = $(PANSRCPATH)/TaFileName.cc $(PANSRCPATH)/TaString.cc \
	$(PANSRCPATH)/TaStatistics.cc  $(PANSRCPATH)/TaOResultsFile.cc
SRC = TaRedAna.cc TaConfFile.cc VaAna.cc TaRegression.cc TaInput.cc \
	TaDitAna.cc TaDitCyc.cc Matrix.cc
DEPS = $(SRC:.cc=.d)
OBJS = $(SRC:.cc=.o) 
DOBJS = $(SRC:.cc=.o)
HEAD = $(SRC:.cc=.hh) $(PANSRC:.cc=.hh)

# Make the dictionary
  CXXFLAGS += -DDICT
  OBJS += RedDict.o

PROGS = redana

install: all
	 mv $(PROGS) ../
	 mv libRedAna.so ../

all: $(PROGS) libRedAna.so

$(PROGS): main.o $(OBJS) $(SRC) $(HEAD) 
	rm -f $@
	@echo "Generating" $@
	$(LD) $(CXXFLAGS) -o $@ main.o $(OBJS) $(ALL_LIBS) 

main.o: main.cc
	@echo "Generating" $@
	$(CXX) -c $(INCLUDES) $(CXXFLAGS) $<	

libRedAna.so: $(OBJS) $(HEAD)
	@echo "Generating" $@
	$(CXX) $(SOFLAGS) -O -o libRedAna.so $(OBJS) $(ALL_LIBS)

# Dictionary
RedDict.cc: $(DOBJS) RedLinkDef.hh
	@echo "Generating Dictionary..."
	$(ROOTSYS)/bin/rootcint -f $@ -c -p -DDICT $(HEAD) RedLinkDef.hh

clean:
	rm -f *.o core *~ *.d *.tar *.so  *Dict* $(PROGS)

realclean:  clean
	rm -f *.d ../$(PROGS) ../libRedAna.so

.SUFFIXES:
.SUFFIXES: .c .cc .cpp .C .o .d

%.o:	%.cc
	$(CXX) $(CXXFLAGS) -c $<

%.d:	%.cc
	@echo Creating dependencies for $<
	@$(SHELL) -ec '$(MAKEDEPEND) -MM $(INCLUDES) -c $< \
	      | sed '\''s%^.*\.o%$*\.o%g'\'' \
	      | sed '\''s%\($*\)\.o[ :]*%\1.o $@ : %g'\'' > $@; \
	      [ -s $@ ] || rm -f $@'

-include $(DEPS)


