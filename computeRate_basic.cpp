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
  
  std::vector<int> volts = {135,138};

  //array per costruire TGraphErrors
  float voltages    [volts.size()];
  float volt_err    [volts.size()];
  float rates       [volts.size()];
  float rates_err   [volts.size()];

  //for che scorre tutti i voltaggi
  for(int i=0; i<volts.size(); i++){

    //acquisizione tree e treeraw
    TFile* fileraw = TFile::Open(Form("data/root/CD%d/%s/%dV/CD%d_%dV_treeraw.root", CD_number,meas,volts[i],CD_number,volts[i]), "read" );
    TFile* file = TFile::Open(Form("data/root/CD%d/%s/%dV/CD%d_%dV_tree.root", CD_number,meas,volts[i],CD_number,volts[i]), "read" );
    TTree* treeraw = (TTree*)fileraw->Get("treeraw");
    TTree* tree = (TTree*)file->Get("tree");

    //preparazione lettura tree

    float amp;
    int event;
    int lumi;
    double trigtime_m;
    double trigtime_s;
    double trigtime_h;
    double trigtime_d;
    //float thresh=0.05; //B60
    float thresh=0.03; //H60 
    tree   ->SetBranchAddress("amp"        , &amp       );
    treeraw->SetBranchAddress("lumi"       , &lumi       );
    treeraw->SetBranchAddress("event"      , &event      );
    treeraw->SetBranchAddress("trigtime_m" , &trigtime_m );
    treeraw->SetBranchAddress("trigtime_s" , &trigtime_s );
    treeraw->SetBranchAddress("trigtime_h" , &trigtime_h );
    treeraw->SetBranchAddress("trigtime_d" , &trigtime_d );
    
    int nEntries = tree->GetEntries();


    //inizializzazione parametri per il ciclo
    float previous_s    = -1.;
    float previous_m    = -1.;
    float previous_h    = -1.;
    float previous_d    = -1.;
    int   previous_lumi = -1;
    int   nCounts       = 0; //forme singole
    int check=0; //ctrl amp
    
    //histogrammi per calcolare rate 
    TH1D* h1_rate    = new TH1D( "rate"     , "", 100, 0., 500.);
    TH1D* delta_tot  = new TH1D( "delta_tot", "", 1  , 0., 10000000.);
    double time_tot = 0;
    
    for( int iEntry = 0; iEntry < nEntries; ++iEntry ) {
	  
      //al primo ciclo non lavoro, al secondo avrò previous_lumi e previous_s
      //settati sull'onda prima e lumi e trig_s sull'onda successiva
      if( iEntry==0 ) {
	treeraw->GetEntry(0);
	previous_lumi = lumi;
      }

      if(check==0){ //amp check
      previous_lumi = lumi;
      previous_s = trigtime_s;
      previous_m = trigtime_m;
      previous_h = trigtime_h;
      previous_d = trigtime_d;
      }

      check=0;
      
      //prendo le nuove lumi e trigtime_s
      treeraw->GetEntry(iEntry);
      tree   ->GetEntry(iEntry);

      if(amp>thresh){
	check=1;
      if(previous_lumi == lumi){ //sono nello stesso file lumi
	nCounts += 1; //aggiungo il numero di onde dell'evento
      }
      
      if(previous_lumi!=lumi &&
	 (trigtime_s!=previous_s ||
	  trigtime_m!=previous_m ||
	  trigtime_d!=previous_d ||
	  trigtime_h!=previous_h 
	  )){
	//sono in file lumi diversi e deltaT validi e ho davvero evento
	//non ho ancora aggiornato i conteggi nCounts per non prendere quelli del file lumi dopo...
	
	//calcolo rate
	double delta_t = (trigtime_s - previous_s) + (trigtime_m - previous_m) + (trigtime_h - previous_h)+ (trigtime_d - previous_d);
	double rate    = nCounts/delta_t;
	time_tot+=delta_t;

	h1_rate->Fill(rate);
	
	//aggiorno per la nuova lumi dopo aver riempito histo
	nCounts = 1; 
      }
    }//amp accettata
    } // for entries    

    delta_tot->Fill(time_tot);
    
    
    std::cout << "------------------------" << std::endl;
    std::cout << "   V = " << volts[i] << " V" << std::endl;
    std::cout << "   Mean rate: " << h1_rate->GetMean()
	      << " +/- " << h1_rate->GetMeanError()
	      << " Hz" << std::endl;
    
    std::cout << "total time: " << time_tot << std::endl;
    
    TFile* file_rate = TFile::Open( Form("data/root/CD%d/%s/%dV/rate_%dV.root", CD_number,meas,volts[i],volts[i]), "recreate" );
    file_rate->cd();
    h1_rate->Write();
    delta_tot->Write();
    file_rate->Close();
    
    std::cout << " -> Saved rate in: " << file_rate->GetName() << std::endl;
  } // for volts
  
  return 0;
  
}
