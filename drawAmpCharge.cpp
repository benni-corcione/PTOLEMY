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

void Input_Param_Ctrls(int argc, char* argv[]);
Double_t Cruijff(Double_t *x, Double_t *par);

int main(int argc, char* argv[]){ 

  //controlli iniziali su variabili da input
  Input_Param_Ctrls(argc,argv);

  //parametri da input
  int   CD_number = (atoi(argv[1])); //cooldown
  int   voltage   = (atoi(argv[3]));
  char* meas      =       argv[2]  ;

  
  //apertura file con il tree
  TFile run(Form("data/root/CD%d/%s/%dV/CD%d_%dV_tree.root",CD_number,meas,voltage,CD_number,voltage));
  TTree *tree = (TTree*)run.Get("tree");
  Long64_t nentries = tree->GetEntries();

   
  //preparazione per la lettura del tree in amp o charge
  float amp;
  float charge;
  int nbins_amp = 700;
  int nbins_cha = 500;
  tree->SetBranchAddress("amp"   , &amp   );
  tree->SetBranchAddress("charge", &charge);
  
  //variabili per l'istogramma
  //float histo_min   = tree->GetMinimum(choice)*magnifying;
  //float histo_max   = tree->GetMaximum(choice)*magnifying;

  Settings settings;
  //funzione che tiene solo una cifra significativa per passare a stringa
  float binsize_amp = settings.Binsize(1./nbins_amp);
  float binsize_cha = settings.Binsize(20./nbins_cha); 

  //x and y labels
  const char* x_amp = "Amplitude [V]";
  std::string y_amp = "Counts / " + to_str(binsize_amp) + " V" ;
  const char* x_cha = "Charge [uC]"  ;
  std::string y_cha = "Counts / " + to_str(binsize_cha)  + " uC";
  
  //creazione istogramma e lettura del tree
  TH1F* h_amp= new TH1F("h_amp", "Amplitude", nbins_amp, 0, 1 );
  TH1F* h_cha= new TH1F("h_cha", "Charge"   , nbins_cha, 0, 20);
 
  for(int iEntry=0; iEntry<nentries; iEntry++){
    tree->GetEntry(iEntry);
    h_amp->Fill(amp);
    h_cha->Fill(charge*1E+6);
  }
  
  //tela
  TCanvas* c = new TCanvas("c","",1400,1200);
  settings.Margin(c);
  c->SetLeftMargin(0.155);
  c->cd();

  //disegno dell'histo con funzione definita in graphics.h
  settings.graphic_histo(h_amp,0.37,0.65,x_amp,y_amp.c_str(),voltage);
  settings.graphic_histo(h_cha,0,10,x_cha,y_cha.c_str(),voltage);
  settings.general_style();
  //gStyle->SetOptFit(0011);

  h_amp->SetMarkerSize(2);
  h_amp->SetLineWidth(3);
  h_amp->Draw(); //"pe" per avere points+errors

  std::string outdir( Form("plots/CD%d/%s/%dV/", CD_number, meas, voltage));
  system( Form("mkdir -p %s", outdir.c_str()) );
  
  c->SaveAs(Form("plots/CD%d/%s/%dV/amp_%dbins.png",CD_number,meas,voltage,nbins_amp));

  h_cha->SetMarkerSize(2);
  h_cha->SetLineWidth(3);
  h_cha->Draw(); //"pe" per avere points+errors

  c->SaveAs(Form("plots/CD%d/%s/%dV/charge_%dbins.png",CD_number,meas,voltage,nbins_cha));
  
  delete(h_amp);
  delete(h_cha);
  delete(c);
  
  return(0);
}


//funzione di controllo sui parametri iniziali
void Input_Param_Ctrls(int argc, char* argv[]){
  //ctrl sull'input inserito
  if (argc!=4){
    std::cout << "USAGE: ./fitAmpChargeData [CD_number] [misura] [voltage]" << std::endl;
    std::cout << "EXAMPLE: ./fitAmpChargeData 201 conteggi 100" << std::endl;
    exit(1);
  }

}

