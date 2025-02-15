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

  TLegend *legend = new TLegend(0.75,0.84,0.9,0.89);
  legend->SetBorderSize(0);
  
  TCanvas* c1 = new TCanvas("c","",1500,1500);
  c1->cd();
  
  //apertura file con il tree
  TFile run(Form("root/%s/%s/%s.root",folder,subfolder,name));
  TTree *tree = (TTree*)run.Get(Form("tree"));
  Long64_t nentries = tree->GetEntries();

  //angoli
  TH2F *mapi = new TH2F("map","",38,-190,+190,20,-100,100);
  TH2F *mapf = new TH2F("map","",38,-190,+190,20,-100,100);

  TPad *pad1 = new TPad("pad1","up pad",0,0.51,1,1);
  pad1->Draw();
  TPad *pad2 = new TPad("pad2","down pad",0,0,1,0.52);
  pad2->Draw();
  
  //preparazione per la lettura del tree_raw
  float theta1_i, theta2_i;
  float theta1_f, theta2_f;
  tree->SetBranchAddress("theta1_i", &theta1_i);
  tree->SetBranchAddress("theta2_i", &theta2_i);
  tree->SetBranchAddress("theta1_f", &theta1_f);
  tree->SetBranchAddress("theta2_f", &theta2_f);
 
  TH1F* theta1_hist = new TH1F("theta1_hist","",100,-200,200);
  TH1F* theta2_hist = new TH1F("theta2_hist","",100,-200,200);
  TH1F* theta1f_hist = new TH1F("theta1f_hist","",100,-200,200);
  TH1F* theta2f_hist = new TH1F("theta2f_hist","",100,-200,200);
  
  for(int iEntry=0; iEntry<nentries; iEntry++){
    tree->GetEntry(iEntry);
    theta1_hist->Fill(theta1_i);
    theta2_hist->Fill(theta2_i);
    theta1f_hist->Fill(theta1_f);
    theta2f_hist->Fill(theta2_f);
    mapi->Fill(theta1_i,theta2_i);
    mapf->Fill(theta1_f,theta2_f);
  }

  /*
  gStyle->SetOptStat(0);
  theta1_hist->SetLineWidth(3);
  theta2_hist->SetLineWidth(3);
  theta1f_hist->SetLineWidth(3);
  theta2f_hist->SetLineWidth(3);
  theta1_hist->SetLineColor(kAzure+7);
  theta2_hist->SetLineColor(kPink);
  theta1f_hist->SetLineColor(kAzure+7);
  theta2f_hist->SetLineColor(kPink);
  theta1f_hist->SetLineStyle(2);
  theta2f_hist->SetLineStyle(2);
  
  theta2_hist->GetYaxis()->SetTitle("Counts");
  theta2_hist->GetXaxis()->SetTitle("Angles");
  theta2_hist->SetTitle("Check of uniformity -> initial angles");
  theta2_hist->Draw();
  theta1_hist->Draw("same");

  legend->AddEntry(theta1_hist,"althazimutale i", "l");
  legend->AddEntry(theta2_hist,"elevazione i", "l");
  
  legend->Draw("same");
  
  std::string outdir( Form("plots/%s/%s",folder,subfolder));
  system( Form("mkdir -p %s", outdir.c_str()) );
  //-p crea anche le parent se prima non esistono
  c1->SaveAs(Form("%s/%s_theta_check_i.png",outdir.c_str(),name));

 
  legend->Clear();
  
  legend->AddEntry(theta1f_hist,"althazimutale f", "l");
  legend->AddEntry(theta2f_hist,"elevazione f", "l");

  theta2f_hist->GetYaxis()->SetTitle("Counts");
  theta2f_hist->GetXaxis()->SetTitle("Angles");
  theta2f_hist->SetTitle("Check of uniformity -> final angles");
  theta2f_hist->Draw();
  theta1f_hist->Draw("same");
  legend->Draw("same");
  c1->SaveAs(Form("%s/%s_theta_check_f.png",outdir.c_str(),name));
  c1->Clear();
  */
  std::string outdir( Form("plots/%s/%s",folder,subfolder));
  system( Form("mkdir -p %s", outdir.c_str()) );
  //-p crea anche le parent se prima non esistono

  gStyle->SetOptStat(0);
  c1->SetRightMargin(0.18);
  c1->SetLeftMargin(0.18);

  gStyle->SetPalette(kBird);
  pad1->cd();
  mapi->Draw("axis");
  mapi->GetXaxis()->SetTitle("althazimutale");
  mapi->GetYaxis()->SetTitle("elevazione");
  mapi->Draw("COLZsame");
  pad2->cd();
  mapf->Draw("axis");
  mapf->GetXaxis()->SetTitle("althazimutale");
  mapf->GetYaxis()->SetTitle("elevazione");
  mapf->Draw("COLZsame");
  c1->SaveAs(Form("%s/%s_theta_check_map.png",outdir.c_str(),name));
  
  delete(c1);
  delete(theta1_hist);
  delete(theta2_hist);
  delete(mapi);
  delete(mapf);
  return(0);
}
