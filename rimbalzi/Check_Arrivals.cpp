#include "TTree.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TStyle.h"
#include "TPad.h"
#include "TLegend.h"
#include "TH2F.h"

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
  if( argc!= 5 ) {
    std::cout << "USAGE: ./CheckAngles [folder] [subfolder] [filename] [offset]" << std::endl;
    std::cout << "EXAMPLE: ./CheckAngles pinhole test rimb_d600um_cnt15um_V30_E10-1 5" << std::endl;
    exit(1);
  }

  char* folder    =      argv[1];
  char* subfolder =      argv[2];
  char* name      =      argv[3];
  float quota     = atof(argv[4]);

  TLegend *legend = new TLegend(0.65,0.74,0.9,0.89);
  legend->SetBorderSize(0);
  
  TCanvas* c1 = new TCanvas("c","",1500,1500);
  c1->cd();
  
  //apertura file con il tree
  TFile run(Form("root/%s/%s/%s.root",folder,subfolder,name));
  TTree *tree = (TTree*)run.Get(Form("tree"));
  Long64_t nentries = tree->GetEntries();

  //angoli
  TH2F mapi("map","",50,-200,200,50,-10,100);
  TH1F ener("ener","",102,-1,100);
 
  //preparazione per la lettura del tree_raw
  float azim_i, elev_i, ki, x_f, y_f, z_f;
  int dist_gen;
  int unit = 5;
  
  tree->SetBranchAddress("azim_i", &azim_i);
  tree->SetBranchAddress("elev_i", &elev_i);
  tree->SetBranchAddress("dist_gen", &dist_gen);
  tree->SetBranchAddress("ke_i", &ki);
  tree->SetBranchAddress("x_f", &x_f);
  tree->SetBranchAddress("y_f", &y_f);
  tree->SetBranchAddress("z_f", &z_f);
  tree->GetEntry(0);
  
  for(int iEntry=0; iEntry<nentries; iEntry++){
    tree->GetEntry(iEntry);
    x_f*=unit;
    y_f*=unit;
    z_f*=unit;
    if(x_f == 110*unit){
      float distance = sqrt(y_f*y_f+z_f*z_f);
      if((distance > (dist_gen-quota)) && (distance < (dist_gen+quota))){
	mapi.Fill(azim_i*180/M_PI,elev_i*180/M_PI);
	ener.Fill(ki);
      }
    }
  }
  
  //gStyle->SetOptStat(0);
  
  std::string outdir( Form("plots/%s/%s",folder,subfolder));
  system( Form("mkdir -p %s", outdir.c_str()) );
  //-p crea anche le parent se prima non esistono

  c1->SetLeftMargin(0.15);
  c1->cd();
  mapi.Draw("axis");
  mapi.GetXaxis()->SetTitle("azimuth_i");
  mapi.GetYaxis()->SetTitle("elevation_i");
  mapi.Draw("COLZsame");
  c1->SaveAs(Form("%s/%s_distcheck_angles.png",outdir.c_str(),name));
  c1->Clear();

  ener.GetXaxis()->SetTitle("energy (eV)");
  ener.GetYaxis()->SetTitle("counts");
  ener.Draw();
  c1->SaveAs(Form("%s/%s_distcheck_ener.png",outdir.c_str(),name));
  c1->Clear();

  return(0);
}
