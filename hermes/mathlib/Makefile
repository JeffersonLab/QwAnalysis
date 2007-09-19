SHELL=/bin/sh

VERS = 96.106
TARFILE=hrc-$(VERS).tar

#CFLAGS=-g -O3 -D__MATH_INLINES -funroll-loops -pipe -Df2cFortran -Wall -Wno-unused -Wno-switch -fwritable-strings -Wno-trigraphs -DCERN_Y2K_FIX -Wno-main -I.
CFLAGS=-g -O3 -D__MATH_INLINES -funroll-loops -pipe -Df2cFortran -Wall -Wno-unused -Wno-switch -Wno-trigraphs -DCERN_Y2K_FIX -Wno-main -I.

CC=gcc
TAR=tar

all: libwmath.a

../config.status: ../configure 
	cd ..; ./configure

Makefile: Makefile.in ../config.status
	../config.status Makefile.in

OBJECTS=gaussint.o gaussver.o m1norm.o m2norm.o maddzeil.o mamalb.o\
	mata.o matherr.o mausgabe.o mcholesk.o mdetermi.o mdyade.o merweite.o\
	mfill.o mfnorm.o mgauss.o mglobal.o minit.o minverse.o mkopiere.o\
	mloese.o minvpos.o\
	mlsqht.o mlsqnorm.o mmultipl.o mpivot.o mrlzerle.o msnorm.o mspaltv.o\
	mswaps.o mswapz.o mtranspo.o munorm.o mxauscho.o mxausrl.o mznorm.o\
	pnewton.o v1norm.o v2norm.o vadd.o vausgabe.o vbetrag.o\
	vplaxlin.o \
	veinheit.o vfill.o vinit.o vskalpro.o vspro.o vunorm.o deq.o

CFILES=$(OBJECTS:.o=.c)

dist:   
	cd ..; $(TAR) rvf $(TARFILE) \
		mathlib/*.[ch] mathlib/Makefile.in

$(OBJECTS): Makefile

libwmath.a: $(OBJECTS) Makefile
	    ar rvs libwmath.a $(OBJECTS)

depend:
	makedepend -- $(CFLAGS) -- $(CFILES)

clean:
	rm -f $(OBJECTS) libwmath.a

