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

float GetScale(TH1F* h_amp, TH1F* h_amp_dc, float scale_min, float scale_max, int number_bins, float delta, float initial_bin);

int main(int argc, char* argv[]) {

  if(argc!=3){
    std::cout << "USAGE: ./correctRate [CD_number] [meas]" << std::endl;
    std::cout << "EXAMPLE: ./correctRate 204 B60_post_cond3_moku" << std::endl;
    exit(1);
  }
  int   CD_number = atoi(argv[1]);
  char* meas      =      argv[2] ;
  
  std::vector<int> volts = GetVoltages(CD_number,meas);
  float rate         [volts.size()];
  float voltage      [volts.size()];
  float rate_error   [volts.size()];
  float newrate      [volts.size()];
  float newrate_error[volts.size()];
  float voltage_error[volts.size()];
  double scale=0;
  for(int i=0; i<volts.size(); i++){
    
    //recupero rate originaria
    TFile* file_rate  = TFile::Open(Form("data/root/CD%d/%s/%d/rate_%dV.root", CD_number,meas,volts[i],volts[i]), "read");
    
    TH1D* h1_rate = (TH1D*)file_rate->Get("rate");
    rate      [i]= h1_rate->GetMean();
    rate_error[i]= h1_rate->GetMeanError();
    file_rate->Close(); 
    
  }
  
  //tela
  TCanvas* c = new TCanvas("c","",1400,1200);
  Settings settings;
  settings.Margin(c);
  c->SetLeftMargin(0.155);
  c->cd();
  
  for(int i=0; i<volts.size(); i++){
    
    //recupero histo dei conteggi e histo dei dark counts
    //apertura file
    TFile run(Form("data/root/CD%d/%s/%dV/CD%d_%dV_tree.root",CD_number,meas,volts[i],CD_number,volts[i]));
    TTree *tree = (TTree*)run.Get("tree");
    Long64_t nentries = tree->GetEntries();
    
    TFile run_dc(Form("data/root/CD%d/%s/0V/CD%d_0V_tree.root",CD_number,meas,CD_number));
    TTree *tree_dc = (TTree*)run_dc.Get("tree");
    Long64_t nentries_dc = tree_dc->GetEntries();
    
    //preparazione per la lettura del tree in amp o charge
    float amp, amp_dc;
    int cosmic_rays, cosmic_rays_dc;
    int electronics, electronics_dc;
    
    tree   ->SetBranchAddress("amp"        , &amp           );
    tree_dc->SetBranchAddress("amp"        , &amp_dc        );
    tree   ->SetBranchAddress("cosmic_rays", &cosmic_rays   );
    tree_dc->SetBranchAddress("cosmic_rays", &cosmic_rays_dc);
    tree   ->SetBranchAddress("electronics", &electronics   );
    tree_dc->SetBranchAddress("electronics", &electronics_dc);
    
    //variabili per l'istogramma
    int nbins = 400;
    int hist_min = 0;
    int hist_max = 1;
    
    Settings settings;
    float binsize = settings.Binsize((float)(hist_max-hist_min)/nbins);
    
    //std::string y_amp = "Counts / " + to_str(binsize) + " V" ;
    
    //creazione istogramma e lettura del tree
    TH1F* h_amp   = new TH1F("h_amp"   , "Amplitude", nbins, hist_min, hist_max);
    TH1F* h_amp_sb= new TH1F("h_amp_sb", "Amplitude", nbins, hist_min, hist_max);
    TH1F* h_amp_dc= new TH1F("h_amp_dc", "Amplitude", nbins, hist_min, hist_max);
    
    for(int iEntry=0; iEntry<nentries; iEntry++){
      tree->GetEntry(iEntry);
      if(cosmic_rays==0 && electronics==0){
	h_amp   ->Fill(amp);
	h_amp_sb->Fill(amp);
      }
    }
    
    for(int iEntry=0; iEntry<nentries_dc; iEntry++){
      tree_dc->GetEntry(iEntry);
      if(cosmic_rays==0 && electronics==0){
	h_amp_dc->Fill(amp_dc);
      }
    }
    
    h_amp   ->SetLineColor(kBlack);
    h_amp_dc->SetLineColor(kPink-4);
    h_amp   ->SetLineWidth(3);
    h_amp_dc->SetLineWidth(3);
    h_amp_dc->GetYaxis()->SetTitle(Form("Counts / %.4f V", binsize));
    h_amp_dc->GetXaxis()->SetTitle("Amplitude [V]");
    h_amp_dc->GetXaxis()->SetRangeUser(0,0.2);
    
    float initial_bin=-2;
    //definizione primo bin interessante
    for(int k=0; k<nbins; k++){
      if((h_amp_dc->GetBinContent(k))!=0 && initial_bin==-2){
	initial_bin = k;
	//std::cout << "first bin center: " << h_amp_dc->GetBinCenter(k) << std::endl;
      }
    }
    //float GetScale(h_amp, h_amp_dc, scale_min, scale_max, number_bins, delta, initial_bin);

    
    
    
    scale = GetScale(h_amp, h_amp_dc, 0, 50, 10, 1, initial_bin);
    //std::cout << "scale raw: " << scale << std::endl;
    
    scale = GetScale(h_amp, h_amp_dc, (scale-0.5), (scale+0.5), 10, 0.01, initial_bin);
    //std::cout << "scale factor: " << scale << std::endl;
    
    h_amp_dc->Scale(scale);
    
    h_amp_dc->Draw("histsame");
    h_amp   ->Draw("same");
    //h_amp_dc->Draw("histsame");
    gStyle->SetOptStat(0);
    c->SaveAs(Form("plots/CD%d/%s/%dV/counts_vs_dc.png",CD_number,meas,volts[i]));
    c->Clear();
    
    h_amp->GetYaxis()->SetTitle(Form("Counts / %.4f V", binsize));
    h_amp->GetXaxis()->SetTitle("Amplitude [V]");
    h_amp->GetXaxis()->SetRangeUser(0,0.2);
    h_amp->GetYaxis()->SetRangeUser(-700,1000);
    h_amp->Draw();
    
    float initial_events = h_amp->Integral(0,nbins);
    float sub_events     = h_amp_dc->Integral(0,nbins);

    h_amp_sb->SetLineColor(kAzure-4);
    h_amp_sb->SetLineWidth(3);
    h_amp_sb->GetYaxis()->SetTitle(Form("Counts / %.4f V", binsize));
    h_amp_sb->GetXaxis()->SetTitle("Amplitude [V]");
    h_amp_sb->GetXaxis()->SetRangeUser(0,0.2);
 
    h_amp_sb->Add(h_amp_dc,-1);
    h_amp   ->Draw("histsame");
    h_amp_sb->Draw("histsame");

    float final_events = h_amp_sb->Integral(0,nbins);
    float ratio = final_events/initial_events;
    c->SaveAs(Form("plots/CD%d/%s/%dV/counts_subtract.png",CD_number,meas,volts[i]));

    //std::cout << "initial events:    " << initial_events << std::endl;
    //std::cout << "subtracted events: " << sub_events     << std::endl;
    //std::cout << "final events:      "    << final_events << std::endl;
    //std::cout << "ratio:             "    << ratio << std::endl;

    float real_rate     = rate[i]      * ratio;
    float real_rate_err = rate_error[i]* ratio;

    //std::cout << "rate:      " << rate[i]   << " +/- " << rate_error[i] << std::endl;
    //std::cout << "real rate: " << real_rate << " +/- " << real_rate_err << std::endl;

    newrate      [i]=real_rate;
    newrate_error[i]=real_rate_err;
    voltage_error[i]=0;
    voltage      [i]=volts[i];
   
    delete(h_amp);
    delete(h_amp_dc);
    delete(h_amp_sb);

  } // for sui voltaggi

  TGraphErrors* gr_rate    = new TGraphErrors(volts.size(),voltage,rate,voltage_error,rate_error);
  TGraphErrors* gr_ratenew = new TGraphErrors(volts.size(),voltage,newrate,voltage_error,newrate_error);
 

  double xmin = volts[0]-1;
  double xmax = volts[volts.size()-1]+1;
  double ymin = -2  ; double ymax = 150;
  
  TLegend* legend = new TLegend(0.15, 0.90, 0.42, 0.83);
  legend->SetBorderSize(0);
  legend->SetFillStyle(0);
  
  gr_rate->SetMarkerSize(2);
  gr_rate->SetLineWidth(2);
  gr_rate->SetMarkerStyle(20);
  gr_rate->SetLineColor(kRed-7);
  gr_rate->SetMarkerColor(kRed-7);
  legend->AddEntry(gr_rate,"old rate","p");
  
  gr_ratenew->SetMarkerSize(2);
  gr_ratenew->SetLineWidth(2);
  gr_ratenew->SetMarkerStyle(20);
  gr_ratenew->SetLineColor(kAzure+1);
  gr_ratenew->SetMarkerColor(kAzure+1);
  legend->AddEntry(gr_ratenew,"real rate","p");
  
  TH2D *h2_axes_g = new TH2D( "axes_g", "", 10, xmin, xmax, 10, ymin, ymax);
  h2_axes_g->SetXTitle( "Nanotube voltage #it{V}_{cnt} (V)" );
  h2_axes_g->SetYTitle( "Rate [Hz]" );
  h2_axes_g->GetYaxis()->SetTitleOffset(1.2);

  c->Clear();
  h2_axes_g->Draw("");
  gr_rate->Draw("pe same");
  gr_ratenew->Draw("pe same");
  legend->Draw("same");
   
  c->SaveAs(Form("plots/CD%d/%s/rate_cfr_old_new.png",CD_number,meas));
 
  return 0;
  
}

float GetScale(TH1F* h_amp, TH1F* h_amp_dc, float scale_min, float scale_max, int number_bins, float delta, float initial_bin){

  float scale = scale_min;
  int   nsteps = int((scale_max-scale_min)/delta);
  float chis[nsteps];

  //calcolo chi quadro
  for(int i=0; i<nsteps; i++){
    float chisquare = 0;
    for(int l=0; l<number_bins; l++){
      float expected =  h_amp   ->GetBinContent(initial_bin+l);
      float measured = (h_amp_dc->GetBinContent(initial_bin+l))*scale;
      chisquare+=(pow(expected-measured,2))/measured;
    }
    chis[i]=chisquare;
    scale+=delta;
  }

  //selezione primo minor chi square
  float best_chi   = 100000000000000000000000000.;
  float best_scale = 0;
  for(int i=0; i<nsteps; i++){
    //std::cout << "scale: " << scale_min+(i*delta)  << "  chi : " << chis[i] << std::endl;
    if(chis[i]<best_chi){
      best_scale=scale_min+(i*delta);
      best_chi = chis[i];
    }
    //std::cout << "best_scale: " << best_scale << std::endl;
    //std::cout << std::endl;
  }
  
  return(best_scale);
  
}



