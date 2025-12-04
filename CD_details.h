//funzioni con dettagli per plottare grafici finali

//get voltages for each cooldown
std::vector<int> GetVoltages(int CD_number, char* misura){
  std::vector<int> volts;
 
  if(CD_number == 188) {
    for(int i=0; i<11; i++){ volts.push_back(95+i); }
    //for(int i=0; i<10; i++){ volts.push_back(96+i); }
  }


  if(CD_number == 204 && strcmp(misura,"B60_preamp_post_cond1")==0){
    volts.push_back(97); volts.push_back(99);
    volts.push_back(101); volts.push_back(103);
    volts.push_back(106);
  }

  if(CD_number == 204 && strcmp(misura,"B60_post_cond3_moku")==0){
    for(int i=0; i<8; i++){ volts.push_back(96+i); }
  }
  
  if(CD_number == 204 && strcmp(misura,"H60_post_cond3_moku")==0){
    volts.push_back(101); }

  if(CD_number == 204 && strcmp(misura,"H60_pre_amp")==0){
    volts.push_back(135); volts.push_back(138);  }
  
  if(CD_number == 204 && strcmp(misura,"B60_pre_amp")==0){
    volts.push_back(135); volts.push_back(138);  }


  if(CD_number == 222 && strcmp(misura,"F60_squidfilter_ER_031025_tr25")==0){
    for(int i=0; i<13; i++){ volts.push_back(118+i); }
  }
  
  if(CD_number == 222 && strcmp(misura,"F60_squidfilter_ER_031025_tr50")==0){
    for(int i=0; i<8; i++){ volts.push_back(123+i); }
  }

  if(CD_number == 222 && strcmp(misura,"E100_squidfilter_ER_031025_tr12")==0){
    for(int i=0; i<7; i++){ volts.push_back(122+i); }
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
  //std::cout << "Phi TES: " << phi_tes << " +/- " << h1_tesWF->GetMeanError() << " eV" << std::endl;
  return phi_tes;
}

//get y max of histo
float GetYMax(int CD_number, char* choice, char* misura){
  float ymax=0;

  if(CD_number == 188 && strcmp(choice,"amp"                    )==0){ ymax = 2.3;}
  if(CD_number == 188 && strcmp(choice,"amp_10kHz"              )==0){ ymax = 3.5;}
  if(CD_number == 188 && strcmp(choice,"amp_100kHz"             )==0){ ymax = 2.2;}
  if(CD_number == 188 && strcmp(choice,"amp_10kHz_100kHz"       )==0){ ymax = 3.5;}
  if(CD_number == 188 && strcmp(choice,"amp_10kHz_160kHz"       )==0){ ymax = 3.5;}
  if(CD_number == 188 && strcmp(choice,"amp_10kHz_100kHz_160kHz")==0){ ymax = 3.5;}
  if(CD_number == 188 && strcmp(choice,"charge"                 )==0){ ymax = 4.3;}
  
  if(CD_number == 204 && strcmp(choice,"amp"              )==0 && strcmp(misura,"B60_preamp_post_cond1")==0){ ymax = 1   ;}
  if(CD_number == 204 && strcmp(choice,"charge"           )==0 && strcmp(misura,"B60_preamp_post_cond1")==0){ ymax = 1.7 ;}
  if(CD_number == 204 && strcmp(choice,"amp"              )==0 && strcmp(misura,"B60_post_cond3_moku"  )==0){ ymax = 0.75;}
  if(CD_number == 204 && strcmp(choice,"amp_10kHz"        )==0 && strcmp(misura,"B60_post_cond3_moku"  )==0){ ymax = 1.75;}
   if(CD_number == 204 && strcmp(choice,"amp_10kHz_160kHz")==0 && strcmp(misura,"B60_post_cond3_moku"  )==0){ ymax = 1.75;}
  if(CD_number == 204 && strcmp(choice,"charge"           )==0 && strcmp(misura,"B60_post_cond3_moku"  )==0){ ymax = 2.4 ;}
  
  if(CD_number == 222 && strcmp(choice,"amp"             )==0 && strcmp(misura,"F60_squidfilter_ER_031025_tr25" )==0){ ymax = 1.4;}
  if(CD_number == 222 && strcmp(choice,"charge"          )==0 && strcmp(misura,"F60_squidfilter_ER_031025_tr25" )==0){ ymax = 11.0;}
  if(CD_number == 222 && strcmp(choice,"amp"             )==0 && strcmp(misura,"F60_squidfilter_ER_031025_tr50" )==0){ ymax = 1.4;}
  if(CD_number == 222 && strcmp(choice,"charge"          )==0 && strcmp(misura,"F60_squidfilter_ER_031025_tr50" )==0){ ymax = 11.0;}
  if(CD_number == 222 && strcmp(choice,"amp"             )==0 && strcmp(misura,"E100_squidfilter_ER_031025_tr12")==0){ ymax = 2.0;}
  if(CD_number == 222 && strcmp(choice,"amp_100kHz"      )==0 && strcmp(misura,"E100_squidfilter_ER_031025_tr12")==0){ ymax = 1.7;}
   if(CD_number == 222 && strcmp(choice,"amp_10kHz"      )==0 && strcmp(misura,"E100_squidfilter_ER_031025_tr12")==0){ ymax = 2.0;}
   if(CD_number == 222 && strcmp(choice,"amp_10kHz_10kHz")==0 && strcmp(misura,"E100_squidfilter_ER_031025_tr12")==0){ ymax = 2.0;}
  if(CD_number == 222 && strcmp(choice,"charge"          )==0 && strcmp(misura,"E100_squidfilter_ER_031025_tr12")==0){ ymax = 9.0;}

 
  return ymax;
}

//histo settings
Histo SetHistoValues(int CD_number, char* choice, char* misura, int voltage){
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

  if(CD_number == 188 && strcmp(choice,"amp_10kHz_100kHz")==0){
    var.set_nbins(101);
    if(voltage>99){ var.set_nbins(200);}
    var.set_range_min(0.);
    var.set_range_max(1.);
    var.set_hist_min(0.);
    var.set_hist_max(0.4);
    binsize = settings.Binsize((var.get_hist_max()-var.get_hist_min())/var.get_nbins());
    var.set_x_name("Amplitude [V]");
    var.set_y_name("Counts / " + to_str(binsize) + " V");
  }

   if(CD_number == 188 && strcmp(choice,"amp_10kHz_160kHz")==0){
    var.set_nbins(100);
    if(voltage>99){ var.set_nbins(200);}
    var.set_range_min(0.);
    var.set_range_max(1.);
    var.set_hist_min(0.);
    var.set_hist_max(0.4);
    binsize = settings.Binsize((var.get_hist_max()-var.get_hist_min())/var.get_nbins());
    var.set_x_name("Amplitude [V]");
    var.set_y_name("Counts / " + to_str(binsize) + " V");
  }

    if(CD_number == 188 && strcmp(choice,"amp_10kHz_100kHz_160kHz")==0){
    var.set_nbins(95);
    if(voltage>99){ var.set_nbins(201);}
    var.set_range_min(0.);
    var.set_range_max(1.);
    var.set_hist_min(0.);
    var.set_hist_max(0.4);
    binsize = settings.Binsize((var.get_hist_max()-var.get_hist_min())/var.get_nbins());
    var.set_x_name("Amplitude [V]");
    var.set_y_name("Counts / " + to_str(binsize) + " V");
  }

   if(CD_number == 188 && strcmp(choice,"amp_10kHz")==0){
    var.set_nbins(106);
    if(voltage>99){ var.set_nbins(200);}
    var.set_range_min(0.);
    var.set_range_max(1.);
    var.set_hist_min(0.);
    var.set_hist_max(0.4);
    binsize = settings.Binsize((var.get_hist_max()-var.get_hist_min())/var.get_nbins());
    var.set_x_name("Amplitude [V]");
    var.set_y_name("Counts / " + to_str(binsize) + " V");
  }

   if(CD_number == 188 && strcmp(choice,"amp_100kHz")==0){
    var.set_nbins(92);
    if(voltage>99){ var.set_nbins(202);}
    var.set_range_min(0.);
    var.set_range_max(1.);
    var.set_hist_min(0.);
    var.set_hist_max(0.8);
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
    var.set_nbins(700);
    var.set_hist_min(0.);
    var.set_hist_max(1.);
    var.set_range_min(0.55);
    var.set_range_max(0.67);
    binsize = settings.Binsize((var.get_hist_max()-var.get_hist_min())/var.get_nbins());
    var.set_x_name("Amplitude [V]");
    var.set_y_name("Counts / " + to_str(binsize) + " V");
  }

    if(CD_number == 204 && strcmp(choice,"amp_10kHz")==0 && strcmp(misura,"B60_post_cond3_moku")==0){
    var.set_nbins(900);
    var.set_hist_min(0.);
    var.set_hist_max(1.);
    var.set_range_min(0.15);
    var.set_range_max(0.25);
    binsize = settings.Binsize((var.get_hist_max()-var.get_hist_min())/var.get_nbins());
    var.set_x_name("Amplitude [V]");
    var.set_y_name("Counts / " + to_str(binsize) + " V");
  }

    if(CD_number == 204 && strcmp(choice,"amp_10kHz_160kHz")==0 && strcmp(misura,"B60_post_cond3_moku")==0){
      var.set_nbins(900);
    var.set_hist_min(0.);
    var.set_hist_max(1.);
    var.set_range_min(0.15);
    var.set_range_max(0.25);
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
    var.set_x_name("Charge [uC]");
    var.set_y_name("Counts / " + to_str(binsize) + " V");
  }

   
   if(CD_number == 222 && strcmp(choice,"amp")==0 && strcmp(misura,"E100_squidfilter_ER_031025_tr12")==0){
    var.set_nbins(1000);
    if(voltage==127){var.set_nbins(900);}
    
    var.set_hist_min(0.);
    var.set_hist_max(1.);
    var.set_range_min(0.05);
    var.set_range_max(0.13);
    binsize = settings.Binsize((var.get_hist_max()-var.get_hist_min())/var.get_nbins());
    var.set_x_name("Amplitude [V]");
    var.set_y_name("Counts / " + to_str(binsize) + " V");
  }

   if(CD_number == 222 && strcmp(choice,"amp_100kHz")==0 && strcmp(misura,"E100_squidfilter_ER_031025_tr12")==0){
    var.set_nbins(1200);
    //if(voltage==127){var.set_nbins(900);}
    
    var.set_hist_min(0.);
    var.set_hist_max(1.);
    var.set_range_min(0.05);
    var.set_range_max(0.13);
    binsize = settings.Binsize((var.get_hist_max()-var.get_hist_min())/var.get_nbins());
    var.set_x_name("Amplitude [V]");
    var.set_y_name("Counts / " + to_str(binsize) + " V");
  }

   if(CD_number == 222 && strcmp(choice,"amp_10kHz")==0 && strcmp(misura,"E100_squidfilter_ER_031025_tr12")==0){
    var.set_nbins(1500);
    //if(voltage==127){var.set_nbins(900);}
    
    var.set_hist_min(0.);
    var.set_hist_max(1.);
    var.set_range_min(0.05);
    var.set_range_max(0.13);
    binsize = settings.Binsize((var.get_hist_max()-var.get_hist_min())/var.get_nbins());
    var.set_x_name("Amplitude [V]");
    var.set_y_name("Counts / " + to_str(binsize) + " V");
  }

   if(CD_number == 222 && strcmp(choice,"amp_10kHz_10kHz")==0 && strcmp(misura,"E100_squidfilter_ER_031025_tr12")==0){
    var.set_nbins(1500);
    //if(voltage==127){var.set_nbins(900);}
    
    var.set_hist_min(0.);
    var.set_hist_max(1.);
    var.set_range_min(0.05);
    var.set_range_max(0.13);
    binsize = settings.Binsize((var.get_hist_max()-var.get_hist_min())/var.get_nbins());
    var.set_x_name("Amplitude [V]");
    var.set_y_name("Counts / " + to_str(binsize) + " V");
  }

   if(CD_number == 222 && strcmp(choice,"charge")==0 && strcmp(misura,"E100_squidfilter_ER_031025_tr12")==0){
    var.set_nbins(300);
    if(voltage==125                ){var.set_nbins(100);}
    if(voltage==127 || voltage==130){var.set_nbins(250);}
     
    var.set_hist_min(0.);
    var.set_hist_max(30);
    var.set_range_min(2);
    var.set_range_max(8);
    binsize = settings.Binsize((var.get_hist_max()-var.get_hist_min())/var.get_nbins());
    var.set_x_name("Charge [uC]");
    var.set_y_name("Counts / " + to_str(binsize) + " V");
   }
   
   if(CD_number == 222 && strcmp(choice,"amp")==0 && strcmp(misura,"F60_squidfilter_ER_031025_tr25")==0){
     if(voltage!=126 && voltage!=128 && voltage!=131 && voltage!=132){var.set_nbins(600);}
     if(voltage==126 || voltage==131 || voltage==132){var.set_nbins(500);}
     if(voltage==128){var.set_nbins(550);}
     
     var.set_hist_min(0.);
     var.set_hist_max(1.);
     var.set_range_min(0.2);
     var.set_range_max(0.3);
     binsize = settings.Binsize((var.get_hist_max()-var.get_hist_min())/var.get_nbins());
     var.set_x_name("Amplitude [V]");
     var.set_y_name("Counts / " + to_str(binsize) + " V");
   }

   
   if(CD_number == 222 && strcmp(choice,"charge")==0 && strcmp(misura,"F60_squidfilter_ER_031025_tr25")==0){
     if(voltage!=121 && voltage!=122 && voltage!=127){var.set_nbins(300);}
     if(voltage==121 || voltage==122){var.set_nbins(250);}
     if(voltage==127){var.set_nbins(400);}
     
     var.set_hist_min(0.);
     var.set_hist_max(30);
     var.set_range_min(10);
     var.set_range_max(16);
     binsize = settings.Binsize((var.get_hist_max()-var.get_hist_min())/var.get_nbins());
     var.set_x_name("Charge [uC]");
     var.set_y_name("Counts / " + to_str(binsize) + " V");
   }

   
   if(CD_number == 222 && strcmp(choice,"amp")==0 && strcmp(misura,"F60_squidfilter_ER_031025_tr50")==0){
     var.set_nbins(700);
     if(voltage==123 || voltage==124){var.set_nbins(900);}
     if(voltage==125                ){var.set_nbins(800);}
     if(voltage==126                ){var.set_nbins(850);}
     if(voltage==132                ){var.set_nbins(600);}
     
     var.set_hist_min(0.);
     var.set_hist_max(1.);
     var.set_range_min(0.2);
     var.set_range_max(0.3);
     binsize = settings.Binsize((var.get_hist_max()-var.get_hist_min())/var.get_nbins());
     var.set_x_name("Amplitude [V]");
     var.set_y_name("Counts / " + to_str(binsize) + " V");
   }
   
   if(CD_number == 222 && strcmp(choice,"charge")==0 && strcmp(misura,"F60_squidfilter_ER_031025_tr50")==0){
     var.set_nbins(350);
     if(voltage==127 || voltage==131 || voltage==132){var.set_nbins(250);}
     
     var.set_hist_min(0.);
     var.set_hist_max(30.);
     var.set_range_min(10);
     var.set_range_max(16);
     binsize = settings.Binsize((var.get_hist_max()-var.get_hist_min())/var.get_nbins());
     var.set_x_name("Charge [uC]");
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
    var.set_sigmaR_min(0.001);  var.set_sigmaR_max(0.1);  var.set_sigmaR(0.03);
    var.set_alfa_min(0)      ;  var.set_alfa_max(1)    ;  var.set_alfa(0)     ;
    var.set_A_min(1)         ;  var.set_A_max(3000)    ;
    var.set_A(histo->Integral());
    
    //fit limits
    if(voltage<100. && voltage!=97.){var.set_fit_min(0.5); }
    else                            {var.set_fit_min(0.6); }
    var.set_fit_max(0.96);
  }

  if(CD_number == 188 && ((strcmp(choice,"amp_10kHz")==0
			  || strcmp(choice,"amp_10kHz_100kHz")==0
			  || strcmp(choice,"amp_10kHz_160kHz")==0
			  || strcmp(choice,"amp_10kHz_100kHz_160kHz")==0))){
    var.set_mu_min(0.2)     ;  var.set_mu_max(0.32)  ;  var.set_mu(0.25)     ;
    var.set_sigmaL_min(0.01) ;  var.set_sigmaL_max(0.5);  var.set_sigmaL(0.1) ;
    var.set_sigmaR_min(0.001);  var.set_sigmaR_max(0.1);  var.set_sigmaR(0.03);
    var.set_alfa_min(0)      ;  var.set_alfa_max(1)    ;  var.set_alfa(0)     ;
    var.set_A_min(1)         ;  var.set_A_max(3000)    ;
    var.set_A(histo->Integral());
    
    //fit limits
    if(voltage==97 || voltage==96){ var.set_mu_max(0.26);  }
    var.set_fit_min(0.12);
    if(voltage>99){var.set_fit_min(0.15);}
    var.set_fit_max(0.33);
  }

  if(CD_number == 188 && strcmp(choice,"amp_100kHz")==0){
    var.set_mu_min(0.6)     ;  var.set_mu_max(0.75)  ;  var.set_mu(0.64)     ;
    var.set_sigmaL_min(0.01) ;  var.set_sigmaL_max(0.5);  var.set_sigmaL(0.1) ;
    var.set_sigmaR_min(0.005);  var.set_sigmaR_max(0.1);  var.set_sigmaR(0.03);
    var.set_alfa_min(0)      ;  var.set_alfa_max(1)    ;  var.set_alfa(0)     ;
    var.set_A_min(1)         ;  var.set_A_max(3000)    ;
    var.set_A(histo->Integral());
    
    //fit limits
    var.set_fit_min(0.4);
    var.set_fit_max(0.8);
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
    var.set_mu_min(0.61)      ;  var.set_mu_max(0.7)    ;  var.set_mu(0.61)    ;
    var.set_sigmaL_min(1E-3) ;  var.set_sigmaL_max(1E-2);  var.set_sigmaL(6E-3); 
    var.set_sigmaR_min(5E-4) ;  var.set_sigmaR_max(0.1);  var.set_sigmaR(2E-3);
    var.set_alfa_min(0)      ;  var.set_alfa_max(1)    ;  var.set_alfa(0)     ;
    var.set_A_min(0)         ;  var.set_A_max(3000)    ;
    var.set_A(histo->Integral());

    //if(voltage>100){var.set_mu_min(0.618); var.set_sigmaL_max(5E-2); var.set_sigmaL_min(4E-3); }
    //fit limits
    if(voltage==96 ){var.set_fit_min(0.604); var.set_fit_max(0.625);}
    if(voltage==97 ){var.set_fit_min(0.603); var.set_fit_max(0.625);}
    if(voltage==98 ){var.set_fit_min(0.6095); var.set_fit_max(0.63);}
    if(voltage==99 ){var.set_fit_min(0.609); var.set_fit_max(0.63);}
    if(voltage==100){var.set_fit_min(0.6125); var.set_fit_max(0.63);}
    if(voltage==101){var.set_fit_min(0.6165); var.set_fit_max(0.63);}
    if(voltage==102){var.set_fit_min(0.6145); var.set_fit_max(0.63);}
    if(voltage==103){var.set_fit_min(0.617); var.set_fit_max(0.635);}
    if(voltage==104){var.set_fit_min(0.616); var.set_fit_max(0.635);}
    if(voltage==105){var.set_fit_min(0.614); var.set_fit_max(0.635);}
    if(voltage==106){var.set_fit_min(0.6179); var.set_fit_max(0.635);}
    if(voltage==107){var.set_fit_min(0.619); var.set_fit_max(0.635);}
    if(voltage==108){var.set_fit_min(0.618); var.set_fit_max(0.635);}
    if(voltage==109){var.set_fit_min(0.6145); var.set_fit_max(0.64);}
    if(voltage==110){var.set_fit_min(0.611); var.set_fit_max(0.64);}

  }

   if(CD_number == 204 && strcmp(choice,"amp_10kHz")==0 && strcmp(misura,"B60_post_cond3_moku")==0){
    var.set_mu_min(0.21)     ;  var.set_mu_max(0.23)    ;  var.set_mu(0.22)    ;
    var.set_sigmaL_min(1E-3) ;  var.set_sigmaL_max(1E-2);  var.set_sigmaL(6E-3); 
    var.set_sigmaR_min(5E-4) ;  var.set_sigmaR_max(0.1);  var.set_sigmaR(2E-3) ;
    var.set_alfa_min(0)      ;  var.set_alfa_max(1)    ;  var.set_alfa(0)      ;
    var.set_A_min(0)         ;  var.set_A_max(3000)    ;
    var.set_A(histo->Integral());

    //if(voltage>100){var.set_mu_min(0.618); var.set_sigmaL_max(5E-2); var.set_sigmaL_min(4E-3); }
    //fit limits
    if(voltage==96 ){var.set_fit_min(0.21); var.set_fit_max(0.225);}
    if(voltage==97 ){var.set_fit_min(0.21); var.set_fit_max(0.225);}
    if(voltage==98 ){var.set_fit_min(0.214); var.set_fit_max(0.23);}
    if(voltage==99 ){var.set_fit_min(0.214); var.set_fit_max(0.23);}
    if(voltage==100){var.set_fit_min(0.216); var.set_fit_max(0.23);}
    if(voltage==101){var.set_fit_min(0.218); var.set_fit_max(0.235);}
    if(voltage==102){var.set_fit_min(0.218); var.set_fit_max(0.235);}
    if(voltage==103){var.set_fit_min(0.22); var.set_fit_max(0.24);}
  }


   if(CD_number == 204 && strcmp(choice,"amp_10kHz_160kHz")==0 && strcmp(misura,"B60_post_cond3_moku")==0){
    var.set_mu_min(0.21)     ;  var.set_mu_max(0.23)    ;  var.set_mu(0.22)    ;
    var.set_sigmaL_min(1E-3) ;  var.set_sigmaL_max(1E-2);  var.set_sigmaL(6E-3); 
    var.set_sigmaR_min(5E-4) ;  var.set_sigmaR_max(0.1);  var.set_sigmaR(2E-3) ;
    var.set_alfa_min(0)      ;  var.set_alfa_max(1)    ;  var.set_alfa(0)      ;
    var.set_A_min(0)         ;  var.set_A_max(3000)    ;
    var.set_A(histo->Integral());

    //if(voltage>100){var.set_mu_min(0.618); var.set_sigmaL_max(5E-2); var.set_sigmaL_min(4E-3); }
    //fit limits
    if(voltage==96 ){var.set_fit_min(0.21); var.set_fit_max(0.225);}
    if(voltage==97 ){var.set_fit_min(0.21); var.set_fit_max(0.225);}
    if(voltage==98 ){var.set_fit_min(0.212); var.set_fit_max(0.23);}
    if(voltage==99 ){var.set_fit_min(0.215); var.set_fit_max(0.23);}
    if(voltage==100){var.set_fit_min(0.216); var.set_fit_max(0.23);}
    if(voltage==101){var.set_fit_min(0.216); var.set_fit_max(0.235);}
    if(voltage==102){var.set_fit_min(0.218); var.set_fit_max(0.235);}
    if(voltage==103){var.set_fit_min(0.218); var.set_fit_max(0.24);}
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
    if(voltage==104){var.set_fit_min(8.6); var.set_fit_max(9.5);} 
    if(voltage==105){var.set_fit_min(8.65); var.set_fit_max(9.5);}
    if(voltage==106){var.set_fit_min(8.7); var.set_fit_max(9.5);}
    if(voltage==107){var.set_fit_min(8.75); var.set_fit_max(9.4);}
    if(voltage==108){var.set_fit_min(8.75); var.set_fit_max(9.3);}
    if(voltage==109){var.set_fit_min(8.85); var.set_fit_max(9.3);}
    if(voltage==110){var.set_fit_min(8.8); var.set_fit_max(9.3);}
  }

  
  
  if(CD_number == 222 && strcmp(choice,"amp")==0 && strcmp(misura,"E100_squidfilter_ER_031025_tr12")==0){
    var.set_mu_min(0.108);
    if(voltage>127){var.set_mu_min(0.115);}

    var.set_mu(0.11);
    if(voltage>126){var.set_mu(0.115);}
    
    var.set_mu_max(0.115);
    if(voltage>126){var.set_mu_max(0.12);}
    
    var.set_sigmaL_min(0.001);  var.set_sigmaL_max(0.5);  var.set_sigmaL(0.01) ;
    var.set_sigmaR_min(0.001);  var.set_sigmaR_max(0.5);  var.set_sigmaR(0.02);
    var.set_alfa_min(0)      ;  var.set_alfa_max(1)    ;  var.set_alfa(0)     ;
    var.set_A_min(50)        ;  var.set_A_max(10000)   ;
    var.set_A(histo->Integral());

    //fit limits
    if(voltage==122){var.set_fit_min(0.1103); var.set_fit_max(0.117);} 
    if(voltage==123){var.set_fit_min(0.111 ); var.set_fit_max(0.118);}
    if(voltage==124){var.set_fit_min(0.112) ; var.set_fit_max(0.12) ;} 
    if(voltage==125){var.set_fit_min(0.112) ; var.set_fit_max(0.12) ;}
    if(voltage==126){var.set_fit_min(0.112) ; var.set_fit_max(0.122);} 
    if(voltage==127){var.set_fit_min(0.112) ; var.set_fit_max(0.122);} 
    if(voltage==128){var.set_fit_min(0.1132) ; var.set_fit_max(0.122);} 
    if(voltage==129){var.set_fit_min(0.1125); var.set_fit_max(0.122);}
    if(voltage==130){var.set_fit_min(0.112 ); var.set_fit_max(0.122);} 
    
  }

   if(CD_number == 222 && strcmp(choice,"amp_100kHz")==0 && strcmp(misura,"E100_squidfilter_ER_031025_tr12")==0){
    var.set_mu_min(0.108);
    if(voltage>127){var.set_mu_min(0.115);}

    var.set_mu(0.11);
    if(voltage>126){var.set_mu(0.115);}
    
    var.set_mu_max(0.115);
    if(voltage>126){var.set_mu_max(0.12);}
    
    var.set_sigmaL_min(0.001);  var.set_sigmaL_max(0.5);  var.set_sigmaL(0.01) ;
    var.set_sigmaR_min(0.001);  var.set_sigmaR_max(0.5);  var.set_sigmaR(0.02);
    var.set_alfa_min(0)      ;  var.set_alfa_max(1)    ;  var.set_alfa(0)     ;
    var.set_A_min(50)        ;  var.set_A_max(10000)   ;
    var.set_A(histo->Integral());

    //fit limits
    if(voltage==122){var.set_fit_min(0.109); var.set_fit_max(0.12);} 
    if(voltage==123){var.set_fit_min(0.1102 ); var.set_fit_max(0.118);}
    if(voltage==124){var.set_fit_min(0.110) ; var.set_fit_max(0.12) ;} 
    if(voltage==125){var.set_fit_min(0.111) ; var.set_fit_max(0.12) ;}
    if(voltage==126){var.set_fit_min(0.1115) ; var.set_fit_max(0.122);} 
    if(voltage==127){var.set_fit_min(0.112) ; var.set_fit_max(0.122);} 
    if(voltage==128){var.set_fit_min(0.111) ; var.set_fit_max(0.122);} 
    if(voltage==129){var.set_fit_min(0.1125); var.set_fit_max(0.122);}
    if(voltage==130){var.set_fit_min(0.111 ); var.set_fit_max(0.122);} 
    
  }

    if(CD_number == 222 && strcmp(choice,"amp_10kHz")==0 && strcmp(misura,"E100_squidfilter_ER_031025_tr12")==0){
    var.set_mu_min(0.087);
    var.set_mu(0.09);
    var.set_mu_max(0.95);
     
    var.set_sigmaL_min(0.0009);  var.set_sigmaL_max(0.5);  var.set_sigmaL(0.01) ;
    var.set_sigmaR_min(0.0009);  var.set_sigmaR_max(0.5);  var.set_sigmaR(0.02);
    var.set_alfa_min(0)      ;  var.set_alfa_max(1)    ;  var.set_alfa(0)     ;
    var.set_A_min(50)        ;  var.set_A_max(10000)   ;
    var.set_A(histo->Integral());

    //fit limits
    if(voltage==122){var.set_fit_min(0.0854); var.set_fit_max(0.1);} 
    if(voltage==123){var.set_fit_min(0.087); var.set_fit_max(0.1);}
    if(voltage==124){var.set_fit_min(0.087) ; var.set_fit_max(0.1) ;} 
    if(voltage==125){var.set_fit_min(0.087) ; var.set_fit_max(0.12) ;}
    if(voltage==126){var.set_fit_min(0.088) ; var.set_fit_max(0.122);} 
    if(voltage==127){var.set_fit_min(0.0895) ; var.set_fit_max(0.122);} 
    if(voltage==128){var.set_fit_min(0.089) ; var.set_fit_max(0.122);} 
    if(voltage==129){var.set_fit_min(0.1125); var.set_fit_max(0.122);}
    if(voltage==130){var.set_fit_min(0.112 ); var.set_fit_max(0.122);} 
    
  }

    
    if(CD_number == 222 && strcmp(choice,"amp_10kHz_10kHz")==0 && strcmp(misura,"E100_squidfilter_ER_031025_tr12")==0){
    var.set_mu_min(0.075);
    var.set_mu(0.076);
    var.set_mu_max(0.08);
    if(voltage>124){var.set_mu_max(0.085);}
    
    var.set_sigmaL_min(0.001);  var.set_sigmaL_max(0.5);  var.set_sigmaL(0.01) ;
    var.set_sigmaR_min(0.001);  var.set_sigmaR_max(0.5);  var.set_sigmaR(0.02);
    var.set_alfa_min(0)      ;  var.set_alfa_max(1)    ;  var.set_alfa(0)     ;
    var.set_A_min(50)        ;  var.set_A_max(10000)   ;
    var.set_A(histo->Integral());

    //fit limits
    if(voltage==122){var.set_fit_min(0.073); var.set_fit_max(0.085);} 
    if(voltage==123){var.set_fit_min(0.0743); var.set_fit_max(0.085);}
    if(voltage==124){var.set_fit_min(0.074) ; var.set_fit_max(0.085) ;} 
    if(voltage==125){var.set_fit_min(0.074) ; var.set_fit_max(0.085) ;}
    if(voltage==126){var.set_fit_min(0.0749) ; var.set_fit_max(0.085) ;}
    if(voltage==127){var.set_fit_min(0.0749) ; var.set_fit_max(0.085) ;}
    if(voltage==128){var.set_fit_min(0.0755) ; var.set_fit_max(0.085) ;}
    
  }

   
   

  if(CD_number == 222 && strcmp(choice,"charge")==0 && strcmp(misura,"E100_squidfilter_ER_031025_tr12")==0){
    var.set_mu_min(5.5)     ;  var.set_mu_max(6.5)    ;  var.set_mu(6)    ;
    var.set_sigmaL_min(0.01);  var.set_sigmaL_max(20 );  var.set_sigmaL(1);
    var.set_sigmaR_min(0.01);  var.set_sigmaR_max(20 );  var.set_sigmaR(1);
    var.set_alfa_min(0)     ;  var.set_alfa_max(1)    ;  var.set_alfa(0)  ;
    var.set_A_min(1)        ;  var.set_A_max(5000)    ;
    var.set_A(histo->Integral());

    //fit limits
    if(voltage==122){var.set_fit_min(5.3); var.set_fit_max(6.7);} 
    if(voltage==123){var.set_fit_min(5.3); var.set_fit_max(6.7);}
    if(voltage==124){var.set_fit_min(5.3); var.set_fit_max(7)  ;} 
    if(voltage==125){var.set_fit_min(4.8); var.set_fit_max(7)  ;}
    if(voltage==126){var.set_fit_min(5.1); var.set_fit_max(7)  ;} 
    if(voltage==127){var.set_fit_min(5.4); var.set_fit_max(7)  ;} 
    if(voltage==128){var.set_fit_min(5.4); var.set_fit_max(7)  ;} 
    if(voltage==129){var.set_fit_min(5.4); var.set_fit_max(7)  ;}
    if(voltage==130){var.set_fit_min(5.1); var.set_fit_max(6.8);} 
    
  }

  
  
  if(CD_number == 222 && strcmp(choice,"amp")==0 && strcmp(misura,"F60_squidfilter_ER_031025_tr25")==0){
    if(voltage==118                ){var.set_mu_min(0.24) ;}
    if(voltage==119 || voltage==120){var.set_mu_min(0.245);}
    if(voltage==121                ){var.set_mu_min(0.255);}
    if(voltage==122                ){var.set_mu_min(0.26 );}
    if(voltage >122 && voltage <126){var.set_mu_min(0.263);}
    if(voltage==126                ){var.set_mu_min(0.268);}
    if(voltage==127                ){var.set_mu_min(0.269);}
    if(voltage >127 && voltage <131){var.set_mu_min(0.271);}
    if(voltage==131                ){var.set_mu_min(0.270);}
    if(voltage==132                ){var.set_mu_min(0.265);}

    if(voltage==118 || voltage==119){var.set_mu_max(0.26 );}
    if(voltage==120                ){var.set_mu_max(0.265);}
    if(voltage >120 && voltage <126){var.set_mu_max(0.27 );}
    if(voltage >125 && voltage <133){var.set_mu_max(0.28 );}

    if(voltage >117 && voltage <121){var.set_mu(0.25 );}
    if(voltage==121                ){var.set_mu(0.26 );}
    if(voltage >121 && voltage <126){var.set_mu(0.265);}
    if(voltage >125 && voltage <131){var.set_mu(0.272);}
    if(voltage >130 && voltage <133){var.set_mu(0.271);}
     
    //var.set_mu_min(0.24)     ;  var.set_mu_max(0.26)   ;  var.set_mu(0.25)    ;
    var.set_sigmaL_min(0.001);  var.set_sigmaL_max(0.5);  var.set_sigmaL(0.01);
    var.set_sigmaR_min(0.001);  var.set_sigmaR_max(0.5);  var.set_sigmaR(0.02);
    var.set_alfa_min(0)      ;  var.set_alfa_max(1)    ;  var.set_alfa(0)     ;
    var.set_A_min(50)        ;  var.set_A_max(10000)    ;
    var.set_A(histo->Integral());

    //fit limits
    if(voltage==118){var.set_fit_min(0.244 ); var.set_fit_max(0.255 );} 
    if(voltage==119){var.set_fit_min(0.2555); var.set_fit_max(0.265 );}
    if(voltage==120){var.set_fit_min(0.2545); var.set_fit_max(0.265 );} 
    if(voltage==121){var.set_fit_min(0.2555); var.set_fit_max(0.27  );}
    if(voltage==122){var.set_fit_min(0.259 ); var.set_fit_max(0.273 );} 
    if(voltage==123){var.set_fit_min(0.262 ); var.set_fit_max(0.2695);}
    if(voltage==124){var.set_fit_min(0.262 ); var.set_fit_max(0.275 );} 
    if(voltage==125){var.set_fit_min(0.262 ); var.set_fit_max(0.271 );}
    if(voltage==126){var.set_fit_min(0.265 ); var.set_fit_max(0.28  );} 
    if(voltage==127){var.set_fit_min(0.2685); var.set_fit_max(0.275 );} 
    if(voltage==128){var.set_fit_min(0.268 ); var.set_fit_max(0.280 );} 
    if(voltage==129){var.set_fit_min(0.269 ); var.set_fit_max(0.28  );}
    if(voltage==130){var.set_fit_min(0.2685); var.set_fit_max(0.28  );}
    if(voltage==131){var.set_fit_min(0.267 ); var.set_fit_max(0.277 );}
    if(voltage==132){var.set_fit_min(0.2645); var.set_fit_max(0.277 );} 
    
  }

  
  if(CD_number == 222 && strcmp(choice,"charge")==0 && strcmp(misura,"F60_squidfilter_ER_031025_tr25")==0){
    if(voltage==118)                {var.set_mu_min(12.3);}
    if(voltage==119 || voltage==120){var.set_mu_min(12.6);}
    if(voltage==121)                {var.set_mu_min(13  );}
    if(voltage==122)                {var.set_mu_min(13.3);}
    if(voltage >122 && voltage <126){var.set_mu_min(13.4);}
    if(voltage==126)                {var.set_mu_min(13.9);}
    if(voltage >126 && voltage <133){var.set_mu_min(13.6);}
    if(voltage==129)                {var.set_mu_min(13.8);}
 
    if(voltage==118)                {var.set_mu_max(13  );}
    if(voltage >118 && voltage <126){var.set_mu_max(14  );}
    if(voltage==126)                {var.set_mu_max(14.5);}
    if(voltage >126 && voltage <133){var.set_mu_max(14.4);}
    if(voltage==129)                {var.set_mu_max(14.3);}

    if(voltage==118)                {var.set_mu(12.6);}
    if(voltage >118 && voltage <121){var.set_mu(13  );}
    if(voltage >120 && voltage <126){var.set_mu(13.5);}
    if(voltage==126)                {var.set_mu(14.1);}
    if(voltage >126 && voltage <133){var.set_mu(14.0);}
    
    var.set_sigmaL_min(0.01) ;  var.set_sigmaL_max(20);  var.set_sigmaL(1) ;
    var.set_sigmaR_min(0.01);  var.set_sigmaR_max(20);  var.set_sigmaR(1);
    var.set_alfa_min(0)     ;  var.set_alfa_max(1)    ;  var.set_alfa(0)     ;
    var.set_A_min(0)        ;  var.set_A_max(5000)    ;
    var.set_A(histo->Integral());

    //fit limits
    if(voltage==118){var.set_fit_min(12.2 ); var.set_fit_max(13.3 );} 
    if(voltage==119){var.set_fit_min(12.7 ); var.set_fit_max(14   );}
    if(voltage==120){var.set_fit_min(12.95); var.set_fit_max(14   );} 
    if(voltage==121){var.set_fit_min(12.9 ); var.set_fit_max(14.4 );}
    if(voltage==122){var.set_fit_min(13.1 ); var.set_fit_max(14.4 );} 
    if(voltage==123){var.set_fit_min(13   ); var.set_fit_max(15   );}
    if(voltage==124){var.set_fit_min(13.25); var.set_fit_max(14.5 );} 
    if(voltage==125){var.set_fit_min(13.1 ); var.set_fit_max(14.4 );}
    if(voltage==126){var.set_fit_min(13.5 ); var.set_fit_max(14.6 );} 
    if(voltage==127){var.set_fit_min(13.4 ); var.set_fit_max(15   );} 
    if(voltage==128){var.set_fit_min(13.54); var.set_fit_max(15.2 );} 
    if(voltage==129){var.set_fit_min(13.4 ); var.set_fit_max(15.6 );}
    if(voltage==130){var.set_fit_min(13.2 ); var.set_fit_max(16   );}
    if(voltage==131){var.set_fit_min(13.2 ); var.set_fit_max(15   );}
    if(voltage==132){var.set_fit_min(13.4 ); var.set_fit_max(15   );} 
    
  }

  
    if(CD_number == 222 && strcmp(choice,"amp")==0 && strcmp(misura,"F60_squidfilter_ER_031025_tr50")==0){
      if(voltage >122 && voltage<126){var.set_mu_min(0.264);}
      if(voltage==126               ){var.set_mu_min(0.268);}
      if(voltage==127               ){var.set_mu_min(0.267);}
      if(voltage >127 && voltage<131){var.set_mu_min(0.269);}
      if(voltage==131               ){var.set_mu_min(0.268);}
      if(voltage==132               ){var.set_mu_min(0.265);}

      if(voltage >122 && voltage <126){var.set_mu_max(0.27 );}
      if(voltage==126                ){var.set_mu_max(0.271);}
      if(voltage >126 && voltage <131){var.set_mu_max(0.275);}
      if(voltage==131 || voltage==132){var.set_mu_max(0.27 );}

      if(voltage >122 && voltage <126){var.set_mu(0.264);}
      if(voltage >125 && voltage <131){var.set_mu(0.27 );}
      if(voltage==131                ){var.set_mu(0.269);}
      if(voltage==132                ){var.set_mu(0.268);}
      
      var.set_sigmaL_min(0.001);  var.set_sigmaL_max(0.5);  var.set_sigmaL(0.02);
      var.set_sigmaR_min(0.001);  var.set_sigmaR_max(0.5);  var.set_sigmaR(0.01);
      var.set_alfa_min(0)      ;  var.set_alfa_max(1)    ;  var.set_alfa(0)     ;
      var.set_A_min(50)        ;  var.set_A_max(10000)   ;
      var.set_A(histo->Integral());
      
      //fit limits
      if(voltage==123){var.set_fit_min(0.2625); var.set_fit_max(0.27 );}
      if(voltage==124){var.set_fit_min(0.2635); var.set_fit_max(0.272);} 
      if(voltage==125){var.set_fit_min(0.264 ); var.set_fit_max(0.274);}
      if(voltage==126){var.set_fit_min(0.267 ); var.set_fit_max(0.276);} 
      if(voltage==127){var.set_fit_min(0.2663); var.set_fit_max(0.28 );} 
      if(voltage==128){var.set_fit_min(0.2675); var.set_fit_max(0.275);} 
      if(voltage==129){var.set_fit_min(0.268 ); var.set_fit_max(0.276);}
      if(voltage==130){var.set_fit_min(0.2675); var.set_fit_max(0.276);}
      if(voltage==131){var.set_fit_min(0.2655); var.set_fit_max(0.273);}
      if(voltage==132){var.set_fit_min(0.264 ); var.set_fit_max(0.272);} 
      
    }
    
  if(CD_number == 222 && strcmp(choice,"charge")==0 && strcmp(misura,"F60_squidfilter_ER_031025_tr50")==0){
    
    var.set_mu_min(13);
    var.set_mu_max(14);
    var.set_mu(13.3);
    if(voltage==132){var.set_mu_min(13.2);}
    if(voltage >126){var.set_mu(13.5)    ;}
    
    var.set_sigmaL_min(0.01);  var.set_sigmaL_max(20);  var.set_sigmaL(1);
    var.set_sigmaR_min(0.10);  var.set_sigmaR_max(20);  var.set_sigmaR(1);
    var.set_alfa_min(0)     ;  var.set_alfa_max(1)   ;  var.set_alfa(0)  ;
    var.set_A_min(0)        ;  var.set_A_max(3000)   ;
    var.set_A(histo->Integral());

    //fit limits
    if(voltage==123){var.set_fit_min(12.5); var.set_fit_max(14.4);}
    if(voltage==124){var.set_fit_min(12.7); var.set_fit_max(14.4);} 
    if(voltage==125){var.set_fit_min(12.7); var.set_fit_max(14.4);}
    if(voltage==126){var.set_fit_min(13.2); var.set_fit_max(14.3);} 
    if(voltage==127){var.set_fit_min(13  ); var.set_fit_max(14.4);} 
    if(voltage==128){var.set_fit_min(13.2); var.set_fit_max(14.4);} 
    if(voltage==129){var.set_fit_min(13.2); var.set_fit_max(14.6);}
    if(voltage==130){var.set_fit_min(12.8); var.set_fit_max(14.6);}
    if(voltage==131){var.set_fit_min(13  ); var.set_fit_max(14.5);}
    if(voltage==132){var.set_fit_min(12.2); var.set_fit_max(15  );} 
    
  }
  
  return var;
}
