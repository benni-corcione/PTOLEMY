#include <iostream>
#include <stdlib.h>
#include <string>

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLine.h"
#include "TStyle.h"
#include "TGraph.h"
#include "pshape_functions.h"

int main(int argc, char* argv[]){

  if(argc!=5){
    std::cout << "USAGE: ./coincidence [chargemin B60] [chargemax B60] [chargemin H60] [chargemax H60]" << std::endl;
    std::cout << "charges need to be expressed in uC" << std::endl;
    std::cout << "EXAMPLE: ./coincidence 0.5 11.5 0.5 6.5" << std::endl;

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
  int   eventB;  int    eventH;
  int   lumiB ;  int    lumiH ;
  float chargeB; float chargeH;
  float pshapeB[2502];
  float pshapeH[2502];
  
  treeB->SetBranchAddress( "event" , &eventB  );
  treeB->SetBranchAddress( "lumi"  , &lumiB   );
  treeB->SetBranchAddress( "charge", &chargeB );
  treeH->SetBranchAddress( "event" , &eventH  );
  treeH->SetBranchAddress( "lumi"  , &lumiH   );
  treeH->SetBranchAddress( "charge", &chargeH );

  treeBraw->SetBranchAddress( "pshape", &pshapeB );
  treeHraw->SetBranchAddress( "pshape", &pshapeH );

  std::cout << "nentries B60: " << nentriesB << "  H60: " << nentriesH << std::endl; 
  TCanvas* c1 = new TCanvas("c1", "c1", 1800, 1000);
  c1->cd();

  //grafico
  std::vector<float> x;
  std::vector<float> y;

  int counter=0;
  //FOR SU netries
  for(unsigned iEntry=0; iEntry<nentriesH; iEntry++){  

    if(iEntry%10000==0) std::cout << "cycle: " << iEntry << std::endl;
    treeB->GetEntry(iEntry);
    treeH->GetEntry(iEntry);

    if(chargeB>chargeB_min && chargeB<chargeB_max &&
       chargeH>chargeH_min && chargeH<chargeH_max){
      counter++;
      
      treeBraw->GetEntry(iEntry);
      treeHraw->GetEntry(iEntry);
      //EVENTI CONTEMPORANEI
      /*
      std::cout << "B60: charge " << chargeB <<  "     " 
	        << "H60: charge " << chargeH << std::endl;
      
      std::cout << "B60: event " << eventB << "  lumi: " << lumiB << "     " 
	        << "H60: event " << eventH << "  lumi: " << lumiH << std::endl;
      */

      /*
      TH1D* Bshape = new TH1D("Bshape", "", 2500, 0., 2500.);
      TH1D* Hshape = new TH1D("Hshape", "", 2500, 0., 2500.);

      for(int i=0; i<2500; ++i ){
	Bshape->SetBinContent(i+1, pshapeB[i]);
	Hshape->SetBinContent(i+1, pshapeH[i]);
	
      }
      
      Bshape->GetYaxis()->SetRangeUser(-0.9,0.2);
      Bshape->SetLineColor(kBlue+2);
      Bshape->SetLineWidth(3);
      Hshape->SetLineColor(kPink);
      Hshape->SetLineWidth(3);
      
      Bshape->Draw("l");
      Hshape->Draw("l same");
      
      std::string outdir("plots/CD204/try");
      system(Form("mkdir -p %s", outdir.c_str()));
      c1->SaveAs(Form("%s/event%d_lumi%d.png",outdir.c_str(),eventH,lumiH));
      
      delete(Bshape);
      delete(Hshape);//prossima entry del tree
      */
      x.push_back(chargeB);
      y.push_back(chargeH);
    }
  }

  std::cout << "eventi triggeranti: " << counter << std::endl;

  c1->Clear();
  int len = x.size();

  float x_B[len];
  float y_H[len];

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
  c1->SaveAs(Form("%s/B60from%sto%s_H60from%sto%s.png",outdir.c_str(),cB_min,cB_max,cH_min,cH_max));

  fileB->Close();
  fileH->Close();

  delete(c1);
  return 0;
}
  
