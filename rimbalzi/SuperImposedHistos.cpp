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

void bubbleSort(std::vector<float>& v,std::vector<TH1F>& histos, std::vector<int>& sub_vec);
std::vector<int> Getcolour(void);
TLegend* CreateLegend(char* fixed, float value);
std::vector<int> InputSettings(char* fixed, float value, char* folder, char* subfolder);
float GetYMax(char* folder, char* subfolder, float value, char* fixed);
float GetYMin(char* folder, char* subfolder,char* fixed);
int GetNBins(char* folder, char* subfolder, int cnt_radius, char* fixed, float ratio);
std::vector <float> GetNorm(float binsize, float hist_min, float hist_max);

int main(int argc, char* argv[]){
  
  //parametri da input
  if( argc!= 6 ) {
    std::cout << "USAGE: ./HitsvsDistance [folder] [subfolder] [prefix] [name of fixed quantity] [value]" << std::endl;
    std::cout << "Allowed names: cnt_radius, VsuE, substrate" << std::endl;
    std::cout << "Cnt radius to be expressed in microns" << std::endl;
    std::cout << "EXAMPLE: ./SuperImposedHistos rimbalzi rimbalzi_1 rimb cnt_radius 200" << std::endl;
    exit(1);
  }

  char* folder    =      argv[1] ;
  char* subfolder =      argv[2] ;
  char* prefix    =      argv[3] ;
  char* fixed     =      argv[4] ;
  float value     = atof(argv[5]);

  
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
  std::vector<int> substrate  = GetSubstrate(folder,subfolder);
 
  std::vector<float> V_su_E;
  std::vector<float> V_su_E_final;
  std::vector<int> cnt_radius_final;
  std::vector<int> sub_vec;

  std::vector<int> cnt_radius = InputSettings(fixed, value, folder, subfolder);
  
  //step di 5um
  int tes_side = 60;       //um
  int unit     = 5;        //unit in simulation: 1 is 5 um
  int hist_min = 0;
  int hist_max = 500;//um
  int x_tes    = 110*unit; //um
  int counter  = -1;
  int counter_histos = 0;

  
  for(int b=0; b<substrate.size(); b++){
    for(int r=0; r<cnt_radius.size(); r++){
      for(int v=0; v<volts.size(); v++){
	for(int e=0; e<energy.size(); e++){
	  if(volts[v]>=energy[e]){
	     
	    //rimb_d600um_cnt15um_V30_E10-1
	    std::string name = std::string(prefix)+ "_d"
	      + std::to_string(distance[0]) + "um_cnt"
	      + std::to_string(cnt_radius[r]) + "um_sub"
	      + std::to_string(substrate[b]) + "um_V"
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
	      float y_i  , z_i  ;
	      V_su_E.push_back(float(1.0*volts[v]/energy[e]));
	     
	      tree->SetBranchAddress("x_f", &x_pos);
	      tree->SetBranchAddress("y_f", &y_pos);
	      tree->SetBranchAddress("z_f", &z_pos);
	      tree->SetBranchAddress("y_i", &y_i  );
	      tree->SetBranchAddress("z_i", &z_i  );
	      
	      //if(cnt_radius[r]==40){hist_max = 50;}
	      //if(cnt_radius[r]==15){hist_max = 20;}
	      
	      int nbins = GetNBins(folder,subfolder,cnt_radius[r],fixed, value);
	      std::string histname(Form("%d", counter));
	      TH1F hits(histname.c_str(),"",nbins,hist_min,hist_max);
	      int binsize = (hist_max-hist_min)/nbins;
	      
	      for(int iEntry=0; iEntry<nentries; iEntry++){
		tree->GetEntry(iEntry);
		x_pos *= unit;
		y_pos *= unit;
		z_pos *= unit;
		
		//canestro
		if(x_pos == x_tes &&
		   y_pos<tes_side*0.5 && y_pos>-tes_side*0.5 &&
		   z_pos<tes_side*0.5 && z_pos>-tes_side*0.5){
		  
		  y_i *= unit;
		  z_i *= unit;
		  //calcolo distanza
		  double distance = sqrt(y_i*y_i+z_i*z_i);
		  
		  hits.Fill(distance);
		  
		} //canestro
	      }//for sul tree
	      
	      
	      std::string histname_norm(Form("%d_norm", counter));
	      TH1F hits_norm(histname_norm.c_str(),"",nbins,hist_min,hist_max);
	      std::vector<float> norm = GetNorm(binsize,hist_min,hist_max);
	      
	      for(int i=0; i<nbins; i++){
		float counts = hits.GetBinContent(i+1);
	
		hits_norm.SetBinContent(i+1,(counts/norm[i]));
		hits_norm.SetBinError(i+1,sqrt(counts)/norm[i]);
	      }
	      
	      
	      if(strcmp(fixed,"cnt_radius")==0 ||
		 (strcmp(fixed,"VsuE")==0 && V_su_E[counter]==value)){
		counter_histos++;
		
		histos.push_back(hits_norm);
		V_su_E_final.push_back(V_su_E[counter]);
		cnt_radius_final.push_back(cnt_radius[r]);
		sub_vec.push_back(substrate[b]);
	      }//if sulle condizioni delle variabili fissate
	      
	    } //else sull'esistenza del file
	    
	  }//if sulle condizioni V e E
	}//for energia
      }//for voltaggio
    }//for sui cnt
  }//for substrate
  std::cout << "There will be "<< counter_histos << " histograms superimposed" << std::endl;
  
  std::vector<int> color = Getcolour();
  c1->cd();

  bubbleSort(V_su_E_final,histos,sub_vec);

  float ymax = GetYMax(folder, subfolder, value, fixed);
  float ymin = GetYMin(folder, subfolder, fixed);

  histos[0].GetYaxis()->SetTitle("Normalized strikes");
  histos[0].GetXaxis()->SetTitle("Distance from TES [um]");
 
  c1->SetLeftMargin(0.15);

  
  
 
  for(int b=0; b<substrate.size(); b++){
    int c_index=-1;

    
    for(int j=0; j<histos.size(); j++){
      
      if(substrate[b]==sub_vec[j]){
       
	//if(j==0 || j==10 || j==16 || j==19 || j==20 ){ //cnt_radius fixed
	//if(j==0 || j==5 || j==10 || j==19){ //V/E=1
	c_index++;
	if(c_index==0){histos[0].Draw("axis");}
	gStyle->SetErrorX(0.0001);
	
	histos[j].GetYaxis()->SetRangeUser(1E-5, ymax);
	histos[j].GetXaxis()->SetRangeUser(0,500);
	histos[j].SetLineWidth(3);
	histos[j].SetMarkerStyle(8);
	histos[j].SetMarkerColor(color[c_index]);
	histos[j].SetLineColor(color[c_index]); // Set color for the histogram line

	if(strcmp(fixed,"cnt_radius")==0){
	  legend->AddEntry(&histos[j],Form("V/E = %.2f",V_su_E_final[j]),"l");
	  c1->SetTitle(Form("Normalized strikes vs distance, cnt_radius = %dum, substrate = %d um",int(value),substrate[b]));
	}

	else if(strcmp(fixed,"VsuE")==0){
	  legend->AddEntry(&histos[j],Form("cnt_radius = %dum",cnt_radius_final[j]),"l");
	  c1->SetTitle(Form("Normalized strikes vs distance, V/E = %.2f, substrate = %d um",value, substrate[b]));
	}

	histos[j].Draw("same");
	legend->Draw("same");
	c1->Update();
	//}//if sui voltaggi permessi
      }//if sul substrato
    } //for sugli histo
    
    std::string outdir( Form("plots/%s/%s",folder,subfolder));
    system( Form("mkdir -p %s", outdir.c_str()) );
    //-p crea anche le parent se prima non esistono
    
    gStyle->SetOptStat(0);
    c1->SaveAs(Form("%s/SuperImposed_%s%d_sub%dum.png",outdir.c_str(),fixed,int(value),substrate[b]));
    legend->Clear();
    c1->Clear();
  } //for sul substrato
  
  delete(legend);
  delete(c1);
  return(0);
  
}

 
void bubbleSort(std::vector<float>& v,std::vector<TH1F>& histos, std::vector<int>& sub_vec) {
  int n = v.size();
  
  // Outer loop that corresponds to the number of
  // elements to be sorted
  for (int i=0; i < n-1; i++) {
     bool swapped = false;
    for (int j=0; j < n-i-1; j++) {
      if (v[j] > v[j+1]){
	std::swap(v[j], v[j+1]);
	std::swap(histos[j], histos[j+1]);
	std::swap(sub_vec[j], sub_vec[j+1]);
	swapped=true;
      }
    }
     if (!swapped) break;
  }
}



std::vector<int> Getcolour(void){
  std::vector<int> color;
  color.push_back(207);
  color.push_back(94);
  color.push_back(91);
  color.push_back(210);
  color.push_back(65);
  color.push_back(62);
  color.push_back(kViolet+6);
  
 
  
  return color;
}

std::vector<int> InputSettings(char* fixed, float value, char* folder, char* subfolder){
  std::vector<int> cnt_radius;
  
  if(strcmp(fixed,"cnt_radius")==0 && strcmp(subfolder,"rimbalzi_2")==0){
    if(value==15 || value==40 || value==100 || value==200 || value==300 || value==500){
    cnt_radius.push_back(int(value));
    }
    else{
      std::cout << "invalid radius for subfolder!" << std::endl;
      exit(1);
    }
  }
  
  if(strcmp(fixed,"cnt_radius")==0 && strcmp(subfolder,"rimbalzi_1")==0){
    if(value==15 || value==40 || value==200 || value==500){
    cnt_radius.push_back(int(value));
    }
    else{
      std::cout << "invalid radius for subfolder!" << std::endl;
      exit(1);
    }
  }

 else if(strcmp(fixed,"VsuE")==0){
    cnt_radius = GetCNTs(folder, subfolder);
  }
  /*
  else{
    std::cout << "Invalid choice!"<< std::endl;
    exit(1);
  }
  */
  return cnt_radius;
}



int GetNBins(char* folder, char* subfolder, int cnt_radius, char* fixed, float ratio){
  int nbins;

  if(strcmp(folder,"rimbalzi")==0 && strcmp(subfolder,"rimbalzi_1")==0 && strcmp(fixed,"cnt_radius")==0){
    if(cnt_radius==500){nbins = 20;  }
    if(cnt_radius==200){nbins = 50;  }
    if(cnt_radius== 40){nbins = 100; }
    if(cnt_radius== 15){nbins = 100; }
  }
  if(strcmp(folder,"rimbalzi")==0 && strcmp(subfolder,"rimbalzi_1")==0 && strcmp(fixed,"VsuE")==0){
    if(ratio==1   ){nbins = 30;} 
    if(ratio==3   ){nbins = 50;} 
    if(ratio==7   ){nbins = 50;} 
    if(ratio==11  ){nbins = 50;}
    if(ratio==35  ){nbins = 50;}
    if(ratio==50  ){nbins = 50;}
    if(ratio==67.5){nbins = 50;}
  }

  if(strcmp(folder,"rimbalzi")==0 && strcmp(subfolder,"rimbalzi_2")==0 && strcmp(fixed,"cnt_radius")==0){
    if(cnt_radius==500){nbins = 250;}
    if(cnt_radius==300){nbins = 250;}
    if(cnt_radius==200){nbins = 250;}
    if(cnt_radius==100){nbins = 250;}
    if(cnt_radius== 40){nbins = 250;}
    if(cnt_radius== 15){nbins = 250;}
  }
  if(strcmp(folder,"rimbalzi")==0 && strcmp(subfolder,"rimbalzi_2")==0 && strcmp(fixed,"VsuE")==0){
    if(ratio==1   ){nbins = 100;} 
    if(ratio==1.25){nbins = 100;} 
    if(ratio==2   ){nbins = 100;} 
    if(ratio==4   ){nbins = 100;}
    if(ratio==10  ){nbins = 100;}
    if(ratio==20  ){nbins = 100;}
    if(ratio==50  ){nbins = 100;}
  }
  
  return nbins;
}

float GetYMax(char* folder, char* subfolder, float value, char* fixed){
  float ymax;
  
  if(strcmp(subfolder,"rimbalzi")==0 && strcmp(subfolder,"rimbalzi_1")==0 && strcmp(fixed,"cnt_radius")==0){
    if(int(value)==500){ymax = 0.06;  }
    if(int(value)==200){ymax = 0.43;  }
    if(int(value)== 40){ymax = 8.;    }
    if(int(value)== 15){ymax = 51;    }
  }
  if(strcmp(folder,"rimbalzi")==0 && strcmp(subfolder,"rimbalzi_1")==0 && strcmp(fixed,"VsuE")==0){
    gPad->SetLogy(1);
    if(value==1   ){ymax = 1.4;}
    if(value==3   ){ymax = 3;  }
    if(value==7   ){ymax = 10; }
    if(value==11  ){ymax = 15; }
    if(value==35  ){ymax = 65; }
    if(value==50  ){ymax = 100;}
    if(value==67.5){ymax = 100;}
  }

  if(strcmp(folder,"rimbalzi")==0 && strcmp(subfolder,"rimbalzi_2")==0 && strcmp(fixed,"cnt_radius")==0){
    
    if(int(value)==500){ymax = 0.2;}
    if(int(value)==300){ymax = 0.5;}
    if(int(value)==200){ymax = 0.7;}
    if(int(value)==100){ymax = 1.5;}
    if(int(value)== 40){ymax = 8.;  }
    if(int(value)== 15){ymax = 45;  }
  }
  
  if(strcmp(folder,"rimbalzi")==0 && strcmp(subfolder,"rimbalzi_2")==0 && strcmp(fixed,"VsuE")==0){
    gPad->SetLogy(1);
    if(value==1   ){ymax = 100;}
    if(value==1.25){ymax = 70;  }
    if(value==2   ){ymax = 70; }
    if(value==4   ){ymax = 70; }
    if(value==10  ){ymax = 100; }
    if(value==20  ){ymax = 100;}
    if(value==50  ){ymax = 100;}
  }
 
  return ymax;
  
}

float GetYMin(char* folder, char* subfolder, char* fixed){
  float ymin;

  if(strcmp(fixed,"cnt_radius")==0){ymin = 0;     }
  if(strcmp(fixed,"VsuE"      )==0){ymin = 0.0005;}
  return ymin;
}



TLegend* CreateLegend(char* fixed, float value){
  TLegend* legend;
  if(strcmp(fixed,"cnt_radius")==0){ legend = new TLegend(0.7,0.65,0.9,0.89); }
  if(strcmp(fixed,"VsuE")==0){ legend = new TLegend(0.65,0.75,0.9,0.89); }
   
  return legend;
}

//devo avere l'area del settore circolare per rinormalizzare
std::vector<float> GetNorm(float binsize, float hist_min, float hist_max){
  std::vector<float> norm;
  int nsteps = (hist_max-hist_min)/binsize;
  
  for(int i=0; i<nsteps; i++){
    float radius1 = hist_min+(i+0)*binsize;
    float radius2 = hist_min+(i+1)*binsize;
   
    norm.push_back(M_PI*(radius2*radius2-radius1*radius1));
  }
  return norm;
}
