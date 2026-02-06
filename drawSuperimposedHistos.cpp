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

  std::vector<int> colors = {800,834,868,879};
  std::vector<int> volts = {97, 100, 106};
  std::vector<TH1D*> histos;

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  for( unsigned i=0; i<volts.size(); i++ ) {

    TFile* file = TFile::Open( Form("data/root/CD204/B60_post_cond3_moku/%dV/CD204_%dV_tree.root", volts[i],volts[i]), "read" );
    TTree* tree = (TTree*)file->Get("tree");
    int nentries = tree->GetEntries();
    float charge;
    float amp;
    tree->SetBranchAddress("charge",&charge);
    tree->SetBranchAddress("amp",&amp);

    //histogram creation for each tree
    std::string histname(Form("%d", volts[i]));
    TH1D* h1_amp = new TH1D( histname.c_str(), "", 700, 0., 1. );

    for(int i=0; i<nentries; i++){
      tree->GetEntry(i);
      //h1_amp->Fill(charge*1E+06);
      h1_amp->Fill(amp);
    }

    float fitmin = 0;
    float fitmax = 0;
    /*
    if( volts[i] == 96 ) {
      fitmin = 8.02;
      fitmax = 8.7;
    } else if( volts[i] == 100 ) {
      fitmin = 8.27;
      fitmax = 9.;
    } else if( volts[i] == 104 ) {
      fitmin = 8.6;
      fitmax = 9.5;
    } else if( volts[i] == 109 ) {
      fitmin = 8.85;
      fitmax = 9.3;
    }
    */

    if( volts[i] == 97 ) {
      fitmin = 0.6065;
      fitmax = 0.625;
    } else if( volts[i] == 100 ) {
      fitmin = 0.6125;
      fitmax = 0.63;
    } else if( volts[i] == 106 ) {
      fitmin = 0.6179;
      fitmax = 0.635;
    } 
    
    //fit function definition
    TF1 *cruijff  = new TF1(Form("cru_%d", volts[i]), Cruijff, fitmin, fitmax, 5);
    cruijff->SetParNames("#mu","sigma_{L}", "#sigma_{R}", "#alpha", "A");
    /*
    cruijff->SetParLimits( 0, 8   , 10  ); // mu
    cruijff->SetParLimits( 1, 0.1 , 0.5 ); // sigma_L
    cruijff->SetParLimits( 2, 0.01, 0.5 ); // sigma_R
    cruijff->SetParLimits( 3, 0   , 1   ); // alpha
    cruijff->SetParLimits( 4, 1   , 3000); // A

    cruijff->SetParameter(0, 8.5 ); // mu
    cruijff->SetParameter(1, 0.1 ); // sigma_L
    cruijff->SetParameter(2, 0.1 ); // sigma_R
    */
    cruijff->SetParLimits( 0, 0.61   , 0.7  ); // mu
    cruijff->SetParLimits( 1, 0.001 , 0.01); // sigma_L
    cruijff->SetParLimits( 2, 5E-4, 0.1 ); // sigma_R
    cruijff->SetParLimits( 3, 0   , 1   ); // alpha
    cruijff->SetParLimits( 4, 1   , 3000); // A

    cruijff->SetParameter(0, 0.61 ); // mu
    cruijff->SetParameter(1, 6E-3 ); // sigma_L
    cruijff->SetParameter(2, 2E-3 ); // sigma_R
    
    //cruijff->SetParameter(3, 0.1 ); // alpha
    cruijff->SetParameter(4, (double)(h1_amp->Integral()) ); // A

    //asymmetry to 0 to have gaussian fit
    cruijff->FixParameter( 3, 0. ); // alpha
    cruijff->SetLineWidth(3);
    cruijff->SetLineColor(kRed);

    h1_amp->SetXTitle("Amplitude (V)");
    h1_amp->SetYTitle(Form("Counts / (%.3f V)", (double)(h1_amp->GetXaxis()->GetBinWidth(13))) );
    //h1_amp->GetXaxis()->SetTitleSize(.07);
    //h1_amp->GetYaxis()->SetTitleSize(.07);
    //h1_amp->GetXaxis()->SetLabelSize(0.065);
    //h1_amp->GetYaxis()->SetLabelSize(0.065);
    //h1_amp->GetYaxis()->SetTitleOffset(+1.2);
    //h1_amp->GetXaxis()->SetTitleOffset(+1.0);
    //h1_amp->GetXaxis()->SetNdivisions(107);
    h1_amp->Fit( cruijff, "RX+" );
   
    histos.push_back(h1_amp);
    
  } //ended cycle on the voltage histograms

  //graphic settings
  TH2D* h2_axes = new TH2D( "axes", "", 10, 0.58, 0.64, 10, 0., 300. );
  // h2_axes->SetXTitle( "Charge (uC)" );
  h2_axes->SetXTitle( "Amplitude (V)" );
  h2_axes->GetXaxis()->SetNdivisions(510);
  h2_axes->GetYaxis()->SetTitleOffset(1.5);
  h2_axes->SetYTitle( Form("Counts / (%.3f V)", (double)(histos[0]->GetXaxis()->GetBinWidth(13))) );
  gStyle->SetOptStat(0);

  h2_axes->Draw();
  
  TLatex* latex = new TLatex();
  latex->SetTextSize(0.035);

  //drawing of superimposed fits histogram
  for( unsigned i=0; i<histos.size(); ++i ) {

    TF1* f = histos[i]->GetFunction(Form("cru_%s", histos[i]->GetName()));
   
    histos[i]->SetMarkerSize(2);
    histos[i]->SetLineWidth(2);
    histos[i]->SetFillStyle(3004);
    histos[i]->SetFillColor(colors[i]+1);
    histos[i]->SetLineColor(colors[i]+1);
    f->SetLineColor(kRed+1);
    f->SetLineWidth(2);
    histos[i]->Draw("same");
    f->Draw("same");
    
  }

  //latex->SetTextColor(kBlack);
  //latex->DrawLatex( .5, .5, "#it{V}_{cnt} = 98 V" );
  //gPad->RedrawAxis("same");
		     
  c1->SaveAs("fits_amp.pdf");
  c1->Clear();
  /*

  //photon resolution comparison 
  TGraphErrors* gr_photonreso      = new TGraphErrors(0);
  TGraphErrors* gr_photonreso_fwhm = new TGraphErrors(0);
  
  float e_photon, reso_photon, reso_photon_err;
  int   n_photon;

  std::ifstream file_photons("data/photonreso.dat");

  while( file_photons >> e_photon >> reso_photon >> reso_photon_err >> n_photon ) {
    int n = gr_photonreso->GetN();
    gr_photonreso->SetPoint( n, e_photon, reso_photon );
    gr_photonreso->SetPointError( n, 0., reso_photon_err ); 
    gr_photonreso_fwhm->SetPoint( n, e_photon, reso_photon*2*sqrt(2*log(2)) );
    gr_photonreso_fwhm->SetPointError( n, 0., reso_photon_err*2*sqrt(2*log(2)) ); 
  }

  float xmin_resomu = 94.00001 - phi;
  float xmax_resomu = 105.9999 - phi;

  //graphic settings
  TH2D* h2_axes_reso = new TH2D( "axes_reso", "", 10, xmin_resomu, xmax_resomu, 10, 0., 2.3 );
  h2_axes_reso->SetXTitle( "Energy (eV)" );
  h2_axes_reso->GetYaxis()->SetTitleOffset(1.5);
  h2_axes_reso->GetXaxis()->SetNdivisions(3, 5, 1);
  //h2_axes_reso->GetXaxis()->SetNdivisions(706);
  h2_axes_reso->SetYTitle( "TES Gaussian energy resolution (eV)" );
  //h2_axes_reso->SetYTitle( "TES electron energy resolution #sigma_{e} (eV)" );
  h2_axes_reso->Draw();

  //photon resolution drawing
  gr_photonreso->SetMarkerSize(2);
  gr_photonreso->SetLineWidth(3);
  gr_photonreso->SetMarkerStyle(21);
  gr_photonreso->SetMarkerColor(kBlack);
  gr_photonreso->SetLineColor  (kBlack);
  gr_photonreso->Draw("PSame");

  //gr_reso_corr->SetMarkerSize(2);
  //gr_reso_corr->SetLineWidth(3);
  //gr_reso_corr->SetMarkerStyle(24);
  //gr_reso_corr->SetMarkerColor(kBlue);
  //gr_reso_corr->SetLineColor(46);
  //gr_reso_corr->Draw("PSame");

  //superimposition of electron resolution
  gr_reso->SetMarkerSize(2);
  gr_reso->SetLineWidth(3);
  gr_reso->SetMarkerStyle(20);
  gr_reso->SetMarkerColor(46);
  gr_reso->SetLineColor(46);
  gr_reso->Draw("PSame");

  TLegend* legend = new TLegend( 0.6, 0.75, 0.9, 0.9 );
  legend->SetTextSize( 0.04 );
  legend->SetFillColor( 0 );
  legend->AddEntry( gr_photonreso, "Photons"  , "PL" );
  legend->AddEntry( gr_reso      , "Electrons", "PL" );
  legend->Draw("same");

  gPad->RedrawAxis();
  
  c1->SaveAs("reso.pdf");
  c1->Clear();


  //graphic settings
  TH2D* h2_axes_reso_fwhm = new TH2D( "axes_reso_fwhm", "", 10, xmin_resomu, xmax_resomu, 10, 0., 2.3*2*sqrt(2*log(2)) );
  h2_axes_reso_fwhm->SetXTitle( "Energy (eV)" );
  h2_axes_reso_fwhm->GetYaxis()->SetTitleOffset(1.5);
  h2_axes_reso_fwhm->GetXaxis()->SetNdivisions(3, 5, 1);
  h2_axes_reso_fwhm->SetYTitle( "#Delta#it{E}_{fwhm} (eV)" );
  h2_axes_reso_fwhm->Draw();

  //photon fwhm resolution drawing
  gr_photonreso_fwhm->SetMarkerSize(2);
  gr_photonreso_fwhm->SetLineWidth(3);
  gr_photonreso_fwhm->SetMarkerStyle(21);
  gr_photonreso_fwhm->SetMarkerColor(kBlack);
  gr_photonreso_fwhm->SetLineColor  (kBlack);
  gr_photonreso_fwhm->Draw("PSame");

  //superimposition of electron fwhm resolution
  gr_reso_fwhm->SetMarkerSize(2);
  gr_reso_fwhm->SetLineWidth(3);
  gr_reso_fwhm->SetMarkerStyle(20);
  gr_reso_fwhm->SetMarkerColor(46);
  gr_reso_fwhm->SetLineColor(46);
  gr_reso_fwhm->Draw("PSame");

  legend->Draw("same");

  gPad->RedrawAxis();
  
  c1->SaveAs("reso_fwhm.pdf");
  c1->Clear();

  //mu vs volts drawing
  //graphic settings
  TH2D *h2_axes_mu = new TH2D( "axes_mu", "", 10, xmin_resomu+phi, xmax_resomu+phi, 10, 0., 1.1 );
  //TH2D *h2_axes_mu = new TH2D( "axes_mu", "", 10, xmin_resomu, xmax_resomu, 10, 0.79, 0.89 );
  h2_axes_mu->SetXTitle( "Electron energy #it{E}_{e} (eV)" );
  h2_axes_mu->SetYTitle( "Position of absorption peak #mu (V)" );
  h2_axes_mu->GetYaxis()->SetTitleOffset(1.5);
  h2_axes_mu->GetXaxis()->SetNdivisions(3, 5, 1);
  h2_axes_mu->GetYaxis()->SetNdivisions(5, 5, 1);
  h2_axes_mu->GetYaxis()->SetAxisColor (colors[0]);
  h2_axes_mu->GetYaxis()->SetTitleColor(colors[0]);
  h2_axes_mu->GetYaxis()->SetLabelColor(colors[0]);
  h2_axes_mu->Draw();

  //mu vs volts
  gr_mu->SetMarkerSize(2);
  gr_mu->SetLineWidth(3);
  gr_mu->SetMarkerStyle(21);
  gr_mu->SetMarkerColor(kBlack);
  gr_mu->SetLineColor(kBlack);
  gr_mu->Draw("PSame");

  //mu corrected with temperature vs volts
  gr_mucorr->SetMarkerSize(2);
  gr_mucorr->SetLineWidth(3);
  gr_mucorr->SetMarkerStyle(21);
  gr_mucorr->SetMarkerColor(46);
  gr_mucorr->SetLineColor(46);
  gr_mucorr->Draw("PSame");

  TLegend* legend_mu = new TLegend( 0.2, 0.2, 0.55, 0.3);
  legend_mu->SetFillColor(0);
  legend_mu->SetTextSize( 0.04 );
  legend_mu->AddEntry( gr_mu,     "Before temp. correction", "PL" );
  legend_mu->AddEntry( gr_mucorr, "After temp. correction" , "PL" );
  legend_mu->Draw("same");
  

  gPad->RedrawAxis();

  c1->SaveAs("mu.pdf");
  c1->Clear();

  //?????
  TPad *pad_mu   = new TPad("pad_mu"  ,"",0,0,1,1);
  TPad *pad_reso = new TPad("pad_reso","",0,0,1,1);
  
  pad_reso->SetFillStyle(4000); //will be transparent
  pad_reso->SetFrameFillStyle(0);

  h2_axes_reso->GetYaxis()->SetAxisColor (46);
  h2_axes_reso->GetYaxis()->SetTitleColor(46);
  h2_axes_reso->GetYaxis()->SetLabelColor(46);

  pad_mu->Draw();
  pad_mu->cd();
  h2_axes_mu->Draw("");

  gr_mu->SetMarkerStyle(21);
  gr_mu->SetMarkerSize(2);
  //gr_mu->SetLineWidth(2);
  gr_mu->Draw("psame");

  pad_reso->Draw("Y+");
  pad_reso->cd();

  h2_axes_reso->Draw("Y+");
  gr_reso->SetLineWidth(1);
  gr_reso->Draw("PSame Y+");

  c1->SaveAs("mureso.pdf");
    */
  return 0;

}


float getCorr( double v, TGraphErrors* gr_temp_vs_V, TGraphErrors* gr_corr ) {
//temperatura
  float temp = -1.;

  //scan over graph points
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

      //interception line
      TF1* line = new TF1( "line", "[0] + [1]*x" );
      line->SetParameter( 0, (y0*x1-y1*x0)/(x1-x0) );
      line->SetParameter( 1, (y1-y0)/(x1-x0) );

      corr = line->Eval( temp );

    }

  } // for over points

  std::cout << "+++ for V = " << v << " V, T = " << temp << " mK, and corr = " << corr << std::endl;

  return corr;

}


