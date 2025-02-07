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

#include "sim_details.h"

int main(int argc, char* argv[]){
  
  //parametri da input
  if( argc!= 4 ) {
    std::cout << "USAGE: ./HitsvsDistance [folder] [subfolder] [prefix]" << std::endl;
    std::cout << "EXAMPLE: ./HitsvsDistance rimbalzi rimbalzi_1 rimb" << std::endl;
    exit(1);
  }

  char* folder    = argv[1];
  char* subfolder = argv[2];
  char* prefix    = argv[3];   

  TCanvas* c1 = new TCanvas("c","",1000,1000);
  c1->cd();

  std::vector<int> volts      = GetVoltages (folder,subfolder);
  std::vector<int> sim        = GetSimNumber(folder,subfolder);
  std::vector<int> energy     = GetEnergy   (folder,subfolder);
  std::vector<int> cnt_radius = GetCNTs     (folder,subfolder);
  std::vector<int> distance   = Getdistance (folder,subfolder);

  //step di 5um
  int tes_side = 60;       //um
  int unit     = 5;        //unit in simulation: 1 is 5 um
  int dist_max = 20*unit;//um
  int x_tes    = 110*unit; //um
  
  for(int d=0; d<distance.size(); d++){
    for(int s=0; s<sim.size(); s++){
      for(int r=0; r<cnt_radius.size(); r++){
	for(int v=0; v<volts.size(); v++){
	  for(int e=0; e<energy.size(); e++){
	    
	    //rimb_d600um_cnt15um_V30_E10-1
	    std::string name = std::string(prefix)+ "_d"
	      + std::to_string(distance[d]) + "um_cnt"
	      + std::to_string(cnt_radius[r]) + "um_V"
	      + std::to_string(volts[v]) + "_E"
	      + std::to_string(energy[e]) + "-"
	      + std::to_string(sim[s]);
	    
	    //apertura file con il tree
	    TFile run(Form("root/%s/%s/%s.root",folder,subfolder,name.c_str()));
	    if (!run.IsOpen()) {;}
	    else{
	      
	      TTree *tree = (TTree*)run.Get(Form("tree"));
	      Long64_t nentries = tree->GetEntries();
	      float x_pos, y_pos, z_pos;
	      
	      tree->SetBranchAddress("x_f", &x_pos);
	      tree->SetBranchAddress("y_f", &y_pos);
	      tree->SetBranchAddress("z_f", &z_pos);
	      
	      TH1F* hits = new TH1F("hits","",20,-5,dist_max);
	      
	      for(int iEntry=0; iEntry<nentries; iEntry++){
		tree->GetEntry(iEntry);
		x_pos *= unit;
		y_pos *= unit;
		z_pos *= unit;
		
		//canestro
		if(x_pos == x_tes &&
		   y_pos<tes_side*0.5 && y_pos>-tes_side*0.5 &&
		   z_pos<tes_side*0.5 && z_pos>-tes_side*0.5){
		  
		  //calcolo distanza
		  float distance = sqrt(y_pos*y_pos+z_pos*z_pos);
		  hits->Fill(distance);
		} //canestro
	      }//for sul tree
	      
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
	      
	      c1->SaveAs(Form("%s/histo_hitstrike/%s_hitsdist.png",outdir.c_str(),name.c_str()));
	      c1->Clear();
	      delete(hits);
	      
	      //fare grafico numero di canestri vs altre quantità
	      
	    } //else sull'esistenza del file
	  }//for energia
	}//for voltaggio					
      }//for cnts_radius
    }//for sim
  }//for distanze
 
 
  delete(c1);
  return(0);
}





