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
  char* choice    =       argv[4]  ;
  char* meas      =       argv[2]  ;
  int   nbins     = (atoi(argv[5]));
  float fit_min   = (atof(argv[6]));
  float fit_max   = (atof(argv[7]));
  float range_min = (atof(argv[8]));
  float range_max = (atof(argv[9]));
		     
  //apertura file con il tree
  TFile run(Form("data/root/CD%d/%s/%dV/CD%d_%dV_tree.root",CD_number,meas,voltage,CD_number,voltage));
  TTree *tree = (TTree*)run.Get("tree");
  Long64_t nentries = tree->GetEntries();

   
  //preparazione per la lettura del tree in amp o charge
  float variable;
  tree->SetBranchAddress(choice, &variable);

  //impostazioni grafiche
  float magnifying = 1; //per avere numeri in carica più comodi (sono in E-6 sennò)
  float histo_min=0;
  float histo_max=0;  
  if      (strcmp(choice,"amp"   )==0){ magnifying = 1e+0; histo_min=0.; histo_max=1.;}
  else if (strcmp(choice,"charge")==0){ magnifying = 1e+6; histo_min=0.; histo_max=30.;}

  //variabili per l'istogramma
  //float histo_min   = tree->GetMinimum(choice)*magnifying;
  //float histo_max   = tree->GetMaximum(choice)*magnifying;

  Settings settings;
  //funzione che tiene solo una cifra significativa per passare a stringa
  float binsize = settings.Binsize((histo_max-histo_min)/nbins); 

  const char* x_name; //x axis label
  std::string y_name; //y axis label 
  if      (strcmp(choice,"amp"   )==0){
    x_name = "Amplitude [V]";
    y_name = "Counts / " + to_str(binsize) + " V" ;
  }
  else if (strcmp(choice,"charge")==0){
    x_name = "Charge [uC]"  ;
    y_name = "Counts / " + to_str(binsize)  + " uC";
  }
  
  //creazione istogramma e lettura del tree
  TH1F* histo = new TH1F("histo", choice, nbins, histo_min, histo_max);
 
  for(int iEntry=0; iEntry<nentries; iEntry++){
    tree->GetEntry(iEntry);
    histo->Fill(variable*magnifying);
  }
  
  //fitting options
  //double fit_min = 0; double fit_max = 0;

  float     mu_min = 0; float     mu_max = 0; float     mu_ = 0;
  float sigmaL_min = 0; float sigmaL_max = 0; float sigmaL_ = 0;
  float sigmaR_min = 0; float sigmaR_max = 0; float sigmaR_ = 0;
  float   alfa_min = 0; float   alfa_max = 0; float   alfa_ = 0;
  float      A_min = 0; float      A_max = 0; float      A_ = 0;

  //inizializzazione parametri di fit
  //++++++++++++++cambiare fit_min e fit_max++++++++++++++
  //CARICA
  if(strcmp(choice,"charge")==0){
    //fit_min    = 8.5; fit_max    =9;
    A_min      = 1;
    A_max      = 5000;
    A_         = histo->Integral();
    mu_min     = 6.5;
    mu_max     = 9.5;
    mu_        = 8;
    sigmaL_min = 0.01;
    sigmaL_max = 20;
    sigmaL_    = 1;
    sigmaR_min = 0.010;
    sigmaR_max = 20;
    sigmaR_    = 1; 
  }

  //AMPIEZZA
  if(strcmp(choice,"amp")==0){
    //fit_min    = 0.615; fit_max    = 0.73;
    A_min      = 50;
    A_max      = 10000;
    A_         = histo->Integral();
    mu_min     = 0.17;
    mu_max     = 0.19;
    mu_        = 0.18;
    sigmaL_min = 0.001;
    sigmaL_max = 0.5;
    sigmaL_    = 0.01;
    sigmaR_min = 0.001;
    sigmaR_max = 0.5;
    sigmaR_    = 0.02;   
  }

 
  //funzione di fit
  //definita in fondo al programma
  TF1 *cruijff  = new TF1("fit_fcn", Cruijff, fit_min, fit_max, 5);

  //parametri del fit
  cruijff->SetParNames("#mu","#sigma_{L}", "#sigma_{R}", "#alpha", "A");
  cruijff->SetParLimits(0,    mu_min,    mu_max);
  cruijff->SetParLimits(1,sigmaL_min,sigmaL_max); 
  cruijff->SetParLimits(2,sigmaR_min,sigmaR_max);
  cruijff->SetParLimits(3,  alfa_min,  alfa_max);
  cruijff->SetParLimits(4,     A_min,     A_max);

  cruijff->SetParameters(mu_, sigmaL_, sigmaR_, alfa_, A_); //parametri iniziali

  //gaussiana asimmetrica e non cruijff
  cruijff->FixParameter(3,0); //alfa value
  cruijff->SetNpx(1000);

  //tela
  TCanvas* c = new TCanvas("c","",1400,1200);
  settings.Margin(c);
  c->SetLeftMargin(0.155);
  c->cd();

  histo->Fit("fit_fcn","rx"); //in range

  //disegno dell'histo con funzione definita in graphics.h
  settings.graphic_histo(histo,range_min,range_max,x_name,y_name.c_str(),voltage);
  settings.general_style();
  //gStyle->SetOptFit(0011);

  histo->SetMarkerSize(2);
  histo->SetLineWidth(3);
  histo->Draw(); //"pe" per avere points+errors

  cruijff->SetLineColor(kRed-4);
  cruijff->SetLineWidth(5);
  cruijff->Draw("same");

  std::string outdir( Form("plots/CD%d/%s/%dV/", CD_number, meas, voltage));
  system( Form("mkdir -p %s", outdir.c_str()) );
  
  c->SaveAs(Form("plots/CD%d/%s/%dV/%s_fit_%dbins.png",CD_number,meas, voltage, choice,nbins));
  std::cout << "cruijff P: " << cruijff->GetProb() << std::endl;

  delete(cruijff);
  delete(histo);
  delete(c);
  
  return(0);
}


//funzione di controllo sui parametri iniziali
void Input_Param_Ctrls(int argc, char* argv[]){
  //ctrl sull'input inserito
  if (argc!=10){
    std::cout << "USAGE: ./fitAmpChargeData [CD_number] [misura] [voltage] [amp/charge] [nbins] [fit_min] [fit_max] [draw_min] [draw_max]" << std::endl;
    std::cout << "[amp/charge]: variable studied" << std::endl;
    std::cout << "EXAMPLE: ./fitAmpChargeData 201 conteggi 100 amp 350 0.5 0.7 0 1" << std::endl;
    exit(1);
  }

  //ctrl su scelta amp/charge
  if(strcmp(argv[4],"amp"   )!=0 &&
     strcmp(argv[4],"charge")!=0 ){ 
    std::cout << "Wrong choice! Choose between 'amp' and 'charge'" << std::endl;
    exit(1);
  }
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


