#include <iostream>
#include <stdlib.h>
#include <string>

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLine.h"
#include "TStyle.h"
#include "pshape_functions.h"

//servono sia evento sia lumi!
int main( int argc, char* argv[] ) {

  //parametri da input
  if( argc!= 4 ) {
    std::cout << "USAGE: ./persistence_plot [CD_number] [misura] [voltaggio]" << std::endl;
    exit(1);
  }

  int   CD_number = (atoi(argv[1]));
  char* meas      =       argv[2]  ;
  int   voltage   = (atoi(argv[3]));
  std::string fileName = "CD"+ std::to_string(CD_number)
                         + "_" + std::to_string(voltage)
                         + "V_treeraw.root";

  //preparazione lettura del tree
  TFile* file = TFile::Open(Form("data/root/CD%d/%s/%dV/%s",CD_number, meas, voltage, fileName.c_str()));
  TTree* tree = (TTree*)file->Get("treeraw");
  int nentries = tree->GetEntries();
  
  //TLegend *legend = new TLegend(0.8, 0.35, 1.1, 0.45);

  //settings tree 
  float pshape[2502];
  int   ev;
  int   lum;
  float sampling;

  tree->SetBranchAddress( "event" ,        &ev       );
  tree->SetBranchAddress( "lumi"  ,        &lum      );
  tree->SetBranchAddress( "pshape",        &pshape   );
  tree->SetBranchAddress( "sampling_time", &sampling );

  //preparazione grafica histo
  TCanvas* c1 = new TCanvas("c1", "c1", 3600, 1700); //1800 850
  c1->cd();

  tree->GetEntry(1);
  std::vector<TH1D*> histos;

  for(unsigned iEntry=0; iEntry<999; iEntry++){
    tree->GetEntry(iEntry);
    
    std::string histname(Form("%dentry", iEntry));
    TH1D* h1 = new TH1D( histname.c_str(), "",2500,0.,2500*sampling*1E+6);
    
    for(int i=0; i<2500; ++i ){
      h1->SetBinContent( i+1, pshape[i] );
    }
    histos.push_back(h1);

    h1->SetLineWidth(3);
    gStyle->SetOptStat(0);
    
    //histo 'a persistenza'
    h1->SetLineColorAlpha(kAzure+1, 0.01);
    //h1->SetLineColor(kAzure+1); 
    
    h1->GetXaxis()->SetTitle("Time (#mus)");
    h1->GetYaxis()->SetTitle("Amplitude (V)");
    h1->GetXaxis()->SetTitleSize(.05); //0.06 su singola, //0.08 su mini
    h1->GetYaxis()->SetTitleSize(.05);
    h1->GetXaxis()->SetLabelSize(0.04); //0.05 su singola, //0.07 su mini
    h1->GetYaxis()->SetLabelSize(0.04);
    h1->GetYaxis()->SetTitleOffset(+0.95);
    h1->GetXaxis()->SetTitleOffset(+0.9);
    h1->GetXaxis()->SetRangeUser(20,40);
    h1->GetYaxis()->SetRangeUser(-1.5,0.2);
    
    c1->SetBottomMargin(0.2);
    c1->SetLeftMargin(0.13);
    c1->SetRightMargin(0.04);
    //gStyle->SetFrameLineWidth(0.5);
    h1->Draw("l same");
    
    
  } 
   

  std::string outdir( Form("plots/CD%d/%s/%dV", CD_number,meas,voltage));
  system( Form("mkdir -p %s", outdir.c_str()) );
  //-p crea anche le parent se prima non esistono

  //pshapes
  c1->SaveAs(Form("%s/persistance_pshapes_zoom.png",outdir.c_str()));
  delete(c1);
 
  return 0;
}
