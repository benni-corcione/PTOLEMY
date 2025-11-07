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
  if( argc!= 4 ) {
    std::cout << "USAGE: ./CheckAngles [folder] [subfolder] [filename]" << std::endl;
    std::cout << "EXAMPLE: ./CheckAngles pinhole test rimb_d600um_cnt15um_V30_E10-1" << std::endl;
    exit(1);
  }

  char* folder    = argv[1];
  char* subfolder = argv[2];
  char* name      = argv[3];   

  TLegend *legend = new TLegend(0.65,0.74,0.9,0.89);
  legend->SetBorderSize(0);
  
  TCanvas* c1 = new TCanvas("c","",1500,1500);
  c1->cd();
  
  //apertura file con il tree
  TFile run(Form("root/%s/%s/%s.root",folder,subfolder,name));
  TTree *tree = (TTree*)run.Get(Form("tree"));
  Long64_t nentries = tree->GetEntries();

  //angoli
  TH2F mapi("map","",200,-200,200,100,-100,100);
  TH2F mapf("map","",200,-200,200,100,-100,100);
 
  //preparazione per la lettura del tree_raw
  float azim_i, azim_f, elev_i, elev_f;
  tree->SetBranchAddress("azim_i", &azim_i);
  tree->SetBranchAddress("azim_f", &azim_f);
  tree->SetBranchAddress("elev_i", &elev_i);
  tree->SetBranchAddress("elev_f", &elev_f);
  
  TH1F* sen_el = new TH1F("sen_el","",50,-0.5,+1.5);
  TH1F* hazim_i = new TH1F("hazim_i","",50,-3,+3);
  TH1F* helev_i = new TH1F("helev_i","",50,-4,+4);
 
  for(int iEntry=0; iEntry<nentries; iEntry++){
    tree->GetEntry(iEntry);
 
    hazim_i->Fill(azim_i); //azim
    helev_i->Fill(elev_i); //elev
    sen_el->Fill(sin(elev_i));
    mapi.Fill(azim_i*180/M_PI,elev_i*180/M_PI);
    mapf.Fill(azim_f*180/M_PI,elev_f*180/M_PI);
  }

  gStyle->SetOptStat(0);
  
  hazim_i->SetLineWidth(4);
  helev_i->SetLineWidth(4);
  hazim_i->SetLineColor(kAzure+7);
  helev_i->SetLineColor(kPink);
  helev_i->GetYaxis()->SetTitle("Counts");
  helev_i->GetXaxis()->SetTitle("Angles");
  helev_i->SetTitle("Check of uniformity -> initial angles");
  
  legend->AddEntry(hazim_i,"althazimutale i", "l");
  legend->AddEntry(helev_i,"elevazione i", "l");
  
  helev_i->Draw();
  hazim_i->Draw("same");
  legend->Draw("same");
  
  std::string outdir( Form("plots/%s/%s",folder,subfolder));
  system( Form("mkdir -p %s", outdir.c_str()) );
  //-p crea anche le parent se prima non esistono
  c1->SaveAs(Form("%s/%s_theta_check_i.png",outdir.c_str(),name));
  c1->Clear();
  legend->Clear();
  
  gStyle->SetOptStat(0);
  sen_el->SetLineWidth(3);
  sen_el->GetYaxis()->SetTitle("Counts");
  sen_el->GetXaxis()->SetTitle("sin(theta)");
  sen_el->SetTitle("Check of uniformity -> initial angles");
  sen_el->Draw();
  c1->SaveAs(Form("%s/%s_senel.png",outdir.c_str(),name));
  c1->Clear();
  legend->Clear();

  c1->SetRightMargin(0.18);
  c1->SetLeftMargin(0.18);

  gStyle->SetPalette(kBird);

  c1->cd();
  TPad pad1("pad1","up pad",0,0.51,1,1);
  pad1.Draw();
  TPad pad2("pad2","down pad",0,0,1,0.52);
  pad2.Draw();
  
  pad1.cd();
  
  mapi.Draw("axis");
  mapi.GetXaxis()->SetTitle("azimuth_i");
  mapi.GetYaxis()->SetTitle("elevation_i");
  mapi.Draw("COLZsame");
 
  pad2.cd();
  mapf.Draw("axis");
  mapf.GetXaxis()->SetTitle("azimuth_f");
  mapf.GetYaxis()->SetTitle("elevation_f");
  mapf.Draw("COLZsame");
  
  c1->SaveAs(Form("%s/%s_theta_check_map.png",outdir.c_str(),name));
   
  //delete(c1);
  delete(sen_el);
  return(0);
}
