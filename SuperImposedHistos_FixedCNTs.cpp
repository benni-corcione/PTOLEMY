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
#include "graphics.h"

void bubbleSort(std::vector<float>& v,std::vector<TH1F>& histos);
std::vector<int> Getcolour(void);
int main(int argc, char* argv[]){
  
  //parametri da input
  if( argc!= 5 ) {
    std::cout << "USAGE: ./HitsvsDistance [folder] [subfolder] [prefix] [cnt_radius]" << std::endl;
    std::cout << "CNTs radius in microns" << std::endl;
    std::cout << "EXAMPLE: ./HitsvsDistance rimbalzi rimbalzi_1 rimb 200" << std::endl;
    exit(1);
  }

  char* folder    =      argv[1] ;
  char* subfolder =      argv[2] ;
  char* prefix    =      argv[3] ;   
  int cnt_radius  = atoi(argv[4]);

  Settings settings;
  TLegend* legend = new TLegend(0.75,0.3,0.9,0.89);
  legend->SetBorderSize(0);
  std::vector<int> color = Getcolour();
  std::vector<TH1F> histos;
  
  TCanvas* c1 = new TCanvas("c","",1000,1000);
  c1->cd();

  std::vector<int> volts      = GetVoltages (folder,subfolder);
  std::vector<int> sim        = GetSimNumber(folder,subfolder);
  std::vector<int> energy     = GetEnergy   (folder,subfolder);
  std::vector<int> distance   = Getdistance (folder,subfolder);

  //step di 5um
  int tes_side = 60;       //um
  int unit     = 5;        //unit in simulation: 1 is 5 um
  int dist_max = 20*unit;//um
  int x_tes    = 110*unit; //um
  int counter  = 0;
  std::vector<float> V_su_E;
    
  for(int v=0; v<volts.size(); v++){
    for(int e=0; e<energy.size(); e++){
      if(volts[v]>=energy[e]){

	//rimb_d600um_cnt15um_V30_E10-1
      std::string name = std::string(prefix)+ "_d"
	+ std::to_string(distance[0]) + "um_cnt"
	+ std::to_string(cnt_radius) + "um_V"
	+ std::to_string(volts[v]) + "_E"
	+ std::to_string(energy[e]) + "-"
	+ std::to_string(sim[0]);
      
      //apertura file con il tree
      TFile run(Form("root/%s/%s/%s.root",folder,subfolder,name.c_str()));
      if (!run.IsOpen()) {;}
      else{
	counter++;

	TTree *tree = (TTree*)run.Get(Form("tree"));
	Long64_t nentries = tree->GetEntries();
	float x_pos, y_pos, z_pos;
	V_su_E.push_back(float(1.0*volts[v]/energy[e]));
	
	tree->SetBranchAddress("x_f", &x_pos);
	tree->SetBranchAddress("y_f", &y_pos);
	tree->SetBranchAddress("z_f", &z_pos);
	
	std::string histname(Form("%d", counter));
	TH1F hits(histname.c_str(),"",50,-5,dist_max);
	
	for(int iEntry=0; iEntry<nentries; iEntry++){
	  tree->GetEntry(iEntry);
	  x_pos *= unit;
	  y_pos *= unit;
	  z_pos *= unit;
	  
	  //canestro
	  if(x_pos == x_tes &&
	     y_pos<tes_side && y_pos>-tes_side &&
	     z_pos<tes_side && z_pos>-tes_side){
	    
	    //calcolo distanza
	    double distance = sqrt(y_pos*y_pos+z_pos*z_pos);
	    hits.Fill(distance);
	  } //canestro
	}//for sul tree

	histos.push_back(hits);
       } //else sull'esistenza del file
      
      }//if sulle condizioni V e E
    }//for energia
  }//for voltaggio

  c1->cd();
  //gPad->SetLogy(1);

  bubbleSort(V_su_E,histos);
  
  histos[1].GetYaxis()->SetTitle("Strikes");
  histos[1].GetXaxis()->SetTitle("Distance from TES [um]");
  histos[1].SetTitle(Form("Strikes vs distance, cnt_radius = %dum",cnt_radius));
  histos[1].GetYaxis()->SetRangeUser(1,3000);
  histos[1].GetXaxis()->SetRangeUser(0,100);
  c1->SetLeftMargin(0.15);
  for(int j=0; j<histos.size(); j++){
    if(j==1 || j==5 || j==8 || j==10 || j==12 || j==14 || j==16 || j==19 || j==20 || j==21 || j==23){
    
    histos[j].SetLineWidth(3);
    histos[j].SetLineColor(color[j]); // Set color for the histogram line

    legend->AddEntry(&histos[j],Form("V/E = %.2f",V_su_E[j]),"l");
    histos[j].Draw("same");
    legend->Draw("same");
    }//if sui voltaggi permessi
  }
  std::string outdir( Form("plots/%s/%s",folder,subfolder));
  system( Form("mkdir -p %s", outdir.c_str()) );
  //-p crea anche le parent se prima non esistono
  
  gStyle->SetOptStat(0);
  c1->SaveAs(Form("%s/hist_VonE_%dCNTs_some.png",outdir.c_str(),cnt_radius));
  
  
  
  //delete(legend);
  //delete(c1);
  return(0);
  
}

 
void bubbleSort(std::vector<float>& v,std::vector<TH1F>& histos) {
  int n = v.size();
  
  // Outer loop that corresponds to the number of
  // elements to be sorted
  for (int i=0; i < n-1; i++) {
     bool swapped = false;
    for (int j=0; j < n-i-1; j++) {
      if (v[j] > v[j+1]){
	std::swap(v[j], v[j+1]);
	std::swap(histos[j], histos[j+1]);
	swapped=true;
      }
    }
     if (!swapped) break;
  }
}



std::vector<int> Getcolour(void){
  std::vector<int> color;
  color.push_back(kYellow);
  color.push_back(90);
  color.push_back(91);
  color.push_back(92);
  color.push_back(95);
  color.push_back(85);
  color.push_back(80);
  color.push_back(75);
  color.push_back(72);
  color.push_back(71);
  color.push_back(70);
  color.push_back(68);
  color.push_back(67);
  color.push_back(66);
  color.push_back(65);
  color.push_back(64);
  color.push_back(63);
  color.push_back(62);
  color.push_back(54);
  color.push_back(53);
  color.push_back(52);
  color.push_back(51);
  color.push_back(kMagenta+2);
  color.push_back(kMagenta-3);
  
  
  

  return color;
}
