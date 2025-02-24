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

Double_t PShape(Double_t *x, Double_t *par);

int main( int argc, char* argv[] ) {

  //parametri da input
  if( argc!= 5 ) {
    std::cout << "USAGE: ./CheckTimeConst [CD_number] [misura] [voltage] [threshold]" << std::endl;
    std::cout << "Threshold: signals with amplitude higher than threshold will be fitted. Express it in Volts" << std::endl;
    std::cout << "EXAMPLE: ./CheckTimeConst 188 conteggi 105 0.7" << std::endl;
    exit(1);
  }

  int   CD_number = atoi(argv[1]);
  char* meas      =      argv[2] ;
  int   voltage   = atoi(argv[3]);
  float threshold = atof(argv[4]);
  
  std::string fileName1 = "CD" + std::to_string(CD_number) + "_" + std::to_string(voltage) + "V_treeraw.root";
  std::string fileName2 = "CD" + std::to_string(CD_number) + "_" + std::to_string(voltage) + "V_tree.root";
 
  //preparazione lettura del tree
  TFile* file1 = TFile::Open(Form("data/root/CD%d/%s/%dV/%s",CD_number, meas, voltage, fileName1.c_str()));
  TFile* file2 = TFile::Open(Form("data/root/CD%d/%s/%dV/%s",CD_number, meas, voltage, fileName2.c_str()));
  TTree* treeraw = (TTree*)file1->Get("treeraw");
  TTree* tree    = (TTree*)file2->Get("tree");
  int nentries = tree->GetEntries();

  TCanvas* c1 = new TCanvas("c1", "c1", 1800, 1000);
  c1->cd();

  TH1F* time_fall = new TH1F("time_fall", "", 100, 8, 12);
   
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

	/*
	h.SetLineColor(kBlue-7);
	h->GetXaxis()->SetTitle("Time (#mus)");
	h->GetYaxis()->SetTitle("Amplitude (V)");
	h->GetXaxis()->SetTitleSize(.07);
	h->GetYaxis()->SetTitleSize(.07);
	h->GetXaxis()->SetLabelSize(0.07);
	h->GetYaxis()->SetLabelSize(0.07);
	h->GetYaxis()->SetTitleOffset(+0.9);
	h->GetXaxis()->SetTitleOffset(+1);
	h->GetYaxis()->SetRangeUser(-1.0,0.1);
    
	c1->SetBottomMargin(0.2);
	c1->SetLeftMargin(0.12);
	c1->SetRightMargin(0.05);
	*/
    
	TF1 shape("fit_fcn", PShape, 0, 100, 6);
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
	
	//g.Draw("alsame");
	//c1->SaveAs(Form("plots/CD%d/%s/%dV/prova/ev%d_lumi%d.png",CD_number,meas,voltage,ev,lumi));
	//c1->Clear();

	/*
	double prob =(g.GetFunction("fit_fcn"))->GetProb();
	std::cout << prob << std::endl;
	*/
	//if((shape.GetProb())>0.05){
	time_fall->Fill(shape.GetParameter(5));
	//}//controllo sul fit

      }//if controllo amp
    }//if sul controllo primario
  }//for sulle entries
  time_fall->Draw();
  c1->SaveAs(Form("plots/CD%d/%s/%dV/prova/times_from%dmV.png",CD_number,meas,voltage,int(threshold*1000)));
  
  delete(c1);
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


