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

#include "graphics.h"
#include "CD_details.h"
#include "common.h"

Double_t Cruijff(Double_t *x, Double_t *par){
  //par[0]=mean      par[1]=costante 0<k<1    par[2]=sigma_R
  //par[3]=alpha_L   par[4]=A
  Double_t fitval = 0;
  Double_t arg_L  = 0;
  Double_t arg_R  = 0;
  Double_t num    = pow((x[0]-par[0]),2);
  Double_t denL   = 2*par[1]*par[1] + par[3]*num;
  //Double_t denL   = 2*par[2]*par[2]*par[1]*par[1];
  Double_t denR   = 2*par[2]*par[2];
  //Double_t asym   = par[3]*num;
  if ( denL > 0 ){ arg_L = -num/denL ;}
  if ( denR > 0 ){ arg_R = -num/denR ;}

  if       ( x[0] < par[0] ){ fitval = par[4] * exp(arg_L); } //LEFT
  else if  ( x[0] > par[0] ){ fitval = par[4] * exp(arg_R); } //RIGHT

  return(fitval);
}

//correzioni date dal cambio di temperatura del TES
float getCorr( double v, TGraphErrors* gr_temp_vs_V, TGraphErrors* gr_corr );


int main(int argc, char* argv[]) {

  if(argc!=2){
    std::cout << "Serve inserire il numero di cooldown" << std::endl; exit(1);
  }
  int CD_number = atoi(argv[1]);
  
  const char* measure = nullptr;
  //std::vector<int> colors{kBlack, 46, 38, 30};
  TStyle* style = setStyle();
  style->SetPadLeftMargin(0.17);
  style->SetPadRightMargin(0.04);
  style->SetPadTopMargin(0.04);
  style->SetPadBottomMargin(0.15);

  std::vector<int> colors; 

  std::vector<int> volts;
  if(CD_number==188){volts.push_back(105); volts.push_back(103); volts.push_back(101); volts.push_back(97); measure = "conteggi"; colors.push_back(kAzure+7); colors.push_back(kGreen-6); colors.push_back(kOrange-3); colors.push_back(kRed-3); }
  if(CD_number==204){volts.push_back(105); volts.push_back(97); measure = "B60_post_cond3_moku"; colors.push_back(kAzure+7); colors.push_back(kGreen-6);}
  if(CD_number==222){volts.push_back(123);  volts.push_back(128); measure = "E100_squidfilter_ER_031025_tr12"; colors.push_back(kOrange-3); colors.push_back(kRed-3);}

  // carbon WF:
  float phi = 4.5;

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

  std::vector<TH1F*> histos;

  // for the fit plot of the paper
  float xmin = 0;
  float xmax = 0;
  int nbins;
  if(CD_number==188){ xmin = 0.73; xmax = 0.93999; nbins = 200;}
  if(CD_number==204){ xmin = 0.55;  xmax = 0.67; nbins = 700;}
  if(CD_number==222){ xmin = 0.09; xmax = 0.13; nbins = 1000;}
  

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();


  for( unsigned i=0; i<volts.size(); i++ ) {

    std::cout << std::endl << std::endl;
    std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
    std::cout << "V = " << volts[i] << std::endl << std::endl;

    //leggo tutti i tree per i voltaggi
    TFile* file = TFile::Open( Form("data/root/CD%d/%s/%dV/CD%d_%dV_tree.root", CD_number, measure, volts[i], CD_number, volts[i]), "read" );
    TTree* tree = (TTree*)file->Get("tree");

    //creo un histo per ogni tree
    std::string histname(Form("%d", volts[i]));
    TH1F* h1_amp = new TH1F( histname.c_str(), "", nbins, 0., 1. );
    tree->Project(histname.c_str(), "amp");

    Fit f_var = SetFitValues(CD_number,"amp",h1_amp,volts[i],(char*)measure);
    
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

    //quindi di fatto è una gaussiana doppiamente definitita e basta
    cruijff->FixParameter( 3, 0.  ); // alpha

  
    cruijff->SetLineColor(colors[i]);
    cruijff->SetLineWidth(3); //8 single (metà per multipli insieme)
    h1_amp->SetLineWidth(4); //4 single
    h1_amp->Fit(cruijff, "RX");
    h1_amp->SetXTitle("Amplitude (V)");
    h1_amp->SetYTitle(Form("Counts / (%.3f V)", (double)(h1_amp->GetXaxis()->GetBinWidth(13))) );
    h1_amp->GetXaxis()->SetTitleSize(.07);
    h1_amp->GetYaxis()->SetTitleSize(.07);
    h1_amp->GetXaxis()->SetLabelSize(0.065);
    h1_amp->GetYaxis()->SetLabelSize(0.065);
    h1_amp->GetYaxis()->SetTitleOffset(+1.2);
    h1_amp->GetXaxis()->SetTitleOffset(+1.0);
   
    c1->Clear();
    TLatex* latex = new TLatex();
    latex->SetTextSize(0.065);
    latex->SetTextColor(colors[i]);
    
    h1_amp->Draw();
    cruijff->Draw("Lsame");
    latex->DrawLatex( 0.3, h1_amp->GetMaximum()-0.05*(h1_amp->GetMaximum()), Form("V_{cnt} = %s V", h1_amp->GetName()));
   
      
    histos.push_back(h1_amp);

  } //for sui voltaggi vari

  int ymax;
  if(CD_number==188){ymax = 280;}
  if(CD_number==204){ymax = 280;}
  if(CD_number==222){ymax = 210;}
  TH2D* h2_axes = new TH2D( "axes", "", 10, xmin, xmax, 10, 0., ymax );
  h2_axes->SetXTitle( "Amplitude (V)" );
  //h2_axes->GetXaxis()->SetNdivisions(505);
  h2_axes->GetYaxis()->SetTitleOffset(1.5);
  h2_axes->SetYTitle( Form("Counts / (%.3f V)", (double)(histos[0]->GetXaxis()->GetBinWidth(13))) );

  h2_axes->Draw();
  
  TLatex* latex = new TLatex();
  latex->SetTextSize(0.035);

  for( unsigned i=0; i<histos.size(); ++i ) {

    histos[i]->SetLineWidth(2);
    histos[i]->SetLineColor(colors[i]);
    histos[i]->GetXaxis()->SetNdivisions(505);
    TF1* f = histos[i]->GetFunction(Form("cru_%s", histos[i]->GetName()));
    f->SetLineColor(colors[i]);
    histos[i]->Draw("same");
    
    latex->SetTextColor(colors[i]);
    float fzero = f->Eval(xmin);
    float nsigma = (volts[i]==105.) ? 4.8 : 3.;
    if(CD_number==188){
    latex->DrawLatex( 0.74, fzero + nsigma*sqrt(fzero), Form("#it{V}_{cnt} = %s V", histos[i]->GetName()) );
    }
    if(CD_number==204){ if(i==0){ latex->DrawLatex( 0.627, 220, Form("#it{V}_{cnt} = %s V", histos[i]->GetName()));}
      if(i==1){ latex->DrawLatex( 0.582, 250, Form("#it{V}_{cnt} = %s V", histos[i]->GetName()) );}}
    
  
  
  if(CD_number==222){ if(i==0){ latex->DrawLatex( 0.102, 180, Form("#it{V}_{cnt} = %s V", histos[i]->GetName()));}
    if(i==1){ latex->DrawLatex( 0.118, 160, Form("#it{V}_{cnt} = %s V", histos[i]->GetName()) );}}
  

  }
  gPad->RedrawAxis();

  c1->SaveAs(Form("plots/articolo2/fits_CD%d.pdf",CD_number));
  

  c1->Clear();

    
  
  

  return 0;

}

