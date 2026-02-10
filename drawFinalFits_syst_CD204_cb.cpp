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

Double_t Crystal_Ball(Double_t *x, Double_t *par) {
  // par[0] = mean (mu)  par[1] = sigma
  // par[2] = alpha (>0) par[3] = n (>0)
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

int main(void) {

  //parametri da input
  int   CD_number = 204; 

  char  misura_char[] = "B60_post_cond3_moku";
  char* misura = misura_char;
  char  choice_char[] = "amp";
  char* choice = choice_char;

  //char* misura = "B60_post_cond3_moku";
  //char* choice= "amp";
  
  std::vector<int> volts = GetVoltages(204, misura);
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
    
    float fitmin_cb = 0.6;
    float fitmax_cb = 0.65;
    if(volts[i]==96 ) fitmin_cb = 0.59 ;
    if(volts[i]==97 ) fitmin_cb = 0.59 ;
    if(volts[i]==98 ) fitmin_cb = 0.602;
    if(volts[i]==99 ) fitmin_cb = 0.60 ;
    if(volts[i]==100) fitmin_cb = 0.592;
    if(volts[i]==101) fitmin_cb = 0.608;
    if(volts[i]==102) fitmin_cb = 0.60 ;
    if(volts[i]==103) fitmin_cb = 0.61 ;
    
    
    TF1 *cb  = new TF1(Form("cb_%d", volts[i]), Crystal_Ball, fitmin_cb, fitmax_cb, 5);
    cb->SetParNames("#mu","sigma", "#alpha", "n", "A");

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
    
    histo->GetXaxis()->SetTitle("Amplitude (V)");
    histo->SetMarkerSize(2);
    histo->SetLineWidth(3);
    histo->Fit(cb, "RX");
    histo->SetNdivisions(510);
    histo->Draw(); //"pe" per avere points+errors

    cb->SetLineColor(kRed-4);
    cb->SetLineWidth(5);
    cb->SetNpx(1000);
    cb->Draw("same");
    settings.graphic_histo(histo,range_min,range_max,x_amp,y_amp.c_str(),volts[i]);
    settings.general_style();
    histo->SetLineWidth(3);

    float energy_ele = volts[i] -phi_tes;
    float energy_err = sqrt(0.0009 + pow((0.04+0.0015*volts[i]),2));
    
    //estrazione parametri dal fit
    float mu_cb        = cb->GetParameter(0);
    float muerr_cb     = cb->GetParError(0);
    float sigmaR_cb    = cb->GetParameter(1);
    float sigmaRerr_cb = cb->GetParError(1);
    
    //RISOLUZIONE GAUSSIANA CB
    float reso_cb    = sigmaR_cb/mu_cb*(volts[i]-phi_tes);
    float reso_errcb = sqrt( sigmaRerr_cb*sigmaRerr_cb*energy_ele*energy_ele/(mu_cb*mu_cb) + sigmaR_cb*sigmaR_cb*energy_ele*energy_ele*muerr_cb*muerr_cb/(mu_cb*mu_cb*mu_cb*mu_cb) + sigmaR_cb*sigmaR_cb*energy_err*energy_err/(mu_cb*mu_cb));

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


    //RISOLUZIONE FWHM DISTRIBUZIONE
    int first_bin = nbins/1.7; 
    int last_bin  = nbins/1.4;
    float h_mezzi  = histo->GetMaximum()/2;
    int bin1 = histo->FindFirstBinAbove(h_mezzi,1,first_bin,last_bin);
    int bin2 = histo->FindLastBinAbove (h_mezzi,1,first_bin,last_bin);
      
    float fwhm_raw     = histo->GetBinCenter(bin2) - histo->GetBinCenter(bin1);
    float fwhm_errraw  = 1./(nbins*sqrt(3));
    float piece1_fraw  = fwhm_errraw*fwhm_errraw*energy_ele*energy_ele/(mu_cb*mu_cb);
    float piece2_fraw  = fwhm_raw*fwhm_raw*muerr_cb*muerr_cb*energy_ele*energy_ele/(mu_cb*mu_cb*mu_cb*mu_cb);
    float piece3_fraw  = fwhm_raw*fwhm_raw*energy_err*energy_err/(mu_cb*mu_cb);
    float reso_fwhm_raw = fwhm_raw/mu_cb*(energy_ele);
    float reso_fwhm_err_raw = sqrt(piece1_fraw+piece2_fraw+piece3_fraw);


    std::cout << "E = " << volts[i]-phi_tes << " DE_gaus = " << reso_cb << " +/- " << reso_errcb << std::endl;
    std::cout << "E = " << volts[i]-phi_tes << " DE_fwhm_cb = " << reso_fwhm_cb << " +/- " << reso_fwhm_err_cb << std::endl;
    std::cout << "E = " << volts[i]-phi_tes << " DE_fwhm_raw = " << reso_fwhm_raw << " +/- " << reso_fwhm_err_raw << std::endl;
    std::cout << "P: " << cb->GetProb()*100 << "%" << std::endl;
   

    TLine *l1 = new TLine(histo->GetBinCenter(bin1), h_mezzi, histo->GetBinCenter(bin2), h_mezzi);
    l1->SetLineWidth(4);
    l1->SetLineColor(64);
    l1->Draw("same");

    TLine *l4 = new TLine(x1, Ytarget, x2, Ytarget);
    l4->SetLineWidth(4);
    l4->SetLineColor(92);
    l4->Draw("same");   

    
    std::string outdir( Form("plots/CD%d/%s/%dV", CD_number, misura, volts[i]));
    system( Form("mkdir -p %s", outdir.c_str()) );
    c1->SaveAs(Form("plots/CD%d/%s/%dV/%dV_systFWHMvsFIT_%dnbins.png",CD_number, misura, volts[i], volts[i],nbins));

    
    float systfwhm = abs(reso_fwhm_cb-reso_fwhm_raw)/(1.*reso_fwhm_cb);
    syst_fwhm.push_back(systfwhm);
    float systfwhm_err = sqrt(((reso_fwhm_err_raw*reso_fwhm_err_raw)/(1.*reso_fwhm_cb*reso_fwhm_cb))+(reso_fwhm_raw*reso_fwhm_raw*reso_fwhm_err_cb*reso_fwhm_err_cb)/(1.*reso_fwhm_cb*reso_fwhm_cb*reso_fwhm_cb*reso_fwhm_cb));
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

