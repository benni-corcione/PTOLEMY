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

#include "common.h"


Double_t Cruijff(Double_t *x, Double_t *par){
  //par[0]=mean      par[1]=costante 0<k<1    par[2]=sigma_R
  //par[3]=alpha_L   par[4]=A
  Double_t fitval = 0;
  Double_t arg_L  = 0;
  Double_t arg_R  = 0;
  Double_t num    = pow((x[0]-par[0]),2);
  Double_t denL   = 2*par[1]*par[1] + par[3]*num;
  Double_t denR   = 2*par[2]*par[2];
  if ( denL > 0 ){ arg_L = -num/denL ;}
  if ( denR > 0 ){ arg_R = -num/denR ;}

  if       ( x[0] < par[0] ){ fitval = par[4] * exp(arg_L); } //LEFT
  else if  ( x[0] > par[0] ){ fitval = par[4] * exp(arg_R); } //RIGHT

  return(fitval);
}


int main(void) {

  TStyle* style = setStyle();

  style->SetPadLeftMargin(0.17);
  style->SetPadRightMargin(0.04);
  style->SetPadTopMargin(0.04);
  style->SetPadBottomMargin(0.15);

  std::vector<int> colors = get_colors();

  std::vector<int> volts = {95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105};

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
  
  //std::string filename = "/Users/massimo/Documents/phd/PTOLEMY/CD data/data/params_CD188conteggiamp_paper.txt";
  //std::ofstream ofs(filename);
  //ofs << "V (V) E (eV) E_err(eV) mu (V)  mu_err (V) sigmaR (V) sigmaR_err (V) sigmaL (V) sigmaL_err (V) reso_gaus (eV) reso_gaus_err (eV) reso_fwhm (eV) reso_fwhm_err (eV)" << std::endl;
  
  std::vector<TH1D*> histos;

  // for the fit plot of the paper
  float xmin = 0.73;
  float xmax = 0.939999;

  //parametri da input
  int   CD_number = 188; 

  char  misura_char[] = "conteggi";
  char* misura = misura_char;
  char  choice_char[] = "amp";
  char* choice = choice_char;

  std::vector<float> syst_fwhm;
  std::vector<float> syst_fwhm_err;
  
  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  for( unsigned i=0; i<volts.size(); i++ ) {

    std::cout << std::endl << std::endl;
    std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
    std::cout << "V = " << volts[i] << std::endl << std::endl;

    TFile* file = TFile::Open( Form("tree/CD188_%dV_tree.root", volts[i]), "read" );
    TTree* tree = (TTree*)file->Get("tree");

    std::string histname(Form("%d", volts[i]));
    int nbins = 200;
    float binsize = (1./nbins);
    TH1D* h1_amp = new TH1D( histname.c_str(), "", nbins, 0., 1. );
    tree->Project(histname.c_str(), "amp");

    float fitmin = (volts[i]<100. && volts[i]!=97.) ? 0.5 : 0.6;
    float fitmax = 0.96;
   
    TF1 *cruijff  = new TF1(Form("cru_%d", volts[i]), Cruijff, fitmin, fitmax, 5);
    cruijff->SetParNames("#mu","sigma_{L}", "#sigma_{R}", "#alpha", "A");
    cruijff->SetParLimits(0,   xmin,   xmax); // mu
    cruijff->SetParLimits(1,   0.01,   0.5); // sigma_L
    cruijff->SetParLimits(2,  0.001,   0.1); // sigma_R
    cruijff->SetParLimits(3,      0,     1 ); // alpha
    cruijff->SetParLimits(4,      1,   3000); // A

    cruijff->SetParameter( 0, 0.8  ); // mu
    cruijff->SetParameter( 1, 0.1  ); // sigma_L
    cruijff->SetParameter( 2, 0.02 ); // sigma_R
    cruijff->SetParameter( 3, 0.1  ); // alpha
    cruijff->SetParameter( 4, (double)(h1_amp->Integral()) ); // A

    cruijff->FixParameter( 3, 0.  ); // alpha

    if(volts[i]==101){cruijff->SetParameter( 0, 0.82);} // mu}

    h1_amp->GetXaxis()->SetTitle("Amplitude (V)");
    h1_amp->GetYaxis()->SetTitle(Form("Counts/%f", binsize));
    h1_amp->SetMarkerSize(2);
    h1_amp->SetLineWidth(3);
    h1_amp->Fit(cruijff, "RX");
    h1_amp->SetNdivisions(510);
    h1_amp->Draw(); //"pe" per avere points+errors

    cruijff->SetLineColor(kRed-4);
    cruijff->SetLineWidth(5);
    cruijff->SetNpx(1000);
    cruijff->Draw("Lsame");
   
   
    float energy_ele = volts[i] - phi_tes;
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
    int first_bin_min = 100;
    int last_bin_min  = 130;
    int first_bin_max = 150;
    int last_bin_max  = 200;
    
    float h_mezzi  = h1_amp->GetMaximum()/2;
    int bin1 = h1_amp->FindFirstBinAbove(h_mezzi,1,first_bin_min,last_bin_min);
    int bin2 = h1_amp->FindLastBinAbove (h_mezzi,1,first_bin_max,last_bin_max);
    
    float fwhm_raw     = h1_amp->GetBinCenter(bin2) - h1_amp->GetBinCenter(bin1);
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

    TLine *l1 = new TLine(h1_amp->GetBinCenter(bin1), h_mezzi, h1_amp->GetBinCenter(bin2), h_mezzi);
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

