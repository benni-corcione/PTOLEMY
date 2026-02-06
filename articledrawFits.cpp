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
#include "TStyle.h"
#include "graphics.h"
#include "AndCommon.h"

Double_t Gaussian(Double_t *x, Double_t *par){
  //par[0]=mean      par[1]=sigma_L    par[2]=sigma_R
  //par[3]=A
  Double_t fitval = 0;
  Double_t num    = pow((x[0]-par[0]),2);
  Double_t denL   = 2*par[1]*par[1];
  Double_t denR   = 2*par[2]*par[2];
  Double_t arg_L  = -num/denL;
  Double_t arg_R  = -num/denR;
  
  if       ( x[0] < par[0] ){ fitval = par[3] * exp(arg_L); } //LEFT
  else if  ( x[0] > par[0] ){ fitval = par[3] * exp(arg_R); } //RIGHT

  return(fitval);
}

int main() {

  std::vector<int> colors = {46,38}; //rosso e blu
  std::vector<int> volts = {97, 103};
  std::vector<TH1D*> histos;

  TCanvas* c1 = new TCanvas( "c1", "", 1000, 1000);
  Settings settings;
  settings.Margin(c1);
  settings.general_style();
  c1->SetLeftMargin(0.140);
  c1->cd();

  for( unsigned i=0; i<volts.size(); i++ ) {

    TFile* file = TFile::Open( Form("data/root/CD204/B60_post_cond3_moku/%dV/CD204_%dV_tree.root", volts[i],volts[i]), "read" );
    TTree* tree = (TTree*)file->Get("tree");
    int nentries = tree->GetEntries();

    float amp;
    tree->SetBranchAddress("amp",&amp);

    //histogram creation for each tree
    std::string histname(Form("%d", volts[i]));
    TH1D* h1_amp = new TH1D( histname.c_str(), "", 700, 0., 1. );

    for(int i=0; i<nentries; i++){
      tree->GetEntry(i);
      h1_amp->Fill(amp);
    }

    float fitmin = 0;
    float fitmax = 0;

    if( volts[i] == 97 ) {
      fitmin = 0.606;
      fitmax = 0.625;
    } else if( volts[i] == 103 ) {
      fitmin = 0.617;
      fitmax = 0.635;
    }
    
    //fit function definition
    TF1 *gaussian  = new TF1(Form("gau_%d", volts[i]), Gaussian, fitmin, fitmax, 4);
    gaussian->SetParNames("#mu","sigma_{L}", "#sigma_{R}", "#alpha", "A");

    
    gaussian->SetParLimits( 0, 0.6   , 0.63); // mu
    gaussian->SetParLimits( 1, 5E-4 , 0.01); // sigma_L
    gaussian->SetParLimits( 2, 5E-4, 0.01 ); // sigma_R
    gaussian->SetParLimits( 3, 0   , 30000); // A
 
    gaussian->SetParameter(0, 0.61 ); // mu
    gaussian->SetParameter(1, 6E-3 ); // sigma_L
    gaussian->SetParameter(2, 2E-3 ); // sigma_R
    gaussian->SetParameter(3, (double)(h1_amp->Integral()) ); // A
    
    gaussian->SetLineWidth(3);
    gaussian->SetLineColor(colors[i]);

    h1_amp->SetXTitle("Amplitude [V]");
    h1_amp->SetYTitle(Form("Counts / (%.3f V)", (double)(h1_amp->GetXaxis()->GetBinWidth(13))) );
    h1_amp->GetXaxis()->SetTitleSize(.09);
    h1_amp->GetYaxis()->SetTitleSize(.09);
    h1_amp->GetXaxis()->SetLabelSize(0.085);
    h1_amp->GetYaxis()->SetLabelSize(0.085);
    h1_amp->GetYaxis()->SetTitleOffset(+1.3);
    h1_amp->GetXaxis()->SetTitleOffset(+1.0);
    h1_amp->GetXaxis()->SetNdivisions(107);
    h1_amp->Fit( gaussian, "RX+" );
   
    histos.push_back(h1_amp);
    
  } //ended cycle on the voltage histograms


  //graphic settings
  TH2D* h2_axes = new TH2D( "axes", "", 10, 0.57, 0.65, 10, 0., 300. );
  // h2_axes->SetXTitle( "Charge (uC)" );
  h2_axes->SetXTitle( "Amplitude (V)" );
  h2_axes->GetXaxis()->SetNdivisions(510);
  h2_axes->GetYaxis()->SetTitleOffset(1.5);
  h2_axes->SetYTitle( Form("Counts / (%.3f V)", (double)(histos[0]->GetXaxis()->GetBinWidth(13))) );
  gStyle->SetOptStat(0);
  h2_axes->GetXaxis()->SetTitleSize(.04);
  h2_axes->GetYaxis()->SetTitleSize(.04);
  h2_axes->GetXaxis()->SetLabelSize(0.04);
  h2_axes->GetYaxis()->SetLabelSize(0.04);
  h2_axes->GetYaxis()->SetTitleOffset(+1.7);
  h2_axes->GetXaxis()->SetTitleOffset(+1.2);
  h2_axes->GetXaxis()->SetNdivisions(505);
    
  h2_axes->Draw();

  
  //drawing of superimposed fits histogram
  for( unsigned i=0; i<histos.size(); ++i ) {
 
    TF1* f = histos[i]->GetFunction(Form("gau_%s", histos[i]->GetName()));
   
    histos[i]->SetMarkerSize(2);
    histos[i]->SetLineWidth(2);
    histos[i]->SetFillStyle(3004);
    histos[i]->SetFillColor(colors[i]);
    histos[i]->SetLineColor(colors[i]);
    f->SetLineColor(colors[i]);
    f->SetLineWidth(3);

    
    
    histos[i]->Draw("same");
    f->Draw("same");
    
  }
  
  gPad->SetTickx(1); // tick sull'asse x anche in alto
  gPad->SetTicky(1); // tick sull'asse y anche a destra
 
  TLatex* latex1 = new TLatex();
  latex1->SetTextSize(0.035);

  latex1->SetTextColor(46);
  latex1->SetNDC();
  latex1->DrawLatex( 0.42, 0.85, "#it{V}_{cnt} = 97 V" );
  latex1->Draw("same");

  TLatex* latex2 = new TLatex();
  latex2->SetTextSize(0.035);

  latex2->SetTextColor(38);
  latex2->SetNDC();
  latex2->DrawLatex( 0.65, 0.85, "#it{V}_{cnt} = 103 V" );
  latex2->Draw("same");
  	     
  c1->SaveAs("fits_amp_article.pdf");
  c1->Clear();

  delete(c1);
  delete(h2_axes);

  return 0;
}

