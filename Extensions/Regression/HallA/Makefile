##
##  This is a wrapper Makefile to just compile the
##  sections of pan needed by the postpan-derived
##  regression tools as used by Qweak.
##


##  Variables used to defien the dependnecies of PAN.

LSRC = pan/src/TaBlind.cc pan/src/TaFileName.cc pan/src/TaIResultsFile.cc \
       pan/src/TaLabelledQuantity.cc \
       pan/src/TaOResultsFile.cc pan/src/TaStatistics.cc pan/src/TaString.cc 
LHEAD = $(LSRC:.cc=.hh) 
LHEAD += pan/src/PanTypes.hh
LHEAD += pan/src/DevTypes.hh


#################  Compilation targets

all: postpan

pan/src/libpan.a: $(LSRC) $(LHEAD)
	$(MAKE) -C pan/src           libpan.a

postpan: pan/src/libpan.a
	$(MAKE) -C postpan/src       install

clean:
	$(MAKE) -C postpan/src       clean
	$(MAKE) -C pan/panguin       clean
	$(MAKE) -C pan/pandb/DBTools clean
	$(MAKE) -C pan/src           clean
	$(MAKE) -C pan/codaclass     clean

realclean:
	$(MAKE) -C postpan/src       realclean
	$(MAKE) -C pan/panguin       realclean
	$(MAKE) -C pan/pandb/DBTools realclean
	$(MAKE) -C pan/src           realclean
	$(MAKE) -C pan/codaclass     realclean

.PHONY : clean realclean all postpan
