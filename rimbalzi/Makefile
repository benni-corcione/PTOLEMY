CC = g++
CFLAGS = -Wall -c -g -fpermissive 


INCLUDES = -I. -I.. -I$(ROOTSYS)/include -I$(ROOFIT_INCLUDE)/ 

ROOTSYS ?= ERROR_RootSysIsNotDefined

ROOTFLAG = `${ROOTSYS}/bin/root-config --cflags --libs`

EXTRALIBS := -L$(ROOTSYS)/lib -L$(ROOFIT_LIBDIR)/ -lHtml -lMathCore -lGenVector -lMinuit -lEG  -lPhysics -lz -lTreePlayer -lRooFitCore -lRooFit

CheckAngles: CheckAngles.cpp 
	$(CC) -Wall $(INCLUDES) -o CheckAngles CheckAngles.cpp $(ROOTFLAG) $(EXTRALIBS)

Check_Arrivals: Check_Arrivals.cpp
	$(CC) -Wall $(INCLUDES) -o Check_Arrivals Check_Arrivals.cpp $(ROOTFLAG) $(EXTRALIBS)

CheckAngles_distgen: CheckAngles_distgen.cpp 
	$(CC) -Wall $(INCLUDES) -o CheckAngles_distgen CheckAngles_distgen.cpp $(ROOTFLAG) $(EXTRALIBS)
TestAngles: TestAngles.cpp 
	$(CC) -Wall $(INCLUDES) -o TestAngles TestAngles.cpp $(ROOTFLAG) $(EXTRALIBS)

Disaligned_TES: Disaligned_TES.cpp sim_details.h
	$(CC) -Wall $(INCLUDES) -o Disaligned_TES Disaligned_TES.cpp $(ROOTFLAG) $(EXTRALIBS)

EnergyDistance: EnergyDistance.cpp sim_details.h
	$(CC) -Wall $(INCLUDES) -o EnergyDistance EnergyDistance.cpp $(ROOTFLAG) $(EXTRALIBS)

Energy_Disaligned: Energy_Disaligned.cpp sim_details.h
	$(CC) -Wall $(INCLUDES) -o Energy_Disaligned Energy_Disaligned.cpp $(ROOTFLAG) $(EXTRALIBS)

HitsvsDistance: HitsvsDistance.cpp sim_details.h
	$(CC) -Wall $(INCLUDES) -o HitsvsDistance HitsvsDistance.cpp $(ROOTFLAG) $(EXTRALIBS)


PositionMap: PositionMap.cpp 
	$(CC) -Wall $(INCLUDES) -o PositionMap PositionMap.cpp $(ROOTFLAG) $(EXTRALIBS)

PositionMap_distgen: PositionMap_distgen.cpp sim_details.h
	$(CC) -Wall $(INCLUDES) -o PositionMap_distgen PositionMap_distgen.cpp $(ROOTFLAG) $(EXTRALIBS)

Presim: Presim.cpp sim_details.h
	$(CC) -Wall $(INCLUDES) -o Presim Presim.cpp $(ROOTFLAG) $(EXTRALIBS)

ReadData: ReadData.cpp 
	$(CC) -Wall $(INCLUDES) -o ReadData ReadData.cpp $(ROOTFLAG) $(EXTRALIBS)

ReadData_distgen: ReadData_distgen.cpp 
	$(CC) -Wall $(INCLUDES) -o ReadData_distgen ReadData_distgen.cpp $(ROOTFLAG) $(EXTRALIBS)

SuperImposedHistos: SuperImposedHistos.cpp
	$(CC) -Wall $(INCLUDES) -o SuperImposedHistos SuperImposedHistos.cpp $(ROOTFLAG) $(EXTRALIBS)

