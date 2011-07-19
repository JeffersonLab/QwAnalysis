# Analyzer for "Pan" (Parity Analyzer)
# for Hall A Parity Experiments.
# R. Suleiman
#
#
# Generate the libDBTools.so library
#
# Would uncommment this if you want rootcint Dictionary
export MAKEDICTIONARY = 1

MYSQL_INCLUDE = /apps/mysql/PRO/include/mysql
MYSQL_LIBS    = $(ROOTSYS)/lib

export OSNAME := $(shell uname)

ifeq ($(OSNAME),SunOS)

   ROOTCFLAGS    = $(shell root-config --cflags)
   ROOTLIBS      = $(shell root-config --libs)
   ROOTGLIBS     = $(shell root-config --glibs)
   CXX           = g++
   CXXFLAGS      = -KPIC -DSUNVERS -I$(shell root-config --incdir) -I$(MAINDIR)
   CXXFLAGS     += $(ROOTCFLAGS)
   LD            = g++
   LDFLAGS       = -g -D
   SOFLAGS       = -G
   GLIB =  -lm -lc -lgen -lw -lnsl -ldl
   SLIB = -L/opt/SUNWspro/SC4.2/lib -lF77 -lM77 -lsunmath
   LIBS = $(GLIB)

endif

# Linux with egcs

ifeq ($(OSNAME),Linux)

   ROOTLIBS      = $(shell root-config --libs)
   ROOTGLIBS     = $(shell root-config --glibs)
   INCLUDES      = -I$(shell root-config --incdir)
   CXX           = g++
   CPPFLAGS = $(shell root-config --cflags) -I$(MYSQL_INCLUDE) 
   CXXFLAGS      = -g -Wall -fPIC  $(INCLUDES)
   LD            = g++
   LDFLAGS       = -g   
   SOFLAGS       = -shared -Wl,-soname,   
   GLIBS         = $(ROOTGLIBS) -L/usr/X11R6/lib -lXpm -lX11
   LIBS = $(GLIBS) $(ROOTLIBS) $(ROOTGLIBS)

endif

MAKEDEPEND    = g++

ALL_LIBS = $(LIBS)

#
# Mysql can be downloaded from http://www.mysql.com 
# It is also found at CUE in /apps/mysql/
# Use Mysql version: mysql-3.23.xx 
# 

INCLUDES +=-I$(MYSQL_INCLUDE) 
ALL_LIBS += $(MYSQL_LIBS)


# Grow the list of sources here.

SRC = TStringBuffer.cc TIntBuffer.cc TFloatBuffer.cc \
      TItem.cc TItemValue.cc TQuery.cc \
      TDBTools.cc

DEPS = $(SRC:.cc=.d)
OBJS = $(SRC:.cc=.o)
HEAD = $(SRC:.cc=.hh)

# Make the dictionary
ifdef MAKEDICTIONARY
  CXXFLAGS += -DDICT
  OBJS += TDict.o
endif  

MYSQL = $(MYSQL_LIBS)/libMySQL.so
COMP = /apps/lib/libz.so

all:  libDBTools.so  libDBTools.a

libDBTools.so :    $(OBJS)
	rm -f $@
	$(CXX) $(SOFLAGS) $(LDFLAGS) $(OBJS) $(MYSQL) $(COMP) -o $@ 

libDBTools.a:	$(OBJS) 
	rm -f $@
	ar cr $@ $(OBJS)

# Dictionary

TDict.cc: $(OBJS) TLinkDef.hh
	@echo "Generating Decoder Dictionary..."
	$(ROOTSYS)/bin/rootcint -f TDict.cc -c -p -DDICT $(HEAD) TLinkDef.hh

clean:
	rm -f *.o core *~ *.d TDict* 

realclean:  clean
	rm -f *.d


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
