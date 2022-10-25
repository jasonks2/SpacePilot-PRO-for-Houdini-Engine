#!/bin/make -f
#

APPNAME = pipeout

#-----------------------------------------------------------------------

CPPFILES =	PPDOUT_Main.C

SPNAV_DIR = /localhome/jasonks2/Desktop/libspnav/libspnav-master
SPNAV_INCL = -I/${SPNAV_DIR}/src/

SPNAV_LIBS = -L/${SPNAV_DIR} -lspnav
SPNAV_LIBDEP = ${SPNAV_DIR}/libspnav.a

CHOP_DIR = ..
CHOP_PPDOUT_LIB = ${CHOP_DIR}/libPPDOUT.a 

OBJECTS=$(CPPFILES:.C=.o)

CXXFLAGS = -O -I..

LDFLAGS = -L.. \
	-lPPDOUT \
	-lm \
	${SPNAV_LIBS} \
	-lX11

#-----------------------------------------------------------------------

default:	$(APPNAME)

$(APPNAME):	$(OBJECTS) $(SPNAV_LIBDEP) $(CHOP_PPDOUT_LIB) 
	$(CXX) -o $@ $(OBJECTS) $(LDFLAGS) $(CHOP_PPDOUT_LIB) $(LDFLAGS)

clean:
	rm -f *.o a.out core

clobber:	clean
	rm -f $(APPNAME)

install:

production:
