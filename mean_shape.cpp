#include <iostream>
#include <stdlib.h>
#include <string>

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLine.h"
#include "TLegend.h"
#include "pshape_functions.h"

//disegno di evento mediato dinamicamente con due soglie alte e basse
int main( int argc, char* argv[] ) {

  //parametri da input
  if( argc!= 4 ) {
    std::cout << "USAGE: ./checkPulseShape [CD_number] [voltage] [event]" << std::endl;
    exit(1);
  }

  int CD_number = (atoi(argv[1]));
  int voltage   = (atoi(argv[2]));
  int event     = (atoi(argv[3]));

  std::string fileName = "CD" + std::to_string(CD_number) + "_" + std::to_string(voltage) + "V_treeraw.root";

  //preparazione lettura del tree
  TFile* file = TFile::Open(Form("tree/%s", fileName.c_str()));
  TTree* tree = (TTree*)file->Get(Form("%dV_treeraw",voltage));
  int nentries = tree->GetEntries();

   if(event > nentries){
    std::cout << "There's no such an event" << std::endl;
    exit(1);
  }

  //settings tree
  int   ev;
  float pshape[2500];
  float amp;
  float baseline;
 
  tree->SetBranchAddress( "event"    , &ev );
  tree->SetBranchAddress( "pshape"   , &pshape );

  //histo
  TH1D* h1 = new TH1D("h1", "", 2500, 0., 2500. );
  TCanvas* c1 = new TCanvas("c1", "c1", 1800, 1000);
  TLegend* legend = new TLegend(0.75,0.2,0.9,0.12);
  c1->cd();

  //mean variables
  int   j_max = 40; //quanti punti medio dinamicamente
  float dyn_mean;
  int   counter[2500];
  int   index[2500] = {0};

  //preparazione array media dinamica
  //pietà non fatemi spiegare l'algoritmo
  //l'ho provato a mano e funziona, provare per credere
  for(int l=0; l<2500; l++){
    counter[l] = int(j_max/2);

    if(l < (     int(j_max/2)+1)){
      counter[l] -= (int(j_max/2)-l);
      index[l]   += (int(j_max/2)-l);
    }
    if(l > (2500-int(j_max/2)-1)){
      counter[l] -= (int(j_max/2)+l-2499);
    }
  }
  
  for( unsigned iEntry=0; iEntry<nentries; ++iEntry ) {
    tree->GetEntry(iEntry);
    
    if( ev==event ) {
      
      baseline = GetBaseline(pshape);
      amp = GetAmp(pshape,baseline);

      for(int i=0; i<2500; i++){
	dyn_mean = 0;
	
	for(int j=0; j<=(int(j_max/2)+counter[i]);j++){
	  dyn_mean += pshape[j+i-int(j_max/2)+index[i]];
	}
	dyn_mean/=(int(j_max/2)+1+counter[i]);
	h1->SetBinContent(i,dyn_mean);
      }
    }
  }
  
  //linee di soglia
  TLine *threshold_1 = new TLine(0,-amp*0.3,2500,-amp*0.3);
  TLine *threshold_2 = new TLine(0,-amp*0.8,2500,-amp*0.8);
  threshold_1->SetLineWidth(2);
  threshold_2->SetLineWidth(2);
  threshold_1->SetLineColor(kRed);
  threshold_2->SetLineColor(kBlue);

  //stile histo
  gStyle->SetOptStat(0);
  h1->SetLineColor(kBlue-6);
  h1->SetLineWidth(2);
  legend->AddEntry(threshold_1,"30% amp","l");
  legend->AddEntry(threshold_2,"80% amp","l");
  legend->SetFillStyle(0);
  legend->SetBorderSize(0);
  
  h1->Draw("l");
  legend->Draw("same");
  threshold_1->Draw("same");
  threshold_2->Draw("same");
  
  c1->SaveAs(Form("CD %d/%d V/dyn_mean_event%d_%dpoints.png",CD_number,voltage,event,j_max));
  
  delete(h1);
  delete(c1);
   
}


  
 
