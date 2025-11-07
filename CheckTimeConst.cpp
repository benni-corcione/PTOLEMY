#include <iostream>
#include <stdlib.h>
#include <string>

#include "TF1.h"
#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TFitResult.h"
#include "TStyle.h"

#include "graphics.h"
#include "pshape_functions.h"
#include "CD_details.h"

Double_t PShape(Double_t *x, Double_t *par);

int main( int argc, char* argv[] ) {

  //parametri da input
  if( argc!= 4 ) {
    std::cout << "USAGE: ./CheckTimeConst [CD_number] [misura] [threshold]" << std::endl;
    std::cout << "Threshold: signals with amplitude higher than threshold will be fitted. Express it in Volts" << std::endl;
    std::cout << "EXAMPLE: ./CheckTimeConst 188 conteggi 0.7" << std::endl;
    exit(1);
  }

  int   CD_number = atoi(argv[1]);
  char* meas      =      argv[2] ;
  float threshold = atof(argv[3]);

  std::vector<int> voltage = GetVoltages(CD_number,meas);
  
  for(int i=0; i<voltage.size(); i++){
     std::cout << "Voltaggio " << voltage[i] << " iniziato" << std::endl;
    
    std::string fileName1 = "CD" + std::to_string(CD_number) + "_" + std::to_string(voltage[i]) + "V_treeraw.root";
    std::string fileName2 = "CD" + std::to_string(CD_number) + "_" + std::to_string(voltage[i]) + "V_tree.root";
 
    //preparazione lettura del tree
    TFile* file1 = TFile::Open(Form("data/root/CD%d/%s/%dV/%s",CD_number, meas, voltage[i], fileName1.c_str()));
    TFile* file2 = TFile::Open(Form("data/root/CD%d/%s/%dV/%s",CD_number, meas, voltage[i], fileName2.c_str()));
    TTree* treeraw = (TTree*)file1->Get("treeraw");
    TTree* tree    = (TTree*)file2->Get("tree");
    int nentries = tree->GetEntries();

    TCanvas* c1 = new TCanvas("c1", "c1", 1800, 1000);
    c1->cd();

    TH1F* time_fall = new TH1F("time_fall", "", 50, 0, 15);
   
    //settings tree 
    float pshape[2500];
    int ev;
    int lumi;
    float amp;
    float sampl;
  
    //ctrl per accettare pshape
    int cosmic_rays;
    int electronics;
    int ctrl_double;
  
    //first pshape
    treeraw->SetBranchAddress("event"        , &ev         );
    treeraw->SetBranchAddress("lumi"         , &lumi       );
    treeraw->SetBranchAddress("pshape"       , &pshape     );
    treeraw->SetBranchAddress("sampling_time", &sampl      );
    tree   ->SetBranchAddress("amp"          , &amp        );
    tree   ->SetBranchAddress("ctrl_double"  , &ctrl_double);
    tree   ->SetBranchAddress("cosmic_rays"  , &cosmic_rays);
    tree   ->SetBranchAddress("electronics"  , &electronics);
  
    treeraw->GetEntry(0);


    //fit sulla pulse shape
    //inizializzazione parametri
    double A0    = -2.2E-4;
    double n     = -0.033 ;
    double A     =  0.76  ;
    double x0    =  23.6  ;
    double tau_R =  0.2   ;
    double tau_F =  9     ;
    
    //preparazione grafica histo
    for(unsigned iEntry=0; iEntry<nentries; iEntry++){
      //histo da fittare
      TH1F g("g","",2500,0,2500*sampl*1E+6);
    
      tree->GetEntry(iEntry);
    
      if(ctrl_double==1 && cosmic_rays==0 && electronics==0){
	treeraw->GetEntry(iEntry);
      
	if(amp>threshold){
	  for(int i=0; i<2500; ++i ){
	    g.SetBinContent(i+1,pshape[i] );
	  }//for istogramma
  
	  //stile histo
	  //gStyle->SetOptStat(0);

	  TF1 shape("fit_fcn", PShape, 0, 40, 6);
	  shape.SetParNames("A0", "n", "A", "x0", "#tau_rise", "#tau_fall");
	  //A0: par0   n: par1   A: par2   x0: par3   tau_rise: par4   tau_fall : par5
	  shape.SetParameters(A0 , n, A, x0, tau_R, tau_F);

	  //shape.SetParLimits(2,threshold-0.1,threshold+0.2);
	  shape.SetParLimits(3,18,25);
	  shape.SetParLimits(4,0.0001,3);
	  shape.SetParLimits(5,5,30);

	  shape.SetLineColor(kRed);
	  shape.SetLineWidth(6);
	  shape.SetNpx(1000);
		
	  g.Fit("fit_fcn","RxMq");
	
	  //if((shape.GetProb())>0.05){
	  time_fall->Fill(shape.GetParameter(5));
	  //}//controllo sul fit

	}//if controllo amp
      }//if sul controllo primario
    }//for sulle entries
  
    time_fall->Draw();
    c1->SaveAs(Form("plots/CD%d/%s/%dV_times_from%dmV.png",CD_number,meas,voltage[i],int(threshold*1000)));

    TFile* file_tau = TFile::Open( Form("data/root/CD%d/%s/%dV/taufit_%dV.root", CD_number,meas,voltage[i],voltage[i]), "recreate" );
    file_tau->cd();
    time_fall->Write();
    file_tau->Close();

    std::cout << "Voltaggio " << voltage[i] << " completato" << std::endl;
    std::cout << std::endl;
  }//voltaggi
  
  return 0;
}


Double_t PShape(Double_t *x, Double_t *par){
  //par[0]=A0   par[1]=n       par[2]=A
  //par[3]=x0   par[4]=t_rise  par[5]=t_fall
  Double_t fitval = 0;
  Double_t arg_L  = -(x[0]-par[3])/par[4];
  Double_t arg_R  = -(x[0]-par[3])/par[5];
  Double_t piece = exp(arg_L)-exp(arg_R);
 
  if      ( x[0] < par[3]){ fitval = par[0]; }
  else if ( x[0] > par[3]){ fitval = par[0]+par[1]+par[2]*piece; }

  return(fitval);
}


