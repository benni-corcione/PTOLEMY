CC = g++
CFLAGS = -Wall -c -g -fpermissive 


INCLUDES = -I. -I.. -I$(ROOTSYS)/include -I$(ROOFIT_INCLUDE)/ 

ROOTSYS ?= ERROR_RootSysIsNotDefined

ROOTFLAG = `${ROOTSYS}/bin/root-config --cflags --libs`

EXTRALIBS := -L$(ROOTSYS)/lib -L$(ROOFIT_LIBDIR)/ -lHtml -lMathCore -lGenVector -lMinuit -lEG  -lPhysics -lz -lTreePlayer -lRooFitCore -lRooFit

ReadData: ReadData.cpp 
	$(CC) -Wall $(INCLUDES) -o ReadData ReadData.cpp $(ROOTFLAG) $(EXTRALIBS)

EfficiencyStudy: EfficiencyStudy.cpp 
	$(CC) -Wall $(INCLUDES) -o EfficiencyStudy EfficiencyStudy.cpp $(ROOTFLAG) $(EXTRALIBS)
