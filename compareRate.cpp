#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <filesystem>

#include "TFile.h"
#include "TTree.h"
#include "TH2D.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLine.h"
#include "TPaveText.h"

#include "AndCommon.h"
#include "pshape_functions.h"
#include "graphics.h"
#include "CD_details.h"

int main() {

  TGraphErrors* gr_rate_fp    = new TGraphErrors(0);
  TGraphErrors* gr_rate_PU_fp = new TGraphErrors(0);
  TGraphErrors* gr_rate       = new TGraphErrors(0);
  TGraphErrors* gr_rate_PU    = new TGraphErrors(0);

  //dati di francesco
  /*
  double thisrate_fp       [11] = {3.70, 5.77, 6.57, 8.42, 8.35, 12.28, 18.19, 28.81, 45.60, 57.02, 76.53};
  double thisrate_fp_err   [11] = {0.16, 0.21, 0.17, 0.15, 0.14, 0.17, 0.22, 0.42, 2.16, 0.98, 1.48};
  double thisrate_PU_fp    [11] = {0.071, 0.035, 0.0070, 0.020, 0.0076, 0.016, 0.010, 0.014, 0.048, 0.035, 0.086};
  double thisrate_PU_fp_err[11] = {0.043, 0.027, 0.0041, 0.0086, 0.0039, 0.0064, 0.0060, 0.0070, 0.013, 0.014, 0.017};
  */
  int CD_number = 204;
  //std::string meas = "conteggi";
  std::string meas2 = "B60_post_cond3_moku";
  std::string meas1 = "B60_post_cond3_moku";
  
  for( int v = 96; v<111; ++v ) {

    //DATI MIEI
    TFile* file_rate  = TFile::Open( Form("data/root/CD%d/%s/%dV/rate_%dV.root", CD_number,meas1.c_str(),v,v), "read");
    
    TH1D* h1_rate     = (TH1D*)file_rate ->Get( "rate"    );
    TH1D* h1_rate_PU  = (TH1D*)file_rate ->Get( "rate_PU" );

    int iPoint  = gr_rate->GetN();
    float thisrate     = h1_rate    ->GetMean();
    float thisrate_PU  = h1_rate_PU ->GetMean();
    
    gr_rate   ->SetPoint( iPoint, v, thisrate    );
    gr_rate_PU->SetPoint( iPoint, v, thisrate_PU );
    gr_rate   ->SetPointError( iPoint, 0., h1_rate   ->GetMeanError() );
    gr_rate_PU->SetPointError( iPoint, 0., h1_rate_PU->GetMeanError() );

    /*
    gr_rate_fp   ->SetPoint( iPoint, v, thisrate_fp[v-95]    );
    gr_rate_PU_fp->SetPoint( iPoint, v, thisrate_PU_fp[v-95] );
    gr_rate_fp   ->SetPointError( iPoint, 0., thisrate_fp_err[v-95] );
    gr_rate_PU_fp->SetPointError( iPoint, 0., thisrate_PU_fp_err[v-95] );
    */
    
    if(v==97 || v==99 || v==101 || v==103 || v==106){
    TFile* file_rate2 = TFile::Open( Form("data/root/CD%d/%s/%dV/rate_%dV.root", CD_number,meas2.c_str(),v,v), "read" );
    TH1D* h1_rate2    = (TH1D*)file_rate2->Get( "rate"    );
    TH1D* h1_rate_PU2 = (TH1D*)file_rate2->Get( "rate_PU" );
    float thisrate2    = h1_rate2   ->GetMean();
    float thisrate_PU2 = h1_rate_PU2->GetMean();
    gr_rate_fp   ->SetPoint( iPoint, v, thisrate2    );
    gr_rate_PU_fp->SetPoint( iPoint, v, thisrate_PU2 );
    gr_rate_fp   ->SetPointError( iPoint, 0., h1_rate2   ->GetMeanError());
    gr_rate_PU_fp->SetPointError( iPoint, 0., h1_rate_PU2->GetMeanError());
    file_rate2->Close();
    }
    
     file_rate->Close();
  }

  
   //preparazione grafica
  // gStyle->SetFrameLineWidth(2);
  gStyle->SetTitleFontSize(0.05);
  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c","",1400,1200);
  Settings settings;
  settings.Margin(c);
  c->cd();

  double xmin = 94;
  double xmax = 111;
  double ymin    = -2   ; double ymax    = 260 ;
  double ymin_PU = -0.5; double ymax_PU = 20;
  
  //single pshapes
  TLegend* legend = new TLegend(0.15, 0.90, 0.42, 0.83);
  legend->SetBorderSize(0);
  legend->SetFillStyle(0);
  
  gr_rate->SetMarkerSize(2);
  gr_rate->SetLineWidth(2);
  gr_rate->SetMarkerStyle(20);
  gr_rate->SetLineColor(kRed-7);
  gr_rate->SetMarkerColor(kRed-7);
  //legend->AddEntry(gr_rate,"bc","p");
  legend->AddEntry(gr_rate,"after 3 conditioning","p");
  
  gr_rate_fp->SetMarkerSize(2);
  gr_rate_fp->SetLineWidth(2);
  gr_rate_fp->SetMarkerStyle(20);
  gr_rate_fp->SetLineColor(kAzure+1);
  gr_rate_fp->SetMarkerColor(kAzure+1);
  //legend->AddEntry(gr_rate_fp,"fp","p");
  legend->AddEntry(gr_rate_fp,"after 1 conditionings","p");
  
  TH2D *h2_axes_g = new TH2D( "axes_g", "", 10, xmin, xmax, 10, ymin, ymax);
  h2_axes_g->SetXTitle( "Nanotube voltage #it{V}_{cnt} (V)" );
  h2_axes_g->SetYTitle( "Rate [Hz]" );
  h2_axes_g->GetYaxis()->SetTitleOffset(1.2);
  
  h2_axes_g->Draw("");
  gr_rate->Draw("pe same");
  gr_rate_fp->Draw("pe same");
  legend->Draw("same");
   
  c->SaveAs(Form("plots/CD%d/%s/rate_cfr.png",CD_number,meas1.c_str()));
  c->Clear();
  legend->Clear();

  //multiple events
  gr_rate_PU->SetMarkerSize(2);
  gr_rate_PU->SetLineWidth(2);
  gr_rate_PU->SetMarkerStyle(20);
  gr_rate_PU->SetLineColor(kRed-7);
  gr_rate_PU->SetMarkerColor(kRed-7);
  legend->AddEntry(gr_rate_PU,"after 3 conditioning","p");
  //legend->AddEntry(gr_rate_PU,"bc","p");
  
  gr_rate_PU_fp->SetMarkerSize(2);
  gr_rate_PU_fp->SetLineWidth(2);
  gr_rate_PU_fp->SetMarkerStyle(20);
  gr_rate_PU_fp->SetLineColor(kAzure+1);
  gr_rate_PU_fp->SetMarkerColor(kAzure+1);
  legend->AddEntry(gr_rate_PU_fp,"after 1 conditioning","p");
  //legend->AddEntry(gr_rate_PU_fp,"fp","p");

  TH2D *h2_axes_g_PU = new TH2D( "axes_g", "", 10, xmin, xmax, 10, ymin_PU, ymax_PU);
  h2_axes_g_PU->SetXTitle( "Nanotube voltage #it{V}_{cnt} (V)" );
  h2_axes_g_PU->SetYTitle( "Rate pileup [Hz]" );
  h2_axes_g_PU->GetYaxis()->SetTitleOffset(1.2);
  
  h2_axes_g_PU->Draw("");
  gr_rate_PU->Draw("pe same");
  gr_rate_PU_fp->Draw("pe same");
  legend->Draw("same");
   
  c->SaveAs(Form("plots/CD%d/%s/rate_cfr_PU.png",CD_number,meas1.c_str()));
  c->Clear();

  
  return 0;
  
}
