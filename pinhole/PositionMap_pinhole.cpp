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

void DrawPad(TH2F* map,int range, int tes_side, int cnt_radius, int pin_radius);
int GetPinRange(int cnt_radius);
int GetMiCRange(int pin_radius);

int main(int argc, char* argv[]){
  
  //parametri da input
  if( argc!= 3 ) {
    std::cout << "USAGE: ./PositionMap [folder] [prefix]" << std::endl;
    std::cout << "EXAMPLE: ./PositionMap pin1 pin" << std::endl;
    exit(1);
  }
  
  char* folder = argv[1];
  char* prefix = argv[2];   

  std::string save_fold(Form("plots/%s",folder));
  system( Form("mkdir -p %s", save_fold.c_str()) );

  //preparazione per la lettura del tree_raw
  float pos_x , pos_y , pos_z ;
  float pos_xi, pos_yi, pos_zi;
  float ke_i, ke_f;

  std::vector<int> volts_cnt  = GetVcnt(folder);
  std::vector<int> cnt_radius = GetCNTs(folder);
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
  std::vector<int> colours = {99,95,92,8,66,62};
  
  int z_max      = 8000; //um
  int y_max      = 8000; //um
  int unit       = 5;    //1step in 5 um
  int tes_side   = 60;   //um
  int pixel_side = 10;
  int x_tes      = 530*unit;
  int x1_pin     = 325*unit;
  int x2_pin     = 330*unit;
  int nbins      = 2*z_max/pixel_side;

  //max distance without pinhole
  float d_wo_pin = 0; //in um
  
  for(int r=0; r<cnt_radius.size(); r++){
    TMultiGraph* mg_f = new TMultiGraph();
    TMultiGraph* mg_r = new TMultiGraph();

    if(cnt_radius[r]==500){d_wo_pin = 800;}
    if(cnt_radius[r]==100){d_wo_pin = 380;}
    if(cnt_radius[r]==50 ){d_wo_pin = 340;}
    
    for(int v=0; v<volts_cnt.size(); v++){
      TGraph* gr_f = new TGraph(); //focalizzazione
      TGraph* gr_r = new TGraph(); //riduzione
       
      for(int p=0; p<pin_radius.size();  p++){
	
	if(pin_radius[p]<cnt_radius[r]){
	  //pin_cnt50um_pin100um_Vcnt90_Vpin10
	  std::string name = std::string(prefix)+ "_cnt"
	    + std::to_string(cnt_radius[r]) + "um_pin"
	    + std::to_string(pin_radius[p]) + "um_Vcnt"
	    + std::to_string(volts_cnt[v]) + "_Vpin"
	    + std::to_string(100-volts_cnt[v]);
	  
	  //apertura file con il tree
	  TFile run(Form("root/%s/%s.root",folder,name.c_str()));
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
	    
	    int hit_pin  = 0;
	    int hit_mic  = 0;
	    int canestri = 0;
	    float d_max    = 0;
	    int range_pin = GetPinRange(cnt_radius[r]);
	    int range_mic = GetMiCRange(pin_radius[p]);

	  
	    //mappa arrivi e partenze
	    //TH2F* map_pin = new TH2F("map_pin","",nbins,yBins,nbins,zBins);
	    TH2F* map_mic = new TH2F("map_mic","",nbins,yBins,nbins,zBins);

	    /*
	      TPad* pad1 = new TPad("pad1","left pad",0,0,0.49,1);
	      pad1->Draw("same");
	      TPad* pad2 = new TPad("pad2","right pad",0.51,0,1,1);
	      pad2->Draw("same");
	      pad1->SetLeftMargin (0.15);
	      pad1->SetRightMargin (0.12);
	      pad1->SetBottomMargin (0.12);
	      pad2->SetRightMargin(0.15);
	      pad2->SetLeftMargin(0.12);
	    */
	    for(int iEntry=0; iEntry<nentries; iEntry++){
	      tree->GetEntry(iEntry);

	      //quantità in micron
	      pos_x *=unit;  pos_y *=unit;  pos_z *=unit;
	      pos_xi*=unit;  pos_yi*=unit;  pos_zi*=unit;

	      //ho colpito il pinhole
	      if(pos_x==x1_pin || pos_x==x2_pin){ 
		hit_pin++;
		//map_pin->Fill(pos_y,pos_z);
	      }
	      
	      //ho colpito il MiC
	      if(pos_x == x_tes){
		hit_mic++;
		map_mic->Fill(pos_y,pos_z);

		float dist = sqrt(pos_y*pos_y+pos_z*pos_z);
		if (dist>d_max){d_max = dist;}
		
		if(pos_y<tes_side*0.5 && pos_y>-tes_side*0.5 &&
		   pos_z<tes_side*0.5 && pos_z>-tes_side*0.5 ){
		  canestri++;
		} //canestro
	      } //MiC
	      
	    }//for sulle entries

	    //gr_mic.SetPoint(dg,dist_gen[dg],hits*100./nentries);
	  
	    gStyle->SetPalette(kBird);

	  
	    //disegna a dx
	    //pad1->cd();
	    //map_pin->Draw("axis");
	    //map_pin->SetTitle(Form("cnt: %d um, pin: %d um, V_cnt=-%d V", cnt_radius[r],pin_radius[p], volts_cnt[v]));
	    //DrawPad(map_pin,range_pin,tes_side,cnt_radius[r],pin_radius[p]);
	     
	    //disegno a sx
	    //pad2->cd();
	    /*
	      c1->SetLeftMargin(0.13);
	      c1->SetRightMargin(0.13);
	      map_mic->Draw("axis");
	      map_mic->SetTitle(Form("%.2f%% tes strikes wrt mic  strikes",canestri*100./hit_mic));
	      DrawPad(map_mic,range_pin,tes_side,cnt_radius[r],pin_radius[p]);
	    
	      std::string save_fold(Form("plots/%s",folder));
	      std::string save_subfold(Form("plots/%s/maps",folder));
	      system( Form("mkdir -p %s", save_fold.c_str()) );
	      system( Form("mkdir -p %s", save_subfold.c_str()));
	   
	      c1->SaveAs(Form("plots/%s/maps/%s_map.png",folder,name.c_str()));
	      c1->Clear();
	    */
	  
	    gr_f->SetPoint(p,pin_radius[p],canestri*100./hit_mic);
	    gr_f->SetMarkerSize(2);
	    gr_f->SetLineWidth(2);
	    gr_f->SetMarkerStyle(8);
	    gr_f->SetMarkerColor(colours[v]);
	    gr_f->SetLineColor(colours[v]);

	    gr_r->SetPoint(p,pin_radius[p],d_max*1./pin_radius[p]);
	    gr_r->SetMarkerSize(2);
	    gr_r->SetLineWidth(2);
	    gr_r->SetMarkerStyle(8);
	    gr_r->SetMarkerColor(colours[v]);
	    gr_r->SetLineColor(colours[v]);

          

	  
	  }//else sul file

	}//for sulla condizione del pinhole
      }//for raggio pin
      legend_f->AddEntry(gr_f,Form("V_cnt = - %d V", volts_cnt[v]),"pl");
      legend_r->AddEntry(gr_r,Form("V_cnt = - %d V", volts_cnt[v]),"pl");
      //gr_tes->SetPoint(pin_radius.size(),0,-100);
      mg_f->Add(gr_f,"pl");
      mg_r->Add(gr_r,"pl");
    }//for volts cnt
     
     
  
    c2->cd();
    mg_f->GetYaxis()->SetRangeUser(0,105);
    //mg_f->GetXaxis()->SetRangeUser(0,110);
    mg_f->GetXaxis()->SetTitle("pinhole radius [um]");
    mg_f->GetYaxis()->SetTitle("strikes on TES/strikes on MiC");
    mg_f->GetHistogram()->SetTitle(Form("%% of strikes vs pinhole radius, cnt_radius = %d um",cnt_radius[r]));
    mg_f->Draw("a");
    legend_f->Draw("same");
    c2->SaveAs(Form("plots/%s/canestri/cnt_radius%d_f.png",folder,cnt_radius[r]));
    c2->Clear();
    legend_f->Clear();


    mg_r->GetYaxis()->SetRangeUser(0,40);
    //mg_r->GetXaxis()->SetRangeUser(0,110);
    mg_r->GetXaxis()->SetTitle("pinhole radius [um]");
    mg_r->GetYaxis()->SetTitle("dmax on MiC/pinhole_radius %%");
    mg_r->GetHistogram()->SetTitle(Form("%% of coverage wrt pinhole vs pinhole radius, cnt_radius = %d um",cnt_radius[r]));
    mg_r->Draw("a");
    legend_r->Draw("same");
    c2->SaveAs(Form("plots/%s/canestri/cnt_radius%d_r.png",folder,cnt_radius[r]));
    c2->Clear();
    legend_r->Clear();
    
  }//for raggio cnt

  
  
  delete(c1);
  delete(c2);
  delete(legend_f);
  delete(legend_r);
  return(0);
}

void DrawPad(TH2F* map,int range, int tes_side, int cnt_radius, int pin_radius){
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
  TEllipse* ell_cnt = new TEllipse(0,0,cnt_radius,0,360,0);
  ell_cnt->SetLineWidth(4);
  ell_cnt->SetFillColor(0);
  ell_cnt->SetFillStyle(0);

  //pinhole
  TEllipse* ell_pin = new TEllipse(0,0,pin_radius,0,360,0);
  ell_pin->SetLineWidth(4);
  ell_pin->SetFillColor(0);
  ell_pin->SetFillStyle(0);
  ell_pin->SetLineColor(kPink+8);
  
  //map->SetTitleSize(11,"t");
  map->GetXaxis()->SetTitle("y axis (um)");
  map->GetYaxis()->SetTitle("z axis (um)");
  map->GetYaxis()->SetRangeUser(-range,range);
  map->GetXaxis()->SetRangeUser(-range,range);
  map->Draw("axis");
  map->Draw("COLZsame");
  line1->Draw("same");
  line2->Draw("same");
  line3->Draw("same");
  line4->Draw("same");
  ell_cnt->Draw("same");
  ell_pin->Draw("same");
}

int GetPinRange(int cnt_radius){
  int range;
  if(cnt_radius==50 ){range = 75;}
  if(cnt_radius==100){range = 150;}
  if(cnt_radius==500){range = 550;}
  return range;
}

int GetMiCRange(int pin_radius){
  int range;
  if(pin_radius==10 ){range = 200;}
  if(pin_radius==25 ){range = 250;}
  if(pin_radius==50 ){range = 300;}
  if(pin_radius==100){range = 400;}
  return range;
}

