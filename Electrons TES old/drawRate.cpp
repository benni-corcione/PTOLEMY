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

#include "common.h"




int main() {


  TStyle* style = setStyle();
  style->SetPadTickY(0);
  style->cd();

  std::vector<int> colors = get_colors();

  std::ifstream ifs ("data/CNTArO2StrongEtching_N1new_mini_INRiM_MICb_30mKplate_d0p5_IvsV_anode_90to110V_20240328_after_conditioning_sweepR.dat" );

  TGraphErrors* gr_iv = new TGraphErrors(0);

  float v, t, i, sigmai;
  double y95_iv = 0.;
  double y96_iv = 0.;
  double y97_iv = 0.;
  double y98_iv = 0.;
  
    //file con voltaggio, temperatura, corrente
    //e sigma di corrente
  while( ifs >> v >> t >> i >> sigmai ) {

    double thisx = v;
    double thisy = -i; //corrente in positivo
    double thisyerr = sigmai;

    //riempimento grafico da file
    int iPoint = gr_iv->GetN();
    gr_iv->SetPoint( iPoint, thisx, thisy );
    gr_iv->SetPointError( iPoint, 0., thisyerr );

    if( thisx == 95. ) y95_iv = thisy;
    if( thisx == 96. ) y96_iv = thisy;
    if( thisx == 97. ) y97_iv = thisy;
    if( thisx == 98. ) y98_iv = thisy;

    if( iPoint>0 ) {

      double prevx, prevy;
      //prende punto precedente del grafico
      gr_iv->GetPoint( iPoint-1, prevx, prevy );

      if( prevx>=95. && thisx<95. )  // remember: in sweepR one starts from high currents
        y95_iv = 0.5*(prevy+thisy);
      
    } // if ipoint > 0

  } // while


  TGraphErrors* gr_rate = new TGraphErrors(0);
  TH1D* h1_ratio = new TH1D( "ratio", "", 100, 0., 100. );

  for( int v = 95; v<106; ++v ) {

    //dovrei capire cosa c'è qui dentro di preciso, un histo con rate calcolate per
    //diversi intervalli temporali?
    TFile* file_rate = TFile::Open( Form("~/Documents/ANDROMeDa/ANDROMeDa/FieldEmissionAnalysis/analysis/rate_%dV.root", v), "read" );
    TH1D* h1_rate = (TH1D*)file_rate->Get( "rate" );

    int iPoint = gr_rate->GetN();
    float thisrate = h1_rate->GetMean();
    gr_rate->SetPoint( iPoint, v, thisrate );
    gr_rate->SetPointError( iPoint, 0., h1_rate->GetMeanError() );

    //questa mi sembra una sorta di normalizzazione 
    if( v==95 ) h1_ratio->Fill( thisrate/y95_iv );
    if( v==96 ) h1_ratio->Fill( thisrate/y96_iv );
    if( v==97 ) h1_ratio->Fill( thisrate/y97_iv );
    if( v==98 ) h1_ratio->Fill( thisrate/y98_iv );

    file_rate->Close();

  }

  // normalize

  TGraphErrors* gr_rate_au = new TGraphErrors(0);

  double x0_rate, y0_rate;
  gr_rate->GetPoint( 0, x0_rate, y0_rate );

  double k = 45.;

  for( unsigned iPoint = 0; iPoint<gr_rate->GetN(); iPoint++ ) {

    double x, y;
    gr_rate->GetPoint( iPoint, x, y );

    double yerr = gr_rate->GetErrorY(iPoint);

    gr_rate_au->SetPoint( iPoint, x, y/k );
    gr_rate_au->SetPointError( iPoint, 0, yerr/k );
    //gr_rate_au->SetPoint( iPoint, x, y );
    //gr_rate_au->SetPointError( iPoint, 0, yerr );

  }


  TFile* fileraw = TFile::Open( "~/FISICA/Nanotubes/ANDROMeDa/FieldEmissionAnalysis/analysis/data/root/CD188/tree_100V.root", "read" );
  TTree* treeraw = (TTree*)fileraw->Get("treeraw");

std::ofstream ofs_pshape("pshape.txt");
  TGraphErrors* gr_pshape = new TGraphErrors(0);

 
  int lumi;
  treeraw->SetBranchAddress( "lumi", &lumi );
  int event;
  treeraw->SetBranchAddress( "event", &event );
  float pshape[2502];
  treeraw->SetBranchAddress( "pshape", pshape );

  for( unsigned iEntry=0; iEntry<treeraw->GetEntries(); ++iEntry ) {

    treeraw->GetEntry(iEntry);

    if( lumi == 2 && event == 5 ) {
    
      for( unsigned iPoint=0; iPoint<2502; ++iPoint ) {
        float x = (float)iPoint*4E-8*1.E6; // in microseconds
        float y = pshape[iPoint];
        gr_pshape->SetPoint( iPoint, x, y );
        ofs_pshape << x << " " << y << std::endl;
      }

      break;

    } // if

  } // for entries

ofs_pshape.close();

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  TPad *pad_current  = new TPad("pad_current" ,"",0,0,1,1);

  TPad *pad_rate  = new TPad("pad_rate ","",0,0,1,1);
  pad_rate->SetFillStyle(4000); //will be transparent
  pad_rate->SetFrameFillStyle(0);

  double xmin = 93.;
  double xmax = 107.;

  TH2D *h2_axes_current = new TH2D( "axes_current", "", 10, xmin, xmax, 10, 0., 2.2 );
  h2_axes_current->SetXTitle( "Nanotube voltage #it{V}_{cnt} (V)" );
  h2_axes_current->SetYTitle( "#it{I}_{cnt} (pA)" );
  //h2_axes_current->SetYTitle( "Current emitted by nanotubes #it{I}_{cnt} (pA)" );
  h2_axes_current->GetYaxis()->SetNdivisions(505);
  h2_axes_current->GetYaxis()->SetTitleOffset(1.2);
  h2_axes_current->GetYaxis()->SetAxisColor (colors[0]);
  h2_axes_current->GetYaxis()->SetTitleColor(colors[0]);
  h2_axes_current->GetYaxis()->SetLabelColor(colors[0]);

  //TH2D *h2_axes_rate  = new TH2D( "axes_rate" , "", 10, xmin, xmax, 10, 0., 99.9 );
  TH2D *h2_axes_rate  = new TH2D( "axes_rate" , "", 10, xmin, xmax, 10, 0., 2.2 );
  h2_axes_rate->SetYTitle( "TES counting rate (a.u.)" );
  //h2_axes_rate->SetYTitle( "d#it{S}/d#it{t} (a.u.)" );
  h2_axes_rate->GetYaxis()->SetNdivisions(505);
  h2_axes_rate->GetYaxis()->SetAxisColor (colors[1]);
  h2_axes_rate->GetYaxis()->SetTitleColor(colors[1]);
  h2_axes_rate->GetYaxis()->SetLabelColor(colors[1]);

  pad_current->Draw();
  pad_current->cd();
  h2_axes_current->Draw("");

  gr_iv->SetMarkerStyle(20);
  gr_iv->SetMarkerColor(kBlack);
  gr_iv->SetLineWidth(3);
  gr_iv->SetLineColor(kBlack);
  gr_iv->SetFillColor(kBlack);

  gr_iv->Draw("L2 same");

  pad_rate->Draw("Y+");
  pad_rate->cd();

  h2_axes_rate->Draw("Y+");

  gr_rate_au->SetMarkerStyle(20);
  gr_rate_au->SetMarkerSize(2);
  gr_rate_au->SetMarkerColor(colors[1]);
  gr_rate_au->SetLineColor(colors[1]);
  gr_rate_au->SetLineWidth(2);

  gr_rate_au->Draw("p same" );
  //gr_rate->Draw("p* same" );


  TPad *pad_pshape  = new TPad("pad_pshape" ,"",0.18,0.45,0.6,0.85);
  pad_pshape->SetFillStyle(4000); //will be transparent
  pad_pshape->SetTopMargin(0.01);
  pad_pshape->SetBottomMargin(0.25);
  pad_pshape->SetLeftMargin(0.25);
  pad_pshape->SetRightMargin(0.06);
  pad_pshape->Draw();
  pad_pshape->cd();

  TH2D* h2_axes_pshape = new TH2D("axes_pshape", "", 10, 0, 100., 10., -0.9, 0.0999 );
  h2_axes_pshape->GetXaxis()->SetNdivisions(505);
  h2_axes_pshape->GetXaxis()->SetLabelSize(0.08);
  h2_axes_pshape->GetXaxis()->SetTitleOffset(1.);
  h2_axes_pshape->GetXaxis()->SetTitleSize(0.09);
  h2_axes_pshape->GetYaxis()->SetNdivisions(505);
  h2_axes_pshape->GetYaxis()->SetLabelSize(0.08);
  h2_axes_pshape->GetYaxis()->SetTitleOffset(1.4);
  h2_axes_pshape->GetYaxis()->SetTitleSize(0.09);
  h2_axes_pshape->SetXTitle( "#it{t} (#mus)" );
  h2_axes_pshape->SetYTitle( "Amplitude (V)" );
  h2_axes_pshape->Draw();

  gr_pshape->SetLineWidth(2);
  gr_pshape->Draw("Lsame");

  c1->SaveAs("rate.pdf");


  return 0;

}
