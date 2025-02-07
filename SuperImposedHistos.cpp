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
TLegend* CreateLegend(char* fixed, float value);
std::vector<int> InputSettings(char* fixed, float value, char* folder, char* subfolder);
int GetYMax(char* folder, char* subfolder, int cnt_radius, char* fixed, float ratio);
int GetNBins(char* folder, char* subfolder, int cnt_radius, char* fixed, float ratio);

int main(int argc, char* argv[]){
  
  //parametri da input
  if( argc!= 6 ) {
    std::cout << "USAGE: ./HitsvsDistance [folder] [subfolder] [prefix] [name of fixed quantity] [value]" << std::endl;
    std::cout << "Allowed names: cnt_radius, VsuE" << std::endl;
    std::cout << "Cnt radius to be expressed in microns" << std::endl;
    std::cout << "EXAMPLE: ./SuperImposedHistos rimbalzi rimbalzi_1 rimb cnt_radius 200" << std::endl;
    exit(1);
  }

  char* folder    =      argv[1] ;
  char* subfolder =      argv[2] ;
  char* prefix    =      argv[3] ;
  char* fixed     =      argv[4] ;
  float value     = atof(argv[5]);

  std::cout << value << std::endl;
  Settings settings;
  TLegend* legend = CreateLegend(fixed,value); 
  legend->SetBorderSize(0);
  std::vector<TH1F> histos;
  
  TCanvas* c1 = new TCanvas("c","",1000,1000);
  c1->cd();

  std::vector<int> volts      = GetVoltages (folder,subfolder);
  std::vector<int> sim        = GetSimNumber(folder,subfolder);
  std::vector<int> energy     = GetEnergy   (folder,subfolder);
  std::vector<int> distance   = Getdistance (folder,subfolder);
  std::vector<float> V_su_E;
  std::vector<float> V_su_E_final;
  std::vector<int> cnt_radius_final;

  std::vector<int> cnt_radius = InputSettings(fixed, value, folder, subfolder);

  //step di 5um
  int tes_side = 60;       //um
  int unit     = 5;        //unit in simulation: 1 is 5 um
  int dist_max = 20*unit;//um
  int x_tes    = 110*unit; //um
  int counter  = -1;
  int counter_histos = 0;
 
  for(int r=0; r<cnt_radius.size(); r++){
    for(int v=0; v<volts.size(); v++){
      for(int e=0; e<energy.size(); e++){
	if(volts[v]>=energy[e]){
	  
	  //rimb_d600um_cnt15um_V30_E10-1
	  std::string name = std::string(prefix)+ "_d"
	    + std::to_string(distance[0]) + "um_cnt"
	    + std::to_string(cnt_radius[r]) + "um_V"
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

	    int nbins = GetNBins(folder,subfolder,cnt_radius[r],fixed, value);
	    std::string histname(Form("%d", counter));
	    TH1F hits(histname.c_str(),"",nbins,-5,dist_max);
	    
	    
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
		double distance = sqrt(y_pos*y_pos+z_pos*z_pos);
		hits.Fill(distance);
	      } //canestro
	    }//for sul tree
	    if(strcmp(fixed,"cnt_radius")==0 ||
	       (strcmp(fixed,"VsuE")==0 && V_su_E[counter]==value)){
	      counter_histos++;
	      histos.push_back(hits);
	      V_su_E_final.push_back(V_su_E[counter]);
	      cnt_radius_final.push_back(cnt_radius[r]);
	    }//if sulle condizioni delle variabili fissate
	    
	  } //else sull'esistenza del file
      
	}//if sulle condizioni V e E
      }//for energia
    }//for voltaggio
  }//for sui cnt

  std::cout << "There will be "<< counter_histos << " histograms superimposed" << std::endl;
  //std::cout << "BS check" << std::endl;
  //std::cout << V_su_E_final.size() << " " << histos.size() << std::endl;
  std::vector<int> color = Getcolour();
  float indexColor = 1.0*color.size()/V_su_E_final.size();
  //std::cout << indexColor << std::endl;
  c1->cd();

  bubbleSort(V_su_E_final,histos);

  int ymax = GetYMax(folder, subfolder, value, fixed, value);
  histos[0].GetYaxis()->SetTitle("Strikes");
  histos[0].GetXaxis()->SetTitle("Distance from TES [um]");
 
  if(strcmp(fixed,"cnt_radius")==0){
    histos[0].SetTitle(Form("Strikes vs distance, cnt_radius = %dum",int(value)));
  }
  else if(strcmp(fixed,"VsuE")==0){
    histos[0].SetTitle(Form("Strikes vs distance, V/E = %.2f",value));
  }
  c1->SetLeftMargin(0.15);
 
  for(int j=0; j<histos.size(); j++){
    if(j==0 || j==5 || j==8 || j==10 || j==12 || j==14 || j==16 || j==19 || j==20 || j==21 || j==23){
    histos[j].GetYaxis()->SetRangeUser(1,ymax);
    histos[j].GetXaxis()->SetRangeUser(-1,55);
    histos[j].SetLineWidth(3);
    histos[j].SetLineColor(color[int(indexColor*j)]); // Set color for the histogram line
    if(strcmp(fixed,"cnt_radius")==0){
      legend->AddEntry(&histos[j],Form("V/E = %.2f",V_su_E_final[j]),"l");
    }
    else if(strcmp(fixed,"VsuE")==0){
      legend->AddEntry(&histos[j],Form("cnt_radius = %dum",cnt_radius_final[j]),"l");
    }
    
    histos[j].Draw("same");
    legend->Draw("same");
    }//if sui voltaggi permessi
  }
  std::string outdir( Form("plots/%s/%s",folder,subfolder));
  system( Form("mkdir -p %s", outdir.c_str()) );
  //-p crea anche le parent se prima non esistono
  
  gStyle->SetOptStat(0);
  c1->SaveAs(Form("%s/SuperImposed_%s%d.png",outdir.c_str(),fixed,int(value)));
  
  
  
  delete(legend);
  delete(c1);
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

std::vector<int> InputSettings(char* fixed, float value, char* folder, char* subfolder){
  std::vector<int> cnt_radius = {0};
  if(strcmp(fixed,"cnt_radius")==0){
    if(value==15 || value==40 || value==200 || value==500){
    cnt_radius.push_back(int(value));
    }
    else{
      std::cout << "invalid radius!" << std::endl;
      exit(1);
    }
  }
  else if(strcmp(fixed,"VsuE")==0){
    cnt_radius = GetCNTs(folder, subfolder);
  }
  else{
    std::cout << "Invalid choice!"<< std::endl;
    exit(1);
  }
  return cnt_radius;
}

int GetNBins(char* folder, char* subfolder, int cnt_radius, char* fixed, float ratio){
  int nbins;

  if(strcmp(folder,"rimbalzi")==0 && strcmp(subfolder,"rimbalzi_1")==0 && strcmp(fixed,"cnt_radius")==0){
    if(cnt_radius==500){nbins = 20; }
    if(cnt_radius==200){nbins = 50; }
    if(cnt_radius== 40){nbins = 100;}
    if(cnt_radius== 15){nbins = 100; gPad->SetLogy(1); }
  }
  if(strcmp(folder,"rimbalzi")==0 && strcmp(subfolder,"rimbalzi_1")==0 && strcmp(fixed,"VsuE")==0){
    if(ratio==1   ){nbins = 30; gPad->SetLogy(1);}
    if(ratio==3   ){nbins = 50; gPad->SetLogy(1);}
    if(ratio==7   ){nbins = 50; gPad->SetLogy(1);}
    if(ratio==11  ){nbins = 50; gPad->SetLogy(1);}
    if(ratio==35  ){nbins = 50; gPad->SetLogy(1);}
    if(ratio==50  ){nbins = 50; gPad->SetLogy(1);}
    if(ratio==67.5){nbins = 50; gPad->SetLogy(1);}
  }
  
  return nbins;
}

int GetYMax(char* folder, char* subfolder, int cnt_radius, char* fixed, float ratio){
  int ymax;
  
  if(strcmp(folder,"rimbalzi")==0 && strcmp(subfolder,"rimbalzi_1")==0 && strcmp(fixed,"cnt_radius")==0){
    if(cnt_radius==500){ymax = 70;  }
    if(cnt_radius==200){ymax = 180;  }
    if(cnt_radius== 40){ymax = 1500; }
    if(cnt_radius== 15){ymax = 3600;}
  }
  if(strcmp(folder,"rimbalzi")==0 && strcmp(subfolder,"rimbalzi_1")==0 && strcmp(fixed,"VsuE")==0){
    if(ratio==1   ){ymax = 120;  }
    if(ratio==3   ){ymax = 200;  }
    if(ratio==7   ){ymax = 500;  }
    if(ratio==11  ){ymax = 1000;  }
    if(ratio==35  ){ymax = 4000;  }
    if(ratio==50  ){ymax = 7000;  }
    if(ratio==67.5){ymax = 10000;  }
  }
  
  return ymax;
}

TLegend* CreateLegend(char* fixed, float value){
  TLegend* legend;
  if(strcmp(fixed,"cnt_radius")==0){ legend = new TLegend(0.75,0.3,0.9,0.89); }
  if(strcmp(fixed,"VsuE")==0){
    if(value==1.){ legend = new TLegend(0.75,0.3,0.9,0.89); }
    else{ legend = new TLegend(0.7,0.75,0.9,0.89); }
  }
  return legend;
}
