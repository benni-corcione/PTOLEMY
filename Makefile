CC = g++
CFLAGS = -Wall -c -g -fpermissive 


INCLUDES = -I. -I.. -I$(ROOTSYS)/include -I$(ROOFIT_INCLUDE)/ 

ROOTSYS ?= ERROR_RootSysIsNotDefined

ROOTFLAG = `${ROOTSYS}/bin/root-config --cflags --libs`

EXTRALIBS := -L$(ROOTSYS)/lib -L$(ROOFIT_LIBDIR)/ -lHtml -lMathCore -lGenVector -lMinuit -lEG  -lPhysics -lz -lTreePlayer -lRooFitCore -lRooFit


amp_charge: amp_charge.cpp graphics.h pshape_functions.h
	$(CC) -Wall $(INCLUDES) -o amp_charge amp_charge.cpp $(ROOTFLAG) $(EXTRALIBS)

B60_H60: B60_H60.cpp graphics.h pshape_functions.h
	$(CC) -Wall $(INCLUDES) -o B60_H60 B60_H60.cpp $(ROOTFLAG) $(EXTRALIBS)

checkpshape: checkpshape.cpp 
	$(CC) -Wall $(INCLUDES) -o checkpshape checkpshape.cpp $(ROOTFLAG) $(EXTRALIBS)

checkpshapePhotons: checkpshapePhotons.cpp 
	$(CC) -Wall $(INCLUDES) -o checkpshapePhotons checkpshapePhotons.cpp $(ROOTFLAG) $(EXTRALIBS)

checkpshape_10k: checkpshape_10k.cpp 
	$(CC) -Wall $(INCLUDES) -o checkpshape_10k checkpshape_10k.cpp $(ROOTFLAG) $(EXTRALIBS)

dark_counts: dark_counts.cpp 
	$(CC) -Wall $(INCLUDES) -o dark_counts dark_counts.cpp $(ROOTFLAG) $(EXTRALIBS)

double_check: double_check.cpp 
	$(CC) -Wall $(INCLUDES) -o double_check double_check.cpp $(ROOTFLAG) $(EXTRALIBS)

draw_three_electrons: draw_three_electrons.cpp
	$(CC) -Wall $(INCLUDES) -o draw_three_electrons draw_three_electrons.cpp $(ROOTFLAG) $(EXTRALIBS)

drawAmpCharge: drawAmpCharge.cpp graphics.h pshape_functions.h
	$(CC) -Wall $(INCLUDES) -o drawAmpCharge drawAmpCharge.cpp $(ROOTFLAG) $(EXTRALIBS)

drawFinalFits: drawFinalFits.cpp AndCommon.h graphics.h CD_details.h
	$(CC) -Wall $(INCLUDES) -o drawFinalFits drawFinalFits.cpp $(ROOTFLAG) $(EXTRALIBS)

fitAmpChargeData: fitAmpChargeData.cpp graphics.h pshape_functions.h
	$(CC) -Wall $(INCLUDES) -o fitAmpChargeData fitAmpChargeData.cpp $(ROOTFLAG) $(EXTRALIBS)

makeAllrawTree: makeAllrawTree.cpp AndCommon.h
	$(CC) -Wall $(INCLUDES) -o makeAllrawTree makeAllrawTree.cpp $(ROOTFLAG) $(EXTRALIBS)

makeAllrawTreePhotons: makeAllrawTreePhotons.cpp AndCommon.h
	$(CC) -Wall $(INCLUDES) -o makeAllrawTreePhotons makeAllrawTreePhotons.cpp $(ROOTFLAG) $(EXTRALIBS)

makeAllrawTreePhotons_10k: makeAllrawTreePhotons_10k.cpp AndCommon.h
	$(CC) -Wall $(INCLUDES) -o makeAllrawTreePhotons_10k makeAllrawTreePhotons_10k.cpp $(ROOTFLAG) $(EXTRALIBS)

makeRawTree: makeRawTree.cpp AndCommon.h lecroyparser.h
	$(CC) -Wall $(INCLUDES) -o makeRawTree makeRawTree.cpp $(ROOTFLAG) $(EXTRALIBS)


makeRawTreePhotons: makeRawTreePhotons.cpp AndCommon.h lecroyparser.h
	$(CC) -Wall $(INCLUDES) -o makeRawTreePhotons makeRawTreePhotons.cpp $(ROOTFLAG) $(EXTRALIBS)

makeRawTreePhotons_10k: makeRawTreePhotons_10k.cpp AndCommon.h lecroyparser.h
	$(CC) -Wall $(INCLUDES) -o makeRawTreePhotons_10k makeRawTreePhotons_10k.cpp $(ROOTFLAG) $(EXTRALIBS)

makeTree: makeTree.cpp pshape_functions.h
	$(CC) -Wall $(INCLUDES) -o makeTree makeTree.cpp $(ROOTFLAG) $(EXTRALIBS)

makeTreePhotons: makeTreePhotons.cpp pshape_functions.h
	$(CC) -Wall $(INCLUDES) -o makeTreePhotons makeTreePhotons.cpp $(ROOTFLAG) $(EXTRALIBS)

makeTreePhotons_10k: makeTreePhotons_10k.cpp pshape_functions_10k.h
	$(CC) -Wall $(INCLUDES) -o makeTreePhotons_10k makeTreePhotons_10k.cpp $(ROOTFLAG) $(EXTRALIBS)

persistence_plot: persistence_plot.cpp pshape_functions.h
	$(CC) -Wall $(INCLUDES) -o persistence_plot persistence_plot.cpp $(ROOTFLAG) $(EXTRALIBS)

persistence_plot_photons: persistence_plot_photons.cpp pshape_functions.h
	$(CC) -Wall $(INCLUDES) -o persistence_plot_photons persistence_plot_photons.cpp $(ROOTFLAG) $(EXTRALIBS)

persistence_plot_10k: persistence_plot_10k.cpp pshape_functions_10k.h
	$(CC) -Wall $(INCLUDES) -o persistence_plot_10k persistence_plot_10k.cpp $(ROOTFLAG) $(EXTRALIBS)

time_correlation: time_correlation.cpp 
	$(CC) -Wall $(INCLUDES) -o time_correlation time_correlation.cpp $(ROOTFLAG) $(EXTRALIBS)

wp_analysis: wp_analysis.cpp pshape_functions.h graphics.h
	$(CC) -Wall $(INCLUDES) -o wp_analysis wp_analysis.cpp $(ROOTFLAG) $(EXTRALIBS)

writeHistoPhotons: writeHistoPhotons.cpp 
	$(CC) -Wall $(INCLUDES) -o writeHistoPhotons writeHistoPhotons.cpp $(ROOTFLAG) $(EXTRALIBS)







