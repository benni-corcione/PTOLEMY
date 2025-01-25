#include <iostream>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <filesystem>

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLine.h"
#include "TStyle.h"
#include "TGraph.h"
#include "pshape_functions.h"
#include "graphics.h"

int main(int argc, char* argv[]){

  if(argc!=5){
    std::cout << "USAGE: ./coincidence [chargemin B60] [chargemax B60] [chargemin H60] [chargemax H60]" << std::endl;
    std::cout << "charges need to be expressed in uC" << std::endl;
    std::cout << "EXAMPLE: ./coincidence 0.5 12 0.5 6.5" << std::endl;

    exit(1);
  }
  
  float chargeB_min = atof(argv[1])*1E-06;
  float chargeB_max = atof(argv[2])*1E-06;
  float chargeH_min = atof(argv[3])*1E-06;
  float chargeH_max = atof(argv[4])*1E-06;
  char* cB_min = argv[1];
  char* cB_max = argv[2];
  char* cH_min = argv[3];
  char* cH_max = argv[4];
  
  //preparazione lettura del tree
  TFile* fileB = TFile::Open("data/root/CD204/B60_pre_cond/135V/CD204_135V_tree.root"); 
  TTree* treeB = (TTree*)fileB->Get("tree");
  int nentriesB = treeB->GetEntries();
  
  TFile* fileH = TFile::Open("data/root/CD204/H60_pre_cond/135V/CD204_135V_tree.root"); 
  TTree* treeH = (TTree*)fileH->Get("tree");
  int nentriesH = treeH->GetEntries();

  TFile* fileBraw = TFile::Open("data/root/CD204/B60_pre_cond/135V/CD204_135V_treeraw.root"); 
  TTree* treeBraw = (TTree*)fileBraw->Get("treeraw");

  TFile* fileHraw = TFile::Open("data/root/CD204/H60_pre_cond/135V/CD204_135V_treeraw.root"); 
  TTree* treeHraw = (TTree*)fileHraw->Get("treeraw");

  
  //settings tree 
  int    eventB , eventH;
  int     lumiB,   lumiH;
  float chargeB, chargeH;
  float    ampB,    ampH;
  float pshapeB[2502];
  float pshapeH[2502];
  double trigtime_sB, trigtime_mB, trigtime_hB;
  double trigtime_sH, trigtime_mH, trigtime_hH;
  
  treeB->SetBranchAddress( "event" , &eventB  );
  treeB->SetBranchAddress( "lumi"  , &lumiB   );
  treeB->SetBranchAddress( "charge", &chargeB );
  treeB->SetBranchAddress( "amp"   , &ampB    );
  treeH->SetBranchAddress( "event" , &eventH  );
  treeH->SetBranchAddress( "lumi"  , &lumiH   );
  treeH->SetBranchAddress( "charge", &chargeH );
  treeH->SetBranchAddress( "amp"   , &ampH    );

  treeBraw->SetBranchAddress( "pshape"    , &pshapeB     );
  treeHraw->SetBranchAddress( "pshape"    , &pshapeH     );
  treeBraw->SetBranchAddress( "trigtime_s", &trigtime_sB );
  treeBraw->SetBranchAddress( "trigtime_m", &trigtime_mB );
  treeBraw->SetBranchAddress( "trigtime_h", &trigtime_hB );
  treeHraw->SetBranchAddress( "trigtime_s", &trigtime_sH );
  treeHraw->SetBranchAddress( "trigtime_m", &trigtime_mH );
  treeHraw->SetBranchAddress( "trigtime_h", &trigtime_hH );

  std::cout << "nentries B60: " << nentriesB << "  H60: " << nentriesH << std::endl; 
  TCanvas* c = new TCanvas("c1", "c1", 1800, 1000);
  c->cd();

  //grafico
  std::vector<float> x;
  std::vector<float> y;
  
  //istogramma
  int nbins_amp    = 300;
  int nbins_charge = 300;
  float amp_min = 0;
  float amp_max = 1;
  float charge_min = 0;
  float charge_max = 20;
  TH1F* h_amp    = new TH1F("h_amp"   ,"",nbins_amp   ,   amp_min,   amp_max);
  TH1F* h_charge = new TH1F("h_charge","",nbins_charge,charge_min,charge_max);

  int counter = 0;
  
  //FOR SU netries
  for(unsigned iEntry=0; iEntry<nentriesH; iEntry++){  

    if(iEntry%10000==0) std::cout << "cycle: " << iEntry << std::endl;
    treeB->GetEntry(iEntry);
    treeH->GetEntry(iEntry);
    
    treeBraw->GetEntry(iEntry);
    treeHraw->GetEntry(iEntry);
    
      //EVENTI CONTEMPORANEI-CHECK
    if(eventB==eventH && lumiB==lumiH &&
       trigtime_sB==trigtime_sH &&
       trigtime_mB==trigtime_mH &&
       trigtime_hB==trigtime_hH){

       if(chargeB>chargeB_min && chargeB<chargeB_max &&
       chargeH>chargeH_min && chargeH<chargeH_max){
	 
      counter++;
      /*
      std::cout << "B60: charge " << chargeB <<  "     " 
	        << "H60: charge " << chargeH << std::endl;
      
      std::cout << "B60: event " << eventB << "  lumi: " << lumiB << "     " 
	        << "H60: event " << eventH << "  lumi: " << lumiH << std::endl;
      

      */
      
      TH1D* Bshape = new TH1D("Bshape", "", 2500, 0., 2500.);
      TH1D* Hshape = new TH1D("Hshape", "", 2500, 0., 2500.);

      for(int i=0; i<2500; ++i ){
	Bshape->SetBinContent(i+1, pshapeB[i]);
	Hshape->SetBinContent(i+1, pshapeH[i]);
	
      }
      
      Bshape->GetYaxis()->SetRangeUser(-0.9,0.2);
      Bshape->SetLineColor(kAzure+1);
      Bshape->SetLineWidth(4);
      Hshape->SetLineColor(kPink+6);
      Hshape->SetLineWidth(4);
      gStyle->SetOptStat(0);
      Bshape->Draw("l");
      Hshape->Draw("l same");
      
      std::string outdir("plots/CD204/H60_pre_cond/135V");
      system(Form("mkdir -p %s", outdir.c_str()));
      c->SaveAs(Form("%s/event%d_lumi%d.png",outdir.c_str(),eventH,lumiH));
      
      delete(Bshape);
      delete(Hshape);
      
	
      //potrò farlo una volta calibrati sulla stessa risposta i tes
      
      //h_amp   ->Fill(ampB+ampH);
      //h_charge->Fill((chargeB+chargeH)*1E+06);

      //std::cout << chargeB*1E+06 << " " << chargeH*1E+06 << std::endl;
     
      x.push_back(chargeB);
      y.push_back(chargeH);}
    } //if su time_coincidence
  }//for entries
  
  std::cout << "eventi triggeranti: " << counter << std::endl;
 

  /*
  Settings settings;
  settings.Margin(c);
  c->SetLeftMargin(0.155);
  c->cd();

  float binsize_amp = settings.Binsize((amp_max-amp_min)/nbins_amp);
  float binsize_cha = settings.Binsize((charge_max-charge_min)/nbins_charge); 

  //x and y labels
  const char* x_amp = "Amplitude [V]";
  std::string y_amp = "Counts / " + to_str(binsize_amp) + " V" ;
  const char* x_cha = "Charge [uC]"  ;
  std::string y_cha = "Counts / " + to_str(binsize_cha)  + " uC";
  
  //disegno dell'histo con funzione definita in graphics.h
  settings.graphic_histo(h_amp   ,0,2 ,x_amp,y_amp.c_str(),135);
  settings.graphic_histo(h_charge,0,20,x_cha,y_cha.c_str(),135);
  settings.general_style();
  //gStyle->SetOptFit(0011);

  h_amp->SetMarkerSize(2);
  h_amp->SetLineWidth(3);
  h_amp->Draw(); //"pe" per avere points+errors
  
  system("mkdir -p plots/CD204/H60_pre_cond/135V/");
  c->SaveAs(      "plots/CD204/H60_pre_cond/135V/amp_sum.png");
  c->Clear();
  
  h_charge->SetMarkerSize(2);
  h_charge->SetLineWidth(3);
  h_charge->Draw(); //"pe" per avere points+errors

  c->SaveAs("plots/CD204/H60_pre_cond/135V/charge_sum.png");
  */
  //CARICA vs CARICA
  
  c->Clear();
  int len = x.size();

  float x_B[len];
  float y_H[len];
  
  //servono array e non vettori per grafico
  for(int j=0; j<len; j++){
    x_B[j]=x[j]*1E+6;
    y_H[j]=y[j]*1E+6;
  }
  
  TGraph *g = new TGraph(len,x_B,y_H);

  g->GetYaxis()->SetTitle("charge H60 (disaligned) [uC]");
  g->GetXaxis()->SetTitle("charge B60 (aligned) [uC]");
  //g->GetXaxis()->SetLimits(-2,12);
  g->SetTitle("Charge comparison");
  g->SetMarkerSize(1.5);
  g->SetMarkerStyle(20);
  g->SetLineColor(kBlack);

  gStyle->SetTitleFontSize(0.07);
  //gStyle->SetOptFit(1111);

  g->Draw("AP");


  for(int i=0; i<10; i++){
    if(cB_min[i]=='.'){cB_min[i]='p';}
    if(cB_max[i]=='.'){cB_max[i]='p';}
    if(cH_min[i]=='.'){cH_min[i]='p';}
    if(cH_max[i]=='.'){cH_max[i]='p';}
  }

  
  std::string outdir("plots/CD204/H60_pre_cond/135V");
  system(Form("mkdir -p %s", outdir.c_str()));
  c->SaveAs(Form("%s/B60from%sto%s_H60from%sto%s.png",outdir.c_str(),cB_min,cB_max,cH_min,cH_max));
  
  fileB->Close();
  fileH->Close();

  delete(c);
  delete(h_amp);
  delete(h_charge);
  
  return 0;
}
  
