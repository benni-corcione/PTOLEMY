#include <iostream>
#include <stdlib.h>

#include "TFile.h"
#include "TTree.h"
#include "TGraphErrors.h"
#include "TH1D.h"
#include "TSpectrum.h"
#include "TVirtualFitter.h"



int main() {


  for( int v = 103; v< 104; v++ ) {

    TFile* file = TFile::Open( Form("data/root/CD188/conteggi/%dV/CD188_%dV_treeraw.root", v,v), "read" );
    TTree* tree = (TTree*)file->Get("treeraw");

    int event;
    tree->SetBranchAddress( "event", &event );
    int lumi;
    tree->SetBranchAddress( "lumi", &lumi );
    double trigtime_s;
    tree->SetBranchAddress( "trigtime_s", &trigtime_s );
    //double trigtime_h;
    //tree->SetBranchAddress( "trigtime_h", &trigtime_h );
    //double trigtime_d;
    //tree->SetBranchAddress( "trigtime_d", &trigtime_d );
    float pshape[2502];
    tree->SetBranchAddress( "pshape", pshape );


    int nEntries = tree->GetEntries();

    float previous_s = -1.;
    int previous_lumi = -1;
    int nCount = 0;
    int nCount_PU = 0;

    TH1D* h1_pshape = new TH1D( "h1_pshape", "", 2502, 0., 2502. );

    TH1D* h1_nPeaks50  = new TH1D( "nPeaks50" , "", 200, 0., 200 );
    TH1D* h1_nPeaks100 = new TH1D( "nPeaks100", "",  50, 0., 50  );

    TH1D* h1_rate    = new TH1D( "rate"   , "", 100, 0., 500. );
    TH1D* h1_rate_PU = new TH1D( "rate_PU", "", 100, 0., 500. );

    for( int iEntry = 0; iEntry < nEntries; ++iEntry ) {

      tree->GetEntry(iEntry);
      
      //al primo ciclo non lavoro, al secondo avrò previous_lumi e previous_s
      //settati sull'onda prima e lumi e trig_s sull'onda successiva
      if( iEntry==0 ) {
        previous_lumi = lumi;
        previous_s = trigtime_s;
      }

      for( unsigned i=0; i<2501; ++i )
        h1_pshape->SetBinContent( i, -pshape[i] ); // make it positive for peak search
      
      nCount += 1; //conto le onde guardate in un file lumi
      
      //le onde larghe non triggerano sia nPeaks50 che nPeaks100?
      int nPeaks50  = h1_pshape->ShowPeaks( 50, "", 0.1 ); 
      h1_nPeaks50->Fill( nPeaks50 );
      int nPeaks100 = h1_pshape->ShowPeaks( 100, "", 0.9 ); 
      h1_nPeaks100->Fill( nPeaks100 );

      //std::cout << "nPeaks50: " << nPeaks50 << std::endl;
      //std::cout << "nPeaks100: " << nPeaks100 << std::endl;

      if(nPeaks100>0){ std::cout << "event: " << event << "  lumi: " << lumi <<  "  nPeaks100: " << nPeaks100 << std::endl; }
      nCount_PU += nPeaks100; //aggiungo pileup (doppie)

      /*
      if( lumi == 116 && event == 0 ) {
      //if( nPeaks100 > 5 ) {
        TFile* file = TFile::Open( "test.root", "recreate" );
        file->cd();
        h1_pshape->Write();
        file->Close();
        exit(1);
      }
      */

      //se non sono nello stesso file di dati :(
      if( lumi != previous_lumi ) {

        float delta_t = trigtime_s - previous_s;
        if( delta_t < 0. ) delta_t += 60.;

        h1_rate->Fill( (float)nCount/delta_t ); //onde nel file lumi/delta_t tra file (di onde non doppie)
        h1_rate_PU->Fill( (float)nCount_PU/delta_t );
	std::cout << delta_t << std::endl;
        nCount = 0;
        nCount_PU = 0;
        previous_lumi = lumi;
        previous_s = trigtime_s;

      }

    } // for entries

    //non mi perdo l'ultimo file nella rate così?
    
    std::cout << "------------------------" << std::endl;
    std::cout << "   V = " << v << " V" << std::endl;
    std::cout << "   Mean rate (no pile-up): " << h1_rate->GetMean() << " +/- " << h1_rate->GetMeanError() << " Hz" << std::endl;
    std::cout << "   Mean rate (pile-up)   : " << h1_rate_PU->GetMean() << " +/- " << h1_rate_PU->GetMeanError() << " Hz (x" << h1_rate_PU->GetMean()/h1_rate->GetMean() << " more)" << std::endl;

    TFile* file_rate = TFile::Open( Form("rate_%dV.root", v), "recreate" );
    file_rate->cd();
    h1_nPeaks50->Write();
    h1_nPeaks100->Write();
    h1_rate->Write();
    h1_rate_PU->Write();
    file_rate->Close();
   
    std::cout << " -> Saved rate in: " << file_rate->GetName() << std::endl;

  } // for volts

  return 0;

}
