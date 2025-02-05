#include "TTree.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH2D.h"
#include "TStyle.h"
#include "TLine.h"
#include "TColor.h"

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
    std::cout << "USAGE: ./PositionMap [folder] [subfolder] [numero_sim]" << std::endl;
    std::cout << "EXAMPLE: ./PositionMap pinhole test 1" << std::endl;
    exit(1);
  }

  char* folder    =      argv[1];
  char* subfolder =      argv[2];
  int   number    = atoi(argv[3]);   

  std::string save_fold(Form("plots/%s",folder));
  std::string save_subfold(Form("plots/%s/%s",folder,subfolder));
  system( Form("mkdir -p %s", save_fold.c_str()) );
  system( Form("mkdir -p %s", save_subfold.c_str()));
  
  //apertura file con il tree
  TFile run(Form("root/%s/%s/%s_sim%d.root",folder,subfolder,subfolder, number));
  TTree *tree = (TTree*)run.Get(Form("tree"));
  Long64_t nentries = tree->GetEntries();
  
  //preparazione per la lettura del tree_raw
  int   event;
  float pos_x, pos_y, pos_z;
  float cnt_radius, distance;
  int   voltage;

  tree->SetBranchAddress("event", &event);
  tree->SetBranchAddress("x_f"  , &pos_x);
  tree->SetBranchAddress("y_f"  , &pos_y);
  tree->SetBranchAddress("z_f"  , &pos_z);
  tree->SetBranchAddress("cnt_radius", &cnt_radius);
  tree->SetBranchAddress("distance", &distance);
  tree->SetBranchAddress("voltage", &voltage);
 
  TCanvas* c1 = new TCanvas("c1","",1000,1000);
  c1->cd();
  
  int z_max = 3000; //um
  int y_max = 3000; //um
  int unit = 10; //1step in 10 um
  int tes_side = 60; // um
  int pixel_side = 30;
  int nbins = z_max/pixel_side;
  
  TH2F *map = new TH2F("map","",nbins,-y_max,y_max,nbins,-z_max,z_max);
  TLine* line1 =new TLine(-tes_side, -tes_side, -tes_side,  tes_side);
  TLine* line2 =new TLine(-tes_side,  tes_side,  tes_side,  tes_side);
  TLine* line3 =new TLine( tes_side,  tes_side,  tes_side, -tes_side);
  TLine* line4 =new TLine( tes_side, -tes_side, -tes_side, -tes_side);
  line1->SetLineWidth(2); line1->SetLineColor(kRed);
  line2->SetLineWidth(2); line2->SetLineColor(kRed);
  line3->SetLineWidth(2); line3->SetLineColor(kRed);
  line4->SetLineWidth(2); line4->SetLineColor(kRed);
  
  for(int iEntry=0; iEntry<nentries; iEntry++){
    tree->GetEntry(iEntry);
    //quantità in micron
    pos_x*=unit;  pos_y*=unit;  pos_z*=unit;
    
    //if(pos_x==1690){
      map->Fill(pos_y,pos_z);
      //}
  }//for sulle entries
   
  gStyle->SetPalette(kBird);
  TColor::InvertPalette();
   
  gStyle->SetOptStat(0);

  map->GetXaxis()->SetTitle("y axis (um)");
  map->GetYaxis()->SetTitle("z axis (um)");
  map->SetTitle("Arrival point of electrons");
  map->Draw("COLZ");
  c1->SetLeftMargin(0.15);
  c1->SetRightMargin(0.12);
  c1->SetBottomMargin(0.12);
  line1->Draw("same");
  line2->Draw("same");
  line3->Draw("same");
  line4->Draw("same");
  
  c1->SaveAs(Form("plots/%s/%s/%s_sim%d_map.png",folder,subfolder,subfolder,number));

  delete(c1);
  delete(map);
  run.Close();
  return(0);
}


