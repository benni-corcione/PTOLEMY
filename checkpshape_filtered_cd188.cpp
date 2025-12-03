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

//servono sia evento sia lumi!
int main( int argc, char* argv[] ) {

  //parametri da input
  if( argc!= 4 ) {
    std::cout << "USAGE: ./checkPulseShape [voltage] [event] [lumi]" << std::endl;
    exit(1);
  }

  int CD_number = 188;
  int voltage   = (atoi(argv[1]));
  int event     = (atoi(argv[2]));
  int lumi      = (atoi(argv[3]));
  const char* meas = "conteggi";

  
  std::string fileName = "CD"+ std::to_string(CD_number)
                       + "_" + std::to_string(voltage) + "V_treeraw.root";

  //preparazione lettura del tree
  TFile* file = TFile::Open(Form("data/root/CD%d/%s/%dV/%s",CD_number, meas, voltage, fileName.c_str())); 
  TTree* tree = (TTree*)file->Get("treeraw");
  int nentries = tree->GetEntries();
  
  TLegend *legend = new TLegend(0.8, 0.35, 1.1, 0.45);

   if(event > nentries){
    std::cout << "There's no such an event" << std::endl;
    exit(1);
  }

  //settings tree 
  float pshape[2502];
  float pshape_filtered_100kHz[2502];
  float pshape_filtered_10kHz[2502];
  int   ev;
  int   lum;
  float sampling;

  tree->SetBranchAddress( "event"          , &ev              );
  tree->SetBranchAddress( "lumi"           , &lum             );
  tree->SetBranchAddress( "pshape"         , &pshape          );
  tree->SetBranchAddress( "pshape_filtered_100kHz", &pshape_filtered_100kHz);
  tree->SetBranchAddress( "pshape_filtered_10kHz", &pshape_filtered_10kHz);
  tree->SetBranchAddress( "sampling_time"  , &sampling        );

  //preparazione grafica histo
  TCanvas* c1 = new TCanvas("c1", "c1", 1800, 850); //1800 850
  c1->cd();

  //check se l'evento esiste
  int check = 0;

  for(unsigned iEntry=0; iEntry<nentries; iEntry++){
    tree->GetEntry(iEntry);

    if(ev==event && lum==lumi){
      TH1D* h1 = new TH1D("h1", "", 2500,0.,2500*sampling*1.E+6);
      TH1D* h2 = new TH1D("h2", "", 2500,0.,2500*sampling*1.E+6);
      TH1D* h3 = new TH1D("h3", "", 2500,0.,2500*sampling*1.E+6);
      check++;
      
      for(int i=0; i<2500; ++i ){
	h1->SetBinContent( i+1, pshape[i] );
	h2->SetBinContent( i+1, pshape_filtered_100kHz[i] );
    h3->SetBinContent( i+1, pshape_filtered_10kHz[i] );
      }
      gStyle->SetOptStat(0);
      h1->SetLineColor(kAzure+5);
      h2->SetLineColor(kRed-7);
      h3->SetLineColor(kPink-5);
      h1->SetLineWidth(3);
      h2->SetLineWidth(3);
      h3->SetLineWidth(3);
      h1->GetXaxis()->SetTitle("Time (#mus)");
      h1->GetYaxis()->SetTitle("Amplitude (V)");
      h1->GetXaxis()->SetTitleSize(.07); //0.06 su singola, //0.08 su mini
      h1->GetYaxis()->SetTitleSize(.07);
      h1->GetXaxis()->SetLabelSize(0.08); //0.05 su singola, //0.07 su mini
      h1->GetYaxis()->SetLabelSize(0.08);
      h1->GetYaxis()->SetTitleOffset(+0.95);
      h1->GetXaxis()->SetTitleOffset(+0.9);
      //h1->GetXaxis()->SetRangeUser(23.1,27);
      h1->GetYaxis()->SetRangeUser(-1,0.05);

      
      c1->SetBottomMargin(0.2);
      c1->SetLeftMargin(0.13);
      c1->SetRightMargin(0.04);
      gStyle->SetFrameLineWidth(0.5);
      h1->Draw("l same");
      h2->Draw("l same");
      h3->Draw("l same");
    }
  } 
  
  if(check==0){
    std::cout << "The event does not exist!" << std::endl;
    exit(1);
  }

  std::string outdir( Form("plots/CD%d/%s/%dV", CD_number, meas, voltage));
  system( Form("mkdir -p %s", outdir.c_str()) );
  //-p crea anche le parent se prima non esistono

  c1->SaveAs(Form("%s/event%dlumi%d_filter100kHzand10kHz.png",outdir.c_str(),event,lumi));
  

  delete(c1);
	    
  return 0;
}
