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


int main(void){
  
  //preparazione per la lettura del tree_raw
  float xi, yi, zi, xf, yf, zf;
  
  TCanvas* c1 = new TCanvas("c1","",1000,1000);
  c1->cd();

  std::vector<float> canestri_tot;
  std::vector<float> canestri_rel;
  
  //impostazione mappe
  int z_max  = 50; //um
  int y_max  = 50; //um
  float unit = 0.25;    //1step in 5 um
  float pixel_side = 2;
  int nbins  = 2*z_max/pixel_side;
  int x_tes  = 4048; //posizione in grid units di tes
  int x_pin2 = 4046; //posizione in grid units di tes

  int z_max_pin2  = 300; //um
  int y_max_pin2  = 300; //um
  float pixel_side_pin2 = 5;
  int nbins_pin2  = 2*z_max_pin2/pixel_side_pin2;
  
  
  int volts_cnt[10] = {-100,-90,-80,-70,-60,-50,-40,-30,-20,-10};
  int volts_pin[10] = {0   ,10 , 20, 30, 40, 50, 60, 70, 80, 90};
  int ntot     = 100000;
  
  TMultiGraph* mg      = new TMultiGraph();
  TMultiGraph* mg_rate = new TMultiGraph();

  TGraph* gr_c = new TGraph(); //focalizzazione
  TGraph* gr_s = new TGraph(); //focalizzazione
  TGraph* gr_rateA = new TGraph(); //focalizzazione
  TGraph* gr_rateB = new TGraph(); //focalizzazione

  float area_factor = 1125;
  float i_A = 50.E-15/1125;
  float i_B = 800.E-15/1125;
  float e_charge = 1.6021E-19;
  float rate_A = i_A/e_charge;
  float rate_B = i_B/e_charge;
  
  for(int v=0; v<10; v++){
    //apertura file con il tree
    TFile run(Form("root/geom0/mcbon0_V1_%d_V2_%d.root",volts_cnt[v], volts_pin[v]));
    if (!run.IsOpen()) {;}
  
    else{
    
      TTree *tree = (TTree*)run.Get(Form("tree"));
      Long64_t nentries = tree->GetEntries();
      tree->SetBranchAddress("x_i", &xi);
      tree->SetBranchAddress("y_i", &yi);
      tree->SetBranchAddress("z_i", &zi);
      tree->SetBranchAddress("x_f", &xf);
      tree->SetBranchAddress("y_f", &yf);
      tree->SetBranchAddress("z_f", &zf);

      // Bordi dei bin per X e Y
      double* zBins = new double[nbins+1];
      double* yBins = new double[nbins+1];
      double* zBins_pin2 = new double[nbins_pin2+1];
      double* yBins_pin2 = new double[nbins_pin2+1];
      for (int i = 0; i <= nbins; ++i) {
	zBins[i] = -z_max + i * 2*z_max / nbins;
	zBins[i] -= pixel_side/2;
	yBins[i] = -y_max + i * 2*y_max / nbins;
	yBins[i] -= pixel_side/2;
      }
      for (int i = 0; i <= nbins_pin2; ++i) {
	zBins_pin2[i] = -z_max_pin2 + i * 2*z_max_pin2 / nbins_pin2;
	zBins_pin2[i] -= pixel_side_pin2/2;
	yBins_pin2[i] = -y_max_pin2 + i * 2*y_max_pin2 / nbins_pin2;
	yBins_pin2[i] -= pixel_side_pin2/2;
      }

      int canestro = 0; //elettroni sul tes
      int sorpasso = 0; //elettroni che passano oltre il primo pinhole
      TH2F* map_pin2  = new TH2F("map_pin2" ,"",nbins_pin2,yBins_pin2,nbins_pin2,zBins_pin2);
      TH2F* map_start = new TH2F("map_start","",nbins,yBins,nbins,zBins);

      for(int iEntry=0; iEntry<nentries; iEntry++){
	tree->GetEntry(iEntry);
      
	//quantità in micron
	//xi*=unit;  yi*=unit;  zi*=unit;
	//xf*=unit;  yf*=unit;  zf*=unit;

	if(xf==x_tes || xf==x_pin2){
	  map_pin2->Fill(yf,zf);
	  sorpasso++;
	  if(xf==x_tes){
	    map_start->Fill(yi,zi);
	    canestro++;
	  }
	}
      }//for sulle entries

      
      std::cout << -volts_cnt[v] << " -> " << canestro << ", " << sorpasso << std::endl;

      gr_c->SetPoint(v,-volts_cnt[v],canestro*100./ntot);
      gr_s->SetPoint(v,-volts_cnt[v],canestro*100./sorpasso);
      gr_rateA->SetPoint(v,-volts_cnt[v],rate_A*canestro/ntot);
      gr_rateB->SetPoint(v,-volts_cnt[v],rate_B*canestro/ntot);

      canestri_tot.push_back(canestro*100./ntot);
      canestri_rel.push_back(canestro*100./sorpasso);
      
      gr_c->SetMarkerSize(2);
      gr_c->SetLineWidth(2);
      gr_c->SetMarkerStyle(8);
      gr_c->SetMarkerColor(46);
      gr_s->SetMarkerSize(2);
      gr_s->SetLineWidth(2);
      gr_s->SetMarkerStyle(8);
      gr_s->SetMarkerColor(38);
      gr_rateA->SetMarkerSize(2);
      gr_rateA->SetLineWidth(2);
      gr_rateA->SetMarkerStyle(8);
      gr_rateA->SetMarkerColor(46);
      gr_rateB->SetMarkerSize(2);
      gr_rateB->SetLineWidth(2);
      gr_rateB->SetMarkerStyle(8);
      gr_rateB->SetMarkerColor(38);


      //area geometrica del TES
      int tes_side = 40; //già in unità di griglia
      TLine* line1 = new TLine(-tes_side*0.5, -tes_side*0.5, -tes_side*0.5,  tes_side*0.5);
      TLine* line2 = new TLine(-tes_side*0.5,  tes_side*0.5,  tes_side*0.5,  tes_side*0.5);
      TLine* line3 = new TLine( tes_side*0.5,  tes_side*0.5,  tes_side*0.5, -tes_side*0.5);
      TLine* line4 = new TLine( tes_side*0.5, -tes_side*0.5, -tes_side*0.5, -tes_side*0.5);
      
      line1->SetLineWidth(4); line1->SetLineColor(46);
      line2->SetLineWidth(4); line2->SetLineColor(46);
      line3->SetLineWidth(4); line3->SetLineColor(46);
      line4->SetLineWidth(4); line4->SetLineColor(46);
      
      gStyle->SetOptStat(0);
      
      
      gStyle->SetPalette(kBird);
      gStyle->SetOptStat(0);
      map_pin2->GetXaxis()->SetTitle("z [um]");
      map_pin2->GetYaxis()->SetTitle("y [um]");
      map_pin2->Draw("axis");
      map_pin2->Draw("COLZsame");
      line1->Draw("same");
      line2->Draw("same");
      line3->Draw("same");
      line4->Draw("same");
      c1->SaveAs(Form("plots/map_pin2_E%d.png",-volts_cnt[v]));
      c1->Clear();

      //buco nei nanotubi
      TEllipse* ell_cnt = new TEllipse(0,0,20,0,360,0);
      ell_cnt->SetLineWidth(4);
      ell_cnt->SetFillColor(0);
      ell_cnt->SetFillStyle(0);
      ell_cnt->SetLineColor(46);
  
      map_start->Draw("axis");
      ell_cnt->Draw("same");
      map_start->Draw("COLZsame");
      map_start->GetXaxis()->SetTitle("z [um]");
      map_start->GetYaxis()->SetTitle("y [um]");
      c1->SaveAs(Form("plots/map_start_E%d.png",-volts_cnt[v]));
      c1->Clear();
   	  
    }//else sul file
  }

  TLegend* legend = new TLegend(0.35,0.3,0.7,0.38);
  
  legend->SetFillStyle(0);
  legend->SetBorderSize(0);
 
  legend->AddEntry(gr_c,"hits on tes/tot (%)","p");
  legend->AddEntry(gr_s,"hits on tes/electrons beyond pin2 (%)","p");

  TLegend* legend_rate = new TLegend(0.4,0.3,0.9,0.38);
  
  legend_rate->SetFillStyle(0);
  legend_rate->SetBorderSize(0);
 
  legend_rate->AddEntry(gr_rateA,"i_cnt = 50   fA / area factor","p");
  legend_rate->AddEntry(gr_rateB,"i_cnt = 800 fA / area factor","p");
  
  legend->SetTextSize(0.03);
  legend_rate->SetTextSize(0.03);
  mg->Add(gr_s,"p");
  mg->Add(gr_c,"p");
  mg_rate->Add(gr_rateA,"p");
  mg_rate->Add(gr_rateB,"p");
  
  
  //mg->GetYaxis()->SetRangeUser(0,105);
  mg->GetXaxis()->SetTitle("E_e [eV]");
  mg->GetYaxis()->SetTitle("strikes (%)");
  mg->Draw("a");
  legend->Draw("same");
  
  
  c1->SaveAs("plots/canestri.png");


  c1->SetLeftMargin(0.155);
  mg_rate->GetXaxis()->SetTitle("E_e [eV]");
  mg_rate->GetYaxis()->SetTitle("Rate (Hz)");
  //mg_rate->GetYaxis()->SetRangeUser(0,0.3);
  mg_rate->Draw("a");
  legend_rate->Draw("same");
  
  
  c1->SaveAs("plots/rate.png");
  

  std::cout << std::endl;
  std::cout << "Energy[eV]  hits_tot[%]  hits_rel[%]" << std::endl;
  for(int i=0; i<canestri_tot.size(); i++){
    int energy = 100;
    int step = 10;
    std::cout << energy - i*step << " "
	      << canestri_tot[i] << " "
	      << canestri_rel[i] << std::endl;
  }

  std::cout << std::endl;
  std::cout << "Energy[eV]  Rate A[Hz]  Rate B[Hz]" << std::endl;
  for(int i=0; i<canestri_tot.size(); i++){
    int energy = 100;
    int step = 10;
    std::cout << energy - i*step << " "
	      << canestri_tot[i]*0.01*rate_A << " "
	      << canestri_tot[i]*0.01*rate_B << std::endl;
  }
  
}

