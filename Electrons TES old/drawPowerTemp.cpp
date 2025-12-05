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


  std::ifstream ifs ("data/power_temp.dat" );

  TGraphErrors* gr_power = new TGraphErrors(0);
  TGraphErrors* gr_temp  = new TGraphErrors(0);
  gr_temp->SetName("temp_vs_V");

  std::vector<int> colors = get_colors();

  gr_power->SetMarkerStyle(21);
  gr_power->SetMarkerSize(1.6);
  gr_power->SetMarkerColor(colors[0]);
  gr_power->SetLineColor  (colors[0]);
  
  gr_temp ->SetMarkerStyle(20);
  gr_temp ->SetMarkerSize(1.6);
  gr_temp ->SetMarkerColor(colors[1]);
  gr_temp ->SetLineColor  (colors[1]);
  

  int iPoint = 0;
  double v,i,r,p,t2,i2,r2,pj,v2,t;

  double tzero = 0.;

  //while( !(ifs->eof()) ) {

  while( ifs >> v >> i >> r >> p >> t2 >> i2 >> r2 >> pj >> v2 >> t ) {

    std::cout << v << " " <<  i << " " << r << " " << p << " " << t2 << " " << i2 << " " << r2 << " " << pj << " " << v2 << " " << t << std::endl;

    gr_power->SetPoint     ( iPoint, v, p*1E+12 ); // in pW
    gr_power->SetPointError( iPoint, 0., 0.005*p*1E+12 ); // in pW

    gr_temp->SetPoint     ( iPoint, v, t*1000. ); // in mK
    gr_temp->SetPointError( iPoint, 0., 0.005*t*1000. );

    //if( iPoint==0 ) tzero = 40.6;
    if( iPoint==0 ) tzero = t*1000.;

    iPoint++;

  }

  std::cout << "Found T_bath = " << tzero << " mK" << std::endl;

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  TPad *pad_power  = new TPad("pad_power" ,"",0,0,1,1);

  TPad *pad_temp  = new TPad("pad_temp ","",0,0,1,1);
  pad_temp->SetFillStyle(4000); //will be transparent
  pad_temp->SetFrameFillStyle(0);

  double xmin = 93.;
  double xmax = 107.;

  TH2D *h2_axes_power = new TH2D( "axes_power", "", 10, xmin, xmax, 10, 3.9 , 5.7999  );
  h2_axes_power->SetXTitle( "Nanotube voltage #it{V}_{cnt} (V)" );
  h2_axes_power->SetYTitle( "#it{P}_{J} (pW)" );
  h2_axes_power->GetYaxis()->SetTitleOffset(1.2);
  h2_axes_power->GetYaxis()->SetAxisColor (colors[0]);
  h2_axes_power->GetYaxis()->SetTitleColor(colors[0]);
  h2_axes_power->GetYaxis()->SetLabelColor(colors[0]);

  TH2D *h2_axes_temp  = new TH2D( "axes_temp" , "", 10, xmin, xmax, 10, 38., 66. );
  h2_axes_temp->SetYTitle( "#it{T}_{local} (mK)" );
  h2_axes_temp->GetYaxis()->SetAxisColor (colors[1]);
  h2_axes_temp->GetYaxis()->SetTitleColor(colors[1]);
  h2_axes_temp->GetYaxis()->SetLabelColor(colors[1]);

  pad_power->Draw();
  pad_power->cd();
  h2_axes_power->Draw("");
  gr_power->Draw("psame");

  pad_temp->Draw("Y+");
  pad_temp->cd();

  h2_axes_temp->Draw("Y+");

  TLine* line_tzero = new TLine( xmin, tzero, xmax, tzero );
  line_tzero->SetLineColor( colors[1] );
  line_tzero->SetLineStyle( 7 );
  line_tzero->SetLineWidth( 5 );
  line_tzero->Draw("same");

  TPaveText* label_tzero = new TPaveText( 100.7, tzero-0.5, 102.5, tzero+0.5 );
  label_tzero->SetFillColor(0);
  label_tzero->SetTextColor( colors[1] ); 
  label_tzero->SetTextSize( 0.04 ); 
  label_tzero->AddText( "#it{T}_{bath}");
  label_tzero->Draw("same");

  gr_temp->Draw("psame Y+");

  //gPad->RedrawAxis();
  c1->SaveAs("temp_power.pdf");

  TFile* file = TFile::Open( "temp_vs_V.root", "recreate" );
  file->cd();
  gr_temp->Write();
  file->Close();

  return 0;

}
