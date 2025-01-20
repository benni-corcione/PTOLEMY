#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "TFile.h"
#include "TTree.h"
#include "TGraphErrors.h"
#include "TH1D.h"
#include "TPad.h"
#include "TLine.h"

#include "AndCommon.h"
#include "pshape_functions.h"
#include "graphics.h"
#include "CD_details.h"

int main(int argc, char* argv[] ) {

  if( argc != 3 ) {

    std::cout << "USAGE: ./computeRate [CD_number] [misura]" << std::endl;
    std::cout << "EXAMPLE: ./computeRate 188 conteggi" << std::endl;
    exit(1);
  }


  int CD_number = atoi(argv[1]);
  char* meas    =      argv[2] ;
  
  std::vector<int> volts = GetVoltages(CD_number, meas);

  //array per costruire TGraphErrors
  float voltages    [volts.size()];
  float volt_err    [volts.size()];
  float rates       [volts.size()];
  float rates_err   [volts.size()];
  float rates_PU    [volts.size()];
  float rates_err_PU[volts.size()];
  
  //for che scorre tutti i voltaggi
  for(int i=0; i<volts.size(); i++){

    //acquisizione tree e treeraw
    TFile* fileraw = TFile::Open(Form("data/root/CD%d/%s/%dV/CD%d_%dV_treeraw.root", CD_number,meas,volts[i],CD_number,volts[i]), "read" );
    TFile* file = TFile::Open(Form("data/root/CD%d/%s/%dV/CD%d_%dV_tree.root", CD_number,meas,volts[i],CD_number,volts[i]), "read" );
    TTree* treeraw = (TTree*)fileraw->Get("treeraw");
    TTree* tree = (TTree*)file->Get("tree");

    //preparazione lettura tree

    int event;
    int lumi;
    int ctrl_double;
    int cosmic_rays;
    int electronics;
    double trigtime_m;
    double trigtime_s;
    
    tree->SetBranchAddress("ctrl_double", &ctrl_double);
    tree->SetBranchAddress("cosmic_rays", &cosmic_rays);
    tree->SetBranchAddress("electronics", &electronics);
    
    treeraw->SetBranchAddress("lumi"       , &lumi       );
    treeraw->SetBranchAddress("event"      , &event      );
    treeraw->SetBranchAddress("trigtime_m" , &trigtime_m );
    treeraw->SetBranchAddress("trigtime_s" , &trigtime_s );
    
    int nEntries = tree->GetEntries();


    //inizializzazione parametri per il ciclo
    float previous_s    = -1.;
    float previous_m    = -1.;
    int   previous_lumi = -1;
    int   nCounts       = 0; //forme singole
    int   nCounts_PU    = 0; //forme con doppie o triple ecc
    int check           = 0; //controllo su scarto di forma d'onda
 
    //histogrammi per calcolare rate 
    TH1D* h1_rate    = new TH1D( "rate"   , "", 100, 0., 500.);
    TH1D* h1_rate_PU = new TH1D( "rate_PU", "", 100, 0., 500.);
    
      
    for( int iEntry = 0; iEntry < nEntries; ++iEntry ) {
	  
      //al primo ciclo non lavoro, al secondo avrò previous_lumi e previous_s
      //settati sull'onda prima e lumi e trig_s sull'onda successiva
      if( iEntry==0 ) {
	treeraw->GetEntry(0);
	previous_lumi = lumi;
      } 

      //riassegno quelle del ciclo precedente se è stata
      //accettata la scorsa pshape, sennò scorro sugli eventi ignorando
      //completamente la pshape scartata
      if(check==1){
	previous_lumi = lumi;
	previous_s = trigtime_s;
	previous_m = trigtime_m;
      }
      
      //prendo le nuove lumi e trigtime_s
      treeraw->GetEntry(iEntry);
      tree   ->GetEntry(iEntry);

      if(cosmic_rays==0 && electronics==0){ //pshape acettata

	check=1; //aggiorno variabile di check
  	
	if(previous_lumi == lumi){ //sono nello stesso file lumi
	  nCounts += ctrl_double; //aggiungo il numero di onde dell'evento
	  if(ctrl_double>1){ nCounts_PU += (ctrl_double-1); } //aggiungo solo numero di PU
	}
	
	if(previous_lumi!=lumi &&
	   (trigtime_s!=previous_s ||
	    trigtime_m!=previous_m) ){ //sono in file lumi diversi e deltaT validi
	  //non ho ancora aggiornato i conteggi nCounts per non prendere quelli del file lumi dopo...
	  
	  //calcolo rate
	  double delta_t = (trigtime_s - previous_s) + (trigtime_m - previous_m);
	  double rate    = nCounts/delta_t;
	  double rate_PU = nCounts_PU/delta_t;
	  
	  h1_rate->Fill(rate);
	  h1_rate_PU->Fill(rate_PU);

	  //aggiorno per la nuova lumi dopo aver riempito histo
	  nCounts = ctrl_double; 
	  nCounts_PU = (ctrl_double-1);
	}
      }//if su pshape accettata
    } // for entries    

    
    std::cout << "------------------------" << std::endl;
    std::cout << "   V = " << volts[i] << " V" << std::endl;
    std::cout << "   Mean rate: " << h1_rate->GetMean()
	      << " +/- " << h1_rate->GetMeanError()
	      << " Hz" << std::endl;
    
    std::cout << "   Mean rate (pile-up)   : " << h1_rate_PU->GetMean()
	      << " +/- " << h1_rate_PU->GetMeanError()
	      << " Hz (x" << h1_rate_PU->GetMean()/h1_rate->GetMean() << " more)" << std::endl;

    TFile* file_rate = TFile::Open( Form("rate_%dV.root", volts[i]), "recreate" );
    file_rate->cd();
    h1_rate->Write();
    h1_rate_PU->Write();
    file_rate->Close();
    
    std::cout << " -> Saved rate in: " << file_rate->GetName() << std::endl;

    //settati parametri per grafici
    voltages[i]     = volts[i];
    volt_err[i]     = 0;
    rates[i]        = h1_rate->GetMean();
    rates_err[i]    = h1_rate->GetMeanError();
    rates_PU[i]     = h1_rate_PU->GetMean();
    rates_err_PU[i] = h1_rate_PU->GetMeanError();


    } // for volts

  //preparazione grafica
  // gStyle->SetFrameLineWidth(2);
  gStyle->SetTitleFontSize(0.05);
  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c","",1400,1200);
  Settings settings;
  settings.Margin(c);
  c->cd();
 
  TGraphErrors* g    = new TGraphErrors(volts.size(),voltages,rates   ,volt_err,rates_err   );
  TGraphErrors* g_PU = new TGraphErrors(volts.size(),voltages,rates_PU,volt_err,rates_err_PU);

  TPad *pad_g    = new TPad("pad_g"   ,"",0,0,1,1);
  TPad *pad_g_PU = new TPad("pad_g_PU","",0,0,1,1);
  pad_g_PU->SetFillStyle(0); //will be transparent
  pad_g_PU->SetFrameFillStyle(0);
  
  g->SetMarkerSize(2);
  g->SetLineWidth(2);
  g->SetMarkerStyle(20);
  g->SetLineColor(kRed-7);
  g->SetMarkerColor(kRed-7);

  g_PU->SetMarkerSize(2);
  g_PU->SetLineWidth(2);
  g_PU->SetMarkerStyle(20);
  g_PU->SetLineColor(kAzure+1);
  g_PU->SetMarkerColor(kAzure+1);

  double xmin = volts[0]-1.;
  double xmax = volts[volts.size()-1]+1.;
  double ymin   ; double ymax   ;
  double ymin_PU; double ymax_PU;

  if(CD_number==188){
    ymin    = -2   ; ymax    = 80 ;
    ymin_PU = -0.05; ymax_PU = 1.25;
  }
    
  TH2D *h2_axes_g = new TH2D( "axes_g", "", 10, xmin, xmax, 10, ymin, ymax);
  h2_axes_g->SetXTitle( "Nanotube voltage #it{V}_{cnt} (V)" );
  h2_axes_g->SetYTitle( "Rate [Hz]" );
  h2_axes_g->GetYaxis()->SetTitleOffset(1.2);
  h2_axes_g->GetYaxis()->SetAxisColor (kRed-7);
  h2_axes_g->GetYaxis()->SetTitleColor(kRed-7);
  h2_axes_g->GetYaxis()->SetLabelColor(kRed-7);

  TH2D *h2_axes_g_PU  = new TH2D( "axes_g_PU" , "", 10, xmin, xmax, 10, ymin_PU, ymax_PU);
  h2_axes_g_PU->SetYTitle( "Rate pileup [Hz]" );
  h2_axes_g_PU->GetYaxis()->SetTitleOffset(1.2);
  h2_axes_g_PU->GetYaxis()->SetAxisColor (kAzure+1);
  h2_axes_g_PU->GetYaxis()->SetTitleColor(kAzure+1);
  h2_axes_g_PU->GetYaxis()->SetLabelColor(kAzure+1);

  pad_g->Draw();
  pad_g->cd();
  h2_axes_g->Draw("");
  g->Draw("pe same");
  //c->SaveAs(Form("plots/CD%d/%s/rate.pdf",CD_number,meas));
  //c->Clear();

  pad_g_PU->Draw("Y+");
  pad_g_PU->cd();
  h2_axes_g_PU->Draw("Y+");
  g_PU->Draw("pe same Y+");

  
  TLine* line_tzero = new TLine( xmin, ymin_PU, xmin, ymax_PU);
  line_tzero->SetLineColor(kRed-7);
  line_tzero->Draw("same");
  
  
  c->SaveAs(Form("plots/CD%d/%s/rate.png",CD_number,meas));
  c->Clear();
  
  return 0;
  
}
