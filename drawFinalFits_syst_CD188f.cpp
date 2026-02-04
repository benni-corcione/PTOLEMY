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

int main(void) {

  //parametri da input
  int   CD_number = 188; 

  char  misura_char[] = "conteggi";
  char* misura = misura_char;
  char  choice_char[] = "amp_100kHz";
  char* choice = choice_char;

  std::vector<int> volts = GetVoltages(CD_number, misura);
  std::vector<float> syst_fwhm;
  std::vector<float> syst_fwhm_err;
 
  Settings settings;
  TCanvas* c1 = new TCanvas( "c1", "", 1500, 1500 );
  settings.Margin(c1);
  c1->SetLeftMargin(0.155);
  c1->cd();

  float phi_tes = GetPhiTes();

  for(unsigned i=0; i<volts.size(); i++) {
    //for(unsigned i=0; i<1; i++) {
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
    int nbins = 92;
    if(volts[i]>99) nbins = 202;

    float binsize = settings.Binsize(1./nbins);
    const char* x_amp = "Amplitude [V]";
    std::string y_amp = "Counts / " + to_str(binsize) + " V" ;
    
    float hist_min  = h_var.get_hist_min();
    float hist_max  = h_var.get_hist_max();
    float range_min = 0.2;
    float range_max = 0.8;
    	      
    //creo un histo per ogni tree
    std::string histname(Form("%d", volts[i]));
    TH1F* histo = new TH1F( histname.c_str(), "", nbins, hist_min, hist_max);
    Long64_t nentries = tree->GetEntries();
    
    for(int iEntry=0; iEntry<nentries; iEntry++){
      tree->GetEntry(iEntry);
      histo->Fill(variable);
    }
    
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
    
     TF1 *cruijff  = new TF1(Form("cru_%d", volts[i]), Cruijff, fit_min, fit_max, 5);

    //parametri del fit
    cruijff->SetParNames("#mu","#sigma_{L}", "#sigma_{R}", "#alpha", "A");
    cruijff->SetParLimits(0,    mu_min,    mu_max);
    cruijff->SetParLimits(1,sigmaL_min,sigmaL_max); 
    cruijff->SetParLimits(2,sigmaR_min,sigmaR_max);
    cruijff->SetParLimits(3,  alfa_min,  alfa_max);
    cruijff->SetParLimits(4,     A_min,     A_max);

    //gaussiana asimmetrica e non cruijff
    cruijff->FixParameter(3,0); //alfa value
    cruijff->SetNpx(1000);
    cruijff->SetParameters(mu_, sigmaL_, sigmaR_, alfa_, A_); //parametri iniziali

    
    
    histo->GetXaxis()->SetTitle("Amplitude (V)");
    histo->SetMarkerSize(2);
    histo->SetLineWidth(3);
    histo->Fit(cruijff, "RX");
    histo->SetNdivisions(510);
    histo->Draw(); //"pe" per avere points+errors

    cruijff->SetLineColor(kRed-4);
    cruijff->SetLineWidth(5);
    cruijff->SetNpx(1000);
    cruijff->Draw("same");
    settings.graphic_histo(histo,range_min,range_max,x_amp,y_amp.c_str(),volts[i]);
    settings.general_style();
    histo->SetLineWidth(3);

    float energy_ele = volts[i] -phi_tes;
    float energy_err = sqrt(0.0009 + pow((0.04+0.0015*volts[i]),2));
    
    //estrazione parametri dal fit
    float mu        = cruijff->GetParameter(0);
    float muerr     = cruijff->GetParError(0);
    float sigmaR    = cruijff->GetParameter(2);
    float sigmaRerr = cruijff->GetParError(2);
    float sigmaL    = cruijff->GetParameter(1);
    float sigmaLerr = cruijff->GetParError(1);

    float muerr_gaus = sqrt( muerr*muerr + 0.01*0.01*mu*mu );
    
    //RISOLUZIONE GAUSSIANA CRUIJFF
    float reso     = sigmaR/mu*(volts[i]);
    float reso_err = sqrt( sigmaRerr*sigmaRerr/(mu*mu) + sigmaR*sigmaR*muerr_gaus*muerr_gaus/(mu*mu*mu*mu) )*volts[i];

    //RISOLUZIONE FWHM CRUIJFF
    float fwhm_fit = (sigmaR+sigmaL)*sqrt(2*log(2));
    float fwhm_err_fit = sqrt(2*log(2))*sqrt(sigmaRerr*sigmaRerr+sigmaLerr*sigmaLerr);
    float piece1_f_fit = fwhm_err_fit*fwhm_err_fit*energy_ele*energy_ele/(mu*mu);
    float piece2_f_fit = fwhm_fit*fwhm_fit*muerr*muerr*energy_ele*energy_ele/(mu*mu*mu*mu);
    float piece3_f_fit = fwhm_fit*fwhm_fit*energy_err*energy_err/(mu*mu);
    float reso_fwhm_fit = fwhm_fit/mu*(energy_ele);
    float reso_fwhm_err_fit = sqrt(piece1_f_fit+piece2_f_fit+piece3_f_fit);

    
    //RISOLUZIONE FWHM DISTRIBUZIONE
    int first_bin_min, last_bin_min, first_bin_max, last_bin_max;
    if(nbins==92){
      first_bin_min = 40 ; last_bin_min = 60 ;
      first_bin_max = 60 ; last_bin_max = 90 ; }
    if(nbins==202){
      first_bin_min = 105; last_bin_min = 120;
      first_bin_max = 120; last_bin_max = 180; }
    
    float h_mezzi  = histo->GetMaximum()/2;
    int bin1 = histo->FindFirstBinAbove(h_mezzi,1,first_bin_min,last_bin_min);
    int bin2 = histo->FindLastBinAbove (h_mezzi,1,first_bin_max,last_bin_max);
    
    float fwhm_raw     = histo->GetBinCenter(bin2) - histo->GetBinCenter(bin1);
    float fwhm_errraw  = 1./(nbins*sqrt(3));
    float piece1_fraw  = fwhm_errraw*fwhm_errraw*energy_ele*energy_ele/(mu*mu);
    float piece2_fraw  = fwhm_raw*fwhm_raw*muerr*muerr*energy_ele*energy_ele/(mu*mu*mu*mu);
    float piece3_fraw  = fwhm_raw*fwhm_raw*energy_err*energy_err/(mu*mu);
    float reso_fwhm_raw = fwhm_raw/mu*(energy_ele);
    float reso_fwhm_err_raw = sqrt(piece1_fraw+piece2_fraw+piece3_fraw);


    std::cout << "E = " << volts[i]-phi_tes << " DE_gaus = " << reso << " +/- " << reso_err << std::endl;
    std::cout << "E = " << volts[i]-phi_tes << " DE_fwhm_fit = " << reso_fwhm_fit << " +/- " << reso_fwhm_err_fit << std::endl;
    std::cout << "E = " << volts[i]-phi_tes << " DE_fwhm_raw = " << reso_fwhm_raw << " +/- " << reso_fwhm_err_raw << std::endl;
    std::cout << "P: " << cruijff->GetProb()*100 << "%" << std::endl;
   

    TLine *l1 = new TLine(histo->GetBinCenter(bin1), h_mezzi, histo->GetBinCenter(bin2), h_mezzi);
    l1->SetLineWidth(4);
    l1->SetLineColor(64);
    l1->Draw("same");

    
    TLine *l4 = new TLine(mu-(sigmaL*sqrt(2*log(2))), (cruijff->GetMaximum())*0.5, mu+(sigmaR*sqrt(2*log(2))),(cruijff->GetMaximum())*0.5);
    l4->SetLineWidth(4);
    l4->SetLineColor(92);
    l4->Draw("same");   

    
    std::string outdir( Form("plots/CD%d/%s/%dV", CD_number, misura, volts[i]));
    system( Form("mkdir -p %s", outdir.c_str()) );
    c1->SaveAs(Form("plots/CD%d/%s/%dV/%dV_systFWHMvsFIT_%dnbins.png",CD_number, misura, volts[i], volts[i],nbins));

    
    float systfwhm = abs(reso_fwhm_fit-reso_fwhm_raw)/(1.*reso_fwhm_fit);
    syst_fwhm.push_back(systfwhm);
    float systfwhm_err = sqrt(((reso_fwhm_err_raw*reso_fwhm_err_raw)/(1.*reso_fwhm_fit*reso_fwhm_fit))+(reso_fwhm_raw*reso_fwhm_raw*reso_fwhm_err_fit*reso_fwhm_err_fit)/(1.*reso_fwhm_fit*reso_fwhm_fit*reso_fwhm_fit*reso_fwhm_fit));
    syst_fwhm_err.push_back(systfwhm_err);

  } //for sui voltaggi vari

  std::cout << std::endl;
  
  for(int j=0; j<syst_fwhm.size(); j++){
    std::cout << volts[j] << " -> " << syst_fwhm[j]*100 << " +- " << syst_fwhm_err[j]*100 << std::endl;
  }
  std::cout << std::endl;

  
  for(int j=0; j<syst_fwhm.size(); j++){
    std::cout << syst_fwhm[j]*100 << ", ";
  }
  std::cout << std::endl;
   for(int j=0; j<syst_fwhm.size(); j++){
    std::cout << syst_fwhm_err[j]*100 << ", ";
  }
  std::cout << std::endl;
  
  return 0;

}

