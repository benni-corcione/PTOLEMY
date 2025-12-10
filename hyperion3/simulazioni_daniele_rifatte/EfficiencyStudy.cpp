#include "TTree.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH2F.h"
#include "TStyle.h"
#include "TLine.h"
#include "TColor.h"
#include "TExec.h"
#include "TEllipse.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TF1.h"
#include "TLegend.h"
#include "TMath.h"

#include <fstream>
#include <string>
#include <stdlib.h>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <random>

int main(int argc, char* argv[]){
  
  std::string save_fold("plots");
  system( Form("mkdir -p %s", save_fold.c_str()) );

  //preparazione per la lettura del tree
  float x_f, y_f, z_f;
  int   V1, V2;

  std::srand(std::time(0)); // inizializza il seme
    
  TCanvas* c1 = new TCanvas("c1","",1100,1100);
  c1->SetRightMargin(0.155);
  c1->SetTopMargin(0.155);
  c1->SetBottomMargin(0.155);
  c1->SetLeftMargin(0.155);
  c1->cd();
  
  int x_sdd      = 150;
  int side_sdd   = 16; //gu lato di 8 mm
  float unit     = 0.5;    //1 step in 0.5 um
  int nsteps     = 100; //number of voltages tested
  int V_start    = -5000;
  int V_step     = 50;
  int V_stop     = -50;
  int nbins      = 100; //quadratini su V1 axis e V2 axis
  int bin_width  = 50;
 
  
  // Bordi dei bin per X e Y
  double* zBins = new double[nbins+1];
  double* yBins = new double[nbins+1];
  for (int i = 0; i <=nbins; ++i) {
    double center = V_start + i*V_step;
    zBins[i] = center - bin_width/2;
    yBins[i] = center - bin_width/2;
   
  }
    
  TH2F* map = new TH2F("map","",nbins,yBins,nbins,zBins);
	    
  for(int iter1 = 0; iter1<nsteps; iter1++){
    if(iter1%10==0) std::cout << "step: " << iter1 << std::endl;
    for(int iter2 = 0; iter2<nsteps; iter2++){
      V1 = V_start + iter1 * V_step;
      V2 = V_start + iter2 * V_step;


      
      std::string name = "simdaniele_V1_" + std::to_string(V_start + iter1 * V_step) +
	+ "_V2_" + std::to_string(V_start + iter2 * V_step) + ".root";
      
      //apertura file con il tree
      TFile run(Form("root/%s",name.c_str()));
      if (!run.IsOpen()) {;}
      
      else{
	
	TTree *tree = (TTree*)run.Get(Form("tree"));
	Long64_t nentries = tree->GetEntries();
	tree->SetBranchAddress("x_f", &x_f);
	tree->SetBranchAddress("y_f", &y_f);
	tree->SetBranchAddress("z_f", &z_f);
	tree->SetBranchAddress("V1" , &V1   );
	tree->SetBranchAddress("V2" , &V2   );
	
	if ( V1!= (V_start + iter1 * V_step))
	  { std::cout << "V1 disallineato" << std::endl; exit(1); }
	
	if ( V2!= (V_start + iter2 * V_step))
	  { std::cout << "V2 disallineato" << std::endl; exit(1); }
      
	int nhits = 0;
	float ntot = 5000;
      
	   
	for(int iEntry=0; iEntry<nentries; iEntry++){
	  tree->GetEntry(iEntry);
	  if(x_f == 150){
	    if(abs(y_f) < side_sdd/2 && abs(z_f) < side_sdd/2){
	      nhits++; }
	  }
	}
      	
     float efficiency = nhits/ntot;
	 //float efficiency = static_cast<double>(std::rand()) / RAND_MAX; // [0,1]
       
     if(efficiency == 0){efficiency += 0.001;}

	map -> SetBinContent( map->GetXaxis()->FindBin(V1),
			      map->GetYaxis()->FindBin(V2),
			      efficiency);
	}//else sul file
    } // V1
  } //V2

  map->SetMinimum(0.0);
  map->SetMaximum(1.0);
  
  gStyle->SetPalette(kBird);
  //gStyle->SetPalette(kTemperatureMap);
  gStyle->SetOptStat(0);
  map->SetTitle("Efficiency");
  map->GetXaxis()->SetNdivisions(505);
  map->GetXaxis()->SetTitle("V1 (V)");
  map->GetYaxis()->SetNdivisions(505);
  map->GetYaxis()->SetTitle("V2 (V)");
  map->GetXaxis()->SetTitleOffset(1.5);
  map->Draw("COLZ");

  c1->SaveAs("efficiency.pdf");
  
  
  delete(c1);

  return(0);
}

