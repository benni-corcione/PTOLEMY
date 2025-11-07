#include <iostream>
#include <fstream>
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

int main(int argc, char* argv[]){

  //parametri da input
  if (argc!=7){
    std::cout << "USAGE: ./draw_ctrls [CD_number] [misura] [voltage] [ctrls listed below]" << std::endl;
    std::cout << "[double] [width] [charge]" << std::endl;
    std::cout << "0: ctrl spento    1: ctrl acceso" << std::endl;
    exit(1);
  }

  int CD_number   = (atoi(argv[1]));
  char* meas      =       argv[2]  ;
  int voltage     = (atoi(argv[3]));
  int doubles     = (atoi(argv[4]));
  int width       = (atoi(argv[5]));
  int ctrl_charge = (atoi(argv[6]));

  std::string which;
  if(doubles    ==1){which = "double";}
  if(width      ==1){which = "width" ;}
  if(ctrl_charge==1){which = "charge";}
  //apertura file con il tree
  TFile run(Form("data/root/CD%d/%s/%dV/CD%d_%dV_tree.root",CD_number,meas,voltage,CD_number,voltage));
  TTree *tree = (TTree*)run.Get("tree");
 
  //apertura file con il tree_raw
  TFile file(Form("data/root/CD%d/%s/%dV/CD%d_%dV_treeraw.root",CD_number,meas,voltage,CD_number,voltage));
  TTree* treeraw = (TTree*)file.Get("treeraw");
  int nentries = treeraw->GetEntries();
 
  //preparazione per la lettura dei tree
  float pshape[2502];
  int   event;
  int   lumi;
  float charge;
  int   ctrl_double;
  int   ctrl_width;

  tree   ->SetBranchAddress("event"       , &event       );
  treeraw->SetBranchAddress("lumi"        , &lumi        );
  treeraw->SetBranchAddress("pshape"      , &pshape      );
  tree   ->SetBranchAddress("charge"      , &charge      );
  tree   ->SetBranchAddress("ctrl_double" , &ctrl_double );
  tree   ->SetBranchAddress("ctrl_width"  , &ctrl_width  );

  
  //preparazione grafica histo
  TCanvas* c1 = new TCanvas("c1", "c1", 1800, 1000);
  c1->cd();

  gStyle->SetOptStat(0);
  int counter = 0;
  
  for(unsigned iEntry=0; iEntry<nentries; iEntry++){
 
    tree->GetEntry(iEntry);
   
      //seleziono solo le forme d'onda che falliscono i controlli di sicurezza
    if ((doubles     == 1 && ctrl_double > 1) ||
        (width       == 1 && (ctrl_width < 60 || ctrl_width>280)) ||
	(ctrl_charge == 1 && charge>8E-06)) {

      counter++;
      treeraw->GetEntry(iEntry);
      
      TH1D* h1 = new TH1D("h1", "", 2500, 0., 2500.);
      TH1D* h1_smooth = new TH1D("h1_smooth", "", 2500, 0., 2500.);

      float smooth[2500]={0};
      DynamicMean(pshape,smooth,10);
  
      for(int i=0; i<2500; ++i ){
	h1->SetBinContent(i+1, pshape[i] );
	h1_smooth->SetBinContent(i+1, smooth[i] );
      } //for pshape

      std::cout << "event:" << event << "  lumi:" << lumi
		<< "  waves:" << ctrl_double
	        << "  width:" << ctrl_width
		<< "  charge:" << charge*1E+6 << std::endl;
      
      h1->GetYaxis()->SetRangeUser(-0.05,0.005);
      h1->SetLineColor(kBlue+2);
      h1_smooth->SetLineColor(kBlue-9);
      h1->SetLineWidth(5);
      h1_smooth->SetLineWidth(3);
      
      h1->Draw("l");
      h1_smooth->Draw("l same");
      
      std::string outdir( Form("plots/CD%d/%s/%dV/ctrls", CD_number, meas, voltage));
      system( Form("mkdir -p %s", outdir.c_str()) );
      c1->SaveAs(Form("%s/%d_%s_event%d_lumi%d.png",outdir.c_str(),ctrl_double,which.c_str(),event,lumi));
      delete(h1);
      
     }//if event
    
  } //for tree
  
  //stile histo
  
  
  delete(c1);

  std::cout << "eventi triggeranti: " << counter << std::endl;
  return 0;
}
 

//intervalli di confidenza a occhio
//CD204 B60_post_cond3_moku
//96-99 --> 60-250
//100-108 --> 60-260
//109 --> 60-270
//110 --> 60-280

//CD188 conteggi
//95 
