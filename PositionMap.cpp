#include "TTree.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH2F.h"
#include "TStyle.h"
#include "TLine.h"
#include "TColor.h"
#include "TExec.h"
#include "TEllipse.h"
#include "TLegend.h"

#include <fstream>
#include <string>
#include <stdlib.h>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <iostream>
#include <cmath>

#include "sim_details.h"

void DrawPad(TPad* pad1,TH2F* map,int range, int tes_side, int cnt_radius);
int GetDepartureRange(int cnt_radius);
int GetArrivalRange(int cnt_radius);
int main(int argc, char* argv[]){
  
  //parametri da input
  if( argc!= 4 ) {
    std::cout << "USAGE: ./PositionMap [folder] [subfolder] [prefix]" << std::endl;
    std::cout << "EXAMPLE: ./PositionMap pinhole test 1" << std::endl;
    exit(1);
  }
  
  char* folder    = argv[1];
  char* subfolder = argv[2];
  char* prefix    = argv[3];   

  std::string save_fold(Form("plots/%s",folder));
  std::string save_subfold(Form("plots/%s/%s",folder,subfolder));
  system( Form("mkdir -p %s", save_fold.c_str()) );
  system( Form("mkdir -p %s", save_subfold.c_str()));

  //preparazione per la lettura del tree_raw
  float pos_x , pos_y , pos_z ;
  float pos_xi, pos_yi, pos_zi;

  std::vector<int> volts      = GetVoltages (folder,subfolder);
  std::vector<int> sim        = GetSimNumber(folder,subfolder);
  std::vector<int> energy     = GetEnergy   (folder,subfolder);
  std::vector<int> cnt_radius = GetCNTs     (folder,subfolder);
  std::vector<int> distance   = Getdistance (folder,subfolder);

  TCanvas* c1 = new TCanvas("c1","",3100,1000);
  c1->cd();

  int z_max      = 2000; //um
  int y_max      = 2000; //um
  int unit       = 5; //1step in 10 um
  int tes_side   = 60; // um
  int pixel_side = 2;
  int x_tes      = 110*unit; 
  int nbins      = z_max/pixel_side;

 
    
  TPad *pad1 = new TPad("pad1","left pad",0,0,0.32,1);
  pad1->Draw();
  TPad *pad2 = new TPad("pad2","center pad",0.33,0,0.65,1);
  pad2->Draw();
  TPad *pad3 = new TPad("pad3","right pad",0.66,0,1,1);
  pad3->Draw();
  pad1->SetLeftMargin (0.15);
  pad1->SetRightMargin (0.12);
  pad1->SetBottomMargin (0.12);
  pad2->SetRightMargin(0.12);
  pad2->SetLeftMargin(0.12);
  pad3->SetRightMargin(0.15);
  pad3->SetLeftMargin(0.12);
 
  for(int d=0; d<distance.size(); d++){
    for(int s=0; s<sim.size(); s++){
      for(int r=0; r<cnt_radius.size(); r++){
	for(int v=0; v<volts.size(); v++){
	  for(int e=0; e<energy.size(); e++){

	    int hits=0;
	    int range      = GetDepartureRange(cnt_radius[r]);
	    int range_zoom = GetArrivalRange(cnt_radius[r]);
	     
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
	      tree->SetBranchAddress("x_f", &pos_x);
	      tree->SetBranchAddress("y_f", &pos_y);
	      tree->SetBranchAddress("z_f", &pos_z);
	      tree->SetBranchAddress("x_i", &pos_xi);
	      tree->SetBranchAddress("y_i", &pos_yi);
	      tree->SetBranchAddress("z_i", &pos_zi);

	      //mappa arrivi
	      TH2F *map      = new TH2F("map","",nbins,-y_max,y_max,nbins,-z_max,z_max);
	      TH2F *map_zoom = new TH2F("map_zoom","",nbins,-y_max,y_max,nbins,-z_max,z_max);

	      //mappa partenze canestri
	      TH2F *map2      = new TH2F("map2","",nbins,-y_max,y_max,nbins,-z_max,z_max);
	      TH2F *map2_zoom = new TH2F("map2_zoom","",nbins,-y_max,y_max,nbins,-z_max,z_max);

	      for(int iEntry=0; iEntry<nentries; iEntry++){
		tree->GetEntry(iEntry);
		//quantità in micron
		pos_x*=unit ;  pos_y*=unit ;  pos_z*=unit;
	        pos_xi*=unit;  pos_yi*=unit;  pos_zi*=unit;
          	map->Fill(pos_y,pos_z);
		map_zoom->Fill(pos_y,pos_z);

		//canestro
		if(pos_x == x_tes &&
		   pos_y<tes_side*0.5 && pos_y>-tes_side*0.5 &&
		   pos_z<tes_side*0.5 && pos_z>-tes_side*0.5){
		  hits++;
		  map2->Fill(pos_yi,pos_zi);
		  map2_zoom->Fill(pos_yi,pos_zi);
		} //canestro
	      
		
	      }//for sulle entries
	      //disegno totale a sx
	      TExec *ex1 = new TExec("ex1","gStyle->SetPalette(kBird);");
	      TExec *ex2 = new TExec("ex2","gStyle->SetPalette(kBird);");
              TExec *ex3 = new TExec("ex3","gStyle->SetPalette(kCandy);");

	  
	      pad1->cd();
	      map->Draw("axis");
	      ex1->Draw();
	      map->SetTitle(Form("Arrivals - cnt: %d um, d: %d um, V=%d V, E=%d eV",cnt_radius[r], distance[d], volts[v], energy[e]));
	      DrawPad(pad1,map,2000,tes_side, cnt_radius[r]);
	      
	      //ex2->Draw();
	      //disegno zoomato al centro
	      pad2->cd();
	      map_zoom->Draw("axis");
	      ex2->Draw();
	      map_zoom->SetTitle(Form("Hits: %d/%lld",hits,nentries));
	      DrawPad(pad2,map_zoom,range_zoom, tes_side, cnt_radius[r]);
	     
	      //disegno totale a sx
	      pad3->cd();
	      map2->Draw("axis");
	      ex3->Draw();
	      map2->SetTitle(Form("Hits start - cnt: %d um, d: %d um, V=%d V, E=%d eV",cnt_radius[r], distance[d], volts[v], energy[e]));
	      DrawPad(pad3,map2,range,tes_side,cnt_radius[r]);
	     
	      
	      c1->SaveAs(Form("plots/%s/%s/maps/%s_map.png",folder,subfolder,name.c_str()));
	       
	      delete(map);
	      delete(map_zoom);
	      delete(map2);
	      delete(map2_zoom);
	     
	    }//else sul file
	  }//for distanza
	}//for sim
      }//for raggio
    }//for volts
  }//for energy
 
  delete(c1);
  return(0);
}

void DrawPad(TPad* pad1,TH2F* map,int range, int tes_side, int cnt_radius){
  //area geometrica del TES
  TLine* line1 = new TLine(-tes_side*0.5, -tes_side*0.5, -tes_side*0.5,  tes_side*0.5);
  TLine* line2 = new TLine(-tes_side*0.5,  tes_side*0.5,  tes_side*0.5,  tes_side*0.5);
  TLine* line3 = new TLine( tes_side*0.5,  tes_side*0.5,  tes_side*0.5, -tes_side*0.5);
  TLine* line4 = new TLine( tes_side*0.5, -tes_side*0.5, -tes_side*0.5, -tes_side*0.5);

  line1->SetLineWidth(4); line1->SetLineColor(kRed);
  line2->SetLineWidth(4); line2->SetLineColor(kRed);
  line3->SetLineWidth(4); line3->SetLineColor(kRed);
  line4->SetLineWidth(4); line4->SetLineColor(kRed);
  
  gStyle->SetOptStat(0);

  //nanotubi
  TEllipse* ell = new TEllipse(0,0,cnt_radius,0,360,0);
  ell->SetLineWidth(4);
  ell->SetFillColor(0);
  ell->SetFillStyle(0);
  
  //map->SetTitleSize(11,"t");
  map->GetXaxis()->SetTitle("y axis (um)");
  map->GetYaxis()->SetTitle("z axis (um)");
  map->GetYaxis()->SetRangeUser(-range,range);
  map->GetXaxis()->SetRangeUser(-range,range);
  map->Draw("COLZsame");
  line1->Draw("same");
  line2->Draw("same");
  line3->Draw("same");
  line4->Draw("same");
  ell->Draw("same");
}

int GetDepartureRange(int cnt_radius){
  int range;
  if(cnt_radius==15 ){range = 100;}
  if(cnt_radius==40 ){range = 100;}
  if(cnt_radius==200){range = 250;}
  if(cnt_radius==500){range = 500;}
  return range;
}

int GetArrivalRange(int cnt_radius){
  int range;
  if(cnt_radius==15 ){range = 100;}
  if(cnt_radius==40 ){range = 150;}
  if(cnt_radius==200){range = 300;}
  if(cnt_radius==500){range = 600;}
  return range;
}
