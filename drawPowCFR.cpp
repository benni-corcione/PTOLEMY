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
#include "TLegend.h"
#include "TStyle.h"

#include "graphics.h"
#include "AndCommon.h"
#include "CD_details.h"


int main() {


  //TStyle* style = setStyle();
  //style->SetPadTickY(0);
  //style->cd();

  Settings settings;
  TCanvas* c1 = new TCanvas( "c1", "", 1500, 1500 );
  settings.Margin(c1);
  c1->SetLeftMargin(0.155);
  c1->cd();
  
  std::ifstream ifs188 ("data/power_CD188.txt"); 
  std::ifstream ifs204 ("data/power_CD204.txt");
  std::ifstream ifs222 ("data/power_CD222.txt");

  TGraphErrors* gr_power188 = new TGraphErrors(0);
  TGraphErrors* gr_power204 = new TGraphErrors(0);
  TGraphErrors* gr_power222 = new TGraphErrors(0);

  TLegend* legend = new TLegend(0.5,0.2,0.95,0.3);
  legend->SetTextSize(0.025);
  legend->SetMargin(0.1);
  legend->SetBorderSize(0);
  legend->SetFillStyle(0);
  
  //std::vector<int> colors = get_colors();

  gr_power188->SetMarkerStyle(20);
  gr_power188->SetMarkerSize(3);
  gr_power188->SetLineWidth(2);
  gr_power188->SetMarkerColor(46);
  gr_power188->SetLineColor  (46);
  gr_power204->SetMarkerStyle(21);
  gr_power204->SetMarkerSize(3);
  gr_power204->SetLineWidth(2);
  gr_power204->SetMarkerColor(38);
  gr_power204->SetLineColor  (38);
  gr_power222->SetMarkerStyle(22);
  gr_power222->SetMarkerSize(3);
  gr_power222->SetMarkerColor(92);
  gr_power222->SetLineWidth(2);
  gr_power222->SetLineColor  (92);
  
  int iPoint = 0;
  double v1,i1,r1,p1;
  double v2,i2,r2,p2;
  double v3,i3,r3,p3;
  float norma1 = 0;
  float norma2 = 0;
  float norma3 = 0;
  //while( !(ifs->eof()) ) {

  while( ifs188 >> v1 >> i1 >> r1 >> p1 ) {

    if(v1==95){norma1=p1*1E+12;}
    gr_power188->SetPoint     ( iPoint, v1, (p1*1E+12)/norma1 ); // in pW
    gr_power188->SetPointError( iPoint, 0., 0.005*p1*1E+12/norma1 ); // in pW
    iPoint++;
      //std::cout << v1 << " " << p1*1E+12 << std::endl;
  }

  iPoint = 0;

  while( ifs204 >> v2 >> i2 >> r2 >> p2 ) {
      
    if(v2==95){norma2=p2*1E+12;}
    gr_power204->SetPoint     ( iPoint, v2, p2*1E+12/norma2 ); // in pW
    gr_power204->SetPointError( iPoint, 0., 0.005*p2*1E+12/norma2 ); // in pW
    iPoint++;

      //std::cout << v2 << " " << p2*1E+12/norma2 << std::endl;
  }

  while( ifs222 >> v3 >> i3 >> r3 >> p3 ) {
      
    if(v3==122){norma3=p3*1E+12;}
    gr_power222->SetPoint     ( iPoint, v3, p3*1E+12/norma3 ); // in pW
    gr_power222->SetPointError( iPoint, 0., 0.015*p3*1E+12/norma3 ); // in pW
    iPoint++;

      //std::cout << v3 << " " << p3*1E+12/norma3 << std::endl;
  }
  
 
  

  //TPad *pad_power  = new TPad("pad_power" ,"",0,0,1,1);

  double xmin = 93.;
  double xmax = 132.;

  TH2D *h2_axes_power = new TH2D( "axes_power", "", 10, xmin, xmax, 10, 0.7 , 1.05  );
  h2_axes_power->SetXTitle( "Nanotube voltage #it{V}_{cnt} (V)" );
  h2_axes_power->SetYTitle( "#it{P}_{J}(#it{V}_{cnt})/#it{P}_{J}(#it{V}_{cnt}^{ min})" );
  h2_axes_power->GetYaxis()->SetTitleOffset(1.9);
  h2_axes_power->GetXaxis()->SetTitleOffset(1.5);
  h2_axes_power->GetYaxis()->SetDecimals(kTRUE);  // forza 1.00 invece di 1
  h2_axes_power->GetYaxis()->SetAxisColor (kBlack);
  h2_axes_power->GetYaxis()->SetTitleColor(kBlack);
  h2_axes_power->GetYaxis()->SetLabelColor(kBlack);

  TLine* line_uno = new TLine( xmin, 1, xmax, 1 );
  line_uno->SetLineColor(kBlack);
  line_uno->SetLineStyle( 7 );
  line_uno->SetLineWidth( 3 );
    
  h2_axes_power->Draw("");
  line_uno->Draw("same");
  gr_power188->Draw("psame");
  gr_power222->Draw("psame");
  gr_power204->Draw("psame");

  gStyle->SetOptStat(0);
  legend->AddEntry(gr_power188, "TES side: 100 um, CNT side: 3 mm", "pe");
  legend->AddEntry(gr_power222, "TES side: 100 um, CNT side: 1 mm", "pe");
  legend->AddEntry(gr_power204, "TES side:   60 um, CNT side: 1 mm", "pe");
  legend->Draw("same");

  
  
  c1->SaveAs("plots/articolo2/power_CFR_188_204_222.pdf");

  return 0;

}
