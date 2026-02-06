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


int main( int argc, char* argv[] ) {

  //parametri da input
  if( argc!= 6 ) {
    std::cout << "USAGE: ./checkPulseShape [CD_number] [misura] [voltage] [which_ctrl] [ctrl_double]" << std::endl;
    std::cout << "ctrl_low : which == 0" << std::endl;
    std::cout << "ctrl_high: which == 1" << std::endl;
    std::cout << "ctrl_double: how many times the threshold has been overcome" << std::endl;
    exit(1);
  }

  int   CD_number = (atoi(argv[1]));
  char* misura    =       argv[2]  ;
  int   voltage   = (atoi(argv[3]));
  int   which     = (atoi(argv[4]));
  int   ctrl      = (atoi(argv[5]));
 
  //apertura file con il tree di variabili di alto livello
  TFile run_1(Form("data/root/CD%d/%s/%dV/CD%d_%dV_tree.root",CD_number,misura,voltage,CD_number,voltage));
  TTree *tree_full = (TTree*)run_1.Get("tree");
  Long64_t nentries = tree_full->GetEntries();

  //apertura file con il tree raw
  TFile run_2(Form("data/root/CD%d/%s/%dV/CD%d_%dV_treeraw.root",CD_number,misura,voltage,CD_number,voltage));
  TTree *tree_raw = (TTree*)run_2.Get("treeraw");

  //preparazione per la lettura dei tree
  int   event;
  int   lumi;
  int   ctrl_double;
  float trigger;
  float amp;
  float pshape[2502];

  tree_full->SetBranchAddress( "event"  , &event   );
  tree_full->SetBranchAddress( "lumi"   , &lumi    );
  tree_full->SetBranchAddress( "amp"    , &amp     );
  tree_full->SetBranchAddress( "trigger", &trigger );
  tree_raw ->SetBranchAddress( "pshape" , &pshape  );

  if      (which == 0){ tree_full->SetBranchAddress( "ctrl_double_low" , &ctrl_double ); }
  else if (which == 1){ tree_full->SetBranchAddress( "ctrl_double_high", &ctrl_double ); }

 
  //preparazione grafica histo
  TH1D* h1 = new TH1D("h1", "", 2500, 0., 2500. );
  TCanvas* c1 = new TCanvas("c1", "c1", 1800, 1000);
  TLegend* legend = new TLegend(0.75,0.2,0.9,0.12);
  gStyle->SetOptStat(0);
  h1->SetLineColor(kBlue-6);
  h1->SetLineWidth(4);
  c1->cd();

  //preparazione media dinamica
  //pietà fidatevi dell'algoritmo o provatelo a parte
  //spiegarlo è un macello
  int   j_max = 30; //quanti punti medio dinamicamente
  float dyn_mean;
  int   counter[2500];
  int   index[2500] = {0};

  //preparazione array media dinamica
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

  
  //lettura tree
  for(int n=0; n<nentries; n++){
    tree_full->GetEntry(n);
    
    if(ctrl_double == ctrl){
      tree_raw->GetEntry(n);
      
      TLine *threshold_1 = new TLine(0,-amp*0.3,2500,-amp*0.3);
      TLine *threshold_2 = new TLine(0,-amp*0.8,2500,-amp*0.8);
      for(int i=0; i<2500; ++i ){
	
	dyn_mean = 0;
	
	for(int j=0; j<=(int(j_max/2)+counter[i]);j++){
	  dyn_mean += pshape[j+i-int(j_max/2)+index[i]];
	}
	dyn_mean/=(int(j_max/2)+1+counter[i]);
	h1->SetBinContent(i,dyn_mean);
      }
      
      threshold_1->SetLineWidth(2);
      threshold_2->SetLineWidth(2);
      threshold_1->SetLineColor(kRed);
      threshold_2->SetLineColor(kBlue);

      legend->AddEntry(threshold_1,"30% amp","l");
      legend->AddEntry(threshold_2,"80% amp","l");
      legend->SetFillStyle(0);
      legend->SetBorderSize(0);

      h1         ->Draw("l"   );
      legend     ->Draw("same");
      threshold_1->Draw("same");
      threshold_2->Draw("same");

      std::string outdir( Form("plots/CD%d/%s/%dV/doubles", CD_number, misura,voltage));
      system( Form("mkdir -p %s", outdir.c_str()) );
      c1->SaveAs(Form("plots/CD%d/%s/%dV/doubles/%ddouble_event%dlumi%d.png",CD_number,misura,voltage,ctrl,event,lumi));
      c1->Clear();
      legend->Clear();

      delete(threshold_1);
      delete(threshold_2);
    }
  }
  
  delete(h1);
  delete(c1);
  delete(legend);
	    
  return 0;
}
