#include <iostream>
#include <stdlib.h>
#include <string>

#include "TF1.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TStyle.h"

Double_t PShape(Double_t *x, Double_t *par);

int main( int argc, char* argv[] ) {

  //parametri da input
  if( argc!= 10 ) {
    std::cout << "USAGE: ./draw_three_electrons [CD_number] [misura] [voltage] [ev1] [lumi1] [ev2] [lumi2] [ev3] [lumi3]" << std::endl;
    std::cout << "EXAMPLE: ./draw_three_electrons 188 conteggi 105 0 1 0 12 0 8" << std::endl;
    exit(1);
  }

  int   CD_number = (atoi(argv[1]));
  char* meas      =       argv[2]  ;
  int   voltage   = (atoi(argv[3]));
  int   event1    = (atoi(argv[4]));
  int   lumi1     = (atoi(argv[5]));
  int   event2    = (atoi(argv[6]));
  int   lumi2     = (atoi(argv[7]));
  int   event3    = (atoi(argv[8]));
  int   lumi3     = (atoi(argv[9]));
  
  std::string fileName = "CD" + std::to_string(CD_number) + "_" + std::to_string(voltage) + "V_treeraw.root";
  //preparazione lettura del tree
  TFile* file = TFile::Open(Form("data/root/CD%d/%s/%dV/%s",CD_number, meas, voltage, fileName.c_str()));
  TTree* tree = (TTree*)file->Get("treeraw");
  int nentries = tree->GetEntries();

 TCanvas* c1 = new TCanvas("c1", "c1", 1800, 1000);
  c1->cd();
  
  //settings tree 
  float pshape[2500];
  int ev;
  int lumi;
  float sampl;
  
  //first pshape
  tree->SetBranchAddress( "event"        , &ev     );
  tree->SetBranchAddress( "lumi"         , &lumi   );
  tree->SetBranchAddress( "pshape"       , &pshape );
  tree->SetBranchAddress( "sampling_time", &sampl  );
  tree->GetEntry(0);
  
  //preparazione grafica histo
  TH1D* h1 = new TH1D("h1", "", 2500, 0., 2500*sampl*1.E6);
  TH1D* h2 = new TH1D("h2", "", 2500, 0., 2500*sampl*1.E6);
  TH1D* h3 = new TH1D("h3", "", 2500, 0., 2500*sampl*1.E6);
 
  
  for(unsigned iEntry=0; iEntry<nentries; iEntry++){
    tree->GetEntry(iEntry);

    if(ev==event1 && lumi==lumi1){
      for(int i=0; i<2500; ++i ){
	h1->SetBinContent( i+1, pshape[i] );
      }
    }

    if(ev==event2 && lumi==lumi2){
      for(int i=0; i<2500; ++i ){
	h2->SetBinContent( i+1, pshape[i] );
      }
    }
    
    if(ev==event3 && lumi==lumi3){
      for(int i=0; i<2500; ++i ){
	h3->SetBinContent( i+1, pshape[i] );
      }
    }
  }
  
  //stile histo
  gStyle->SetOptStat(0);
  
  h1->SetLineColor(30);
  h1->SetLineWidth(4);
  h2->SetLineColor(216);
  h2->SetLineWidth(4);
  h3->SetLineColor(38);
  h3->SetLineWidth(4);
    
  h3->GetXaxis()->SetTitle("Time (#mus)");
  h3->GetYaxis()->SetTitle("Amplitude (V)");
  h3->GetXaxis()->SetTitleSize(.07);
  h3->GetYaxis()->SetTitleSize(.07);
  h3->GetXaxis()->SetLabelSize(0.07);
  h3->GetYaxis()->SetLabelSize(0.07);
  h3->GetYaxis()->SetTitleOffset(+0.9);
  h3->GetXaxis()->SetTitleOffset(+1);
  h3->GetYaxis()->SetRangeUser(-0.79,0.1);
  //h1->SetMarkerStyle(20);
  //h1->SetMarkerSize(1);
  //h1->SetMarkerColor(kBlue-6);
  c1->SetBottomMargin(0.2);
  c1->SetLeftMargin(0.15);
  c1->SetRightMargin(0.05);
  //gStyle->SetFrameLineWidth(0.5);
  h3->Draw("l");
  h2->Draw("l same");
  h1->Draw("l same");
  c1->SaveAs(Form("plots/CD%d/%s/%dV/ev%dlumi%d_ev%dlumi%d_ev%dlumi%d.png",CD_number,meas,voltage,event1,lumi1,event2,lumi2,event3,lumi3));

    
  //fit sulla pulse shape
  //inizializzazione parametri
  double A0    = -2.2E-4;
  double n     = -0.033 ;
  double A     =  0.76  ;
  double x0    =  23.6  ;
  double tau_R =  0.2   ;
  double tau_F =  9   ;

  TF1 *shape = new TF1("fit_fcn", PShape, 0, 40, 6);
  shape->SetParNames("A0", "n", "A", "x0", "#tau_rise", "#tau_fall");
  //A0: par0   n: par1   A: par2   x0: par3   tau_rise: par4   tau_fall : par5
  shape->SetParameters(A0 , n, A, x0, tau_R, tau_F);
  //shape->SetParLimits(2,0.55,0.6);

  
  shape->SetParLimits(3,18,24.5);
  shape->SetParLimits(4,0.001,1.5);
  shape->SetParLimits(5,1,18);
  //shape->FixParameter(2, 0.76 );
  //shape->FixParameter(5, 10.0 );
  //shape->SetParLimits(2,0.7,0.81);
  shape->SetLineColor(kRed);
  shape->SetLineWidth(6);
  shape->SetNpx(1000);
  //h2->Fit("fit_fcn","RxM");
  //h1->Fit("fit_fcn","RxM+");
  //h3->Fit("fit_fcn","RxM+");
  c1->SaveAs(Form("plots/CD%d/%s/%dV/ev%dlumi%d_ev%dlumi%d_ev%dlumi%d.png",CD_number,meas,voltage,event1,lumi1,event2,lumi2,event3,lumi3));
  
  delete(h1);
  delete(h2);
  delete(h3);
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


