CC = g++
CFLAGS = -Wall -c -g -fpermissive 


INCLUDES = -I. -I.. -I$(ROOTSYS)/include -I$(ROOFIT_INCLUDE)/ 

ROOTSYS ?= ERROR_RootSysIsNotDefined

ROOTFLAG = `${ROOTSYS}/bin/root-config --cflags --libs`

EXTRALIBS := -L$(ROOTSYS)/lib -L$(ROOFIT_LIBDIR)/ -lHtml -lMathCore -lGenVector -lMinuit -lEG  -lPhysics -lz -lTreePlayer -lRooFitCore -lRooFit

addBranchFilteredPShapes_cd188 : addBranchFilteredPShapes_cd188.cpp graphics.h pshape_functions.h
	$(CC) -Wall $(INCLUDES) -o addBranchFilteredPShapes_cd188 addBranchFilteredPShapes_cd188.cpp $(ROOTFLAG) $(EXTRALIBS)

addBranchFilteredPShapes_cd204 : addBranchFilteredPShapes_cd204.cpp graphics.h pshape_functions.h
	$(CC) -Wall $(INCLUDES) -o addBranchFilteredPShapes_cd204 addBranchFilteredPShapes_cd204.cpp $(ROOTFLAG) $(EXTRALIBS)

addBranchFilteredPShapes_cd222 : addBranchFilteredPShapes_cd222.cpp graphics.h pshape_functions.h
	$(CC) -Wall $(INCLUDES) -o addBranchFilteredPShapes_cd222 addBranchFilteredPShapes_cd222.cpp $(ROOTFLAG) $(EXTRALIBS)

amp_charge: amp_charge.cpp graphics.h pshape_functions.h
	$(CC) -Wall $(INCLUDES) -o amp_charge amp_charge.cpp $(ROOTFLAG) $(EXTRALIBS)

articledrawFits: articledrawFits.cpp graphics.h pshape_functions.h
	$(CC) -Wall $(INCLUDES) -o articledrawFits articledrawFits.cpp $(ROOTFLAG) $(EXTRALIBS)

articledrawMu: articledrawMu.cpp graphics.h pshape_functions.h
	$(CC) -Wall $(INCLUDES) -o articledrawMu articledrawMu.cpp $(ROOTFLAG) $(EXTRALIBS)

B60_H60: B60_H60.cpp graphics.h pshape_functions.h
	$(CC) -Wall $(INCLUDES) -o B60_H60 B60_H60.cpp $(ROOTFLAG) $(EXTRALIBS)

checkpshape: checkpshape.cpp 
	$(CC) -Wall $(INCLUDES) -o checkpshape checkpshape.cpp $(ROOTFLAG) $(EXTRALIBS)

checkpshape_filtered_cd188: checkpshape_filtered_cd188.cpp 
	$(CC) -Wall $(INCLUDES) -o checkpshape_filtered_cd188 checkpshape_filtered_cd188.cpp $(ROOTFLAG) $(EXTRALIBS)

checkpshape_filtered_cd204: checkpshape_filtered_cd204.cpp 
	$(CC) -Wall $(INCLUDES) -o checkpshape_filtered_cd204 checkpshape_filtered_cd204.cpp $(ROOTFLAG) $(EXTRALIBS)

checkpshape_filtered_cd222: checkpshape_filtered_cd222.cpp 
	$(CC) -Wall $(INCLUDES) -o checkpshape_filtered_cd222 checkpshape_filtered_cd222.cpp $(ROOTFLAG) $(EXTRALIBS)

checkpshapePhotons: checkpshapePhotons.cpp 
	$(CC) -Wall $(INCLUDES) -o checkpshapePhotons checkpshapePhotons.cpp $(ROOTFLAG) $(EXTRALIBS)

checkpshape_10k: checkpshape_10k.cpp 
	$(CC) -Wall $(INCLUDES) -o checkpshape_10k checkpshape_10k.cpp $(ROOTFLAG) $(EXTRALIBS)

CheckTimeConst: CheckTimeConst.cpp
	$(CC) -Wall $(INCLUDES) -o CheckTimeConst CheckTimeConst.cpp $(ROOTFLAG) $(EXTRALIBS)

coincidence: coincidence.cpp 
	$(CC) -Wall $(INCLUDES) -o coincidence coincidence.cpp $(ROOTFLAG) $(EXTRALIBS)

coincidence_old: coincidence_old.cpp 
	$(CC) -Wall $(INCLUDES) -o coincidence_old coincidence_old.cpp $(ROOTFLAG) $(EXTRALIBS)

compareRate: compareRate.cpp graphics.h pshape_functions.h CD_details.h AndCommon.h
	$(CC) -Wall $(INCLUDES) -o compareRate compareRate.cpp $(ROOTFLAG) $(EXTRALIBS)

compareRateFE: compareRateFE.cpp graphics.h pshape_functions.h CD_details.h AndCommon.h
	$(CC) -Wall $(INCLUDES) -o compareRateFE compareRateFE.cpp $(ROOTFLAG) $(EXTRALIBS)

computeRate_nopileupplot: computeRate_nopileupplot.cpp graphics.h pshape_functions.h CD_details.h AndCommon.h
	$(CC) -Wall $(INCLUDES) -o computeRate_nopileupplot computeRate_nopileupplot.cpp $(ROOTFLAG) $(EXTRALIBS)

computeRate: computeRate.cpp graphics.h pshape_functions.h CD_details.h AndCommon.h
	$(CC) -Wall $(INCLUDES) -o computeRate computeRate.cpp $(ROOTFLAG) $(EXTRALIBS)

computeRate_basic: computeRate_basic.cpp graphics.h pshape_functions.h CD_details.h AndCommon.h
	$(CC) -Wall $(INCLUDES) -o computeRate_basic computeRate_basic.cpp $(ROOTFLAG) $(EXTRALIBS)

correctRate: correctRate.cpp graphics.h pshape_functions.h CD_details.h AndCommon.h
	$(CC) -Wall $(INCLUDES) -o correctRate correctRate.cpp $(ROOTFLAG) $(EXTRALIBS)

correctRate_bis: correctRate_bis.cpp graphics.h pshape_functions.h CD_details.h AndCommon.h
	$(CC) -Wall $(INCLUDES) -o correctRate_bis correctRate_bis.cpp $(ROOTFLAG) $(EXTRALIBS)

computeRate_darkcounts: computeRate_darkcounts.cpp graphics.h pshape_functions.h CD_details.h AndCommon.h
	$(CC) -Wall $(INCLUDES) -o computeRate_darkcounts computeRate_darkcounts.cpp $(ROOTFLAG) $(EXTRALIBS)

computeRate_fp: computeRate_fp.cpp graphics.h pshape_functions.h CD_details.h AndCommon.h
	$(CC) -Wall $(INCLUDES) -o computeRate_fp computeRate_fp.cpp $(ROOTFLAG) $(EXTRALIBS)

dark_counts: dark_counts.cpp 
	$(CC) -Wall $(INCLUDES) -o dark_counts dark_counts.cpp $(ROOTFLAG) $(EXTRALIBS)

double_check: double_check.cpp  pshape_functions.h
	$(CC) -Wall $(INCLUDES) -o double_check double_check.cpp $(ROOTFLAG) $(EXTRALIBS)

draw_three_electrons: draw_three_electrons.cpp
	$(CC) -Wall $(INCLUDES) -o draw_three_electrons draw_three_electrons.cpp $(ROOTFLAG) $(EXTRALIBS)

draw_ctrls: draw_ctrls.cpp
	$(CC) -Wall $(INCLUDES) -o draw_ctrls draw_ctrls.cpp $(ROOTFLAG) $(EXTRALIBS)

drawAmpCharge: drawAmpCharge.cpp graphics.h pshape_functions.h
	$(CC) -Wall $(INCLUDES) -o drawAmpCharge drawAmpCharge.cpp $(ROOTFLAG) $(EXTRALIBS)

drawFinalFits: drawFinalFits.cpp AndCommon.h graphics.h CD_details.h
	$(CC) -Wall $(INCLUDES) -o drawFinalFits drawFinalFits.cpp $(ROOTFLAG) $(EXTRALIBS)

drawLowAmp: drawLowAmp.cpp AndCommon.h graphics.h CD_details.h
	$(CC) -Wall $(INCLUDES) -o drawLowAmp drawLowAmp.cpp $(ROOTFLAG) $(EXTRALIBS)

drawMultipleHistos: drawMultipleHistos.cpp graphics.h pshape_functions.h
	$(CC) -Wall $(INCLUDES) -o drawMultipleHistos drawMultipleHistos.cpp $(ROOTFLAG) $(EXTRALIBS)

drawMultipleFits: drawMultipleFits.cpp graphics.h pshape_functions.h CD_details.h
	$(CC) -Wall $(INCLUDES) -o drawMultipleFits drawMultipleFits.cpp $(ROOTFLAG) $(EXTRALIBS)

drawResolutionCFR: drawResolutionCFR.cpp graphics.h pshape_functions.h
	$(CC) -Wall $(INCLUDES) -o drawResolutionCFR drawResolutionCFR.cpp $(ROOTFLAG) $(EXTRALIBS)

drawResolutionCFRall: drawResolutionCFRall.cpp graphics.h pshape_functions.h
	$(CC) -Wall $(INCLUDES) -o drawResolutionCFRall drawResolutionCFRall.cpp $(ROOTFLAG) $(EXTRALIBS)

drawResolutionCFRall_100kHz: drawResolutionCFRall_100kHz.cpp graphics.h pshape_functions.h
	$(CC) -Wall $(INCLUDES) -o drawResolutionCFRall_100kHz drawResolutionCFRall_100kHz.cpp $(ROOTFLAG) $(EXTRALIBS)

drawResolutionCFRall_10kHz: drawResolutionCFRall_10kHz.cpp graphics.h pshape_functions.h
	$(CC) -Wall $(INCLUDES) -o drawResolutionCFRall_10kHz drawResolutionCFRall_10kHz.cpp $(ROOTFLAG) $(EXTRALIBS)

drawResolutionCFR_filters_cd188: drawResolutionCFR_filters_cd188.cpp graphics.h pshape_functions.h
	$(CC) -Wall $(INCLUDES) -o drawResolutionCFR_filters_cd188 drawResolutionCFR_filters_cd188.cpp $(ROOTFLAG) $(EXTRALIBS)

drawResolutionCFR_filters_cd204: drawResolutionCFR_filters_cd204.cpp graphics.h pshape_functions.h
	$(CC) -Wall $(INCLUDES) -o drawResolutionCFR_filters_cd204 drawResolutionCFR_filters_cd204.cpp $(ROOTFLAG) $(EXTRALIBS)

drawResolutionCFR_filters_cd222: drawResolutionCFR_filters_cd222.cpp graphics.h pshape_functions.h
	$(CC) -Wall $(INCLUDES) -o drawResolutionCFR_filters_cd222 drawResolutionCFR_filters_cd222.cpp $(ROOTFLAG) $(EXTRALIBS)

drawPowCFR: drawPowCFR.cpp graphics.h pshape_functions.h
	$(CC) -Wall $(INCLUDES) -o drawPowCFR drawPowCFR.cpp $(ROOTFLAG) $(EXTRALIBS)

drawSuperimposedHistos: drawSuperimposedHistos.cpp
	$(CC) -Wall $(INCLUDES) -o drawSuperimposedHistos drawSuperimposedHistos.cpp $(ROOTFLAG) $(EXTRALIBS)

FilterAmpCal: FilterAmpCal.cpp AndCommon.h graphics.h CD_details.h
	$(CC) -Wall $(INCLUDES) -o FilterAmpCal FilterAmpCal.cpp $(ROOTFLAG) $(EXTRALIBS)

fitAmpChargeData: fitAmpChargeData.cpp graphics.h pshape_functions.h
	$(CC) -Wall $(INCLUDES) -o fitAmpChargeData fitAmpChargeData.cpp $(ROOTFLAG) $(EXTRALIBS)

lowAmpPeak: lowAmpPeak.cpp graphics.h pshape_functions.h
	$(CC) -Wall $(INCLUDES) -o lowAmpPeak lowAmpPeak.cpp $(ROOTFLAG) $(EXTRALIBS)

lowAmpPeak_pro: lowAmpPeak_pro.cpp graphics.h pshape_functions.h
	$(CC) -Wall $(INCLUDES) -o lowAmpPeak_pro lowAmpPeak_pro.cpp $(ROOTFLAG) $(EXTRALIBS)

makeAllrawTree: makeAllrawTree.cpp AndCommon.h makeRawTree.cpp
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

makeTree_BolCal: makeTree_BolCal.cpp pshape_functions.h
	$(CC) -Wall $(INCLUDES) -o makeTree_BolCal makeTree_BolCal.cpp $(ROOTFLAG) $(EXTRALIBS)

makeTree_high: makeTree_high.cpp pshape_functions.h
	$(CC) -Wall $(INCLUDES) -o makeTree_high makeTree_high.cpp $(ROOTFLAG) $(EXTRALIBS)

makeTreePhotons: makeTreePhotons.cpp pshape_functions.h
	$(CC) -Wall $(INCLUDES) -o makeTreePhotons makeTreePhotons.cpp $(ROOTFLAG) $(EXTRALIBS)

makeTreePhotons_10k: makeTreePhotons_10k.cpp pshape_functions_10k.h
	$(CC) -Wall $(INCLUDES) -o makeTreePhotons_10k makeTreePhotons_10k.cpp $(ROOTFLAG) $(EXTRALIBS)

persistence_plot: persistence_plot.cpp pshape_functions.h
	$(CC) -Wall $(INCLUDES) -o persistence_plot persistence_plot.cpp $(ROOTFLAG) $(EXTRALIBS)

persistence_plot_10k: persistence_plot_10k.cpp pshape_functions_10k.h
	$(CC) -Wall $(INCLUDES) -o persistence_plot_10k persistence_plot_10k.cpp $(ROOTFLAG) $(EXTRALIBS)

persistence_plot_photons: persistence_plot_photons.cpp pshape_functions.h
	$(CC) -Wall $(INCLUDES) -o persistence_plot_photons persistence_plot_photons.cpp $(ROOTFLAG) $(EXTRALIBS)

pshape_characterization: pshape_characterization.cpp pshape_functions.h
	$(CC) -Wall $(INCLUDES) -o pshape_characterization pshape_characterization.cpp $(ROOTFLAG) $(EXTRALIBS)

SelfCalCurve: SelfCalCurve.cpp
	$(CC) -Wall $(INCLUDES) -o SelfCalCurve SelfCalCurve.cpp $(ROOTFLAG) $(EXTRALIBS)

SelfCalibration: SelfCalibration.cpp
	$(CC) -Wall $(INCLUDES) -o SelfCalibration SelfCalibration.cpp $(ROOTFLAG) $(EXTRALIBS)

SelfCalibrationPhotons: SelfCalibrationPhotons.cpp
	$(CC) -Wall $(INCLUDES) -o SelfCalibrationPhotons SelfCalibrationPhotons.cpp $(ROOTFLAG) $(EXTRALIBS)

time_correlation: time_correlation.cpp 
	$(CC) -Wall $(INCLUDES) -o time_correlation time_correlation.cpp $(ROOTFLAG) $(EXTRALIBS)

wp_analysis: wp_analysis.cpp pshape_functions.h graphics.h
	$(CC) -Wall $(INCLUDES) -o wp_analysis wp_analysis.cpp $(ROOTFLAG) $(EXTRALIBS)

writeHistoPhotons: writeHistoPhotons.cpp 
	$(CC) -Wall $(INCLUDES) -o writeHistoPhotons writeHistoPhotons.cpp $(ROOTFLAG) $(EXTRALIBS)

TimeConstantStudy: TimeConstantStudy.cpp
	$(CC) -Wall $(INCLUDES) -o TimeConstantStudy TimeConstantStudy.cpp $(ROOTFLAG) $(EXTRALIBS)






