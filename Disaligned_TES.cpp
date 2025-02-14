#include "TTree.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH2F.h"
#include "TGraph.h"
#include "TMultiGraph.h"
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

void DrawPad(TH2F* map,int range, int tes_side, int cnt_radius, int segmento);
int GetDepartureRange(int cnt_radius);
int GetArrivalRange(int cnt_radius);
void bubbleSort(std::vector<double>& v_e, std::vector<int>& cnts, std::vector<double>& hits);
std::vector<int> GetColors(void);

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

  //quantità per futuri grafici comparativi
  std::vector<int> cnt_vec;
  std::vector<double> canestro;
  std::vector<double> VsuE_vec;

  
  TCanvas* c1 = new TCanvas("c1","",1000,1000);
  c1->cd();

  int z_max      = 2000; //um
  int y_max      = 2000; //um
  int unit       = 5; //1step in 10 um
  int tes_side   = 60; // um
  int pixel_side = 10;
  int x_tes      = 110*unit; 
  int nbins      = z_max/pixel_side;
  int segmento   = 470; //dal centro al bordo del tes
  //posizione altro tes
  int tes2_ymin  = -segmento-tes_side;
  int tes2_ymax  = -segmento;
  int tes2_zmin  = segmento;
  int tes2_zmax  = segmento+tes_side;
 
  c1->SetLeftMargin(0.15);
  c1->SetRightMargin(0.1);
  
  for(int d=0; d<distance.size(); d++){
    for(int s=0; s<sim.size(); s++){
      for(int r=0; r<cnt_radius.size(); r++){
	for(int v=0; v<volts.size(); v++){
	  for(int e=0; e<energy.size(); e++){
	    if(volts[v]>=energy[e]){
	      int hits = 0;
	      int range = 600; //GetDepartureRange(cnt_radius[r]);
	      
	      
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
		
		//mappa partenze canestri
		TH2F *map = new TH2F("map","",nbins,-y_max,y_max,nbins,-z_max,z_max);
		
		for(int iEntry=0; iEntry<nentries; iEntry++){
		  tree->GetEntry(iEntry);
		  
		  //quantità in micron
		  pos_x*=unit ;  pos_y*=unit ;  pos_z*=unit;
		  pos_xi*=unit;  pos_yi*=unit;  pos_zi*=unit;
		  
		  //canestro nell'altro tes
		  if(pos_x == x_tes &&
		     pos_y<tes2_ymax && pos_y>tes2_ymin &&
		     pos_z<tes2_zmax && pos_z>tes2_zmin){
		    hits++;
		    map->Fill(pos_yi,pos_zi);
		    //map->Fill(pos_y,pos_z);
		  } //canestro
	      
		  
		}//for sulle entries
		
		gStyle->SetPalette(kBird);
		map->Draw("axis"); 
		map->SetTitle(Form("%d canestri in H60 - cnt: %d um, d: %d um, V=%d V, E=%d eV",hits, cnt_radius[r], distance[d], volts[v], energy[e]));
		DrawPad(map,range,tes_side,cnt_radius[r],segmento);
		
		c1->SaveAs(Form("plots/%s/%s/maps/%s_H60.png",folder,subfolder,name.c_str()));
		
		delete(map);

		canestro.push_back(hits);
		cnt_vec.push_back(cnt_radius[r]);
		VsuE_vec.push_back(1.*volts[v]/energy[e]);
		
	      }//else sul file
	    }//condizione energia
	  }//for distanza
	}//for sim
      }//for raggio
    }//for volts
  }//for energy

  
//PREPARAZIONE ARRAY PER GRAFICI:
  bubbleSort(VsuE_vec, cnt_vec, canestro);

  int Npoints = canestro.size();
  TMultiGraph *mg = new TMultiGraph(); //più plot insieme

  int colour[4]= {kRed-7,kOrange,kGreen-6,kAzure+7};
  TLegend* legend = new TLegend(0.7,0.12,0.9,0.26);
  legend->SetBorderSize(0);

  //1: hits vs V/E con cnt_radius fissato
  for(int r=0; r<cnt_radius.size(); r++){
    
    //ciclo sui singoli punti acquisiti a parità di cnt_radius
    std::vector<double> x, y; 
    int points=0;

    //riempio vettori
    for(int i=0; i<Npoints; i++){ 
      if(cnt_radius[r]==cnt_vec[i]){
	points++;
	x.push_back(VsuE_vec[i]);
	y.push_back(canestro[i]);
      }//if sul cnt_radius giusto
    }//for sugli Npoints da scorrere

    //converto vettori in array per grafico
    double x_[points], y_[points];
    for(int i=0; i<points;i++){
      x_[i]=x[i]; y_[i]=y[i];
    }
    
    TGraph* hitsVsuE = new TGraph(points,x_,y_);
    hitsVsuE->GetYaxis()->SetRangeUser(1,1E+5);
    hitsVsuE->SetMarkerColor(colour[r]);
    hitsVsuE->SetMarkerStyle(8);
    hitsVsuE->SetMarkerSize(2);
    legend->AddEntry(hitsVsuE,Form("cnt_radius = %d um", cnt_radius[r]),"p");
    mg->Add(hitsVsuE,"p");
    
  }

  c1->SetLogy();
  c1->SetLeftMargin(0.11);
  mg->GetYaxis()->SetRangeUser(1,200);
  mg->GetXaxis()->SetRangeUser(0,4.7);
  mg->GetXaxis()->SetTitle("V/E");
  mg->GetYaxis()->SetTitle("Canestri");
  mg->GetHistogram()->SetTitle("Canestri vs V/E");
  mg->Draw("a");
  legend->Draw("same");
  c1->SaveAs(Form("plots/%s/%s/canestri/HitsvsVE_H60.png",folder,subfolder));
  c1->Clear();
    
  delete(mg);

  //2: hits vs cnt_radius con V/E fissato
  //inizializzazione di un vettore che ha dentro i numeri di V/E presenti
  std::vector<double> VsuE_list;
  VsuE_list.push_back(VsuE_vec[0]);
  
  for(int i=1; i<Npoints; i++){
    if(VsuE_vec[i-1]!=VsuE_vec[i]){
      VsuE_list.push_back(VsuE_vec[i]);
    }
  }
 
  TMultiGraph *mg2 = new TMultiGraph(); //più plot insieme
  TLegend* legend2 = new TLegend(0.85,0.12,1,0.76);
  legend2->SetBorderSize(0);
  std::vector<int> colori = GetColors();
  for(int r=0; r<VsuE_list.size(); r++){
    
    //ciclo sui singoli punti acquisiti a parità di cnt_radius
    std::vector<double> x, y; 
    int points=0;

    //riempio vettori
    for(int i=0; i<Npoints; i++){ 
      if(VsuE_list[r]==VsuE_vec[i]){
	points++;
	x.push_back(cnt_vec[i]);
	y.push_back(canestro[i]);
      }//if sul VsuE giusto
    }//for sugli Npoints da scorrere
    
    //converto vettori in array per grafico
    double x_[points], y_[points];
    for(int i=0; i<points;i++){
      x_[i]=x[i]; y_[i]=y[i];
    }
    
    TGraph* hitscnts = new TGraph(points,x_,y_);
    hitscnts->GetYaxis()->SetRangeUser(1,1E+5);
    hitscnts->SetMarkerColor(colori[r]);
    hitscnts->SetLineColor(colori[r]);
    hitscnts->SetMarkerStyle(8);
    hitscnts->SetLineWidth(2);
    hitscnts->SetMarkerSize(2);
    legend2->AddEntry(hitscnts,Form("V/E = %.2f", VsuE_list[r]),"p");
    mg2->Add(hitscnts,"p");
    
    }
  
  c1->SetLogy();
  c1->SetLeftMargin(0.11);
  c1->SetRightMargin(0.16);

  mg2->GetYaxis()->SetRangeUser(1,200);
  mg2->GetXaxis()->SetTitle("cnt radius [um]");
  mg2->GetYaxis()->SetTitle("Canestri");
  mg2->GetHistogram()->SetTitle("Canestri vs cnt_radius");
  mg2->Draw("a");
  legend2->Draw("same");
  c1->SaveAs(Form("plots/%s/%s/canestri/Hitsvscnts_H60.png",folder,subfolder));
  c1->Clear();

  
  delete(c1);
  return(0);
}

void DrawPad(TH2F* map,int range, int tes_side, int cnt_radius, int segmento){
  //area geometrica del TES
  TLine* line1 = new TLine(-tes_side*0.5, -tes_side*0.5, -tes_side*0.5,  tes_side*0.5);
  TLine* line2 = new TLine(-tes_side*0.5,  tes_side*0.5,  tes_side*0.5,  tes_side*0.5);
  TLine* line3 = new TLine( tes_side*0.5,  tes_side*0.5,  tes_side*0.5, -tes_side*0.5);
  TLine* line4 = new TLine( tes_side*0.5, -tes_side*0.5, -tes_side*0.5, -tes_side*0.5);

  line1->SetLineWidth(4); line1->SetLineColor(kRed);
  line2->SetLineWidth(4); line2->SetLineColor(kRed);
  line3->SetLineWidth(4); line3->SetLineColor(kRed);
  line4->SetLineWidth(4); line4->SetLineColor(kRed);

  int tes2_ymin  = -segmento-tes_side;
  int tes2_ymax  = -segmento;
  int tes2_zmin  = segmento;
  int tes2_zmax  = segmento+tes_side;
  
  TLine* line1_2 = new TLine(tes2_ymin, tes2_zmax, tes2_ymax, tes2_zmax);
  TLine* line2_2 = new TLine(tes2_ymax, tes2_zmax, tes2_ymax, tes2_zmin);
  TLine* line3_2 = new TLine(tes2_ymax, tes2_zmin, tes2_ymin, tes2_zmin);
  TLine* line4_2 = new TLine(tes2_ymin, tes2_zmin, tes2_ymin, tes2_zmax);

  line1_2->SetLineWidth(4); line1_2->SetLineColor(kRed);
  line2_2->SetLineWidth(4); line2_2->SetLineColor(kRed);
  line3_2->SetLineWidth(4); line3_2->SetLineColor(kRed);
  line4_2->SetLineWidth(4); line4_2->SetLineColor(kRed);
 
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
  line1_2->Draw("same");
  line2_2->Draw("same");
  line3_2->Draw("same");
  line4_2->Draw("same");
  ell->Draw("same");
}

int GetDepartureRange(int cnt_radius){
  int range;
  if(cnt_radius==15 ){range = 100;}
  if(cnt_radius==40 ){range = 100;}
  if(cnt_radius==200){range = 250;}
  if(cnt_radius==500){range = 700;}
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




void bubbleSort(std::vector<double>& v_e, std::vector<int>& cnts, std::vector<double>& hits){
  int n = v_e.size();
  
  // Outer loop that corresponds to the number of
  // elements to be sorted
  for (int i=0; i < n-1; i++) {
     bool swapped = false;
    for (int j=0; j < n-i-1; j++) {
      if (v_e[j] > v_e[j+1]){
	std::swap(v_e[j], v_e[j+1]);
	std::swap(cnts[j], cnts[j+1]);
	std::swap(hits[j], hits[j+1]);
	swapped=true;
      }
    }
     if (!swapped) break;
  }
}

std::vector<int> GetColors(void){
  std::vector<int> colors;
  colors.push_back(kRed+3);
  colors.push_back(kRed+1);
  colors.push_back(kRed-7);//
  colors.push_back(kOrange+8);
  colors.push_back(kOrange+1);
  colors.push_back(kOrange);//
  colors.push_back(89); //giallo
  colors.push_back(kSpring-9);
  colors.push_back(79); //verde chiaro
  colors.push_back(kGreen-3);
  colors.push_back(kTeal-7);//
  colors.push_back(kCyan+1);
  colors.push_back(kAzure+10);
  colors.push_back(kAzure+1);
  colors.push_back(kAzure-5);//
  colors.push_back(kBlue-2);
  colors.push_back(kViolet+1);
  colors.push_back(kViolet-9);
  colors.push_back(kMagenta+1);
  colors.push_back(kPink+6);
  return colors;
}
