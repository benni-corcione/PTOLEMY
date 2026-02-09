#include <iostream>
#include <fstream>
#include "TFile.h"
#include "TTree.h"
#include "TH2D.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLine.h"
#include "TPaveText.h"
#include "TLatex.h"
#include "TArrow.h"
#include "TF1.h"
#include "TLegend.h"
#include "TFitResult.h"


#include "graphics.h"
#include "AndCommon.h"
#include "CD_details.h"


void Input_Param_Ctrls(int argc, char* argv[]);


Double_t Crystal_Ball(Double_t *x, Double_t *par) {
  // par[0] = mean (mu)
  // par[1] = sigma
  // par[2] = alpha (>0)
  // par[3] = n (>0)
  // par[4] = norm (A)

  Double_t t = (x[0] - par[0]) / par[1];
  Double_t alpha = par[2];
  Double_t n = par[3];
  Double_t A = par[4];

  if (t > -alpha) {
    return A * TMath::Exp(-0.5 * t * t);
  } else {
    Double_t absAlpha = TMath::Abs(alpha);
    Double_t factor = TMath::Power(n / absAlpha, n) * TMath::Exp(-0.5 * absAlpha * absAlpha);
    Double_t tail = TMath::Power((n / absAlpha) - absAlpha - t, -n);
    return A * factor * tail;
  }
}

//cruiff (che può diventare una gaussiana) da fittare
Double_t Cruijff(Double_t *x, Double_t *par){
  //par[0]=mean      par[1]=sigma_L    par[2]=sigma_R
  //par[3]=alpha_L   par[4]=A
  Double_t fitval = 0;
  Double_t arg_L  = 0;
  Double_t arg_R  = 0;
  Double_t num    = pow((x[0]-par[0]),2);
  Double_t denL   = 2*par[1]*par[1];
  Double_t denR   = 2*par[2]*par[2];
  Double_t asym   = par[3]*num;
  if ( denL + asym != 0 ){ arg_L = -num/(denL+asym); }
  if ( par[2]      != 0 ){ arg_R = -num/ denR      ; }

  if       ( x[0] < par[0] ){ fitval = par[4] * exp(arg_L); } //LEFT
  else if  ( x[0] > par[0] ){ fitval = par[4] * exp(arg_R); } //RIGHT

  return(fitval);
}

int main(int argc, char* argv[]) {

  //controlli iniziali su variabili da input
  Input_Param_Ctrls(argc,argv);

  //parametri da input
  int   CD_number = (atoi(argv[1])); //cooldown
  char* choice    =       argv[3]  ;
  char* misura    =       argv[2]  ;

  std::vector<int> volts = GetVoltages(CD_number,misura);
  std::vector<float> prob;
  std::vector<float> syst_gaus;
  std::vector<float> syst_gaus_err;
 
  Settings settings;
  TCanvas* c1 = new TCanvas( "c1", "", 1500, 1500 );
  settings.Margin(c1);
  c1->SetLeftMargin(0.155);
  c1->cd();

  std::string filename = "data/params_CD" + std::to_string(CD_number)
    + std::string(misura) + std::string(choice) + ".txt";
  std::ofstream ofs(filename);
  ofs << "V (V) E (eV) E_err (V) mu (V)  mu_err (V) sigmaR (V) sigmaR_err (V) sigmaL (V) sigmaL_err (V) reso_gaus (eV) reso_gaus_err (eV) reso_fwhm (eV) reso_fwhm_err (eV)" << std::endl;


  float phi_tes = GetPhiTes();
  
  for(unsigned i=0; i<volts.size(); i++) {
    std::cout << std::endl << std::endl;
    std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
    std::cout << "V = " << volts[i] << std::endl << std::endl;
    
    //leggo tutti i tree per i voltaggi
    TFile* file = TFile::Open( Form("data/root/CD%d/%s/%dV/CD%d_%dV_tree.root", CD_number, misura, volts[i], CD_number, volts[i]), "read" );

    TTree* tree = (TTree*)file->Get("tree");
    
    //lettura del tree in amp o charge
    float variable;
    tree->SetBranchAddress(choice, &variable);

    Histo h_var = SetHistoValues(CD_number,choice,misura,volts[i]);

    
    //impostazione per gli histos
    //int nbins = h_var.get_nbins();
    int nbins = 800;

    float binsize = settings.Binsize(1./nbins);
    const char* x_amp = "Amplitude [V]";
    std::string y_amp = "Counts / " + to_str(binsize) + " V" ;
    
    float hist_min  = h_var.get_hist_min();
    float hist_max  = h_var.get_hist_max();
    float range_min = h_var.get_range_min();
    float range_max = h_var.get_range_max();
    	      
    //creo un histo per ogni tree
    std::string histname(Form("%d", volts[i]));
    TH1F* histo = new TH1F( histname.c_str(), "", nbins, hist_min, hist_max);
    Long64_t nentries = tree->GetEntries();
    
    for(int iEntry=0; iEntry<nentries; iEntry++){
      tree->GetEntry(iEntry);
      histo->Fill(variable);
    }
    
    float fitmin_cb;
    float fitmax_cb;

    if(CD_number==188){
      fitmin_cb = 0.4;
      fitmax_cb = 0.8;
      if(volts[i] <97 ) fitmin_cb = 0.3 ;
      if(volts[i]==97 ) fitmin_cb = 0.3 ;
      if(volts[i]==98 ) fitmin_cb = 0.35;
      if(volts[i]==102) fitmin_cb = 0.3 ;
      if(volts[i]==103) fitmin_cb = 0.3 ;
      if(volts[i]==104) fitmin_cb = 0.3 ;
    }

    if(CD_number==204){
      fitmin_cb = 0.6 ;
      fitmax_cb = 0.625;
      if(volts[i]==96 ) fitmin_cb = 0.59 ; //800 bin
      //if(volts[i]==96 ) fitmin_cb = 0.595  ; //700 bin
      //if(volts[i]==97 ) fitmin_cb = 0.59  ; //700 bin
      if(volts[i]==97 ) fitmin_cb = 0.59  ; //800 bin
      if(volts[i]==98 ) fitmin_cb = 0.602;
      //if(volts[i]==99 ) fitmin_cb = 0.601; //700 bin
      if(volts[i]==99 ) fitmin_cb = 0.60; //800 bin
      if(volts[i]==100) fitmin_cb = 0.592;
      if(volts[i]==101) fitmin_cb = 0.608;
      if(volts[i]==102) fitmin_cb = 0.60 ;
      if(volts[i]==103) fitmin_cb = 0.61 ;
      if(volts[i]>96)   fitmax_cb = 0.65;
    }
    
    TF1 *cb  = new TF1(Form("cb_%d", volts[i]), Crystal_Ball, fitmin_cb, fitmax_cb, 5);
    cb->SetParNames("#mu","sigma", "#alpha", "n", "A");

    if(CD_number==204){
      
      cb->SetParLimits(0, 0.609, 0.72); // mu
      cb->SetParLimits(1, 0.001,  0.5); // sigma
      cb->SetParLimits(2,  0.01,  100); // alpha
      cb->SetParLimits(3,  0.01,  150); // n
      cb->SetParLimits(4,     1, 3000); // A

      cb->SetParameter(0, 0.62); // mu
      cb->SetParameter(1, 0.01); // sigma_L
      cb->SetParameter(2, 0.04); // alpha
      cb->SetParameter(3,  90 ); // n
      cb->SetParameter(4, (double)(histo->Integral()) ); // A

      if(volts[i]==95 ){cb->SetParameter(0, 0.61 );}
      //if(volts[i]==96 ){cb->SetParameter(0, 0.61 );
      //                cb->SetParameter(1, 0.035);}
      if(volts[i]==97 ){cb->SetParameter(0, 0.611);
	cb->SetParameter(1, 0.035);
      	cb->SetParLimits(0, 0.608, 0.612);
      	cb->SetParLimits(1, 0.002, 0.004); } 
      if(volts[i]==98 ){cb->SetParameter(0, 0.65 );
	cb->SetParLimits(0, 0.614, 0.625); }
      if(volts[i]==99 ){cb->SetParameter(0, 0.65 );
	cb->SetParLimits(0, 0.613, 0.625); }
      if(volts[i]==100){cb->SetParameter(0, 0.62 );
	cb->SetParameter(1, 0.035);
	cb->SetParLimits(2, 0, 2 );
	cb->SetParameter(3, 0.3  );
	cb->SetParameter(4, 300  );}
      if(volts[i]==101){cb->SetParameter(0, 0.62 );}
      if(volts[i]==102){cb->SetParameter(0, 0.62 );
	cb->SetParameter(1, 0.035);
	cb->SetParLimits(0, 0.618, 0.625);
	cb->SetParLimits(1, 0.001, 0.04 );} 
      if(volts[i]==103){cb->SetParameter(0, 0.62 );
	cb->SetParameter(1, 0.035);
	cb->SetParameter(3, 0.3  ); 
	cb->SetParLimits(0, 0.618, 0.625);
	cb->SetParLimits(2, 0, 2 );}
      if(volts[i]==104){cb->SetParameter(0, 0.66 ); 
	cb->SetParameter(1, 0.035);}   
    } // if(CD_number==204)
    
    Fit f_var = SetFitValues(CD_number,choice,histo,volts[i],misura);
    
    //assegnazione parametri di fit
    float mu_min     = f_var.get_mu_min();
    float mu_max     = f_var.get_mu_max();
    float mu_        = f_var.get_mu();
    float sigmaL_min = f_var.get_sigmaL_min();
    float sigmaL_max = f_var.get_sigmaL_max();
    float sigmaL_    = f_var.get_sigmaL();
    float sigmaR_min = f_var.get_sigmaR_min();
    float sigmaR_max = f_var.get_sigmaR_max();
    float sigmaR_    = f_var.get_sigmaR();
    float alfa_min   = f_var.get_alfa_min();
    float alfa_max   = f_var.get_alfa_max();
    float alfa_      = f_var.get_alfa();
    float A_min      = f_var.get_A_min();
    float A_max      = f_var.get_A_max();
    float A_         = f_var.get_A();
    float fit_min    = f_var.get_fit_min();
    float fit_max    = f_var.get_fit_max();

    if(volts[i]==99 && nbins == 500 ){mu_min = 0.61; fit_min = 0.607;}
    if(volts[i]==100 && nbins == 500 ){mu_min = 0.615; fit_min = 0.611;}
    
    TF1 *cruijff  = new TF1(Form("cru_%d", volts[i]), Cruijff, fit_min, fit_max, 5);

    
    //parametri del fit
    cruijff->SetParNames("#mu","#sigma_{L}", "#sigma_{R}", "#alpha", "A");
    cruijff->SetParLimits(0,     mu_min,     mu_max);
    cruijff->SetParLimits(1, sigmaL_min, sigmaL_max); 
    cruijff->SetParLimits(2, sigmaR_min, sigmaR_max);
    cruijff->SetParLimits(3,   alfa_min,   alfa_max);
    cruijff->SetParLimits(4,      A_min,      A_max);

    //gaussiana asimmetrica e non cruijff
    cruijff->FixParameter(3,0); //alfa value
    cruijff->SetNpx(1000);
    cruijff->SetParameters(mu_, sigmaL_, sigmaR_, alfa_, A_); //parametri iniziali
    cruijff->SetLineColor(kMagenta+2);
    cruijff->SetLineWidth(5);
    cruijff->SetNpx(1000);
    
    histo->GetXaxis()->SetTitle("Amplitude (V)");
    histo->SetMarkerSize(2);
    histo->SetLineWidth(3);
    histo->Fit(cb, "RX0");
    histo->Fit(cruijff,"RX0Q");
   
    histo->SetNdivisions(510);
   

    cb->SetLineColor(kRed-4);
    cb->SetLineWidth(5);
    cb->SetNpx(1000);
   
    //cruijff->Draw("same");
    settings.graphic_histo(histo,range_min,range_max,x_amp,y_amp.c_str(),volts[i]);
    settings.general_style();
    histo->SetLineWidth(4);
    
    histo->Draw(); //"pe" per avere points+errors
    cb->Draw("same");
    
     //estrazione parametri dal fit
    float mu_cb        = cb->GetParameter(0);
    float muerr_cb     = cb->GetParError(0);
    float sigmaR_cb    = cb->GetParameter(1);
    float sigmaRerr_cb = cb->GetParError(1);
    
    //l'errore sulla stabilità di ites-ibias è insito nei segnali che otteniamo
    //risoluzione energetica gaussiana cb
    float energy_ele = volts[i] -phi_tes;
    float energy_err = sqrt(0.0009 + pow((0.04+0.0015*volts[i]),2));
    float reso_cb    = sigmaR_cb/mu_cb*(volts[i]-phi_tes);
    float reso_errcb = sqrt( sigmaRerr_cb*sigmaRerr_cb*energy_ele*energy_ele/(mu_cb*mu_cb) + sigmaR_cb*sigmaR_cb*energy_ele*energy_ele*muerr_cb*muerr_cb/(mu_cb*mu_cb*mu_cb*mu_cb) + sigmaR_cb*sigmaR_cb*energy_err*energy_err/(mu_cb*mu_cb));


    //estrazione parametri dal fit
    float mu        = cruijff->GetParameter(0);
    float muerr     = cruijff->GetParError(0);
    float sigmaR    = cruijff->GetParameter(2);
    float sigmaRerr = cruijff->GetParError(2);
    float sigmaL    = cruijff->GetParameter(1);
    float sigmaLerr = cruijff->GetParError(1);

   
    //l'errore sulla stabilità di ites-ibias è insito nei segnali che otteniamo
    float reso = sigmaR/mu*(volts[i]-phi_tes);
    float reso_err = sqrt( sigmaRerr*sigmaRerr*energy_ele*energy_ele/(mu*mu) + sigmaR*sigmaR*energy_ele*energy_ele*muerr*muerr/(mu*mu*mu*mu) + sigmaR*sigmaR*energy_err*energy_err/(mu*mu));

    /*
    float fwhm      = (sigmaR+sigmaL)*sqrt(2*log(2));
    float fwhm_err  = sqrt(2*log(2))*sqrt(sigmaRerr*sigmaRerr+sigmaLerr*sigmaLerr);
    float piece1_f  = fwhm_err*fwhm_err*energy_ele*energy_ele/(mu*mu);
    float piece2_f  = fwhm*fwhm*muerr*muerr*energy_ele*energy_ele/(mu*mu*mu*mu);
    float piece3_f  = fwhm*fwhm*energy_err*energy_err/(mu*mu);
    float reso_fwhm = fwhm/mu*(energy_ele);
    float reso_fwhm_err = sqrt(piece1_f+piece2_f+piece3_f);
    */

    //RISOLUZIONE FWHM CB
    float cb_max      = cb->GetMaximum(fitmin_cb, fitmax_cb);
    float Ytarget     = cb_max*0.5;
    float x1          = cb->GetX(Ytarget, fitmin_cb, mu_cb);
    float x2          = cb->GetX(Ytarget, mu_cb, fitmax_cb);
    
    float fwhm_cb     = x2-x1;
    float fwhm_errcb  = 0;
    float piece1_fcb  = fwhm_errcb*fwhm_errcb*energy_ele*energy_ele/(mu_cb*mu_cb);
    float piece2_fcb  = fwhm_cb*fwhm_cb*muerr_cb*muerr_cb*energy_ele*energy_ele/(mu_cb*mu_cb*mu_cb*mu_cb);
    float piece3_fcb  = fwhm_cb*fwhm_cb*energy_err*energy_err/(mu_cb*mu_cb);
    float reso_fwhm_cb = fwhm_cb/mu_cb*(energy_ele);
    float reso_fwhm_err_cb = sqrt(piece1_fcb+piece2_fcb+piece3_fcb);
    
     //scrittura su file param.txt
    ofs << volts[i] << " " << energy_ele << " "
	<< energy_err << " " 
	<< mu << " " << muerr << " "
	<< sigmaR << " " << sigmaRerr << " "
	<< sigmaL << " " << sigmaLerr << " "
	<< reso_cb << " " << reso_errcb << " "
	<< reso_fwhm_cb << " " << reso_fwhm_err_cb << std::endl;

    
    std::cout << "-----CRYSTAL  BALL-----" << std::endl; 
    std::cout << "E = " << volts[i]-phi_tes << " DE_gaus = " << reso_cb << " +/- " << reso_errcb << std::endl;
    std::cout << "E = " << volts[i]-phi_tes << " DE_fwhm = " << reso_fwhm_cb << " +/- " << reso_fwhm_err_cb << std::endl;
    std::cout << "P: " << cb->GetProb()*100 << "%" << std::endl;

    prob.push_back(cb->GetProb()*100);

    std::string outdir( Form("plots/CD%d/%s/%dV", CD_number, misura, volts[i]));
    system( Form("mkdir -p %s", outdir.c_str()) );
    c1->SaveAs(Form("plots/CD%d/%s/%dV/crystralball_%dbins_final.png",CD_number,misura,volts[i],nbins));

    
    float systgaus = abs(reso_cb-reso)/(1.*reso);
    syst_gaus.push_back(systgaus);
    float systgaus_err = sqrt(((reso_errcb*reso_errcb)/(1.*reso*reso))+(reso_cb*reso_cb*reso_err*reso_err)/(1.*reso*reso*reso*reso));
    syst_gaus_err.push_back(systgaus_err);

  } //for sui voltaggi vari

  ofs.close();

  std::cout << std::endl;
  for(int i=0; i<prob.size(); i++){
    std::cout << volts[i] << " V --> prob: " << prob[i] << std::endl;

  }
  return 0;

}

//+++++++++++++++++++++++++++++++++++++++++++//
//++++++++++++++FUNZIONI UTILI+++++++++++++++//
//+++++++++++++++++++++++++++++++++++++++++++//

//funzione di controllo sui parametri iniziali
void Input_Param_Ctrls(int argc, char* argv[]){
  //ctrl sull'input inserito
  if (argc!=4){
    std::cout << "USAGE: ./drawFinalFits [CD_number] [misura] [choice]" << std::endl;
    std::cout << "[choice] : amp/charge" << std::endl; 
    std::cout << "EXAMPLE: ./drawFinalFits 204 B60_preamp_post_cond1 amp" << std::endl;
    exit(1);
  }

  //ctrl su scelta amp/charge
  if(strcmp(argv[3],"amp"   )!=0 &&
     strcmp(argv[3],"charge")!=0 &&
     (atoi(argv[1]))==188 && (atoi(argv[1]))==204){
    std::cout << "Wrong choice! Choose between 'amp' and 'charge'" << std::endl;
    exit(1);
  }

  //ctrl su scelta amp/charge
  if(strcmp(argv[3],"amp"                    )!=0 &&
     strcmp(argv[3],"charge"                 )!=0 &&
     strcmp(argv[3],"amp_10kHz"              )!=0 &&
     strcmp(argv[3],"amp_100kHz"             )!=0 &&
     strcmp(argv[3],"amp_10kHz_100kHz"       )!=0 &&
     strcmp(argv[3],"amp_10kHz_160kHz"       )!=0 &&
     strcmp(argv[3],"amp_10kHz_100kHz_160kHz")!=0 &&
     (atoi(argv[1]))==188){
    std::cout << "Wrong choice! Choose between 'amp','amp_10kHz','amp_100kHz','amp_10kHz_100kHz','amp_10kHz_160kHz' and 'charge'" << std::endl;
    exit(1);
  }

  //ctrl su scelta amp/charge
  if(strcmp(argv[3],"amp"             )!=0 &&
     strcmp(argv[3],"charge"          )!=0 &&
     strcmp(argv[3],"amp_10kHz"       )!=0 &&
     strcmp(argv[3],"amp_10kHz_160kHz")!=0 &&
     (atoi(argv[1]))==204){
    std::cout << "Wrong choice! Choose between 'amp','amp_10kHz', 'amp_10kHz_160kHz' and 'charge'" << std::endl;
    exit(1);
  }

  //ctrl su scelta amp/charge
  if(strcmp(argv[3],"amp"             )!=0 &&
     strcmp(argv[3],"charge"          )!=0 &&
     strcmp(argv[3],"amp_10kHz"       )!=0 &&
     strcmp(argv[3],"amp_100kHz"      )!=0 &&
     strcmp(argv[3],"amp_10kHz_10kHz" )!=0 &&
     (atoi(argv[1]))==222){
    std::cout << "Wrong choice! Choose between 'amp','amp_10kHz', 'amp_10kHz_10kHz', 'amp_100kHz' and 'charge'" << std::endl;
    exit(1);
  }

  //ctrl su scelta amp/charge
  if(strcmp(argv[3],"amp")==0 && (atoi(argv[1]))==188){
    std::cout << "Per studiare questi dati vai nella cartella Electron TES old e usa drawFits.cpp" << std::endl;
    exit(1);
  }
  
}


