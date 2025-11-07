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
  int   voltage   = (atoi(argv[2]));
  char* choice    =       argv[3]  ;
  int rescale     = (atoi(argv[4])); 
 
  //apertura file con il tree
  TFile run_tes1("data/root/CD204/B60_post_cond3_moku/100V/CD204_100V_tree.root");
 
  TTree *tree_tes1 = (TTree*)run_tes1.Get("tree");
  Long64_t nentries_tes1 = tree_tes1->GetEntries();

  //apertura file con il tree
  TFile run_tes2("data/root/CD222/F60_squidfilter_ER_031025_tr50/125V/CD222_125V_tree.root");
  //TFile run_tes2(Form("data/root/CD204/B60_post_cond3_moku/101V/CD204_101V_tree.root"));
  TTree *tree_tes2 = (TTree*)run_tes2.Get("tree");
  Long64_t nentries_tes2 = tree_tes2->GetEntries();

  //preparazione per la lettura del tree in amp o charge
  float variable;
  tree_tes1->SetBranchAddress(choice, &variable);
  tree_tes2->SetBranchAddress(choice, &variable);

  //impostazioni grafiche
  float magnifying = 1; //per avere numeri in carica più comodi (sono in E-6 sennò)
  float offset     = 0; //per allungare il range di disegno permesso wrt var_min/var_max
  float var_min    = 0; //bin minimo histo
  float var_max    = 0; //bin maximo histo
  if      (strcmp(choice,"amp"   )==0){ magnifying=1e+0; offset=0.05; var_min=0 ; var_max=2;}
  else if (strcmp(choice,"charge")==0){ magnifying=1e+6; offset=5   ; var_min=-10; var_max=20;}

  //fattore di scala tarato sul trigger
  float scale;
  float trigger_tes1;
  float trigger_tes2;
  tree_tes1->SetBranchAddress("trigger", &trigger_tes1);
  tree_tes2->SetBranchAddress("trigger", &trigger_tes2);
  tree_tes1->GetEntry(0); tree_tes2->GetEntry(0);
  if     (rescale == 0){ scale = 1.;}
  else if(rescale == 1){ scale = trigger_tes2/trigger_tes1; }
  scale = 0.2675/0.6175;
  //if(rescale == 1){ scale = 1.825; }

  //float off = trigger_tes2-trigger_tes1;
  //float off=0.35-0.035;
  //std::cout << off << std::endl;
  float off = 0;

  std::cout << "Rescale of a factor " << scale << std::endl;
  //variabili per l'istogramma
  int nbins = 1500;
  Settings settings;
  float binsize = settings.Binsize((var_max-var_min)/nbins);
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
  TH1F* histo1 = new TH1F("histo1", choice, nbins, var_min, var_max);
  TH1F* histo2 = new TH1F("histo2", choice, nbins, var_min, var_max);

  //ciclo su stesso numero di eventi
  int nentries = 0;
  if (nentries_tes1 >  nentries_tes2){nentries = nentries_tes2;}
  if (nentries_tes2 >  nentries_tes1){nentries = nentries_tes1;}
  if (nentries_tes2 == nentries_tes1){nentries = nentries_tes1;}

  for(int iEntry=0; iEntry<nentries; iEntry++){

    tree_tes1->GetEntry(iEntry);
    histo1->Fill(variable*magnifying*scale); //ampiezza riscalata

    tree_tes2->GetEntry(iEntry);
    for(int i=0; i<3; i++){
    histo2->Fill((variable-off)*magnifying);
    }
  }
  
  //tela
  TCanvas* c = new TCanvas("c","",1400,1200);
  settings.Margin(c);
  c->SetLeftMargin(0.155);
  c->cd();

  //range del disegno
  float range1_min = 0;
  float range2_min = 0;
  float range_max  = 0;

  if       (strcmp(choice,"amp"  )==0){
    if(rescale==0){ range1_min = 0.;}
    if(rescale==1){ range1_min = 0.;}
    range2_min =0.05; range_max = 0.3; }

  else if (strcmp(choice,"charge")==0){
    if(rescale==0){ range1_min = -1;}
    if(rescale==1){ range1_min = -1;}
    range2_min = -1; range_max = 8; }
  
   
  //disegno dell'histo con funzione definita in graphics.h
  settings.graphic_histo(histo1,range1_min,range_max,x_name,y_name.c_str(),voltage);
  settings.graphic_histo(histo2,range2_min,range_max,x_name,y_name.c_str(),voltage);
  settings.general_style();
  //gStyle->SetOptFit(0011);

  TLegend* legend = new TLegend(0.40, 0.9, 0.70, 0.82);
  legend->SetBorderSize(0);
  legend->SetFillStyle(0);
  
  histo1->SetMarkerSize(2);
  histo1->SetLineWidth(3);
  histo1->SetLineColor(46);
  histo1->SetFillStyle(3004);
  histo1->SetFillColor(46);
  legend->AddEntry(histo1,"trigger 12 mV","l");
 
  histo2->SetMarkerSize(2);
  histo2->SetLineWidth(3);
  histo2->SetLineColor(38);
  histo2->SetFillStyle(3004);
  histo2->SetFillColor(38);
  legend->AddEntry(histo2,"trigger 25 mV","l");
  //histo2->GetYaxis()->SetRangeUser(0,500);
  //histo2->GetXaxis()->SetRangeUser(0.5,1.05);
  
  histo1->Draw();
  histo2->Draw("same");
  legend->Draw("same");

  
  std::string outdir(Form("plots/CD222/F60_squidfilter_ER_031025_tr50/125V",CD_number,voltage));
  system( Form("mkdir -p %s", outdir.c_str()) );
  
  std::string attach;
  if(rescale==0){attach="raw";}
  if(rescale==1){attach="rescaled";}
  c->SaveAs(Form("%s/cfr_fisarmonica.png",outdir.c_str(),choice,voltage,attach.c_str()));
  

  delete(histo1);
  delete(histo2);
  delete(c);
  
  return(0);
}


//funzione di controllo sui parametri iniziali
void Input_Param_Ctrls(int argc, char* argv[]){
  //ctrl sull'input inserito
  if (argc!=5){
    std::cout << "USAGE: ./B60_H60 [CD_number] [voltage] [amp/charge] [rescaling]" << std::endl;
    std::cout << "[amp/charge]: variable studied" << std::endl;
    std::cout << "[rescale on]:1 [rescale off]:0" << std::endl;
    std::cout << "EXAMPLE: ./B60_H60 201 100 amp 0" << std::endl;
    exit(1);
  }

  //ctrl su scelta amp/charge
  if(strcmp(argv[3],"amp"   )!=0 &&
     strcmp(argv[3],"charge")!=0 ){ 
    std::cout << "Wrong choice! Choose between 'amp' and 'charge'" << std::endl;
    exit(1);
  }

  if(atoi(argv[3])!=0 && atoi(argv[3])!=1){
    std::cout << "Invalid choice! Choose 0 or 1 for rescaling!" << std::endl;
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


