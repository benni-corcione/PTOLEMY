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


int main() {

  TStyle* style = setStyle();

  style->SetPadLeftMargin(0.17);
  style->SetPadRightMargin(0.04);
  style->SetPadTopMargin(0.04);
  style->SetPadBottomMargin(0.15);

  std::vector<int> colors = get_colors();

  //std::vector<int> volts = {96, 99, 103, 105};
  std::vector<int> volts = {105, 104, 103, 102, 101, 100, 99, 98, 97, 96, 95};

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

  
  std::vector<TH1D*> histos;

  // for the fit plot of the paper
  float xmin = 0.73;
  float xmax = 0.939999;

  TGraphErrors* gr_mu = new TGraphErrors(0);
  TGraphErrors* gr_mucorr = new TGraphErrors(0);
  TGraphErrors* gr_reso = new TGraphErrors(0);
  TGraphErrors* gr_reso_corr = new TGraphErrors(0);
  TGraphErrors* gr_reso_fwhm = new TGraphErrors(0);

  TFile* file_temp = TFile::Open("temp_vs_V.root", "read" );
  TGraphErrors* gr_temp_vs_V = (TGraphErrors*)file_temp->Get("temp_vs_V");

  TFile* file_corr = TFile::Open("corr_VvsT.root", "read" );
  TGraphErrors* gr_corr = (TGraphErrors*)file_corr->Get("corr");

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  for( unsigned i=0; i<volts.size(); i++ ) {

    std::cout << std::endl << std::endl;
    std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
    std::cout << "V = " << volts[i] << std::endl << std::endl;

    TFile* file = TFile::Open( Form("tree/CD188_%dV_tree.root", volts[i]), "read" );
    //if( volts[i] == 100. ) file = TFile::Open( "tree/100Vb_tree.root", "read" );
    TTree* tree = (TTree*)file->Get("tree");

    std::string histname(Form("%d", volts[i]));
    int nbins = 200;
    if(volts[i]==96){nbins=150;}
    if(volts[i]==95){nbins=160;}
    if(volts[i]==98){nbins=160;}
    TH1D* h1_amp = new TH1D( histname.c_str(), "", nbins, 0., 1. );
    tree->Project(histname.c_str(), "amp");

    float fitmin = (volts[i]<100. && volts[i]!=97.) ? 0.5 : 0.5;
    if(volts[i]==98) fitmin=0.55;
    if(volts[i]==105) fitmin=0.5;
    float fitmax = 0.96;

  // par[0] = mean (mu)
  // par[1] = sigma
  // par[2] = alpha (>0)
  // par[3] = n (>0)
  // par[4] = norm (A)
    
    TF1 *cb  = new TF1(Form("cb_%d", volts[i]), Crystal_Ball, fitmin, fitmax, 5);
    cb->SetParNames("#mu","sigma", "#alpha", "n", "A");
    cb->SetParLimits(0,   xmin,   xmax); // mu
    cb->SetParLimits(1,  0.001,    0.5); // sigma
    cb->SetParLimits(2,   0.01,    100); // alpha
    cb->SetParLimits(3,   50,    100); // n
    cb->SetParLimits(4,      1,   3000); // A

    cb->SetParameter( 0, 0.80 ); // mu
    cb->SetParameter( 1, 0.01 ); // sigma_L
    cb->SetParameter( 2, 0.04 ); // alpha
    cb->SetParameter( 3,  90 ); // n
    cb->SetParameter( 4, (double)(h1_amp->Integral()) ); // A

    //cb->FixParameter( 3, 0.  ); // alpha

    if(volts[i]==103 ){cb->SetParameter( 0, 0.85);} // mu}
    if(volts[i]==105 ){cb->SetParameter( 0, 0.87);} // mu}
    if(volts[i]==102 ){cb->SetParameter( 1, 0.035);} // mu}
    if(volts[i]==103 ){cb->SetParameter( 1, 0.035);} // mu}
    if(volts[i]==105 ){cb->SetParameter( 1, 0.035);} // mu}
    if(volts[i]==102 ){cb->SetParameter( 0, 0.82);} // mu}
    if(volts[i]==100 ){cb->SetParameter( 0, 0.835);} // mu}
    if(volts[i]==100 ){cb->SetParameter( 1, 0.035);} // mu}
    if(volts[i]==101 ){cb->SetParameter( 0, 0.84 );} // mu}
    if(volts[i]==99 ){cb->SetParameter( 0, 0.822 );} // mu}
    if(volts[i]==98 ){cb->SetParameter( 0, 0.82 );} // mu}
    if(volts[i]==97 ){cb->SetParameter( 0, 0.81 );} // mu}
    if(volts[i]==96 ){cb->SetParameter( 0, 0.805 );} // mu}
    if(volts[i]==95 ){cb->SetParameter( 0, 0.805 );} // mu}
    h1_amp->Fit( cb, "RX" );

    c1->Clear();
    h1_amp->Draw();
    cb->SetNpx(1000);
    cb->Draw("Lsame");
    c1->SaveAs(Form("fits/cb_%d.pdf", volts[i]));

    float mu        = cb->GetParameter(0);
    float muerr     = cb->GetParError(0);
    float sigma     = cb->GetParameter(1);
    float sigmaerr  = cb->GetParError(1);
   
    float reso = sigma/mu*volts[i];
    float reso_corr = sqrt( reso*reso - 0.1*0.1 );
    float reso_err = sqrt( sigmaerr*sigmaerr/(mu*mu) + sigma*sigma*muerr*muerr/(mu*mu*mu*mu) )*volts[i];

   
   
   std::cout << "E = " << volts[i] << " reso_gaus = " << reso << " +/- " << reso_err << std::endl;
   std::cout << "P = " << cb->GetProb()*100 << "%" << std::endl; 
  }
 

  return 0;

}


float getCorr( double v, TGraphErrors* gr_temp_vs_V, TGraphErrors* gr_corr ) {

  float temp = -1.;

  for( unsigned i=0; i<gr_temp_vs_V->GetN(); ++i ) {

    double x, y;
    gr_temp_vs_V->GetPoint( i, x, y );

    if( v == x ) temp = y;

  }

  if( temp > 62. ) temp = 61.99;

  float corr = -1.;

  for( unsigned i=1; i<gr_corr->GetN(); ++i ) {

    double x0, y0;
    gr_corr->GetPoint( i-1, x0, y0 );

    double x1, y1;
    gr_corr->GetPoint( i, x1, y1 );

    if( temp>x0 && temp<x1 ) {

      TF1* line = new TF1( "line", "[0] + [1]*x" );
      line->SetParameter( 0, (y0*x1-y1*x0)/(x1-x0) );
      line->SetParameter( 1, (y1-y0)/(x1-x0) );

      corr = line->Eval( temp );

    }

  } // for points

  std::cout << "+++ for V = " << v << " V, T = " << temp << " mK, and corr = " << corr << std::endl;

  return corr;

}

