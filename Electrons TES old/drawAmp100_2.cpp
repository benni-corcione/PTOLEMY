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
#include "TLatex.h"
#include "TArrow.h"

#include "common.h"




int main() {

  TStyle* style = setStyle();

  style->SetPadLeftMargin(0.17);
  style->SetPadRightMargin(0.04);
  style->SetPadTopMargin(0.04);
  style->SetPadBottomMargin(0.15);

  //std::string run = "CD188_100V";

  TFile* file = TFile::Open( "tree/CD188_100V_tree.root", "read" );
  //TFile* file = TFile::Open("tree/100Vb_tree.root", "read" );
  TTree* tree = (TTree*)file->Get("tree");

  int nbins = 100;
  float xmin = 0.;
  float xmax = 1.0;
  float ymin = 0.;
  float ymax = 225.;

  TH2D* h2_axes = new TH2D( "axes", "", 10, xmin, xmax, 10, ymin, ymax );
  h2_axes->SetXTitle("TES amplitude (V)" );
  h2_axes->GetYaxis()->SetTitleOffset(1.5);
  h2_axes->SetYTitle( Form("Counts / (%.2f V)", (double)((xmax-xmin)/nbins)) );

  TH1D* h1_amp = new TH1D("amp100", "", nbins, xmin, xmax );
  h1_amp->SetLineWidth(2);
  h1_amp->SetFillStyle(3004); //righette orizzontali
  h1_amp->SetFillColor(kBlack);

  //così mi sa ha riempito l'istogramma
  tree->Project( "amp100", "amp" );

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  h2_axes->Draw();

  float trig = 0.166;
  TLine* line_trigger = new TLine( trig, ymin, trig, ymax );
  line_trigger->SetLineColor( 29 );
  line_trigger->SetLineWidth( 8 );
  line_trigger->Draw("same");

  TLatex* label_trigger = new TLatex( trig-0.02, 1620/12., "Trigger threshold" );
  label_trigger->SetTextSize( 0.035 );
  label_trigger->SetTextColor(29);
  label_trigger->SetTextAngle(90);
  label_trigger->Draw("same");

  TLatex* label_vcnt = new TLatex( 0.25, 200, "#it{V}_{cnt} = 100 V" );
  label_vcnt->SetTextSize( 0.042 );
  label_vcnt->SetTextColor(kBlack);
  label_vcnt->Draw("same");

  TLatex* label_abspeak = new TLatex();
  label_abspeak->SetTextSize( 0.035 );
  label_abspeak->SetTextColor(46);
  label_abspeak->SetTextAlign(30);
  //label_abspeak->DrawLatex( 0.65, 2000, "Absorption" );
  //label_abspeak->DrawLatex( 0.65, 1900, "peak" );
  //label_abspeak->DrawLatex( 0.65, 2000, "e^{-} total" );
  //label_abspeak->DrawLatex( 0.65, 1870, "absorption" );
  label_abspeak->DrawLatex( 0.69, 175., "e^{-} total" );
  label_abspeak->DrawLatex( 0.69, 165., "absorption" );

  float arrowsize = 0.015;

  TArrow* ar1 = new TArrow( 0.71, 177., 0.77, 191., arrowsize, "|>" );
  //TArrow* ar1 = new TArrow( 0.67, 2050, 0.74, 2250, arrowsize, "|>" );
  ar1->SetLineWidth(4);
  ar1->SetLineColor(46);
  ar1->SetFillColor(46);
  ar1->Draw();

  float shift = 37.;

  label_abspeak->SetTextColor(94);
  label_abspeak->SetTextAlign(30);
  label_abspeak->DrawLatex( 0.52, 1210/12.+shift, "e^{-} partial" );
  label_abspeak->DrawLatex( 0.52, 1080/12.+shift, "absorption" );

  TArrow* ar2 = new TArrow( 0.55, 1200/12.+shift, 0.63, 1200/12.+shift, arrowsize, "|>" );
  //TArrow* ar2 = new TArrow( 0.55, 1200, 0.63, 1200, arrowsize, "|>" );
  ar2->SetLineWidth(4);
  ar2->SetLineColor(94);
  ar2->SetFillColor(94);
  ar2->Draw();

  TArrow* ar3 = new TArrow( 0.53, 1050/12.+shift, 0.59, 850/12.+shift, arrowsize, "|>" );
  //TArrow* ar3 = new TArrow( 0.53, 1050, 0.59, 850, arrowsize, "|>" );
  ar3->SetLineWidth(4);
  ar3->SetLineColor(94);
  ar3->SetFillColor(94);
  ar3->Draw();

  TArrow* ar4 = new TArrow( 0.47, 1030/12.+shift, 0.48, 700/12.+shift, arrowsize, "|>" );
  //TArrow* ar4 = new TArrow( 0.47, 1030, 0.48, 700, arrowsize, "|>" );
  ar4->SetLineWidth(4);
  ar4->SetLineColor(94);
  ar4->SetFillColor(94);
  ar4->Draw();

  h1_amp->Draw("same");

  gPad->RedrawAxis();

  c1->SaveAs("amp100_2.pdf");

  return 0;

}
