#include "TStyle.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TF1.h"
#include "TFile.h"
#include "TH1F.h"
#include "TPad.h"
#include "TMath.h"
#include <iostream>
#include <fstream>

//programma che fa gli histo più generali possibili delle variabili
int main(int argc, char* argv[]){
  
  //parametri da input
  if( argc!= 4 ) {
    std::cout << "USAGE: ./makehisto [CD_number] [misura] [power]" << std::endl;
    exit(1);
  }

  int CD_number = (atoi(argv[1]));
  char* meas    =       argv[2]  ; 
  char* ch_char =       argv[3] ;
  
  for(int i=0; i<4; i++){ //i<4 è per sicurezza ma il punto dovrebbe essere trovato prima
    if(ch_char[i]=='.'){ch_char[i]='p';} }

  
  //tree settings
  TFile run(Form("data/root/CD%d/%s/%smW/CD%d_%smWtree.root",CD_number,meas,ch_char,CD_number,ch_char));
  TTree *tree = (TTree*)run.Get("tree");
  Long64_t nentries = tree->GetEntries();
  Long64_t n_bins = 500;

  float charge;

  //estremi degli histo
  float charge_min = tree->GetMinimum("charge");
  float charge_max = tree->GetMaximum("charge");

  
  //file to write histo
  std::string filename = "data/CD" + std::to_string(CD_number)
    + "_" + std::string(ch_char) + "mW_histo.txt";
  std::ofstream ofs(filename);

  //prime righe del file
   ofs << "LECROYWR620Zi,00000,Histogram"      << std::endl;
   ofs << "Segments,1,SegmentSize,1000"        << std::endl;
   ofs << "Segment,TrigTime,TimeSinceSegment1" << std::endl;
   ofs << "#1,data,ora,0"                      << std::endl;                 
   ofs << "Amplitude,Counts"                   << std::endl;
   
  //lettura del tree e riempimento dell'istogramma
  TH1F *h_charge = new TH1F(Form("%smW_charge", ch_char),"charge", n_bins, charge_min*1E+6, charge_max*1E+6);
  tree->SetBranchAddress("charge", &charge);
  
  for(unsigned iEntry=0; iEntry<nentries; ++iEntry) {
    tree->GetEntry(iEntry);
    h_charge->Fill((charge)*1E+6); //in unità più comode uC
  }
  
  for(int bin=0; bin<n_bins; bin++){
    float bincenter = h_charge->GetXaxis()->GetBinCenter(bin);
    int counts = h_charge->GetBinContent(bin);
    ofs << bincenter << "," << counts << std::endl;
  }
  
  ofs.close();
  delete(h_charge);
  
  std::cout << "Histo saved in " << filename << std::endl;

  return 0;
}

