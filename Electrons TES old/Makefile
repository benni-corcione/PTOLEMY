CC = g++
CFLAGS = -Wall -c -g -fpermissive 

#ROOFIT_INCLUDE := $(shell cd $(CMSSW_BASE); scram tool info roofitcore | grep INCLUDE= | sed 's|INCLUDE=||')
#ROOFIT_LIBDIR := $(shell cd $(CMSSW_BASE); scram tool info roofitcore | grep LIBDIR= | sed 's|LIBDIR=||')

INCLUDES = -I. -I.. -I$(ROOTSYS)/include  -I../../CommonTools/ -I../../CommonTools/interface/ -I../interface/  #-I$(ROOFIT_INCLUDE)/ C-I$(CMSSW_BASE)/src 

ROOTSYS ?= ERROR_RootSysIsNotDefined

ROOTFLAG = `${ROOTSYS}/bin/root-config --cflags --libs`

EXTRALIBS := -L$(ROOTSYS)/lib -lHtml -lMathCore -lGenVector -lMinuit -lTreePlayer -lRooFitCore -lRooFit #-L$(ROOFIT_LIBDIR)/ 

drawFits : drawFits.cpp common.h
	$(CC) -Wall $(INCLUDES) -o drawFits drawFits.cpp $(ROOTFLAG) $(EXTRALIBS)

drawAmp100: drawAmp100.cpp common.h
	$(CC) -Wall $(INCLUDES) -o drawAmp100 drawAmp100.cpp $(ROOTFLAG) $(EXTRALIBS)


drawFits_BC : drawFits_BC.cpp common.h
	$(CC) -Wall $(INCLUDES) -o drawFits_BC drawFits_BC.cpp $(ROOTFLAG) $(EXTRALIBS)

drawPow : drawPow.cpp common.h
	$(CC) -Wall $(INCLUDES) -o drawPow drawPow.cpp $(ROOTFLAG) $(EXTRALIBS)

drawSpectra : drawSpectra.cpp common.h
	$(CC) -Wall $(INCLUDES) -o drawSpectra drawSpectra.cpp $(ROOTFLAG) $(EXTRALIBS)
