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

#include "sim_details.h"


int main(int argc, char* argv[]){
  
  //parametri da input
  if( argc!= 3 ) {
    std::cout << "USAGE: ./FocalAnalysis [folder] [prefix]" << std::endl;
    std::cout << "EXAMPLE: ./FocalAnalysis focal1 focal" << std::endl;
    exit(1);
  }
  
  char* folder = argv[1];
  char* prefix = argv[2];   

  std::string save_fold(Form("plots/%s",folder));
  system( Form("mkdir -p %s", save_fold.c_str()) );

  //preparazione per la lettura del tree_raw
  float pos_x , pos_y , pos_z ;
 
  std::vector<int> v_pin      = GetVpin(folder);
  std::vector<int> pin_radius = GetPin (folder);

  TCanvas* c1 = new TCanvas("c1","",1000,1000);
  TCanvas* c2 = new TCanvas("c2","",1000,1000);
  c1->cd();

  TLegend* legend_f = new TLegend(0.6,0.70,0.9,0.90);
  TLegend* legend_r = new TLegend(0.12,0.70,0.42,0.90);
  legend_f->SetBorderSize(0);
  legend_f->SetFillStyle(0);
  legend_r->SetBorderSize(0);
  legend_r->SetFillStyle(0);
  std::vector<int> colours = {96,92,8,66};
  
  int unit       = 5;    //1step in 5 um
  int tes_side   = 60;   //um
  int pixel_side = 10;
  int x_tes      = 530*unit;
  int x1_pin     = 325*unit;
  int x2_pin     = 330*unit;

  //max distance without pinhole
  int d_wo_pin = 800; //in um
  
  TMultiGraph* mg_f = new TMultiGraph();
  TMultiGraph* mg_r = new TMultiGraph();

  for(int p=0; p<pin_radius.size();  p++){
    TGraph* gr_f = new TGraph(); //focalizzazione
    TGraph* gr_r = new TGraph(); //riduzione
     
    for(int v=0; v<v_pin.size(); v++){

      //focal_pin10um_Vpin50
      std::string name;

      if(v_pin[v]<0){
	name = std::string(prefix)+ "_pin"
	+ std::to_string(pin_radius[p]) + "um_Vpinm"
	+ std::to_string(-v_pin[v]);
      }

      else if(v_pin[v]>=0){
	name = std::string(prefix)+ "_pin"
	+ std::to_string(pin_radius[p]) + "um_Vpin"
	+ std::to_string(v_pin[v]);
      }
      
	  
      //apertura file con il tree
      TFile run(Form("root/%s/%s.root",folder,name.c_str()));
      if (!run.IsOpen()) {;}
	  
      else{
	    
	TTree *tree = (TTree*)run.Get(Form("tree"));
	Long64_t nentries = tree->GetEntries();
	tree->SetBranchAddress("x_f", &pos_x);
	tree->SetBranchAddress("y_f", &pos_y);
	tree->SetBranchAddress("z_f", &pos_z);
	    
	int hit_pin  = 0;
	int hit_mic  = 0;
	int canestri = 0;
	int d_max    = 0;
	    
	for(int iEntry=0; iEntry<nentries; iEntry++){
	  tree->GetEntry(iEntry);

	  //quantità in micron
	  pos_x *=unit;  pos_y *=unit;  pos_z *=unit;

	  //ho colpito il MiC
	  if(pos_x == x_tes){
	    hit_mic++;

	    int dist = sqrt(pos_y*pos_y+pos_z*pos_z);
	    if (dist>d_max){d_max = dist;}
		
	    if(pos_y<tes_side*0.5 && pos_y>-tes_side*0.5 &&
	       pos_z<tes_side*0.5 && pos_z>-tes_side*0.5 ){
	      canestri++;
	    } //canestro
	  } //MiC
	      
	}//for sulle entries

	if(hit_mic==0){hit_mic=1;}
	
	gr_f->SetPoint(v,v_pin[v],canestri*100./hit_mic);
	gr_f->SetMarkerSize(2);
	gr_f->SetLineWidth(2);
	gr_f->SetMarkerStyle(8);
	gr_f->SetMarkerColor(colours[p]);
	gr_f->SetLineColor(colours[p]);

	gr_r->SetPoint(v,v_pin[v],d_max*100./d_wo_pin);
	gr_r->SetMarkerSize(2);
	gr_r->SetLineWidth(2);
	gr_r->SetMarkerStyle(8);
	gr_r->SetMarkerColor(colours[p]);
	gr_r->SetLineColor(colours[p]);

      }//else sul file

    }//for voltaggio

    legend_f->AddEntry(gr_f,Form("pin_radius = %d um", pin_radius[p]),"pl");
    legend_r->AddEntry(gr_r,Form("pin_radius = %d um", pin_radius[p]),"pl");
    //gr_tes->SetPoint(pin_radius.size(),0,-100);
    mg_f->Add(gr_f,"pl");
    mg_r->Add(gr_r,"pl");

   }//for raggio pin
  
    c2->cd();
    mg_f->GetYaxis()->SetRangeUser(0,105);
    //mg_f->GetXaxis()->SetRangeUser(0,110);
    mg_f->GetXaxis()->SetTitle("V_pin [V]");
    mg_f->GetYaxis()->SetTitle("strikes on TES/strikes on MiC");
    mg_f->GetHistogram()->SetTitle("% of strikes vs pinhole radius, cnt_radius = 500 um");
    mg_f->Draw("a");
    legend_f->Draw("same");
    c2->SaveAs(Form("plots/%s/canestri/focal.png",folder));
    c2->Clear();
    legend_f->Clear();


    mg_r->GetYaxis()->SetRangeUser(0,40);
    //mg_r->GetXaxis()->SetRangeUser(0,110);
    mg_r->GetXaxis()->SetTitle("V_pin [V]");
    mg_r->GetYaxis()->SetTitle("dmax on MiC/dmax without pinhole");
    mg_r->GetHistogram()->SetTitle("% of coverage wrt no-pinhole vs pinhole radius, cnt_radius = %d um");
    mg_r->Draw("a");
    legend_r->Draw("same");
    c2->SaveAs(Form("plots/%s/canestri/reduc.png",folder));
    c2->Clear();
    legend_r->Clear();
    
 

  
  
  delete(c1);
  delete(c2);
  delete(legend_f);
  delete(legend_r);
  return(0);
}

