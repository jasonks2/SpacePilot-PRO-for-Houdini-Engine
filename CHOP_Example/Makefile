#!/bin/make -f
#

LIB	= PPDOUT
VERSION	= 1.0

#-----------------------------------------------------------------------
#   Please keep the list of sources sorted alphabetically
#

CPPFILES =  \
	PPD_PipeOut.C

OBJECTS=$(CPPFILES:.C=.o)

#-----------------------------------------------------------------------

default:	lib$(LIB).a


PPD_PipeOut.o: PPD_PipeOut.h PPD_PipeOut.C #if .h is changed .o must be recompiled
	${CXX} -c PPD_PipeOut.C ${CXXFLAGS}
	
lib$(LIB).a:	$(OBJECTS)
	ar crv $@ $(OBJECTS)

clean:
	rm -f *.o core a.out

clobber:	clean
	rm -f *.a

production:	default
