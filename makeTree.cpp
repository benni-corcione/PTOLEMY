#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <sstream>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TMath.h"
#include "pshape_functions.h"

int main(int argc, char* argv[]){

  //parametri da input
  if (argc!=5){
    std::cout << "USAGE: ./makeTree [CD_number] [measure] [voltage] [trigger]" << std::endl;
    std::cout << "Give the absolute value of the trigger in Volts" << std::endl;
    std::cout << "EXAMPLE: ./makeTree 188 conteggi 101 0.166" << std::endl;
    exit(1);
  }

  int   CD_number = (atoi(argv[1])); //cooldown number
  int   voltage   = (atoi(argv[3]));
  float trigger   = (atof(argv[4]));
  char* meas      =       argv[2]  ;
  
  //unificazioni vari tree già esistenti
  std::stringstream stream;  
  stream << "cd data/root/CD" << std::to_string(CD_number)
	 << "/" << std::string(meas) 
	 << "/" << std::to_string(voltage) << "V" << std::endl;
  
  stream << "hadd -f CD" << std::to_string(CD_number)
	 << "_" << std::to_string(voltage)
	 << "V_treeraw.root " << "C"
	 << "*" << std::endl;

  //elimino tutti i tree singoli inutili
  //stream << "rm -f C1--*" << std::endl;
  //non cosa sia il file DS_Store.root che mi crea
  stream << "rm -f DS_*" << std::endl;
  system(stream.str().c_str());

  std::cout << "All the trees were unified in one single tree named CD" << CD_number << "_" << voltage << "V_treeraw.root" << std::endl;
 
  //apertura file con il tree
  TFile run(Form("data/root/CD%d/%s/%dV/CD%d_%dV_treeraw.root",CD_number,meas,voltage, CD_number, voltage));
  TTree *tree_raw = (TTree*)run.Get(Form("treeraw"));
  Long64_t nentries = tree_raw->GetEntries();
  std::cout << nentries << std::endl;

  //preparazione per la lettura del tree_raw
  int   event_raw;
  int   lumi_raw;
  float sampling_raw;
  float pshape[2502];

  tree_raw->SetBranchAddress("CD_number"     , &CD_number    );
  tree_raw->SetBranchAddress("voltage"       , &voltage      );
  tree_raw->SetBranchAddress("event"         , &event_raw    );
  tree_raw->SetBranchAddress("lumi"          , &lumi_raw     );
  tree_raw->SetBranchAddress("sampling_time" , &sampling_raw );
  tree_raw->SetBranchAddress("pshape"        ,  pshape       );

  
  //creazione variabili del nuovo tree
  int   event;
  int   lumi;
  float baseline;
  float baseline_error;
  float charge;
  float amp;
  float sampling;

  Controls ctrl; //classe definita in pshape_functions.h
  int ctrl_double;
  int ctrl_trigger;
  int ctrl_width;
  
  
  //nuovo tree
  std::string outfile_name = Form("CD%d_%dV_tree.root",CD_number,voltage);
  TFile* outfile = TFile::Open(Form("data/root/CD%d/%s/%dV/%s",CD_number,meas,voltage,outfile_name.c_str()),"recreate");
  TTree* tree = new TTree("tree","tree");

  tree->Branch("CD_number"        , &CD_number        ,         "CD_number/I");
  tree->Branch("voltage"          , &voltage          ,           "voltage/I"); 
  tree->Branch("event"            , &event            ,             "event/I");
  tree->Branch("lumi"             , &lumi             ,              "lumi/I");
  tree->Branch("trigger"          , &trigger          ,           "trigger/F");
  tree->Branch("baseline"         , &baseline         ,          "baseline/F");
  tree->Branch("baseline_error"   , &baseline_error   ,    "baseline_error/F");
  tree->Branch("charge"           , &charge           ,            "charge/F");
  tree->Branch("amp"              , &amp              ,               "amp/F");
  tree->Branch("sampling"         , &sampling         ,          "sampling/F");
  tree->Branch("ctrl_double"      , &ctrl_double      ,       "ctrl_double/I");
  tree->Branch("ctrl_width"       , &ctrl_width       ,        "ctrl_width/I");
  tree->Branch("ctrl_trigger"     , &ctrl_trigger     ,      "ctrl_trigger/I");
 

  //riempimento del tree
  for(unsigned iEntry=0; iEntry<nentries; iEntry++){
 
    tree_raw->GetEntry(iEntry);
   
    //functions defined in utility.h
    event           = event_raw;
    lumi            = lumi_raw;
    sampling        = sampling_raw;
    baseline        = GetBaseline(pshape);
    baseline_error  = GetBaselineError(pshape, baseline);
    charge          = GetCharge(pshape, sampling_raw, baseline);
    amp             = GetAmp(pshape, baseline);

    ctrl         = Ctrl_pshape(pshape, amp, trigger, baseline);
    ctrl_double  = ctrl.get_ctrl_double();
    ctrl_trigger = ctrl.get_ctrl_trigger();
    ctrl_width   = ctrl.get_ctrl_width();
    
    tree->Fill();
    
  } //for on tree entries

  
  outfile->cd();
  tree->Write();
  
  std::cout << "tree entries: " << tree->GetEntries() << std::endl;
  std::cout << "tree saved in " << outfile_name << std::endl;
  outfile->Close();
  
    
  return(0);
}
	       	       
  

  
