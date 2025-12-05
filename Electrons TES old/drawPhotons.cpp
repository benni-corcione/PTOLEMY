#include <iostream>
#include <fstream>
#include "TFile.h"
#include "TTree.h"
#include "TH2D.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLine.h"
#include "TPaveText.h"
#include "TF1.h"

#include "common.h"




TH1* getHistogram( TGraph* graph );


int main() {


  TStyle* style = setStyle();
  //style->SetPadTickY(0);
  style->SetPadLeftMargin(0.17); 
  style->SetPadRightMargin(0.14);
  style->SetPadTopMargin(0.14);
  style->SetPadBottomMargin(0.15);
  style->SetPadTickX(0);
  style->cd();

  std::ifstream ifs ("data/photons.dat" );

  std::vector<float> v_energy;
  std::vector<int> v_counts;
  int counts; 
  float energy; 
  float energy2; // useless
  float gaus;


  TGraph* gr_counts = new TGraph(0);
  TGraph* gr_f1gaus = new TGraph(0);

  //nel file ci sono già i punti della gaussiana del fit
  while( ifs >> energy >> counts >> energy2 >> gaus ) {
    gr_counts->SetPoint( gr_counts->GetN(), energy, counts );
    gr_f1gaus->SetPoint( gr_f1gaus->GetN(), energy2, gaus );
    v_energy.push_back(energy);
    v_counts.push_back(counts);
  }

  

  TCanvas* c1 = new TCanvas("c1", "", 600, 600 );
  c1->cd();

  float ymax = 940.;

  //Pad dell'energia (asse sotto)
  TPad *pad_e  = new TPad("pad_current" ,"",0,0,1,1);

  //Pad del photon number (asse sopra)
  TPad *pad_n  = new TPad("pad_n ","",0,0,1,1);
  pad_n->SetFillStyle(4000); //will be transparent
  pad_n->SetFrameFillStyle(0);


  pad_e->Draw();
  pad_e->cd();

  float xMin = 89.500001;
  //float xMin = 90.000001;
  float xMax = 105.7999;

  TH2D* h2_axes = new TH2D( "axes", "", 10, xMin, xMax, 10, 0., ymax );
  h2_axes->SetXTitle("Energy (eV)");
  h2_axes->SetYTitle("Counts");
  h2_axes->GetYaxis()->SetTitleOffset(1.5);
  h2_axes->GetXaxis()->SetNdivisions(505);
  h2_axes->GetXaxis()->SetTitleOffset(1.1);
  h2_axes->Draw("");

  TH1* h_counts = getHistogram(gr_counts);
  h_counts->SetFillStyle(3004);
  h_counts->SetFillColor(kBlack);
  h_counts->SetLineColor(kBlack);
  h_counts->SetLineWidth(1);
  h_counts->Draw("same");

  ////gr_counts->SetFillStyle(3004);
  //gr_counts->SetLineWidth(1);
  //gr_counts->SetLineColor(kGray);
  //gr_counts->SetFillColor(kGray);
  //gr_counts->Draw("B same");

  gr_f1gaus->SetLineWidth(4);
  gr_f1gaus->SetLineColor(46);
  gr_f1gaus->Draw("Lsame");

  pad_e->RedrawAxis();
  pad_e->Draw();

  pad_n->Draw("X+");
  pad_n->cd();

  TH2D* h2_axes_n = new TH2D( "axes_n", "", 10, xMin*406./1240., xMax*406./1240., 10, 0., ymax );
  h2_axes_n->SetXTitle("Number of photons (#lambda_{#gamma} = 406 nm)" );
  h2_axes_n->SetYTitle("Counts");
  h2_axes_n->GetYaxis()->SetTitleOffset(1.5);
  h2_axes_n->GetXaxis()->SetNdivisions(6);
  h2_axes_n->GetXaxis()->SetTitleOffset(1.);
  //h2_axes_n->GetXaxis()->SetAxisColor (46);
  //h2_axes_n->GetXaxis()->SetTitleColor(46);
  //h2_axes_n->GetXaxis()->SetLabelColor(46);
  h2_axes_n->Draw("X+");

  pad_n->Draw("same");

  c1->SaveAs("photons.pdf");

  return 0;
 
}

//passaggio da grafico a histo
TH1* getHistogram( TGraph* graph ) {

  double xPrev, x;
  double yPrev, y;
  
  float bins[graph->GetN()];

  for( unsigned i=0; i<graph->GetN(); ++i ) {

    if( i==0 ) {
      xPrev = 0.;
      graph->GetPoint( i, x, y );
    } else {
      graph->GetPoint( i-1, xPrev, yPrev );
      graph->GetPoint(   i, x, y );
    }
    bins[i] = 0.5*(x+xPrev);
    std::cout << bins[i] <<std::endl;

  } // for i
  

  TH1D* histo = new TH1D( Form("h_%s", graph->GetName()), "", graph->GetN()-1, bins );

  for( unsigned i=0; i<graph->GetN(); ++i ) {
    graph->GetPoint( i, x, y );
    histo->SetBinContent( histo->FindBin(x), y );
  } // for i

  return histo;

}
