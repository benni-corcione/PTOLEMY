#include <iostream>
#include <fstream>
#include <stdlib.h>
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




std::vector<std::string> splitString( const std::string& s, const std::string& divider );



int main() {


  TStyle* style = setStyle();
  style->SetPadTickY(0);
  style->cd();


  std::ifstream ifs ("data/V_vs_T.dat" );

  std::vector<int> colors = get_colors();

  std::vector<double> x;
  std::vector<TGraph*> graphs;
  TGraphErrors* gr_corr = new TGraphErrors(0);
  gr_corr->SetName( "corr" );

  std::string line;

  while( std::getline(ifs, line) )  {

    std::vector<std::string> words = splitString( line, "\t" );

    if( words[0] == "T" ) {

      for( unsigned i=1; i<words.size(); i++ ) x.push_back( atof(words[i].c_str()) );

    } else {

      TGraph* graph = new TGraph(0);
      graph->SetName( Form("graph_%s", words[0].c_str()) );

      for( unsigned i=1; i<words.size(); i++ ) {
        double y = atof(words[i].c_str());
        if( y > 0. ) graph->SetPoint( graph->GetN(), x[i-1], atof(words[i].c_str()) );
      }

      TF1* f1 = new TF1( Form("f1_%s", graph->GetName()), "pol3", 20., 70. );

      graph->Fit( f1, "R" );

      graphs.push_back( graph );

      int iPoint_corr = gr_corr->GetN();

      gr_corr->SetPoint( iPoint_corr, atof(words[0].c_str()), 1./f1->Eval(100.) );

    }

  } // while 

  //TF1* f1_corr = new TF1("f1_corr", "pol2", 0., 64. );
  //gr_corr->Fit( f1_corr, "R" );

  TFile* file = TFile::Open("corr_VvsT.root", "recreate");
  file->cd();
  for( unsigned i=0; i<graphs.size(); ++i ) graphs[i]->Write();
  gr_corr->Write();
  file->Close();

  //for( unsigned 

  return 0;

}

/*
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

  return 0;

}
*/


std::vector<std::string> splitString( const std::string& s, const std::string& divider ) {

  std::string s_copy(s);
  while( s_copy[s_copy.size()-1]==divider )
    s_copy.erase( s_copy.size()-1, s_copy.size() ); // if trailing char is divider, remove it

  size_t pos = 0;
  std::vector<std::string> words;
  std::string word;
  while ((pos = s_copy.find(divider)) != std::string::npos) {
    word = s_copy.substr(0, pos);
    s_copy.erase(0, pos + divider.length());
    if( word!=divider && word.size()>0 )  words.push_back(word);
  }
  //s_copy.erase( s_copy.size()-1, s_copy.size() ); // chopping off trailing char
  words.push_back(s_copy); // last part


  return words;

}

