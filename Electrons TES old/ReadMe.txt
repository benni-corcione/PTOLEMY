LIBRARIES:

common   : FP function library (plots,, TGraph, string, some statistical analysis)
graphics : graphic options and similar functions recalled in programs

PROGRAMS C++:

drawAmp100               : draw of 100 V histogram of paper 1 (2024) 
drawFinalFits_Syst_CD188 : draw superimposed FWHM calculated from raw data and asymmetric gaussian fit function and then estimate systematic uncertainty as the discrepancy obtained in the resolution (fwhm_err derived from the sigma_R and sigma_L errors) for cd188 without any filter
drawFits                 : fit histos and compute gaussian resolution compared to the photon one producing plots of paper 1 (2024)
drawPhotons              : produce photon plot of paper 1 (2024)
drawPowerTemp            : produce T and PJ plot as in paper 1 (2024)
drawRate                 : draw electron rate vs fe as in paper 1 (2024) - but rate trees need to be redone
drawSpectra              : produce comparison between cd 204 and cd 188 amp (inset) for paper 2 (2026)
drawVvsT                 : compute correction due to ites_ibias displacement and write it on file - still don't know how it compute the correction
