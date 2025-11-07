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
#include "TGraphErrors.h"
#include "TMultiGraph.h"

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
Double_t CrystalBall(Double_t *x, Double_t *par);
float GetFitMin(int cnt_radius,int dist_gen);
float GetFitMax(int cnt_radius,int dist_gen);
int GetDistMax(int cnt_radius);
int GetNbins(int cnt_radius,int dist_gen);
float GetYMax(int cnt_radius);
void SPL(TF1* fitfunction, int cnt_radius, int dist_gen);
int GetBins(int cnt_radius,int dist_gen);
	  
int main(int argc, char* argv[]){
  
  //parametri da input
  if( argc!= 4 ) {
    std::cout << "USAGE: ./EnergyDistance [folder] [subfolder] [prefix]" << std::endl;
    std::cout << "cnt_radius to be expressed in um" << std::endl;
    std::cout << "EXAMPLE: ./EnergyDistance rimbalzi rimbalzi_3 rimb" << std::endl;
    exit(1);
  }
  
  char* folder     =      argv[1];
  char* subfolder  =      argv[2];
  char* prefix     =      argv[3];

  std::vector<int> cnt_radius = GetCNTs(folder,subfolder);

  std::string save_fold(Form("plots/%s",folder));
  std::string save_subfold(Form("plots/%s/%s",folder,subfolder));
  system( Form("mkdir -p %s", save_fold.c_str()) );
  system( Form("mkdir -p %s", save_subfold.c_str()));

  //preparazione per la lettura del tree_raw
  float pos_x , pos_y , pos_z ;
  float pos_xi, pos_yi, pos_zi;
  float ke_i, ke_f;

  //parametri standard non variati mai
  //int volts     = 100;
  //int distance  = 600; //um
  //int substrate = 4000; //raggio di 0.4 cm

  std::vector<int> dist_gen = GetDistGen(folder,subfolder);

  TCanvas* c1 = new TCanvas("c1","",2000,1000);
  TCanvas* c2 = new TCanvas("c2","",1000,1000);
  c1->cd();
  //gStyle->SetOptStat(0);
  
  int unit       = 5; //1step in 5 um
  int tes_side   = 60; // um
  int x_tes      = 110*unit; 

  TGraph gr_hits(0);
  TGraph gr_strikes(0);
  TGraph gr_strikes2(0);

  TMultiGraph mg; //più plot insieme
  int colour[6]= {kRed-7,94,91,kGreen-6,kAzure+7,kViolet-9};
  TLegend* legend = new TLegend(0.6,0.12,0.9,0.3);
  //TLegend* legend = new TLegend(0.6,0.70,0.9,0.88);
  legend->SetBorderSize(0);
  
  for(int c=0; c<cnt_radius.size(); c++){

    TGraphErrors* gr_mu = new TGraphErrors(0);
    c1->cd();
    for(int dg=0; dg<dist_gen.size(); dg++){
      
      //rimb_cnt15um_dist500um
      std::string name = std::string(prefix)+ "_cnt"
	+ std::to_string(cnt_radius[c]) + "um_dist"
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
	      
	int hits = 0;
	int discesa = 0;
	int nbins = GetBins(cnt_radius[c],dist_gen[dg]);
      
	TH1F* energy = new TH1F("energy","",nbins,0,100);
	TH1F* splats = new TH1F("splats","",200,0,100);

	for(int iEntry=0; iEntry<nentries; iEntry++){
	  tree->GetEntry(iEntry);
	  //quantità in micron
	  pos_x *=unit;  pos_y *=unit;  pos_z *=unit;
	  pos_xi*=unit;  pos_yi*=unit;  pos_zi*=unit;

	  //canestro
	  if(pos_x == x_tes){
	    discesa++;
	    if(pos_y<tes_side*0.5 && pos_y>-tes_side*0.5 &&
	       pos_z<tes_side*0.5 && pos_z>-tes_side*0.5 ){
	      hits++;
	      energy->Fill(ke_i);
	    }//if canestro
	  }//if discesa

	  if(pos_x != x_tes){
	    splats->Fill(ke_i);
	  }

	}//for sulle entries
      
	gr_strikes.SetPoint(dg,dist_gen[dg],hits*100./nentries);
	std::cout << "percentuale di elettroni scesi: " << discesa*100./nentries <<  " %" << std::endl;
	/*
	float fit_min = GetFitMin(cnt_radius[c],dist_gen[dg]);
	float fit_max = GetFitMax(cnt_radius[c],dist_gen[dg]);
	
	TF1* fitfunction = new TF1("fitfunction",Cruijff,fit_min,fit_max,5);
	double par[5];
	fitfunction->SetParameters(par);
	fitfunction->SetLineColor(kRed-4);
	fitfunction->SetLineWidth(4);
	fitfunction->SetNpx(1000);

	//set par limits
	SPL(fitfunction, cnt_radius[c], dist_gen[dg]);
	*/
	//energy->Draw("axis");
	
        energy->Draw();
	//energy->Fit("fitfunction","RmXq","",fit_min,fit_max);
	energy->GetXaxis()->SetTitle("Energy (eV)");
	energy->GetYaxis()->SetTitle("Counts");
	energy->SetLineWidth(2);
	energy->SetLineColor(53);
	gr_mu->SetPoint(dg,dist_gen[dg],energy->GetMean());
	gr_mu->SetPointError(dg,0,energy->GetMeanError());

      
      
	std::string save_fold(Form("plots/%s/%s/energy",folder,subfolder));
	std::string save_subfold(Form("plots/%s/%s/energy/%dum",folder,subfolder,cnt_radius[c]));
	system( Form("mkdir -p %s", save_fold.c_str()) );
	system( Form("mkdir -p %s", save_subfold.c_str()) );
      
	c1->SaveAs(Form("plots/%s/%s/energy/%dum/%s_energy.png",folder,subfolder,cnt_radius[c],name.c_str()));
	c1->Clear();
	
	splats->SetTitle("Spectrum of splats on CNTs plate");
	splats->GetXaxis()->SetTitle("Energy (eV)");
	splats->GetYaxis()->SetTitle("Counts");
	splats->SetLineColor(53);
	splats->SetLineWidth(2);
	splats->Draw();
	// c1->SaveAs(Form("plots/%s/%s/energy/%s_splats.png",folder,subfolder,name.c_str()));
      
      }//else sul file
    }//for dist_gen

    //punti comodi per gli assi
    int Npoints = gr_mu->GetN();
    int dmax = GetDistMax(cnt_radius[c]);
    gr_mu->SetPoint(Npoints,-1,-50);
    gr_mu->SetPointError(Npoints,0,0);
    gr_mu->SetPoint(Npoints+1,810,-50);
    gr_mu->SetPointError(Npoints+1,0,0);
	
    float ymax = GetYMax(cnt_radius[c]);
    c2->cd();
    c2->SetLeftMargin(0.13);
    gStyle->SetTitleFontSize(0.07);
    gr_mu->SetTitle("#mu");
    gr_mu->GetXaxis()->SetTitle("Initial distance");
    gr_mu->GetYaxis()->SetTitle("Energy peak [eV]");
    gr_mu->GetYaxis()->SetRangeUser(-2,100);
    gr_mu->GetXaxis()->SetRangeUser(-2,810);
    gr_mu->SetMarkerSize(1.5);
    gr_mu->SetMarkerStyle(8);
    gr_mu->SetLineWidth(2);
    gr_mu->SetMarkerColor(colour[c]);
    gr_mu->SetLineColor(colour[c]);
    legend->AddEntry(gr_mu,Form("cnt_radius = %d um", cnt_radius[c]),"pe");
    mg.Add(gr_mu,"pe");

  }//for cnt_radius
  
  //c1->SetLogx();
  c2->SetLeftMargin(0.11);
  mg.GetYaxis()->SetRangeUser(-2,100);
  mg.GetXaxis()->SetRangeUser(-10,810);
  mg.GetXaxis()->SetTitle("initial distance [um]");
  mg.GetYaxis()->SetTitle("Energy peak [eV]");
  mg.GetHistogram()->SetTitle("Energy of strikes vs initial distance");
  mg.Draw("axis");
  mg.Draw("pe same");
  legend->Draw("same");
  c2->SaveAs(Form("plots/%s/%s/energy.png",folder,subfolder));
  c2->Clear();
  
  
    /*
    //punti comodi per gli assi
    int Npoints2 = gr_hits.GetN();
    gr_strikes.SetPoint(Npoints2,-1,-10);
    gr_strikes.SetPoint(Npoints2,810,-10);
    gr_strikes.GetXaxis()->SetRangeUser(-1,710);
    gr_strikes.SetTitle("Percentage of strikes on the TES");
    gr_strikes.GetXaxis()->SetTitle("Initial distance");
    gr_strikes.GetYaxis()->SetTitle("% strikes");
    gr_strikes.GetYaxis()->SetRangeUser(-1,10);
    gr_strikes.SetMarkerSize(2);
    gr_strikes.SetLineWidth(4);
    gr_strikes.SetMarkerStyle(20);
    gr_strikes.SetMarkerColor(46);
    gr_strikes.SetLineColor(46);
  
    gStyle->SetTitleFontSize(0.07);
    gr_strikes.Draw("AP");
  
    c2->SaveAs(Form("plots/%s/%s/energy/%dum/energydist_strikes.png",folder,subfolder,cnt_radius[c]));
    c2->Clear();
    */

  //delete(gr_mu);
  return(0);
}


Double_t CrystalBall(Double_t *x, Double_t *par) {
  // par[0] = media (mean)
  // par[1] = deviazione standard (sigma)
  // par[2] = parametro alpha (forma della coda)
  // par[3] = parametro n (decadimento della coda)
  // par[4] = parametro di normalizzazione per la parte Lorentziana
    
  //parte Lorentziana
  if (*x > par[0] + par[2] * par[1]) {
    return par[4] * TMath::Power((par[3] / TMath::Abs(par[2])) - (*x - par[0]) / par[1], -par[3]);
  }
  //parte Gaussiana
  else {
    return TMath::Exp(-0.5 * TMath::Power((*x - par[0]) / par[1], 2));
  }
}

Double_t Landau(Double_t *x, Double_t *par) {
  Double_t t = x[0];  // variabile di integrazione
  Double_t val = TMath::Exp(-t - par[0]*par[0] / (4.0 * t));  // Formula dell'integrale
  return val;
}


//cruiff (che può diventare una gaussiana) da fittare
Double_t Cruijff(Double_t *x, Double_t *par){
  //par[0]=mean      par[1]=sigma_L    par[2]=sigma_R
  //par[3]=alpha_R   par[4]=A          par[5]=alpha_L
  Double_t fitval = 0;
  Double_t arg_L  = 0;
  Double_t arg_R  = 0;
  Double_t num    = pow((x[0]-par[0]),2);
  Double_t denL   = 2*par[1]*par[1];
  Double_t denR   = 2*par[2]*par[2];
  Double_t asymR  = par[3]*num;
  Double_t asymL  = 0;  
  if ( denR + asymR != 0 ){ arg_R = -num/(denR+asymR); }
  if ( denL + asymL != 0 ){ arg_L = -num/(denL+asymL); }

  if       ( x[0] > par[0] ){ fitval = par[4] * exp(arg_R); } //LEFT
  else if  ( x[0] < par[0] ){ fitval = par[4] * exp(arg_L); } //RIGHT

  return(fitval);
}

float GetFitMin(int cnt_radius,int dist_gen){
  float fit_min=0;
  if(cnt_radius==15){
    if(dist_gen<50  ){ fit_min = 0;     }
    if(dist_gen==50 ){ fit_min = 2;     }
    if(dist_gen==75 ){ fit_min = 3;     }
    if(dist_gen==100){ fit_min = 4.5;   }
    if(dist_gen==150){ fit_min = 9;     } 
    if(dist_gen==200){ fit_min = 11;    }
    if(dist_gen==250){ fit_min = 14;    }
    if(dist_gen==300){ fit_min = 18.75; }
  }

  if(cnt_radius==40){
    if(dist_gen<50  ){ fit_min = 0;     }
    if(dist_gen==50 ){ fit_min = 1.5;   }
    if(dist_gen==75 ){ fit_min = 4;     }
    if(dist_gen==100){ fit_min = 4.5;   }
    if(dist_gen==150){ fit_min = 9;     }
    if(dist_gen==200){ fit_min = 12;    }
    if(dist_gen==250){ fit_min = 16;    }
    if(dist_gen==300){ fit_min = 18.75; }
    if(dist_gen==350){ fit_min = 22.50; }
  }

  if(cnt_radius==100){
    if(dist_gen<50  ){ fit_min = 0;     }
    if(dist_gen==50 ){ fit_min = 2;     }
    if(dist_gen==75 ){ fit_min = 3;     }
    if(dist_gen==100){ fit_min = 5;     }
    if(dist_gen==150){ fit_min = 8.5;   }
    if(dist_gen==200){ fit_min = 12;    }
    if(dist_gen==250){ fit_min = 15;    }
    if(dist_gen==300){ fit_min = 18.75; }
    if(dist_gen==350){ fit_min = 22.5;  }
    if(dist_gen==400){ fit_min = 25;    }
  }

  if(cnt_radius==200){
    if(dist_gen<50  ){ fit_min = 0;   }
    if(dist_gen==50 ){ fit_min = 1.5; } 
    if(dist_gen==75 ){ fit_min = 3;   }
    if(dist_gen==100){ fit_min = 4.5; }
    if(dist_gen==150){ fit_min = 9;   }
    if(dist_gen==200){ fit_min = 13;  }
    if(dist_gen==250){ fit_min = 16;  }
    if(dist_gen==300){ fit_min = 20;  }
    if(dist_gen==350){ fit_min = 22;  } 
    if(dist_gen==400){ fit_min = 26;  }
    if(dist_gen==450){ fit_min = 30.5;  }
    if(dist_gen==500){ fit_min = 33;  }
  }

  if(cnt_radius==300){
    if(dist_gen<50  ){ fit_min = 0;   }
    if(dist_gen==50 ){ fit_min = 1.5; }
    if(dist_gen==75 ){ fit_min = 3.5; }
    if(dist_gen==100){ fit_min = 5.5; }
    if(dist_gen==150){ fit_min = 9;   }
    if(dist_gen==200){ fit_min = 13;  }
    if(dist_gen==250){ fit_min = 16;  }
    if(dist_gen==300){ fit_min = 20;  }
    if(dist_gen==350){ fit_min = 24;  }
    if(dist_gen==400){ fit_min = 28;  }
    if(dist_gen==450){ fit_min = 30;  }
    if(dist_gen==500){ fit_min = 33;  }
    if(dist_gen==550){ fit_min = 38;  }
    if(dist_gen==600){ fit_min = 42;  }
  }
  
  if(cnt_radius==500){
    if(dist_gen<50  ){ fit_min = 0;    }
    if(dist_gen==50 ){ fit_min = 2.5;  }
    if(dist_gen==75 ){ fit_min = 3.5;  }
    if(dist_gen==100){ fit_min = 6;    }
    if(dist_gen==150){ fit_min = 10;   }
    if(dist_gen==200){ fit_min = 11;   }
    if(dist_gen==250){ fit_min = 14;   }
    if(dist_gen==300){ fit_min = 22;   }
    if(dist_gen==350){ fit_min = 25;   }
    if(dist_gen==400){ fit_min = 30;   }
    if(dist_gen==450){ fit_min = 33;   } 
    if(dist_gen==500){ fit_min = 35;   }
    if(dist_gen==550){ fit_min = 41.5; }
    if(dist_gen==600){ fit_min = 44;   }
    if(dist_gen==650){ fit_min = 48.5; }
    if(dist_gen==700){ fit_min = 52;   }
    if(dist_gen==750){ fit_min = 56;   }
    if(dist_gen==800){ fit_min = 60;   }
  }
  return fit_min;
}

float GetFitMax(int cnt_radius,int dist_gen){
  float fit_max=0;
  if(cnt_radius==15){
    if(dist_gen==0  ){ fit_max = 5;  }
    if(dist_gen==15 ){ fit_max = 5;  }
    if(dist_gen==25 ){ fit_max = 10; }
    if(dist_gen==50 ){ fit_max = 10; }
    if(dist_gen==75 ){ fit_max = 10; }
    if(dist_gen==100){ fit_max = 20; }
    if(dist_gen==150){ fit_max = 20; }
    if(dist_gen==200){ fit_max = 30; }
    if(dist_gen==250){ fit_max = 30; }
    if(dist_gen==300){ fit_max = 40; }
  }

  if(cnt_radius==40){
    if(dist_gen==0  ){ fit_max = 5;  }
    if(dist_gen==15 ){ fit_max = 5;  }
    if(dist_gen==25 ){ fit_max = 10; }
    if(dist_gen==50 ){ fit_max = 10; }
    if(dist_gen==75 ){ fit_max = 20; }
    if(dist_gen==100){ fit_max = 20; }
    if(dist_gen==150){ fit_max = 20; }
    if(dist_gen==200){ fit_max = 30; }
    if(dist_gen==250){ fit_max = 30; }
    if(dist_gen==300){ fit_max = 40; }
    if(dist_gen==350){ fit_max = 50; }
  }

  if(cnt_radius==100){
    if(dist_gen==0  ){ fit_max = 5;  }
    if(dist_gen==15 ){ fit_max = 5;  }
    if(dist_gen==25 ){ fit_max = 10; }
    if(dist_gen==50 ){ fit_max = 10; }
    if(dist_gen==75 ){ fit_max = 20; }
    if(dist_gen==100){ fit_max = 20; }
    if(dist_gen==150){ fit_max = 30; }
    if(dist_gen==200){ fit_max = 30; }
    if(dist_gen==250){ fit_max = 40; }
    if(dist_gen==300){ fit_max = 40; }
    if(dist_gen==350){ fit_max = 50; }
    if(dist_gen==400){ fit_max = 45; }
  }

  if(cnt_radius==200){
    if(dist_gen==0  ){ fit_max = 5;  }
    if(dist_gen==15 ){ fit_max = 5;  }
    if(dist_gen==25 ){ fit_max = 10; }
    if(dist_gen==50 ){ fit_max = 10; }
    if(dist_gen==75 ){ fit_max = 20; }
    if(dist_gen==100){ fit_max = 20; }
    if(dist_gen==150){ fit_max = 25; }
    if(dist_gen==200){ fit_max = 40; }
    if(dist_gen==250){ fit_max = 50; }
    if(dist_gen==300){ fit_max = 40; }
    if(dist_gen==350){ fit_max = 40; }
    if(dist_gen==400){ fit_max = 60; }
    if(dist_gen==450){ fit_max = 60; }
    if(dist_gen==500){ fit_max = 60; }
  }

  if(cnt_radius==300){ 
    if(dist_gen==0  ){ fit_max = 5;  }
    if(dist_gen==15 ){ fit_max = 5;  }
    if(dist_gen==25 ){ fit_max = 10; }
    if(dist_gen==50 ){ fit_max = 20; }
    if(dist_gen==75 ){ fit_max = 20; }
    if(dist_gen==100){ fit_max = 20; }
    if(dist_gen==150){ fit_max = 30; }
    if(dist_gen==200){ fit_max = 40; }
    if(dist_gen==250){ fit_max = 40; }
    if(dist_gen==300){ fit_max = 50; }
    if(dist_gen==350){ fit_max = 50; }
    if(dist_gen==400){ fit_max = 60; }
    if(dist_gen==450){ fit_max = 60; }
    if(dist_gen==500){ fit_max = 70; }
    if(dist_gen==550){ fit_max = 70; }
    if(dist_gen==600){ fit_max = 70; }
  }
  
  if(cnt_radius==500){
    if(dist_gen==0  ){ fit_max = 5;  } 
    if(dist_gen==15 ){ fit_max = 5;  }
    if(dist_gen==25 ){ fit_max = 10; }
    if(dist_gen==50 ){ fit_max = 20; }
    if(dist_gen==75 ){ fit_max = 20; }
    if(dist_gen==100){ fit_max = 20; }
    if(dist_gen==150){ fit_max = 20; }
    if(dist_gen==200){ fit_max = 30; }
    if(dist_gen==250){ fit_max = 30; }
    if(dist_gen==300){ fit_max = 50; }
    if(dist_gen==350){ fit_max = 50; }
    if(dist_gen==400){ fit_max = 50; }
    if(dist_gen==450){ fit_max = 50; }
    if(dist_gen==500){ fit_max = 60; }
    if(dist_gen==550){ fit_max = 70; }
    if(dist_gen==600){ fit_max = 70; }
    if(dist_gen==650){ fit_max = 80; }
    if(dist_gen==700){ fit_max = 80; }
    if(dist_gen==750){ fit_max = 80; }
    if(dist_gen==800){ fit_max = 80; }
  }
  return fit_max;
}

float GetYMax(int cnt_radius){
  float ymax=0;
  if(cnt_radius==15 ){ ymax = 25; }
  if(cnt_radius==40 ){ ymax = 75; }
  if(cnt_radius==100){ ymax = 75; }
  if(cnt_radius==200){ ymax = 75; }
  if(cnt_radius==300){ ymax = 75; }
  if(cnt_radius==500){ ymax = 75; }
  return ymax;

}
   

void SPL(TF1* fitfunction, int cnt_radius, int dist_gen){
  if(cnt_radius==15){
    fitfunction->SetParLimits(1,0.01,10);
    fitfunction->SetParLimits(2,0.001,100);
    fitfunction->SetParLimits(4, 10,10000);
    fitfunction->SetParLimits(3, 0.01,10); 
    if(dist_gen<150){
      fitfunction->SetParLimits(0,0,10);
    }
    if(dist_gen>150){
      fitfunction->SetParLimits(0,10.5,30);
      fitfunction->SetParLimits(1,0.01,10);
    }
    fitfunction->SetParameter(0, 0.1);
    fitfunction->SetParameter(1, 0.1);
    fitfunction->SetParameter(2, 1);
    fitfunction->SetParameter(3, 3);
    fitfunction->SetParameter(4, 10);
  }//if sui cnt da 15um
  

  if(cnt_radius==40 || cnt_radius==100 || cnt_radius==200 || cnt_radius==300){
    fitfunction->SetParLimits(1,0.01,10);
    fitfunction->SetParLimits(2,0.001,100);
    if(cnt_radius==200 && dist_gen==15 ){ fitfunction->SetParLimits(1,5,100); }
    fitfunction->SetParLimits(4, 10,10000);
    fitfunction->SetParLimits(3, 0.01,10); 
    if(dist_gen<150){
      fitfunction->SetParLimits(0,0,10);
    }
    if(dist_gen>150){
      fitfunction->SetParLimits(0,10.5,50);
      fitfunction->SetParLimits(1,0.001,100);
      if(cnt_radius==300 && dist_gen==600){fitfunction->SetParLimits(0,40,50);}
      if(cnt_radius==300){fitfunction->SetParLimits(1,0.01,20);}
      if(cnt_radius==300){fitfunction->SetParLimits(2,0.01,20);}
      if(cnt_radius==40 && dist_gen==350){fitfunction->SetParLimits(0,20,25);}
      if(cnt_radius==200 && dist_gen==450){fitfunction->SetParLimits(1,0.1,10);}
    }
    fitfunction->SetParameter(0, 0.1);
    fitfunction->SetParameter(1, 0.1);
    fitfunction->SetParameter(2, 1);
    fitfunction->SetParameter(3, 3);
    fitfunction->SetParameter(4, 10);
  }//if sui cnt da 40um  
  
  if(cnt_radius==500){ 
    fitfunction->SetParLimits(1,0.01,10);
    fitfunction->SetParLimits(2,0.001,100);
    fitfunction->SetParLimits(4, 10,10000);
    fitfunction->SetParLimits(3, 0.01,10);
    if(dist_gen<100){
      fitfunction->SetParLimits(0,0,10);
    }
    if(dist_gen>90){
      fitfunction->SetParLimits(0,8,70);
      fitfunction->SetParLimits(1,0.01,10);
    }
    fitfunction->SetParameter(0, 20);
    if(dist_gen>300 && dist_gen<550){ fitfunction->SetParameter(0, 30); }
    if(dist_gen>500                ){ fitfunction->SetParameter(0, 50); }
    fitfunction->SetParameter(1, 0.2);
    fitfunction->SetParameter(2, 1);
    fitfunction->SetParameter(3, 3);
    fitfunction->SetParameter(4, 10);
  }//if sui cnt da 500um
}

int GetDistMax(int cnt_radius){
  int dmax=0;
  if(cnt_radius==15 ){ dmax = 300; }
  if(cnt_radius==40 ){ dmax = 350; }
  if(cnt_radius==100){ dmax = 400; }
  if(cnt_radius==200){ dmax = 500; }
  if(cnt_radius==300){ dmax = 600; }
  if(cnt_radius==500){ dmax = 800; }
  return dmax;
}


int GetBins(int cnt_radius,int dist_gen){
  int nbins = 0;

  if(dist_gen<300){nbins = 200;}
  
  if(cnt_radius==15 ){ if(dist_gen>=300){nbins = 150;}}
  if(cnt_radius==40 ){ if(dist_gen>=300){nbins = 150;}}
  if(cnt_radius==100){ if(dist_gen>=300){nbins = 150;}}
  if(cnt_radius==200){ if(dist_gen>=300){nbins = 100;}}
  if(cnt_radius==300){ if(dist_gen>=300){nbins = 100;}}
  if(cnt_radius==500){ if(dist_gen>=300){nbins = 100;}}

  return nbins;
}
