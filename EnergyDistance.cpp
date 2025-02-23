#include "TTree.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH2F.h"
#include "TStyle.h"
#include "TLine.h"
#include "TColor.h"
#include "TExec.h"
#include "TEllipse.h"
#include "TF1.h"
#include "TLegend.h"
#include "TMath.h"
#include "TGraphErrors.h"

#include <fstream>
#include <string>
#include <stdlib.h>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <iostream>
#include <cmath>

#include "sim_details.h"


Double_t Cruijff(Double_t *x, Double_t *par);
Double_t Landau(Double_t *x, Double_t *par);
Double_t CrystalBall(Double_t *x, Double_t *par);
float GetFitMin(int cnt_radius,int dist_gen);
float GetFitMax(int cnt_radius,int dist_gen);
int GetNbins(int cnt_radius,int dist_gen);
float GetYMax(int cnt_radius);
	  
int main(int argc, char* argv[]){
  
  //parametri da input
  if( argc!= 5 ) {
    std::cout << "USAGE: ./EnergyDistance [folder] [subfolder] [prefix] [cnt_radius]" << std::endl;
    std::cout << "cnt_radius to be expressed in um" << std::endl;
    std::cout << "EXAMPLE: ./EnergyDistance rimbalzi rimbalzi_3 rimb 15" << std::endl;
    exit(1);
  }
  
  char* folder     =      argv[1];
  char* subfolder  =      argv[2];
  char* prefix     =      argv[3];
  int   cnt_radius = atoi(argv[4]);

  std::string save_fold(Form("plots/%s",folder));
  std::string save_subfold(Form("plots/%s/%s",folder,subfolder));
  system( Form("mkdir -p %s", save_fold.c_str()) );
  system( Form("mkdir -p %s", save_subfold.c_str()));

  //preparazione per la lettura del tree_raw
  float pos_x , pos_y , pos_z ;
  float pos_xi, pos_yi, pos_zi;
  float ke_i, ke_f;

  //parametri standard non variati mai
  //int volts     = 100;
  //int distance  = 600; //um
  //int substrate = 4000; //raggio di 0.4 cm

  std::vector<int> dist_gen = GetDistGen(folder,subfolder);

  TCanvas* c1 = new TCanvas("c1","",2000,1000);
  TCanvas* c2 = new TCanvas("c2","",1000,1000);
  c1->cd();
  gStyle->SetOptStat(0);
  
  int unit       = 5; //1step in 5 um
  int tes_side   = 60; // um
  int x_tes      = 110*unit; 

  TGraphErrors gr_mu(0);
  TGraph gr_hits(0);
  TGraph gr_strikes(0);
  TGraph gr_strikes2(0);
  
  for(int dg=0; dg<dist_gen.size(); dg++){
	      
    //rimb_cnt15um_dist500um
    std::string name = std::string(prefix)+ "_cnt"
      + std::to_string(cnt_radius) + "um_dist"
      + std::to_string(dist_gen[dg]) + "um";
	      
    //apertura file con il tree
    TFile run(Form("root/%s/%s/%s.root",folder,subfolder,name.c_str()));
    if (!run.IsOpen()) {;}
    else{
		
      TTree *tree = (TTree*)run.Get(Form("tree"));
      Long64_t nentries = tree->GetEntries();
      tree->SetBranchAddress("x_f", &pos_x);
      tree->SetBranchAddress("y_f", &pos_y);
      tree->SetBranchAddress("z_f", &pos_z);
      tree->SetBranchAddress("x_i", &pos_xi);
      tree->SetBranchAddress("y_i", &pos_yi);
      tree->SetBranchAddress("z_i", &pos_zi);
      tree->SetBranchAddress("ke_i", &ke_i);
      tree->SetBranchAddress("ke_f", &ke_f);
	      
      int hits = 0;
      int discesa = 0;
      int nbins = GetNbins(cnt_radius,dist_gen[dg]);
      
	      
      TH1F* energy     = new TH1F("energy"    ,"",150,-0.5,100.5);
      TH1F* energy_fit = new TH1F("energy_fit","",nbins,-0.1,2  );
      TH1F* splat      = new TH1F("splat"     ,"",150,-0.5,100.5);

      for(int iEntry=0; iEntry<nentries; iEntry++){
	tree->GetEntry(iEntry);
	//quantità in micron
	pos_x *=unit;  pos_y *=unit;  pos_z *=unit;
	pos_xi*=unit;  pos_yi*=unit;  pos_zi*=unit;

	//canestro
	if(pos_x == x_tes){
	  discesa++;
	  if(pos_y<tes_side*0.5 && pos_y>-tes_side*0.5 &&
	     pos_z<tes_side*0.5 && pos_z>-tes_side*0.5 ){
	    hits++;
	    energy    ->Fill(ke_i);
	    energy_fit->Fill(ke_i);
	  }//if canestro
	}//if discesa

	if(pos_x != x_tes){
	  splat->Fill(ke_i);
	}

      }//for sulle entries

      gr_hits.SetPoint(dg,dist_gen[dg],discesa*100./nentries);
      gr_strikes.SetPoint(dg,dist_gen[dg],hits*100./nentries);
      gr_strikes2.SetPoint(dg,dist_gen[dg],hits*100./discesa);
      //std::cout << discesa << " " << hits << " " << nentries << std::endl;
	
      if(dg<3){ //raggio generazione<=150 um
	float fit_min = GetFitMin(cnt_radius,dist_gen[dg]);
	float fit_max = GetFitMax(cnt_radius,dist_gen[dg]);
	
	TF1 fitfunction("fitfunction",Cruijff,fit_min,fit_max,5);
	double par[5];
	fitfunction.SetParameters(par);
	fitfunction.SetLineColor(kRed-4);
	fitfunction.SetLineWidth(4);
	fitfunction.SetNpx(1000);
	
	fitfunction.SetParLimits(0,0.001,0.3);
	if(dg>1){ fitfunction.SetParLimits(0,0.2,0.5); }
	fitfunction.SetParLimits(1,0.01,1);
	if(dg==0){ fitfunction.SetParLimits(1,0.0001,0.1); }
	fitfunction.SetParLimits(2,0.001,0.2);
	fitfunction.SetParLimits(4, 10,10000);
	fitfunction.SetParLimits(3, 0.1,10);
	
	fitfunction.SetParameter(0, 0.1);
	fitfunction.SetParameter(1, 0.01);
	fitfunction.SetParameter(2, 0.1);
	fitfunction.SetParameter(3, 3);
	fitfunction.SetParameter(4, 10);

	
	energy_fit->Fit("fitfunction","RmX","",fit_min,fit_max);
	energy_fit->GetXaxis()->SetTitle("Energy (eV)");
	energy_fit->GetYaxis()->SetTitle("Counts");
	energy_fit->SetLineWidth(2);
	energy_fit->SetLineColor(53);
	gr_mu.SetPoint(dg,dist_gen[dg],fitfunction.GetParameter(0));
	gr_mu.SetPointError(dg,0,fitfunction.GetParError(0));
	energy_fit->Draw("same");
	c1->SaveAs(Form("plots/%s/%s/maps/%s_energy.png",folder,subfolder,name.c_str()));
	c1->Clear();
	
      }//if sul fit

      
      energy->GetXaxis()->SetTitle("Energy (eV)");
      energy->GetYaxis()->SetTitle("Counts");
      energy->SetLineColor(53);
      energy->SetLineWidth(2);
      energy->Draw();
      c1->SaveAs(Form("plots/%s/%s/maps/%s_energy_tot.png",folder,subfolder,name.c_str()));

      splat->SetTitle("Spectrum of splats on CNTs plate");
      splat->GetXaxis()->SetTitle("Energy (eV)");
      splat->GetYaxis()->SetTitle("Counts");
      splat->SetLineColor(53);
      splat->SetLineWidth(2);
      splat->Draw();
      c1->SaveAs(Form("plots/%s/%s/maps/%s_splat.png",folder,subfolder,name.c_str()));
      
    }//else sul file
  }//for dist_gen

  //punti comodi per gli assi
  int Npoints = gr_mu.GetN();
  gr_mu.SetPoint(Npoints,101,-50);
  gr_mu.SetPointError(Npoints,0,0);
  gr_mu.SetPoint(Npoints+1,-1,-50);
  gr_mu.SetPointError(Npoints+1,0,0);
	
  float ymax = GetYMax(cnt_radius);
  c2->cd();
  c2->SetLeftMargin(0.13);
  gr_mu.SetTitle("#mu");
  gr_mu.GetXaxis()->SetTitle("Initial distance");
  gr_mu.GetYaxis()->SetTitle("Energy peak [eV]");
  gr_mu.GetYaxis()->SetRangeUser(0,ymax);
  gr_mu.SetMarkerSize(3);
  gr_mu.SetLineWidth(4);
  gr_mu.SetMarkerStyle(20);
  gr_mu.SetMarkerColor(46);
  gr_mu.SetLineColor(46);
  gStyle->SetTitleFontSize(0.07);
  gr_mu.Draw("APE");
  
  c2->SaveAs(Form("plots/%s/%s/maps/energydist.png",folder,subfolder));
  c2->Clear();


  //punti comodi per gli assi
  int Npoints2 = gr_hits.GetN();
  gr_hits.SetPoint(Npoints2,-10,-10);
  
  gr_hits.SetTitle("Percentage of hits on the TES plate");
  gr_hits.GetXaxis()->SetTitle("Initial distance");
  gr_hits.GetYaxis()->SetTitle("% hits");
  gr_hits.GetYaxis()->SetRangeUser(0,100);
  gr_hits.GetXaxis()->SetRangeUser(-10,710);
  gr_hits.SetMarkerSize(2);
  gr_hits.SetLineWidth(4);
  gr_hits.SetMarkerStyle(20);
  gr_hits.SetMarkerColor(46);
  gr_hits.SetLineColor(46);
  gStyle->SetTitleFontSize(0.07);
  gr_hits.Draw("AP");
  
  c2->SaveAs(Form("plots/%s/%s/maps/energydist_hits.png",folder,subfolder));
  c2->Clear();

  gr_hits.GetYaxis()->SetRangeUser(15,25);
  gr_hits.Draw("AP");
  c2->SaveAs(Form("plots/%s/%s/maps/energydist_hitszoom.png",folder,subfolder));
  c2->Clear();
  
  gr_strikes.SetPoint(Npoints2,-10,-10);
  gr_strikes.GetXaxis()->SetRangeUser(-10,710);
  gr_strikes.SetTitle("Percentage of strikes on the TES");
  gr_strikes.GetXaxis()->SetTitle("Initial distance");
  gr_strikes.GetYaxis()->SetTitle("% strikes");
  gr_strikes.GetYaxis()->SetRangeUser(-5,100);
  gr_strikes.SetMarkerSize(2);
  gr_strikes.SetLineWidth(4);
  gr_strikes.SetMarkerStyle(20);
  gr_strikes.SetMarkerColor(46);
  gr_strikes.SetLineColor(46);
  gr_strikes2.SetMarkerSize(2);
  gr_strikes2.SetLineWidth(4);
  gr_strikes2.SetMarkerStyle(20);
  gr_strikes2.SetMarkerColor(91);
  gr_strikes2.SetLineColor(91);
  
  gStyle->SetTitleFontSize(0.07);
  gr_strikes.Draw("AP");
  gr_strikes2.Draw("samel");
  
  c2->SaveAs(Form("plots/%s/%s/maps/energydist_strikes.png",folder,subfolder));
  c2->Clear();

  gr_strikes.GetYaxis()->SetRangeUser(-0.1,3);
  gr_strikes.Draw("AP");
  gr_strikes2.Draw("samel");
  c2->SaveAs(Form("plots/%s/%s/maps/energydist_strikeszoom.png",folder,subfolder));
  c2->Clear();
  
  return(0);
}


Double_t CrystalBall(Double_t *x, Double_t *par) {
  // par[0] = media (mean)
  // par[1] = deviazione standard (sigma)
  // par[2] = parametro alpha (forma della coda)
  // par[3] = parametro n (decadimento della coda)
  // par[4] = parametro di normalizzazione per la parte Lorentziana
    
  //parte Lorentziana
  if (*x > par[0] + par[2] * par[1]) {
    return par[4] * TMath::Power((par[3] / TMath::Abs(par[2])) - (*x - par[0]) / par[1], -par[3]);
  }
  //parte Gaussiana
  else {
    return TMath::Exp(-0.5 * TMath::Power((*x - par[0]) / par[1], 2));
  }
}

Double_t Landau(Double_t *x, Double_t *par) {
  Double_t t = x[0];  // variabile di integrazione
  Double_t val = TMath::Exp(-t - par[0]*par[0] / (4.0 * t));  // Formula dell'integrale
  return val;
}


//cruiff (che può diventare una gaussiana) da fittare
Double_t Cruijff(Double_t *x, Double_t *par){
  //par[0]=mean      par[1]=sigma_L    par[2]=sigma_R
  //par[3]=alpha_R   par[4]=A          par[5]=alpha_L
  Double_t fitval = 0;
  Double_t arg_L  = 0;
  Double_t arg_R  = 0;
  Double_t num    = pow((x[0]-par[0]),2);
  Double_t denL   = 2*par[1]*par[1];
  Double_t denR   = 2*par[2]*par[2];
  Double_t asymR  = par[3]*num;
  Double_t asymL  = 0;  
  if ( denR + asymR != 0 ){ arg_R = -num/(denR+asymR); }
  if ( denL + asymL != 0 ){ arg_L = -num/(denL+asymL); }

  if       ( x[0] > par[0] ){ fitval = par[4] * exp(arg_R); } //LEFT
  else if  ( x[0] < par[0] ){ fitval = par[4] * exp(arg_L); } //RIGHT

  return(fitval);
}

float GetFitMin(int cnt_radius,int dist_gen){
  float fit_min=0;
  if(cnt_radius==15){
    fit_min = -0.1;
  }
  return fit_min;
}

float GetFitMax(int cnt_radius,int dist_gen){
  float fit_max=0;
  if(cnt_radius==15){
    if(dist_gen==0  ){ fit_max = 0.5; }
    if(dist_gen==50 ){ fit_max = 0.7; }
    if(dist_gen==100){ fit_max = 1.0; }
    if(dist_gen==150){ fit_max = 0.5; }
    if(dist_gen==200){ fit_max = 0.5; }
    if(dist_gen==250){ fit_max = 0.5; }
    if(dist_gen==300){ fit_max = 0.5; }
    if(dist_gen==350){ fit_max = 0.5; }
    if(dist_gen==400){ fit_max = 0.5; }
    if(dist_gen==450){ fit_max = 0.5; }
    if(dist_gen==500){ fit_max = 0.5; }
    if(dist_gen==550){ fit_max = 0.5; }
    if(dist_gen==600){ fit_max = 0.5; }
    if(dist_gen==650){ fit_max = 0.5; }
    if(dist_gen==700){ fit_max = 0.5; }
  }
  return fit_max;
}

int GetNbins(int cnt_radius,int dist_gen){
  int nbins=0;
  if(cnt_radius==15){
    if(dist_gen==0  ){ nbins = 100; }
    if(dist_gen==50 ){ nbins = 70; }
    if(dist_gen==100){ nbins = 40; }
    if(dist_gen==150){ nbins = 250; }
    if(dist_gen==200){ nbins = 250; }
    if(dist_gen==250){ nbins = 250; }
    if(dist_gen==300){ nbins = 250; }
    if(dist_gen==350){ nbins = 250; }
    if(dist_gen==400){ nbins = 250; }
    if(dist_gen==450){ nbins = 250; }
    if(dist_gen==500){ nbins = 250; }
    if(dist_gen==550){ nbins = 250; }
    if(dist_gen==600){ nbins = 250; }
    if(dist_gen==650){ nbins = 250; }
    if(dist_gen==700){ nbins = 250; }
  }
  return nbins;
}

float GetYMax(int cnt_radius){
  float ymax=0;
  if(cnt_radius==15){ ymax = 0.3; }

  return ymax;

}
   
