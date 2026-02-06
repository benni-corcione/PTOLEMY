LIBRARIES:

AndCommon            : FP function library (plots,, TGraph, string, some statistical analysis)
CD_details           : info to reconstruct plot and analysis on different cooldowns
common               : as AndCommon
graphics             : graphic options and similar functions recalled in programs
lecroyparser         : library to read trc files from waverunner
pshape_functions     : functions acting on the pshapes saved point by point (2500 pt per pshape)
pshape_functions_10k : functions acting on the pshapes saved point by point (10000 pt per pshape)


PROGRAMS C++:

addBranchFilteredPShapes_cd188  : add to tree a branch for each low-pass filter previously applied on the pshapes and saved in a dedicated file for cd188 (100kHz, 10kHz, 100kHz+10kHz, 160kHz+10kHz, 160kHz+100kHz+10kHz) - raw data without any filter
addBranchFilteredPShapes_cd204  : add to tree a branch for each low-pass filter previously applied on the pshapes and saved in a dedicated file for cd204 (10kHz, 160kHz+10kHz) - raw data taken with bessel at 100kHz
addBranchFilteredPShapes_cd222  : add to tree a branch for each low-pass filter previously applied on the pshapes and saved in a dedicated file for cd222 (10kHz, 100kHz, 10kHz+10kHz) - raw data taken with low-pass at 160kHz+10kHz
amp_charge                      : plot of amp vs charge of a dataset of a given voltage (with linear fit)
articledrawFits                 : drawing of two superhimposed histos of amp with fits to show how the full-absorption peak scales with energy
B60_H60                         : superimposed histos of counts of aligned/disaligned TES wrt CNTs, where the tree have to be declared inside the program
bin_study                       : study of how much the different binning influences the fit results on cd 188 - but it has to be corrected
check_pshape_double             : draw pshape that satisfy requirements on pshape ctrl - to be corrected to the updated ctrls present in the trees
checkpshape                     : draw selected pshape of electron (2500 pt per pshape)
checkpshapePhotons              : draw selected pshape of photon (different tree wrt electrons) (2500 pt per pshape)
checkpshape_10k                 : draw selected pshape of electron (10000 pt per pshape)
checkpshape_filtered_cd188      : draw selected pshape of electron comparing different filterings for cd1 188
checkpshape_filtered_cd204      : draw selected pshape of electron comparing different filterings for cd1 204
checkpshape_filtered_cd222      : draw selected pshape of electron comparing different filterings for cd1 222
CheckTimeConst                  : take dataset and fit all pshapes that passes routine ctrls, then takes all the so-obtained time constants and make an histogram of them
coincidence                     : check for simoultaneous events on TESs not aligned under CNTs (electrons bouncing from one TES to another, and the time resolution of TES is not high enough to discriminate them), plot them if exist and count the total (CD 204)
compareRate                     : compare rate of CNTs emission after conditionings of single signals and pileupped signals
compareRateFE                   : compare rate of CNTs emission from signal rate with the one estrapolated by the FE current registered with the MIC
computeRate                     : compute rate of signals from signal arrival times
computeRate_basic               : compute rate of signals without distinguishing double events
computeRate_darkcounts          : compute rate of dark counts
computeRate_fp                  : compute rate of with a slow and basic/not accurate program of FP
computeRate_nopileupplot        : compute rate of signals and plotting the rate without pileup
correctRate                     : 1. we start from rate computed with also dark counts photon-like 2.take ad a reference the histo of dark counts and, minimizing the chi square of histo_tot - histo_dark counts, find the best scale factor that, once multiplied to the histo_dark counts, make it corresponds to the first part of the histogram (assumption: all the low-energy events in the very first bins corresponds only to dark counts) 3.the histo dark counts is then subtracted bin per bin from the histo_tot 4.rate is corrected as: tot rate * real events/total events.
dark_counts                     : persistency plot of pshapes + histos of amp & charge
double_check                    : algorithm with deltaY cumulated and smoothed to individuate double events; each double event is drawn
drawAmpCharge                   : draw amp and charge of specific dataset
drawAmpCharge_filtered_cd188    : draw amp and charge of specific dataset after low-pass filters being applied for cd 188
drawFinalFits                   : draw fits and final trends of mu and reso for each cd, meas, amp/charge chosen, only if the functions are updated with the correct options of draw and fits (to fastly recover drawings and fits of 'stacks' of  atasets), in the final version used for the papers
drawFinalFits_cb                : draw superimposed fit of gaussian and crystal ball and estimate systematic uncertainty on gaussian resolution as discrepancy (not on fwhm resolution as no error can be applied to the fwhm numerically estimated from the fit)
drawFinalFits_syst_CD188f       : draw superimposed FWHM calculated from raw data and asymmetric gaussian fit function and then estimate systematic uncertainty as the discrepancy obtained in the resolution (fwhm_err derived from the sigma_R and sigma_L errors) for cd188 filtered at 100kHz
drawFinalFits_syst_CD204        : draw superimposed FWHM calculated from raw data and crystal ball fit function and then estimate systematic uncertainty as the discrepancy obtained in the resolution (fwhm_err = 0 as no error could be associated to the fhwm numerically calculated from the fit results) for cd204
drawFits                        : draw fits indentical to the ones of the paper (2024)
drawFits_cb                     : draw crystal ball fits on the data of the paper (2024)
drawLowAmp                      : draw low amplitude peak which appears in some histos (e.g. cd 204)
drawMultipleFits                : draw two superimposed fits with arbitrary units on x, as presented in the second paper (206)
drawMultipleHistos              : draw superimposed histos of amp of cd 188, cd 204 and cd 222
drawPowCFR                      : draw comparison of Joule power dissipated by TES in different cd, normalized to the minimum Vcnt 
drawResolutionCFR_filters_cd188 : draw comparison of energy resolution (both gaus and fwhm) of cd188 with only statistical uncertainty, comparing every filter applied (10kHz, 100kHz, no filter, 100kHz+100kHz, 160kHz+10kHz, 160kHz+100kHz+10kHz)
drawResolutionCFR_filters_cd204 : draw comparison of energy resolution (both gaus and fwhm) of cd204 with only statistical uncertainty, comparing every filter applied (10kHz, no filter)
drawResolutionCFR_filters_cd222 : draw comparison of energy resolution (both gaus and fwhm) of cd222 with only statistical uncertainty, comparing every filter applied (100kHz, 10kHz, no filter, 10kHz+10kHz)
drawResolutionCFRall            : draw comparison of energy resolution (both gaus and fwhm) between cd188, cd204 and cd222, with only statistical uncertainty and no filters applied except for the ones of the datataking
drawResolutionCFRall_100kHz     : draw comparison of energy resolution (both gaus and fwhm) between cd188, cd188_100kHz and cd204, with both statistical and systematic uncertainty applied 
drawSpectra                     : draw cd204 amp with cd188 amp in inset as for the paper
drawSuperimposedHistos          : draw multiple histos one on top of the other fitting the amp full-absorption peak
drawSyst_Bin                    : cfr with plot and fit of same datset fitted at different binnings to calculate the systematics from the discrepancy in the energy resolution
drawSyst_Range                  : cfr with plot and fit of same datset fitted at different fitmin and fitmax to calculate the systematics from the discrepancy in the energy resolution
draw_ctrls                      : draw pshape with selected controls (charge, width, doubles triggered)
draw_three_electrons            : draw pshapes of three electrons superimposed (fit if you want)
fitAmpChargeData                : fit of histograms (ranges and fits params have to be decided)
lowAmpPeak                      : asymmetric gaussian fit of low amp peak (e.g. cd 204)
lowAmpPeak_pro                  : asymmetric gaussian fit of low amp peak (e.g. cd 204) + exponential fit (or polynomial) on which the peak arises
makeAllrawTree                  : loop over files in folder that compiles makeRawTree (2500 pt per pshape)
makeAllrawTreePhotons           : loop over files in folder (with photons only please) that compiles makeRawTreePhotons (2500 pt per pshape)
makeAllrawTreePhotons_10k       : loop over files in folder (with photons only please) that compiles makeRawTreePhotons (10000 pt per pshape)
makeRawTree                     : create one rawtree for each file.trc read (2500 pt per pshape)
makeRawTreePhotons              : create one rawtree for each file.trc read (photons only) (2500 pt per pshape)
makeRawTreePhotons_10k          : create one rawtree for each file.trc read (photons only) (10000 pt per pshape)
makeTree                        : unifies rawtrees in one single rawtree and make a high level quantities tree (2500 pt per pshape)
makeTreePhotons                 : unifies rawtrees in one single rawtree and make a high level quantities tree (photons only) (2500 pt per pshape)
makeTreePhotons_10k             : unifies rawtrees in one single rawtree and make a high level quantities tree (photons only) (10000 pt per pshape)
makeTree_high                   : add branches of cosmic rays and electronic noise label from standard tree
mean_shape                      : draw dynamic averaged pshape of selected pshape
persistance_plot                : draw persistance plot of pshapes (2500 pt per pshape)
persistance_plot_10k            : draw persistance plot of pshapes (10000 pt per pshape)
persistance_plot_photons        : draw persistance plot of pshapes of photons (2500 pt per pshape)
pshape_characterization         : for a complete dataset, it separately draw each event belonging to electronics, cosmic rays and not triggered, placing them in separated folders
SelfCalCurve                    : makes a curve describing amp vs energy, deviding the total energy range in a finite number of steps. For each pshape the value of ETF energy is calculated and it will take part of the correspondent energy step. Then at the end of the cicle, each point will be constituted by the mean of the energies belonging to the step and the mean of the amplitudes of those signals, plotting such self-calibration curve
SelfCalibration                 : starting from a dataset (of electrons), it takes the pshape and calculate the ETF energy, filling and plotting a ETF energy histogram (to be compared to the amp one)      
SelfCalibrationPhotons          : starting from a dataset (of photons), it takes the pshape and calculate the ETF energy, filling and plotting a ETF energy histogram (to be compared to the amp one)   
SystematicFWHM                  : systematic uncertainty on fwhm resolution assigned considering three different binnings to assign manually (paper one, double and half) and calculating discrepancy between paper fwhm resolution and fwhm resolution of a different binning
wp_analysis                     : draw histos of amp/charge superimposed and normalized to 1 for each i_bias in the stack
writeHistoPhotons               : write on txt file amp/charge histo of photons (of decided bins) to import on Origin

