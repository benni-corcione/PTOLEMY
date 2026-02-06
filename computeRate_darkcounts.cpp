#include <iostream>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <filesystem>
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
  
  //acquisizione tree e treeraw
  TFile* fileraw = TFile::Open(Form("data/root/CD%d/%s/0V/CD%d_0V_treeraw.root", CD_number,meas,CD_number), "read" );
  TFile* file = TFile::Open(Form("data/root/CD%d/%s/0V/CD%d_0V_tree.root", CD_number,meas,CD_number), "read" );
  TTree* treeraw = (TTree*)fileraw->Get("treeraw");
  TTree* tree = (TTree*)file->Get("tree");
  
  //preparazione lettura tree
  
  int event;
  int lumi;
  int ctrl_double;
  int cosmic_rays;
  int electronics;
  double trigtime_m;
  double trigtime_h;
  double trigtime_s;
  
  tree->SetBranchAddress("ctrl_double", &ctrl_double);
  tree->SetBranchAddress("cosmic_rays", &cosmic_rays);
  tree->SetBranchAddress("electronics", &electronics);
  
  treeraw->SetBranchAddress("lumi"       , &lumi       );
  treeraw->SetBranchAddress("event"      , &event      );
  treeraw->SetBranchAddress("trigtime_m" , &trigtime_m );
  treeraw->SetBranchAddress("trigtime_s" , &trigtime_s );
  treeraw->SetBranchAddress("trigtime_h" , &trigtime_h );
  
  int nEntries = tree->GetEntries();

  
  //histogrammi per calcolare rate 
  TH1D* h1_rate    = new TH1D( "rate"     , "", 100, 0., 500.);
  TH1D* h1_rate_PU = new TH1D( "rate_PU"  , "", 100, 0., 500.);
  TH1D* delta_tot  = new TH1D( "delta_tot", "", 1  , 0., 10000.);
  double time_tot = 0;  
  
  //inizializzazione parametri per il ciclo
  float previous_s    = -1.;
  float previous_m    = -1.;
  float previous_h    = -1.;
  int   previous_lumi = -1;
  int   nCounts       = 0; //forme singole
  int   nCounts_PU    = 0; //forme con doppie o triple ecc
  int   check         = 0; //controllo su scarto di forma d'onda
  
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
	  trigtime_h!=previous_h)){
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
  std::cout << "   V = 0 V" << std::endl;
  std::cout << "   Mean rate: " << h1_rate->GetMean()
	    << " +/- " << h1_rate->GetMeanError()
	    << " Hz" << std::endl;
  
  std::cout << "   Mean rate (pile-up)   : " << h1_rate_PU->GetMean()
	    << " +/- " << h1_rate_PU->GetMeanError()
	    << " Hz (x" << h1_rate_PU->GetMean()/h1_rate->GetMean() << " more)" << std::endl;
  std::cout << "Total time: " << time_tot << std::endl;
  
  delete(h1_rate);
  delete(h1_rate_PU);
  delete(delta_tot);
  
  
  return 0;
  
}
