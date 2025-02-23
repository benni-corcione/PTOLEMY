#include "TTree.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH2F.h"
#include "TStyle.h"
#include "TLine.h"
#include "TColor.h"
#include "TExec.h"
#include "TEllipse.h"
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

#include "sim_details.h"

Double_t Cruijff(Double_t *x, Double_t *par);
Double_t Landau(Double_t *x, Double_t *par);
Double_t AG(Double_t *x, Double_t *par);
Double_t CrystalBall(Double_t *x, Double_t *par);
void DrawPad(TH2F* map,int range, int tes_side, int cnt_radius);
int GetDepartureRange(int cnt_radius);
int GetArrivalRangeZoom(int cnt_radius);
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
  float ke_i, ke_f;

  std::vector<int> volts      = GetVoltages (folder,subfolder);
  std::vector<int> cnt_radius = GetCNTs     (folder,subfolder);
  std::vector<int> distance   = Getdistance (folder,subfolder);
  std::vector<int> dist_gen   = GetDistGen  (folder,subfolder);
  std::vector<int> substrate  = GetSubstrate(folder,subfolder);

  TCanvas* c1 = new TCanvas("c1","",3100,1000);
  c1->cd();

  int z_max      = 8000; //um
  int y_max      = 8000; //um
  int unit       = 5; //1step in 5 um
  int tes_side   = 60; // um
  int pixel_side = 8;
  int x_tes      = 110*unit; 
  int nbins      = 2*z_max/pixel_side; 
    
 

  for(int s=0; s<substrate.size(); s++){
    for(int d=0; d<distance.size(); d++){
      for(int v=0; v<volts.size(); v++){
	for(int r=0; r<cnt_radius.size(); r++){
	  for(int dg=0; dg<dist_gen.size(); dg++){
	      
	   
	      
	    //rimb_d600um_cnt15um_sub5000um_V30_dist500um
	    std::string name = std::string(prefix)+ "_cnt"
	      + std::to_string(cnt_radius[r]) + "um_dist"
	      + std::to_string(dist_gen[dg]) + "um";
	      
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
	      tree->SetBranchAddress("ke_i", &ke_i);
	      tree->SetBranchAddress("ke_f", &ke_f);
	      
	      
	      // Bordi dei bin per X e Y
	      double* zBins = new double[nbins+1];
	      double* yBins = new double[nbins+1];
	      for (int i = 0; i <= nbins; ++i) {
		zBins[i] = -z_max + i * 2*z_max / nbins;
		zBins[i] -= pixel_side/2;
		yBins[i] = -y_max + i * 2*y_max / nbins;
		yBins[i] -= pixel_side/2;
	      }

	      int hits=0;
	      int discesa=0;
	      int range      = GetArrivalRange(cnt_radius[r]);
	      int range_zoom = GetArrivalRangeZoom(cnt_radius[r]);
	      int range2     = GetDepartureRange(cnt_radius[r]);
	      
	      //mappa arrivi e partenze
	      TH2F* map = new TH2F("map","",nbins,yBins,nbins,zBins);
	      TH2F* map_zoom = new TH2F("map_zoom","",nbins,yBins,nbins,zBins);
	      TH2F* map2 = new TH2F("map2","",nbins,yBins,nbins,zBins);
	      TH1F* energy = new TH1F("energy","",150,-0.5,5);	

	      TPad* pad1 = new TPad("pad1","left pad",0,0,0.32,1);
	      pad1->Draw();
	      TPad* pad2 = new TPad("pad2","center pad",0.33,0,0.65,1);
	      pad2->Draw();
	      TPad* pad3 = new TPad("pad3","right pad",0.66,0,1,1);
	      pad3->Draw();
	      pad1->SetLeftMargin (0.15);
	      pad1->SetRightMargin (0.12);
	      pad1->SetBottomMargin (0.12);
	      pad2->SetRightMargin(0.12);
	      pad2->SetLeftMargin(0.12);
	      pad3->SetRightMargin(0.15);
	      pad3->SetLeftMargin(0.12);
	      
	      for(int iEntry=0; iEntry<nentries; iEntry++){
		tree->GetEntry(iEntry);
		//quantità in micron
		pos_x *=unit;  pos_y *=unit;  pos_z *=unit;
		pos_xi*=unit;  pos_yi*=unit;  pos_zi*=unit;

		if(pos_x==x_tes){ //sono ricaduto giù sul piano
		  discesa++;
		  map->Fill(pos_y,pos_z);
		  map_zoom->Fill(pos_y,pos_z);
		}
		  
		//canestro
		if(pos_x == x_tes &&
		   pos_y<tes_side*0.5 && pos_y>-tes_side*0.5 &&
		   pos_z<tes_side*0.5 && pos_z>-tes_side*0.5 ){
		  hits++;
		  //energy->Fill(ke_i);
		  map2->Fill(pos_yi,pos_zi);
		  
		} //canestro
		  
		  
	      }//for sulle entries
		std::cout << discesa << " " << hits << std::endl;	  
	
	      TExec *ex1 = new TExec("ex1","gStyle->SetPalette(kBird);");
	      TExec *ex2 = new TExec("ex2","gStyle->SetPalette(kBird);");
	      TExec *ex3 = new TExec("ex3","gStyle->SetPalette(kCandy);");
		
		
	      pad1->cd();
	      map->Draw("axis");
	      ex1->Draw();
	      map->SetTitle(Form("%.2f%% arrivals - cnt: %d um, d: %d um, sub=%d, V=%d V, r=%d um",discesa*100./nentries, cnt_radius[r], distance[d], substrate[s], volts[v], dist_gen[dg]));
	      DrawPad(map,range,tes_side, cnt_radius[r]);
		
	      //ex2->Draw();
	      //disegno zoomato al centro
	      pad2->cd();
	      map_zoom->Draw("axis");
	      ex2->Draw();
	      map_zoom->SetTitle(Form("Hits: %.3f%%",hits*100.0/nentries));
	      DrawPad(map_zoom,range_zoom, tes_side, cnt_radius[r]);
		
	      //disegno totale a sx
	      pad3->cd();
	      map2->Draw("axis");
	      ex3->Draw();
	      map2->SetTitle(Form("Hits start - cnt: %d um, d: %d um, sub=%d, V=%d V, r=%d um",cnt_radius[r], distance[d], substrate[s], volts[v], dist_gen[dg]));
	      DrawPad(map2,range2,tes_side,cnt_radius[r]);
		
		
	      c1->SaveAs(Form("plots/%s/%s/maps/%s_map.png",folder,subfolder,name.c_str()));
	      c1->Clear();
	      
	      /*
	      TF1 fitfunction("fitfunction",Cruijff,0,1,5);
	      double par[5];
	      fitfunction.SetParameters(par);
	      fitfunction.SetLineColor(kRed-4);
	      fitfunction.SetLineWidth(4);
	      fitfunction.SetNpx(1000);
	
	      fitfunction.SetParameter(0, 0.2);
	      fitfunction.SetParameter(1, 0.01);
	      fitfunction.SetParameter(2, 0.1);
	      fitfunction.SetParameter(3, 3);
	      fitfunction.SetParameter(4, 10);

	      fitfunction.SetParLimits(0,0.2,0.3);
	      fitfunction.SetParLimits(1,0.02,0.1);
	      fitfunction.SetParLimits(2,0.001,0.1);
	      fitfunction.SetParLimits(4, 10,10000);
	      fitfunction.SetParLimits(3, 0.1,10);

	      //energy->GetXaxis()->SetRange(,5);
	      energy->Fit("fitfunction","RmX","",-0.5,0.7);
	      energy->Draw("same");
	      c1->SaveAs(Form("plots/%s/%s/maps/%s_energy.png",folder,subfolder,name.c_str()));
	      */
	      
	    }//else sul file
	  }//for dist_gen
	}//for raggio cnt
      }//for volts
    }//for distanza
  }//for substrato
  
  return(0);
}

void DrawPad(TH2F* map,int range, int tes_side, int cnt_radius){
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
  if(cnt_radius==15 ){range = 800;}
  if(cnt_radius==40 ){range = 150;}
  if(cnt_radius==100){range = 200;}
  if(cnt_radius==200){range = 300;}
  if(cnt_radius==300){range = 400;}
  if(cnt_radius==500){range = 600;}
  return range;
}

int GetArrivalRangeZoom(int cnt_radius){
  int range;
  if(cnt_radius==15 ){range = 150;}
  if(cnt_radius==40 ){range = 150;}
  if(cnt_radius==100){range = 200;}
  if(cnt_radius==200){range = 300;}
  if(cnt_radius==300){range = 400;}
  if(cnt_radius==500){range = 600;}
  return range;
}

int GetArrivalRange(int cnt_radius){
  int range;
  if(cnt_radius==15 ){range = 7000;}
  if(cnt_radius==40 ){range = 150;}
  if(cnt_radius==100){range = 225;}
  if(cnt_radius==200){range = 300;}
  if(cnt_radius==300){range = 450;}
  if(cnt_radius==500){range = 600;}
  return range;
}



// Funzione di Crystal Ball senza RooFit, direttamente in ROOT
Double_t CrystalBall(Double_t *x, Double_t *par) {
    // par[0] = media (mean)
    // par[1] = deviazione standard (sigma)
    // par[2] = parametro alpha (forma della coda)
    // par[3] = parametro n (decadimento della coda)
    // par[4] = parametro di normalizzazione per la parte Lorentziana
    
    // Calcola la parte della coda Lorentziana
    if (*x > par[0] + par[2] * par[1]) {
        // Parte Lorentziana
        return par[4] * TMath::Power((par[3] / TMath::Abs(par[2])) - (*x - par[0]) / par[1], -par[3]);
    }
    // Altrimenti, usa la parte Gaussiana
    else {
        // Parte Gaussiana
        return TMath::Exp(-0.5 * TMath::Power((*x - par[0]) / par[1], 2));
    }
}


//cruiff (che può diventare una gaussiana) da fittare
Double_t AG(Double_t *x, Double_t *par){
  //par[0]=mean      par[1]=sigma_L    par[2]=sigma_R
  //par[3]=A   
  Double_t fitval = 0;
  Double_t arg_L  = 0;
  Double_t arg_R  = 0;
  Double_t num    = pow((x[0]-par[0]),2);
  Double_t denL   = 2*par[1]*par[1];
  Double_t denR   = 2*par[2]*par[2];
  if ( denL        != 0 ){ arg_L = -num/(denL); }
  if ( par[2]      != 0 ){ arg_R = -num/ denR ; }

  if       ( x[0] < par[0] ){ fitval = par[3] * exp(arg_L); } //LEFT
  else if  ( x[0] > par[0] ){ fitval = par[3] * exp(arg_R); } //RIGHT

  return(fitval);
}

Double_t Landau(Double_t *x, Double_t *par) {
    Double_t t = x[0];  // variabile di integrazione
    Double_t val = TMath::Exp(-t - par[0]*par[0] / (4.0 * t));  // Formula dell'integrale
    return val;
}


//cruiff (che può diventare una gaussiana) da fittare
Double_t Cruijff(Double_t *x, Double_t *par){
  //par[0]=mean      par[1]=sigma_L    par[2]=sigma_R
  //par[3]=alpha_L   par[4]=A
  Double_t fitval = 0;
  Double_t arg_L  = 0;
  Double_t arg_R  = 0;
  Double_t num    = pow((x[0]-par[0]),2);
  Double_t denL   = 2*par[1]*par[1];
  Double_t denR   = 2*par[2]*par[2];
  Double_t asym   = par[3]*num;
  if ( denR + asym != 0 ){ arg_R = -num/(denR+asym); }
  if ( denL        != 0 ){ arg_L = -num/ denL      ; }

  if       ( x[0] > par[0] ){ fitval = par[4] * exp(arg_R); } //LEFT
  else if  ( x[0] < par[0] ){ fitval = par[4] * exp(arg_L); } //RIGHT

  return(fitval);
}
