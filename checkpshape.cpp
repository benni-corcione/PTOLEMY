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

//servono sia evento sia lumi!
int main( int argc, char* argv[] ) {

  //parametri da input
  if( argc!= 6 ) {
    std::cout << "USAGE: ./checkPulseShape [CD_number] [meas] [voltage] [event] [lumi]" << std::endl;
    exit(1);
  }

  int CD_number = (atoi(argv[1]));
  int voltage   = (atoi(argv[3]));
  int event     = (atoi(argv[4]));
  int lumi      = (atoi(argv[5]));
  char* meas    =       argv[2]  ; 

  
  std::string fileName = "CD"+ std::to_string(CD_number)
                       + "_" + std::to_string(voltage) + "V_treeraw.root";

  //preparazione lettura del tree
  TFile* file = TFile::Open(Form("data/root/CD%d/%s/%dV/%s",CD_number, meas, voltage, fileName.c_str())); 
  TTree* tree = (TTree*)file->Get("treeraw");
  int nentries = tree->GetEntries();
  
  TLegend *legend = new TLegend(0.8, 0.35, 1.1, 0.45);

   if(event > nentries){
    std::cout << "There's no such an event" << std::endl;
    exit(1);
  }

  //settings tree 
  float pshape[2502];
  int   ev;
  int   lum;
  float sampling;

  tree->SetBranchAddress( "event"        , &ev       );
  tree->SetBranchAddress( "lumi"         , &lum      );
  tree->SetBranchAddress( "pshape"       , &pshape   );
  tree->SetBranchAddress( "sampling_time", &sampling );

  //preparazione grafica histo
  TCanvas* c1 = new TCanvas("c1", "c1", 1800, 850); //1800 850
  c1->cd();

  //check se l'evento esiste
  int check = 0;

  for(unsigned iEntry=0; iEntry<nentries; iEntry++){
    tree->GetEntry(iEntry);

    if(ev==event && lum==lumi){
      TH1D* h1 = new TH1D("h1", "", 2500,0.,2500*sampling*1.E+6);
      check++;
      for(int i=0; i<2500; ++i ){
	h1->SetBinContent( i+1, pshape[i] );
      }
      gStyle->SetOptStat(0);
      h1->SetLineColor(kViolet+5);
      h1->SetLineWidth(3);
      h1->GetXaxis()->SetTitle("Time (#mus)");
      h1->GetYaxis()->SetTitle("Amplitude (V)");
      h1->GetXaxis()->SetTitleSize(.07); //0.06 su singola, //0.08 su mini
      h1->GetYaxis()->SetTitleSize(.07);
      h1->GetXaxis()->SetLabelSize(0.08); //0.05 su singola, //0.07 su mini
      h1->GetYaxis()->SetLabelSize(0.08);
      h1->GetYaxis()->SetTitleOffset(+0.95);
      h1->GetXaxis()->SetTitleOffset(+0.9);
      //h1->GetXaxis()->SetRangeUser(23.1,27);
      h1->GetYaxis()->SetRangeUser(-0.15,0.005);
      //h1->GetYaxis()->SetRangeUser(-0.5,0.5);
      //h1->GetYaxis()->SetRangeUser(-03,0.05); //dark counts
      //h1->GetXaxis()->SetNdivisions(406);
      //h1->SetMarkerStyle(20);
      //h1->SetMarkerSize(1);
      //h1->SetMarkerColor(kBlue-6);
      c1->SetBottomMargin(0.2);
      c1->SetLeftMargin(0.13);
      c1->SetRightMargin(0.04);
      gStyle->SetFrameLineWidth(0.5);
      h1->Draw("l");
    }
  } 
  
  if(check==0){
    std::cout << "The event does not exist!" << std::endl;
    exit(1);
  }
  
  //stile histo
 
  /*
  //stampa baseline
  double somma = 0;
  
  for(int i=0; i<450; i++){
    somma+=(h1->GetBinContent(i));
  }
  somma/=450.;
  */
  /*
  TLine *min = new TLine(0,-0.8345,100,-0.8345);
  min->SetLineWidth(5);
  min->SetLineColor(kBlue-9);
  min->SetLineStyle(9);
  min->Draw("same");
  
 
  legend->SetBorderSize(0);
  legend->SetFillStyle(0);
  legend->Draw("same");
  */
  /*
  TLine *baseline = new TLine(0,somma,100,somma);
  baseline->SetLineWidth(5);
  baseline->SetLineColor(kPink-5);
  baseline->SetLineStyle(9);
  baseline->Draw("same");
  
  legend->AddEntry(baseline,"baseline","l");
  //legend->AddEntry(min,"V_{min}","l");
  legend->SetBorderSize(0);
  legend->SetFillStyle(0);
  legend->Draw("same");
  */

  std::string outdir( Form("plots/CD%d/%s/%dV", CD_number, meas, voltage));
  system( Form("mkdir -p %s", outdir.c_str()) );
  //-p crea anche le parent se prima non esistono

  c1->SaveAs(Form("%s/event%dlumi%d.png",outdir.c_str(),event,lumi));
  

  delete(c1);
	    
  return 0;
}
