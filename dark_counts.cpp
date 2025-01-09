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
#include "pshape_functions.h"

//servono sia evento sia lumi!
int main( int argc, char* argv[] ) {

  //parametri da input
  if( argc!= 3 ) {
    std::cout << "USAGE: ./dark_counts [CD_number] [misura]" << std::endl;
    exit(1);
  }

  int   CD_number = (atoi(argv[1]));
  char* meas      =       argv[2]  ;
  
  std::string fileName = "CD"+ std::to_string(CD_number)
                       + "_0V_treeraw.root";

  //preparazione lettura del tree
  TFile* file = TFile::Open(Form("data/root/CD%d/%s/0V/%s",CD_number, meas,fileName.c_str()));
  TTree* tree = (TTree*)file->Get("treeraw");
  int nentries = tree->GetEntries();
  
  //TLegend *legend = new TLegend(0.8, 0.35, 1.1, 0.45);

  //settings tree 
  float pshape[2502];
  int   ev;
  int   lum;
  float sampling;

  tree->SetBranchAddress( "event" ,        &ev       );
  tree->SetBranchAddress( "lumi"  ,        &lum      );
  tree->SetBranchAddress( "pshape",        &pshape   );
  tree->SetBranchAddress( "sampling_time", &sampling );

  //preparazione grafica histo
  TCanvas* c1 = new TCanvas("c1", "c1", 13000, 8500); //1800 850
  TCanvas* c2 = new TCanvas("c2", "c2", 800, 800); //1800 850
  TCanvas* c3 = new TCanvas("c3", "c3", 800, 800); //1800 850
  
  TH1D* ampli = new TH1D("ampli", "Amplitude",100,0.03,0.15);
  TH1D* charg = new TH1D("charg", "Charge",100,-1E-8,1E-6);
  c1->cd();
 
  tree->GetEntry(1);
  std::vector<TH1D*> histos;
  int count1 = 0;
  int count2 = 0;
    
  for(unsigned iEntry=0; iEntry<nentries; iEntry++){
    tree->GetEntry(iEntry);
    float baseline = GetBaseline(pshape);
    float ampl     = GetAmp(pshape,baseline);
    float ch       = GetCharge(pshape, sampling,baseline);
   
    std::string histname(Form("%dentry", iEntry));
    TH1D* h1 = new TH1D( histname.c_str(), "",2500,0.,2500*sampling*1.E+6 );

    int check_noise=0;
    int check_peak=0;
    
    for(int i=0; i<2500; ++i ){
      h1->SetBinContent( i+1, pshape[i] );
      if(pshape[i]>0.03){ check_noise++;}
      //ho superato abbastanza la base in positivo
    }
    //qualche altro check
    //if(pshape[550]>-0.05){ check_peak++;}
    histos.push_back(h1);

    h1->SetLineWidth(7);
    gStyle->SetOptStat(0);
    
    //histo 'a persistenza'
    if(check_noise  > 0){ h1->SetLineColorAlpha(kWhite, 0.04); }
    if(check_noise == 0){ h1->SetLineColorAlpha(kRed, 0.04); count1++;  charg->Fill(ch); ampli->Fill(ampl); }
    //if(check_peak  == 1 &&
       //check_noise == 0){ h1->SetLineColorAlpha(kCyan-3, 0.04); count2++;  charg->Fill(ch);
    //amplitude->Fill(ampl);
    h1->GetXaxis()->SetTitle("Time (#mus)");
    h1->GetYaxis()->SetTitle("Amplitude (V)");
    h1->GetXaxis()->SetTitleSize(.05); //0.06 su singola, //0.08 su mini
    h1->GetYaxis()->SetTitleSize(.05);
    h1->GetXaxis()->SetLabelSize(0.06); //0.05 su singola, //0.07 su mini
    h1->GetYaxis()->SetLabelSize(0.06);
    h1->GetYaxis()->SetTitleOffset(+0.95);
    h1->GetXaxis()->SetTitleOffset(+0.9);
    //h1->GetXaxis()->SetRangeUser(1.5,4);
    h1->GetYaxis()->SetRangeUser(-0.5,0.03);
    
    c1->SetBottomMargin(0.2);
    c1->SetLeftMargin(0.13);
    c1->SetRightMargin(0.04);
    //gStyle->SetFrameLineWidth(0.5);
    h1->Draw("l same");
    
    
  } 
   

  std::string outdir( Form("plots/CD%d/%s/dark_counts", CD_number,meas));
  system( Form("mkdir -p %s", outdir.c_str()) );
  //-p crea anche le parent se prima non esistono

  //pshapes
  c1->SaveAs(Form("%s/dark_counts_pshapes.png",outdir.c_str()));
  delete(c1);
  
  //amplitude
  c2->cd();
  ampli->Draw();
  c2->SaveAs(Form("%s/dark_counts_ampl.png",outdir.c_str()));
  delete(c2);
       
  //charge
  c3->cd();
  charg->Draw();
  c3->SaveAs(Form("%s/dark_counts_charge.png",outdir.c_str()));
  delete(c3);
       
  //delete(histos);
  return 0;
}
