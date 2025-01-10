#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "TFile.h"
#include "TTree.h"
#include "TGraphErrors.h"
#include "TH1D.h"

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

  for(int i=0; i<volts.size(); i++){

    TFile* fileraw = TFile::Open(Form("data/root/CD%d/%s/%dV/CD%d_%dV_treeraw.root", CD_number,meas,volts[i],CD_number,volts[i]), "read" );
    TFile* file = TFile::Open(Form("data/root/CD%d/%s/%dV/CD%d_%dV_tree.root", CD_number,meas,volts[i],CD_number,volts[i]), "read" );
    TTree* treeraw = (TTree*)fileraw->Get("treeraw");
    TTree* tree = (TTree*)file->Get("tree");

    int event;
    int lumi;
    int ctrl_double;
    float pshape[2502];
    double trigtime_m;
    double trigtime_s;
    tree->SetBranchAddress("ctrl_double", &ctrl_double);
    treeraw->SetBranchAddress("lumi"       , &lumi       );
    treeraw->SetBranchAddress("event"      , &event      );
    treeraw->SetBranchAddress("pshape",       pshape     );
    treeraw->SetBranchAddress("trigtime_m" , &trigtime_m );
    treeraw->SetBranchAddress("trigtime_s" , &trigtime_s );

    
    int nEntries = tree->GetEntries();

    //onde in questo file non computate
    //ho un intervallo di tempo in meno di quanti filedati
    //scarto primo file perchè non ho ref di inizio riempimento
    treeraw->GetEntry(0);
    int lumi0 = lumi;

  
    float previous_s = -1.;
    float previous_m = -1.;
    int previous_lumi = -1;
    int nCounts = 0;
    int nCounts_add = 0;
    

    TH1D* h1_rate = new TH1D( "rate", "", 100, 0., 500.);

      
    for( int iEntry = 0; iEntry < nEntries; ++iEntry ) {
 //
       //al primo ciclo non lavoro, al secondo avrò previous_lumi e previous_s
      //settati sull'onda prima e lumi e trig_s sull'onda successiva
      if( iEntry==0 ) {
	treeraw->GetEntry(0);
	previous_lumi = lumi0;
      } 

      //riassegno quelle del ciclo precedente
      previous_lumi = lumi; 
      previous_s = trigtime_s;
      previous_m = trigtime_m;
      
      //prendo le nuove lumi e trigtime_s
      treeraw->GetEntry(iEntry);
      tree->GetEntry(iEntry);

      // PRINT DI CONTROLLO
      /*
      std::cout << "iEntry: " << iEntry << std::endl;
  
      std::cout << "previous lumi: " << previous_lumi
		<< "   previous time_m: " << previous_m 
                << "   previous time_s: " << previous_s << std::endl;
      std::cout << "lumi: " << lumi
		<< "   time_m: " << trigtime_m
		<< "   time_s: " << trigtime_s << std::endl;
		std::cout << "ctrl_double_pre: " << ctrl_double << std::endl;
      */

      nCounts_add=ctrl_double;
      
      //if(lumi!=lumi0){ //ignoro primo file dati
	
	if(previous_lumi == lumi){ //sono nello stesso file lumi
	  nCounts+=ctrl_double; //aggiungo il numero di onde dell'evento
	  
	}
	if(previous_lumi!=lumi){
	  
	  //if(previous_lumi!=lumi0){
	    double delta_t = (trigtime_s - previous_s) + (trigtime_m - previous_m);
	    int nCounts_rate = nCounts+nCounts_add;
	    double rate = nCounts/delta_t;
	    //std::cout <<"rate: " << nCounts_rate << "/" << delta_t << "= " << rate << std::endl;
	    h1_rate->Fill(rate);
	    //}
	  nCounts = ctrl_double; //aggiorno per la nuova lumi
	  
	}
	//}

      /* //PRINT DI CONTROLLO
      std::cout << "ctrl_double_post: " << ctrl_double << std::endl;
      std::cout << "events: " << nCounts << std::endl;
      std::cout << std::endl;
      */
      
    } // for entries    
    
    std::cout << "------------------------" << std::endl;
    std::cout << "   V = " << volts[i] << " V" << std::endl;
    std::cout << "   Mean rate: " << h1_rate->GetMean() << " +/- " << h1_rate->GetMeanError() << " Hz" << std::endl;
    
    TFile* file_rate = TFile::Open( Form("rate_%dV.root", volts[i]), "recreate" );
    file_rate->cd();
    h1_rate->Write();
    file_rate->Close();
    
    std::cout << " -> Saved rate in: " << file_rate->GetName() << std::endl;
    
    } // for volts

 
  return 0;
  
}
