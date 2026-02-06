#include <iostream>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <filesystem>
#include "TFile.h"
#include "TTree.h"
#include "TGraphErrors.h"
#include "TH1D.h"
#include "TH1F.h"
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
    double trigtime_h;
    
    tree->SetBranchAddress("ctrl_double", &ctrl_double);
    tree->SetBranchAddress("cosmic_rays", &cosmic_rays);
    tree->SetBranchAddress("electronics", &electronics);
    
    treeraw->SetBranchAddress("lumi"       , &lumi       );
    treeraw->SetBranchAddress("event"      , &event      );
    treeraw->SetBranchAddress("trigtime_m" , &trigtime_m );
    treeraw->SetBranchAddress("trigtime_s" , &trigtime_s );
    treeraw->SetBranchAddress("trigtime_h" , &trigtime_h );
    
    int nEntries = tree->GetEntries();


    //inizializzazione parametri per il ciclo
    float previous_s    = -1.;
    float previous_m    = -1.;
    float previous_h    = -1.;
    int   previous_lumi = -1;
    int   nCounts       = 0; //forme singole
    int   nCounts_PU    = 0; //forme con doppie o triple ecc
    int check           = 0; //controllo su scarto di forma d'onda
 
    //histogrammi per calcolare rate 
    TH1D* h1_rate    = new TH1D( "rate"     , "", 100, 0., 500.);
    TH1D* h1_rate_PU = new TH1D( "rate_PU"  , "", 100, 0., 500.);
    TH1D* delta_tot  = new TH1D( "delta_tot", "", 1  , 0., 1000000.);
    double time_tot = 0;
    
      
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
	previous_h = trigtime_h;
      }

      check=0;
      
      //prendo le nuove lumi e trigtime_s
      treeraw->GetEntry(iEntry);
      tree   ->GetEntry(iEntry);

      if(cosmic_rays==0 && electronics==0 && ctrl_double>0){ //pshape acettata

	check=1; //aggiorno variabile di check
  	
	if(previous_lumi == lumi){ //sono nello stesso file lumi
	  nCounts += ctrl_double; //aggiungo il numero di onde dell'evento
	  if(ctrl_double>1){ nCounts_PU += (ctrl_double-1); } //aggiungo solo numero di PU
	}
	
	if(previous_lumi!=lumi &&
	   (trigtime_s!=previous_s ||
	    trigtime_m!=previous_m ||
	    trigtime_h!=previous_h 
	    )){
	  //sono in file lumi diversi e deltaT validi e ho davvero evento
	  //non ho ancora aggiornato i conteggi nCounts per non prendere quelli del file lumi dopo...
	 
	  //calcolo rate
	  double delta_t = (trigtime_s - previous_s) + (trigtime_m - previous_m) + (trigtime_h - previous_h);
	  double rate    = nCounts/delta_t;
	  double rate_PU = nCounts_PU/delta_t;
	  time_tot+=delta_t;
	  
	  h1_rate->Fill(rate);
	  h1_rate_PU->Fill(rate_PU);

	  //aggiorno per la nuova lumi dopo aver riempito histo
	  nCounts = ctrl_double; 
	  nCounts_PU = (ctrl_double-1);
	  
	}
      }//if su pshape accettata
    } // for entries    

    delta_tot->Fill(time_tot);
    
    
    std::cout << "------------------------" << std::endl;
    std::cout << "   V = " << volts[i] << " V" << std::endl;
    std::cout << "   Mean rate: " << h1_rate->GetMean()
	      << " +/- " << h1_rate->GetMeanError()
	      << " Hz" << std::endl;
    
    std::cout << "   Mean rate (pile-up)   : " << h1_rate_PU->GetMean()
	      << " +/- " << h1_rate_PU->GetMeanError()
	      << " Hz (x" << h1_rate_PU->GetMean()/h1_rate->GetMean() << " more)" << std::endl;

    std::cout << "total time: " << time_tot << std::endl;
    
    TFile* file_rate = TFile::Open( Form("data/root/CD%d/%s/%dV/rate_%dV.root", CD_number,meas,volts[i],volts[i]), "recreate" );
    file_rate->cd();
    h1_rate->Write();
    h1_rate_PU->Write();
    delta_tot->Write();
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
  
  g->SetMarkerSize(3);
  g->SetLineWidth(3);
  g->SetMarkerStyle(20);
  g->SetLineColor(46);
  g->SetMarkerColor(46);

  double xmin = volts[0]-1.;
  double xmax = volts[volts.size()-1]+1.;
  double ymin; double ymax;

  if(CD_number==188){
    ymin    = -2   ; ymax    = 80 ;
  }

  if(CD_number==204 && strcmp(meas,"B60_post_cond3_moku")==0){
    ymin    = -2   ; ymax    = 290 ;
  }

   if(CD_number==204 && strcmp(meas,"B60_preamp_post_cond1")==0){
    ymin    = -2   ; ymax    = 150 ;
  }
    
  TH2D *h2_axes_g = new TH2D( "axes_g", "", 10, xmin, xmax, 10, ymin, ymax);
  h2_axes_g->SetXTitle( "Nanotube voltage #it{V}_{cnt} (V)" );
  h2_axes_g->SetYTitle( "TES counting rate [Hz]" );
  h2_axes_g->GetYaxis()->SetTitleOffset(1.2);
  h2_axes_g->GetYaxis()->SetTitleSize(0.047);
  h2_axes_g->GetXaxis()->SetTitleSize(0.047);
  h2_axes_g->GetYaxis()->SetLabelSize(0.047);
  h2_axes_g->GetXaxis()->SetLabelSize(0.047);
  
  h2_axes_g->Draw("");
  g->Draw("pe same");
  c->SaveAs(Form("plots/CD%d/%s/rate_nopilup.pdf",CD_number,meas));
  c->Clear();

  return 0;
  
}
