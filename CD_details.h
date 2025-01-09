//funzioni con dettagli per plottare grafici finali

//get voltages for each cooldown
std::vector<int> GetVoltages(int CD_number, char* misura){
  std::vector<int> volts;
 
  if(CD_number == 188) {
    for(int i=0; i<11; i++){ volts.push_back(95+i); }
  }

  if(CD_number == 204 && strcmp(misura,"B60_preamp_post_cond1")==0){
    volts.push_back(97); volts.push_back(99);
    volts.push_back(101); volts.push_back(103);
    volts.push_back(106);
  }

   if(CD_number == 204 && strcmp(misura,"B60_post_cond3_moku")==0){
    for(int i=0; i<15; i++){ volts.push_back(96+i); }
  }

  return volts;
}

//get tes workfunction
float GetPhiTes(void){
  
  TH1D* h1_tesWF = new TH1D( "tes_WF", "", 100, 4., 5.);
  h1_tesWF->Fill(4.38); // these numbers from the measurements done by Alessandro
  h1_tesWF->Fill(4.34);
  h1_tesWF->Fill(4.44);
  h1_tesWF->Fill(4.37);
  h1_tesWF->Fill(4.25);
  h1_tesWF->Fill(4.41);
  h1_tesWF->Fill(4.37);
  h1_tesWF->Fill(4.29);
  h1_tesWF->Fill(4.56);
  
  float phi_tes = h1_tesWF->GetMean();
  std::cout << "Phi TES: " << phi_tes << " +/- " << h1_tesWF->GetMeanError() << " eV" << std::endl;
  return phi_tes;
}

//get y max of histo
float GetYMax(int CD_number, char* choice, char* misura){
  float ymax=0;
  if(CD_number == 188 && strcmp(choice,"amp"   )==0){ ymax = 2.3;}
  if(CD_number == 188 && strcmp(choice,"charge")==0){ ymax = 4.3;}
  if(CD_number == 204 && strcmp(choice,"amp"   )==0 && strcmp(misura,"B60_preamp_post_cond1")==0) { ymax = 1  ;}
  if(CD_number == 204 && strcmp(choice,"charge")==0 && strcmp(misura,"B60_preamp_post_cond1")==0){ ymax = 1.7;}
  if(CD_number == 204 && strcmp(choice,"amp")==0 && strcmp(misura,"B60_post_cond3_moku")==0){ ymax = 0.75;}

  if(CD_number == 204 && strcmp(choice,"charge")==0 && strcmp(misura,"B60_post_cond3_moku")==0){ ymax = 2.4;}
  
  return ymax;
}

//histo settings
Histo SetHistoValues(int CD_number, char* choice, char* misura){
  Histo var;
  float binsize;
  Settings settings;
  
  if(CD_number == 188 && strcmp(choice,"amp")==0){
    var.set_nbins(200);
    var.set_range_min(0.);
    var.set_range_max(1.);
    var.set_hist_min(0.);
    var.set_hist_max(1.);
    binsize = settings.Binsize((var.get_hist_max()-var.get_hist_min())/var.get_nbins());
    var.set_x_name("Amplitude [V]");
    var.set_y_name("Counts / " + to_str(binsize) + " V");
  }

  if(CD_number == 188 && strcmp(choice,"charge")==0){
    var.set_nbins(200);
    var.set_range_min(6);
    var.set_range_max(19);
    var.set_hist_min(0.);
    var.set_hist_max(19);
    binsize = settings.Binsize((var.get_hist_max()-var.get_hist_min())/var.get_nbins());
    var.set_x_name("Charge [uA]");
    var.set_y_name("Counts / " + to_str(binsize) + " uA");
  }

  if(CD_number == 204 && strcmp(choice,"amp")==0 && strcmp(misura,"B60_preamp_post_cond1")==0){
    var.set_nbins(550);
    var.set_hist_min(0.);
    var.set_hist_max(1.);
    var.set_range_min(0.55);
    var.set_range_max(0.7);
    binsize = settings.Binsize((var.get_hist_max()-var.get_hist_min())/var.get_nbins());
    var.set_x_name("Amplitude [V]");
    var.set_y_name("Counts / " + to_str(binsize) + " V");
  }
  
  if(CD_number == 204 && strcmp(choice,"charge")==0 && strcmp(misura,"B60_preamp_post_cond1")==0){
    var.set_nbins(400);
    var.set_hist_min(0.);
    var.set_hist_max(20.);
    var.set_range_min(6);
    var.set_range_max(11);
    binsize = settings.Binsize((var.get_hist_max()-var.get_hist_min())/var.get_nbins());
    var.set_x_name("Charge [uC]");
    var.set_y_name("Counts / " + to_str(binsize) + " V");
  }

  if(CD_number == 204 && strcmp(choice,"amp")==0 && strcmp(misura,"B60_post_cond3_moku")==0){
    var.set_nbins(725);
    var.set_hist_min(0.);
    var.set_hist_max(1.);
    var.set_range_min(0.55);
    var.set_range_max(0.67);
    binsize = settings.Binsize((var.get_hist_max()-var.get_hist_min())/var.get_nbins());
    var.set_x_name("Amplitude [V]");
    var.set_y_name("Counts / " + to_str(binsize) + " V");
  }

   if(CD_number == 204 && strcmp(choice,"charge")==0 && strcmp(misura,"B60_post_cond3_moku")==0){
    var.set_nbins(350);
    var.set_hist_min(0.);
    var.set_hist_max(20.);
    var.set_range_min(7);
    var.set_range_max(10.3);
    binsize = settings.Binsize((var.get_hist_max()-var.get_hist_min())/var.get_nbins());
    var.set_x_name("Amplitude [V]");
    var.set_y_name("Counts / " + to_str(binsize) + " V");
  }
  
  return var;
}

//set fit options
Fit SetFitValues(int CD_number, char* choice, TH1F* histo, int voltage, char* misura){
  Fit var;
  if(CD_number == 188 && strcmp(choice,"amp")==0){
    var.set_mu_min(0.73)     ;  var.set_mu_max(0.939999)  ;  var.set_mu(0.8)     ;
    var.set_sigmaL_min(0.01) ;  var.set_sigmaL_max(0.5);  var.set_sigmaL(0.1) ;
    var.set_sigmaR_min(0.001);  var.set_sigmaR_max(0.1);  var.set_sigmaR(0.02);
    var.set_alfa_min(0)      ;  var.set_alfa_max(1)    ;  var.set_alfa(0)     ;
    var.set_A_min(1)         ;  var.set_A_max(3000)    ;
    var.set_A(histo->Integral());

    //fit limits
    if(voltage<100. && voltage!=97.){var.set_fit_min(0.5); }
    else                            {var.set_fit_min(0.6); }
    var.set_fit_max(0.96);
  }

   if(CD_number == 188 && strcmp(choice,"charge")==0){
    var.set_mu_min(10)     ;  var.set_mu_max(17)  ;  var.set_mu(13)     ;
    var.set_sigmaL_min(0.1) ;  var.set_sigmaL_max(6);  var.set_sigmaL(1) ;
    var.set_sigmaR_min(0.001);  var.set_sigmaR_max(1);  var.set_sigmaR(0.05);
    var.set_alfa_min(0)      ;  var.set_alfa_max(1)    ;  var.set_alfa(0)     ;
    var.set_A_min(1)         ;  var.set_A_max(3000)    ;
    var.set_A(histo->Integral());

    //fit limits
    if(voltage==95) {var.set_fit_min(7); var.set_fit_max(11.1);}
    if(voltage==96) {var.set_fit_min(7); var.set_fit_max(11.6);}
    if(voltage==97) {var.set_fit_min(7); var.set_fit_max(11.3);}
    if(voltage==98) {var.set_fit_min(8); var.set_fit_max(12);} 
    if(voltage==99) {var.set_fit_min(7); var.set_fit_max(12);}
    if(voltage==100){var.set_fit_min(7); var.set_fit_max(11.4);}
    if(voltage==101) {var.set_fit_min(7); var.set_fit_max(12.2);}
    if(voltage==102) {var.set_fit_min(7); var.set_fit_max(13);}
    if(voltage==103){var.set_fit_min(7); var.set_fit_max(13.2);}
    if(voltage==104){var.set_fit_min(8); var.set_fit_max(14.5);}
    if(voltage==105){var.set_fit_min(11); var.set_fit_max(18);}
  }


  if(CD_number == 204 && strcmp(choice,"amp")==0 && strcmp(misura,"B60_preamp_post_cond1")==0){
    var.set_mu_min(0.6)      ;  var.set_mu_max(0.7)    ;  var.set_mu(0.65)    ;
    var.set_sigmaL_min(1E-3) ;  var.set_sigmaL_max(0.5);  var.set_sigmaL(6E-3);
    var.set_sigmaR_min(1E-4) ;  var.set_sigmaR_max(0.1);  var.set_sigmaR(2E-3);
    var.set_alfa_min(0)      ;  var.set_alfa_max(1)    ;  var.set_alfa(0)     ;
    var.set_A_min(0)         ;  var.set_A_max(3000)    ;
    var.set_A(histo->Integral());

    //fit limits
    if(voltage==97) {var.set_fit_min(0.645); var.set_fit_max(0.665);}
    if(voltage==99) {var.set_fit_min(0.6515); var.set_fit_max(0.67);}
    if(voltage==101){var.set_fit_min(0.647); var.set_fit_max(0.67);}
    if(voltage==103){var.set_fit_min(0.648); var.set_fit_max(0.67);}
    if(voltage==106){var.set_fit_min(0.648); var.set_fit_max(0.67);}
  }

  if(CD_number == 204 && strcmp(choice,"charge")==0 && strcmp(misura,"B60_preamp_post_cond1")==0){
    var.set_mu_min(8)     ;  var.set_mu_max(10)  ;  var.set_mu(8.7)     ;
    var.set_sigmaL_min(0.01) ;  var.set_sigmaL_max(0.5);  var.set_sigmaL(0.1) ;
    var.set_sigmaR_min(0.01);  var.set_sigmaR_max(0.5);  var.set_sigmaR(0.1);
    var.set_alfa_min(0)      ;  var.set_alfa_max(1)    ;  var.set_alfa(0)     ;
    var.set_A_min(0)         ;  var.set_A_max(3000)    ;
    var.set_A(histo->Integral());

    //fit limits
    if(voltage==106){var.set_fit_min(9.1);  var.set_fit_max(11);}
    if(voltage==103){var.set_fit_min(9.0); var.set_fit_max(9.6);}
    if(voltage==101){var.set_fit_min(8.86); var.set_fit_max(9.5);}
    if(voltage==99) {var.set_fit_min(8.79); var.set_fit_max(9.3);}
    if(voltage==97) {var.set_fit_min(8.47); var.set_fit_max(9.2);}
   
  }


  if(CD_number == 204 && strcmp(choice,"amp")==0 && strcmp(misura,"B60_post_cond3_moku")==0){
    var.set_mu_min(0.61)      ;  var.set_mu_max(0.7)    ;  var.set_mu(0.61)     ;
    var.set_sigmaL_min(1E-3) ;  var.set_sigmaL_max(1E-2);  var.set_sigmaL(6E-3);
    var.set_sigmaR_min(5E-4) ;  var.set_sigmaR_max(0.1);  var.set_sigmaR(2E-3);
    var.set_alfa_min(0)      ;  var.set_alfa_max(1)    ;  var.set_alfa(0)     ;
    var.set_A_min(0)         ;  var.set_A_max(3000)    ;
    var.set_A(histo->Integral());

    //fit limits
    if(voltage==96 ){var.set_fit_min(0.6045); var.set_fit_max(0.625);}
    if(voltage==97 ){var.set_fit_min(0.606); var.set_fit_max(0.625);}
    if(voltage==98 ){var.set_fit_min(0.609); var.set_fit_max(0.63);}
    if(voltage==99 ){var.set_fit_min(0.610); var.set_fit_max(0.63);}
    if(voltage==100){var.set_fit_min(0.612); var.set_fit_max(0.63);}
    if(voltage==101){var.set_fit_min(0.6155); var.set_fit_max(0.63);}
    if(voltage==102){var.set_fit_min(0.615); var.set_fit_max(0.63);}
    if(voltage==103){var.set_fit_min(0.617); var.set_fit_max(0.635);}
    if(voltage==104){var.set_fit_min(0.6185); var.set_fit_max(0.635);}
    if(voltage==105){var.set_fit_min(0.615); var.set_fit_max(0.635);}
    if(voltage==106){var.set_fit_min(0.617); var.set_fit_max(0.635);}
    if(voltage==107){var.set_fit_min(0.6195); var.set_fit_max(0.64);}
    if(voltage==108){var.set_fit_min(0.6195); var.set_fit_max(0.635);}
    if(voltage==109){var.set_fit_min(0.6155); var.set_fit_max(0.64);}
    if(voltage==110){var.set_fit_min(0.610); var.set_fit_max(0.64);}

  }


  if(CD_number == 204 && strcmp(choice,"charge")==0 && strcmp(misura,"B60_post_cond3_moku")==0){
    var.set_mu_min(8)     ;  var.set_mu_max(10)  ;  var.set_mu(8.5)     ;
    var.set_sigmaL_min(0.1) ;  var.set_sigmaL_max(0.5);  var.set_sigmaL(0.1) ;
    var.set_sigmaR_min(0.01);  var.set_sigmaR_max(0.5);  var.set_sigmaR(0.1);
    var.set_alfa_min(0)      ;  var.set_alfa_max(1)    ;  var.set_alfa(0)     ;
    var.set_A_min(0)         ;  var.set_A_max(3000)    ;
    var.set_A(histo->Integral());

    //fit limits
    //if(voltage==95){var.set_fit_min(7.9);}
    if(voltage==96) {var.set_fit_min(8.02); var.set_fit_max(8.7);} 
    if(voltage==97) {var.set_fit_min(8.0); var.set_fit_max(8.8);}
    if(voltage==98) {var.set_fit_min(8.2); var.set_fit_max(8.8);} 
    if(voltage==99) {var.set_fit_min(8.3); var.set_fit_max(9);}
    if(voltage==100){var.set_fit_min(8.27); var.set_fit_max(9.);} 
    if(voltage==101){var.set_fit_min(8.37); var.set_fit_max(9);} 
    if(voltage==102){var.set_fit_min(8.45); var.set_fit_max(9.1);} 
    if(voltage==103){var.set_fit_min(8.5); var.set_fit_max(9.3);}
    if(voltage==104){var.set_fit_min(8.6); var.set_fit_max(9.5);} //
    if(voltage==105){var.set_fit_min(8.65); var.set_fit_max(9.5);}
    if(voltage==106){var.set_fit_min(8.7); var.set_fit_max(9.5);}
    if(voltage==107){var.set_fit_min(8.75); var.set_fit_max(9.4);}
    if(voltage==108){var.set_fit_min(8.75); var.set_fit_max(9.3);}
    if(voltage==109){var.set_fit_min(8.85); var.set_fit_max(9.3);}
    if(voltage==110){var.set_fit_min(8.8); var.set_fit_max(9.3);}

    
    
  }
  
 
  return var;
}
