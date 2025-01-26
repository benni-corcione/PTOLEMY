#include "TTree.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TStyle.h"

#include <fstream>
#include <string>
#include <stdlib.h>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <iostream>
#include <cmath>


int main(int argc, char* argv[]){
  
  //parametri da input
  if( argc!= 4 ) {
    std::cout << "USAGE: ./HitsvsDistance [folder] [subfolder] [numero_sim]" << std::endl;
    std::cout << "EXAMPLE: ./HitsvsDistance pinhole test 1" << std::endl;
    exit(1);
  }

  char* folder    =      argv[1];
  char* subfolder =      argv[2];
  int   number    = atoi(argv[3]);   

  TCanvas* c1 = new TCanvas("c","",1000,1000);
  c1->cd();
  
  //apertura file con il tree
  TFile run(Form("root/%s/%s/%s_sim%d.root",folder,subfolder,subfolder, number));
  TTree *tree = (TTree*)run.Get(Form("tree"));
  Long64_t nentries = tree->GetEntries();
  
  //preparazione per la lettura del tree_raw
  float x_pos, y_pos, z_pos;
 
  tree->SetBranchAddress("x_f", &x_pos);
  tree->SetBranchAddress("y_f", &y_pos);
  tree->SetBranchAddress("z_f", &z_pos);

  //step di 10um
  int tes_side = 500; //um
  int unit = 10; //unit in simulation: 1 is 10 um
  int dist_max = 1000; //um
  int x_tes = 20*unit; //um

  TH1F* hits = new TH1F("hits","",10,0,dist_max);
  
  for(int iEntry=0; iEntry<nentries; iEntry++){
    tree->GetEntry(iEntry);
    x_pos *= unit;
    y_pos *= unit;
    z_pos *= unit;
    
    //canestro
    if(x_pos == x_tes &&
       y_pos<tes_side && y_pos>-tes_side &&
       z_pos<tes_side && z_pos>-tes_side){
      //calcolo distanza
      float distance = sqrt(y_pos*y_pos+z_pos*z_pos);
      hits->Fill(distance);
      //std::cout << distance << std::endl;
    }
  }

  //hits->SetMarkerStyle(8);
  //hits->SetMarkerSize(2);
  //hits->SetMarkerColor(kAzure+7);
  gStyle->SetOptStat(0);
  hits->SetLineWidth(3);
  hits->SetLineColor(kAzure+7);
  hits->GetYaxis()->SetTitle("Strikes");
  hits->GetXaxis()->SetTitle("Distance from TES [um]");
  hits->SetTitle("Strikes vs distance");
  hits->Draw();

  std::string outdir( Form("plots/%s/%s",folder,subfolder));
  system( Form("mkdir -p %s", outdir.c_str()) );
  //-p crea anche le parent se prima non esistono

  c1->SaveAs(Form("%s/%s_sim%d_hits_vs_dist.png",outdir.c_str(),subfolder,number));
  delete(c1);
  delete(hits);
  return(0);
}
