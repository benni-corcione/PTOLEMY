#include "TTree.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TStyle.h"
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
void bubbleSort(std::vector<double>& v_e, std::vector<int>& cnts, std::vector<double>& hits);
std::vector<int> GetColors(void);
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

  //quantità variabili da ciclare
  std::vector<int> volts      = GetVoltages (folder,subfolder);
  std::vector<int> sim        = GetSimNumber(folder,subfolder);
  std::vector<int> energy     = GetEnergy   (folder,subfolder);
  std::vector<int> cnt_radius = GetCNTs     (folder,subfolder);
  std::vector<int> distance   = Getdistance (folder,subfolder);

  //quantità per futuri grafici comparativi
  std::vector<int> cnt_vec;
  std::vector<double> canestro;
  std::vector<double> VsuE_vec;
  
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
	    if(volts[v]>=energy[e]){
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

		//histo casistica per casistica della distanza per colpire
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
	      
		//c1->SaveAs(Form("%s/histo_hitstrike/%s_hitsdist.png",outdir.c_str(),name.c_str()));
		c1->Clear();

		//salvo numero di canestri, V/E e cnt_radius
		canestro.push_back(hits->GetEntries());
		cnt_vec.push_back(cnt_radius[r]);
		VsuE_vec.push_back(1.*volts[v]/energy[e]);
	      
		delete(hits);
	      }
	    } //else sull'esistenza del file
	  }//for energia
	}//for voltaggio					
      }//for cnts_radius
    }//for sim
  }//for distanze
 
  //lavoro su grafici comparativi a partire delle quantità che ho salvato, che rendo array
 
  
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
  mg->GetYaxis()->SetRangeUser(1,1E+5);
  mg->GetXaxis()->SetTitle("V/E");
  mg->GetYaxis()->SetTitle("Canestri");
  mg->GetHistogram()->SetTitle("Canestri vs V/E");
  mg->Draw("a");
  legend->Draw("same");
  c1->SaveAs(Form("plots/%s/%s/canestri/HitsvsVE.png",folder,subfolder));
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

  
  for(int v=0; v<VsuE_list.size(); v++){
    //ciclo sui singoli punti acquisiti a parità di cnt_radius
    TGraph hitscnts(0);
    for(int i=0; i<Npoints; i++){
      if(VsuE_vec[i]==VsuE_list[v]){
	hitscnts.SetPoint(i+1,cnt_vec[i],canestro[i]);
      }//for sugli Npoints da scorrere
    }
    hitscnts.SetTitle(Form("Canestri con V/E = %.2f",VsuE_list[v]));
    hitscnts.GetXaxis()->SetTitle("cnts radius [um]");

    hitscnts.GetYaxis()->SetRangeUser(1, 52000);
    hitscnts.GetYaxis()->SetTitle("Canestri");
    hitscnts.SetMarkerStyle(8);
    hitscnts.SetMarkerSize(2);
    hitscnts.SetMarkerColor(kTeal-7);
    hitscnts.Draw("ap");
    c1->SaveAs(Form("plots/%s/%s/canestri/Hitsvscnts_VE%.2f.png",folder,subfolder,VsuE_list[v]));
    c1->Clear();
    legend->Clear();
  }//for sui nanotubi
  
  //+++++++++++++++++++++++++++++++++++++++++++++//
  //grafico complessivo
 
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
    legend2->AddEntry(hitscnts,Form("V/E = %.2f", VsuE_list[r]),"pl");
    mg2->Add(hitscnts,"pl");
    
    }
  
  c1->SetLogy();
  c1->SetLeftMargin(0.11);
  c1->SetRightMargin(0.16);

  mg2->GetYaxis()->SetRangeUser(1,6E+4);
  mg2->GetXaxis()->SetTitle("cnt radius [um]");
  mg2->GetYaxis()->SetTitle("Canestri");
  mg2->GetHistogram()->SetTitle("Canestri vs cnt_radius");
  mg2->Draw("a");
  legend2->Draw("same");
  c1->SaveAs(Form("plots/%s/%s/canestri/Hitsvscnts.png",folder,subfolder));
  c1->Clear();
  
  
    
  delete(c1);
  return(0);
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
