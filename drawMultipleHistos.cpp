#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdlib.h>

#include "TFile.h"
#include "TF1.h"
#include "TTree.h"
#include "TString.h"
#include "TH2D.h"
#include "TMath.h"
#include "TAxis.h"
#include "pshape_functions.h"
#include "graphics.h"
#include "AndCommon.h"
 
//variabili da modificare di volta in volta: range_min, range_max, fit_min, fit_max, nbins

Double_t Cruijff(Double_t *x, Double_t *par);

int main(void){ 

  //apertura file con il tree
  TFile run_tes1("data/root/CD188/conteggi/105V/CD188_105V_tree.root");
  TFile run_tes2("data/root/CD204/B60_post_cond3_moku/105V/CD204_105V_tree.root");
  TFile run_tes3("data/root/CD222/E100_squidfilter_ER_031025_tr12/127V/CD222_127V_tree.root");
 
  TTree *tree_tes1 = (TTree*)run_tes1.Get("tree");
  TTree *tree_tes2 = (TTree*)run_tes2.Get("tree");
  TTree *tree_tes3 = (TTree*)run_tes3.Get("tree");
  
  Long64_t nentries = 5000;

  //preparazione per la lettura del tree in amp o charge
  float variable;
  tree_tes1->SetBranchAddress("amp", &variable);
  tree_tes2->SetBranchAddress("amp", &variable);
  tree_tes3->SetBranchAddress("amp", &variable);

  //impostazioni grafiche
  float magnifying = 1; //per avere numeri in carica più comodi (sono in E-6 sennò)
  float offset     = 0.05; //per allungare il range di disegno permesso wrt var_min/var_max
  float var_min    = 0; //bin minimo histo
  float var_max    = 2; //bin maximo histo

  //fattore di scala tarato sul trigger
  float scale2 = 0.88/0.621;
  float scale3 = 0.88/0.1155;

  float off = 0;

  //variabili per l'istogramma
  int nbins = 200;
  Settings settings;
  float binsize = settings.Binsize((var_max-var_min)/nbins);
  const char* x_name = "Amplitude [V]"; //x axis label
  //std::string y_name =  "Counts / " + to_str(binsize) + " V" ; //y axis label 
  std::string y_name =  "Counts / 0.01 V" ; //y axis label 

  //creazione istogramma e lettura del tree
  TH1F* histo1 = new TH1F("histo1", "amp", nbins, var_min, var_max);
  TH1F* histo2 = new TH1F("histo2", "amp", nbins, var_min, var_max);
  TH1F* histo3 = new TH1F("histo3", "amp", nbins, var_min, var_max);

  for(int iEntry=0; iEntry<nentries; iEntry++){

    tree_tes1->GetEntry(iEntry);
    histo1->Fill(variable*magnifying); //ampiezza riscalata

    tree_tes2->GetEntry(iEntry);
    //for(int i=0; i<3; i++){
    histo2->Fill((variable)*magnifying*scale2);
    //}

    tree_tes3->GetEntry(iEntry);
    //for(int i=0; i<3; i++){
    histo3->Fill((variable)*magnifying*scale3);
    //}
  }
  
  //tela
  TCanvas* c = new TCanvas("c","",1400,1200);
  settings.Margin(c);
  c->SetLeftMargin(0.155);
  c->cd();

  //disegno dell'histo con funzione definita in graphics.h
  settings.graphic_histo(histo1,0,1,x_name,y_name.c_str(),105);
  settings.graphic_histo(histo2,0,1,x_name,y_name.c_str(),105);
  settings.graphic_histo(histo3,0,1,x_name,y_name.c_str(),127);
  settings.general_style();
  //gStyle->SetOptFit(0011);

  TLegend* legend = new TLegend(0.35,0.82,0.9,0.9);
  legend->SetTextSize(0.03);
  legend->SetMargin(0.1);
  legend->SetBorderSize(0);
  legend->SetFillStyle(0);
  
  histo1->SetMarkerSize(2);
  histo1->SetLineWidth(4);
  histo1->SetLineColor(46);
  histo1->SetFillStyle(3004);
  histo1->SetFillColor(46);
  //legend->AddEntry(histo1,"TES side: 100 um, CNT side: 3 mm","l");
 
  histo3->SetMarkerSize(2);
  histo3->SetLineWidth(4);
  histo3->SetLineColor(92);
  histo3->SetFillStyle(3002);
  histo3->SetFillColor(92);
  legend->AddEntry(histo3,"TES side: 100 um, CNT side: 1 mm","l");
  histo3->SetTitle("101 eV");

  histo2->SetMarkerSize(2);
  histo2->SetLineWidth(4);
  histo2->SetLineColor(38);
  histo2->SetFillStyle(3002);
  histo2->SetFillColor(38);
  legend->AddEntry(histo2,"TES side:   60 um, CNT side: 1 mm","l");
  
  histo3->Draw();
  histo2->Draw("same");
  //histo1->Draw("same");
  legend->Draw("same");

  c->SaveAs("plots/articolo2/histos_222_204.png");
  

  delete(histo1);
  delete(histo2);
  delete(histo3);
  delete(c);
  
  return(0);
}


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


