#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>

#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TMath.h"
#include "TGraph.h"
#include "pshape_functions.h"
#include "graphics.h"

//programma per studiare ampiezza vs carica degli eventi

int main(int argc, char* argv[]){

  //parametri da input
  if (argc!=4){
    std::cout << "USAGE: ./amp_charge [CD_number] [meas] [voltage]" << std::endl;
    exit(1);
  }

  int CD_number = (atoi(argv[1])); //cooldown
  int voltage   = (atoi(argv[3]));
  char* meas    =       argv[2]  ;

  //apertura file con il tree
  TFile run(Form("data/root/CD%d/%s/%dV/CD%d_%dV_tree.root",CD_number,meas,voltage,CD_number,voltage));
 
  TTree *tree = (TTree*)run.Get("tree");
  Long64_t nentries = tree->GetEntries();
 
  //preparazione per la lettura del tree
  float amp;
  float charge;
 
  tree->SetBranchAddress("charge", &charge );
  tree->SetBranchAddress("amp"   , &amp    );

  float x[nentries];
  float y[nentries];
  float magnifying = 1e+6; //cariche in unità comode
 
  //graphic options
  gStyle->SetFrameLineWidth(2);
  TCanvas* c = new TCanvas("c","",1400,1200);
  Settings settings;
  settings.Margin(c);
  c->cd();
  //c->SetLogy();
  
  //lettura del tree
  for(int iEntry=0; iEntry<nentries; iEntry++){
    tree->GetEntry(iEntry);
    y[iEntry] = amp;
    x[iEntry] = charge*magnifying;
  }

  TGraph *g = new TGraph(nentries,x,y);

  //range del fit lineare da osservare
  //float range_min = 2;
  //float range_max = 12;

  
  // g->Fit("pol1","","",range_min,range_max); 
  //g->GetFunction("pol1")->SetLineColor(kBlue-6);
  //g->GetFunction("pol1")->SetLineWidth(3);
  g->GetYaxis()->SetTitle("Amplitude [V]");
  g->GetXaxis()->SetTitle("Charge [uC]");
  g->GetXaxis()->SetLimits(-2,15);
  g->SetMinimum(0);
  g->SetMaximum(0.15);
  g->SetTitle(Form("%d V",voltage));
  g->SetMarkerSize(1.5);
  g->SetMarkerStyle(20);
  g->SetLineColor(kBlack);

  gStyle->SetTitleFontSize(0.07);
  //gStyle->SetOptFit(1111);

  g->Draw("AP");

  std::string outdir( Form("plots/CD%d/%s/%dV", CD_number, meas, voltage));
  system( Form("mkdir -p %s", outdir.c_str()) );
  //-p crea anche le parent se prima non esistono

  c->SaveAs(Form("%s/amp_charge.png",outdir.c_str()));
  
  delete(g);
  delete(c);
  
  return(0);
}
	       	       
