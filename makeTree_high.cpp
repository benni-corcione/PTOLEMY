#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <sstream>
#include <filesystem>
#include <algorithm>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TMath.h"
#include "pshape_functions.h"

int main(int argc, char* argv[]){

  //parametri da input
  if (argc!=4){
    std::cout << "USAGE: ./makeTree [CD_number] [measure] [voltage]" << std::endl;
    std::cout << "EXAMPLE: ./makeTree 188 conteggi 101" << std::endl;
    exit(1);
  }

  int   CD_number = (atoi(argv[1])); //cooldown number
  int   voltage   = (atoi(argv[3]));
  char* meas      =       argv[2]  ;
  
  //apertura file con il tree
  TFile run(Form("data/root/CD%d/%s/%dV/CD%d_%dV_tree.root",CD_number,meas,voltage, CD_number, voltage), "update");
  TTree *tree = (TTree*)run.Get(Form("tree"));
  Long64_t nentries = tree->GetEntries();
  
  //preparazione per la lettura del tree_raw
  int   event;
  int   lumi;
  float trigger;
  float baseline;
  float baseline_error;
  float charge;
  float amp;
  float sampling;
  int ctrl_double;
  int ctrl_trigger;
  int ctrl_width;

  tree->SetBranchAddress("CD_number"        , &CD_number     );
  tree->SetBranchAddress("voltage"          , &voltage       ); 
  tree->SetBranchAddress("event"            , &event         );
  tree->SetBranchAddress("lumi"             , &lumi          );
  tree->SetBranchAddress("trigger"          , &trigger       );
  tree->SetBranchAddress("baseline"         , &baseline      );
  tree->SetBranchAddress("baseline_error"   , &baseline_error);
  tree->SetBranchAddress("charge"           , &charge        );
  tree->SetBranchAddress("amp"              , &amp           );
  tree->SetBranchAddress("sampling"         , &sampling      );
  tree->SetBranchAddress("ctrl_double"      , &ctrl_double   );
  tree->SetBranchAddress("ctrl_width"       , &ctrl_width    );
  tree->SetBranchAddress("ctrl_trigger"     , &ctrl_trigger  );
  
  //creazione variabili del nuovo tree
  int cosmic_rays = 0;
  int electronics = 0;
 
  //nuovo tree
  std::string outfile_name = Form("CD%d_%dV_tree.root",CD_number,voltage);
  TFile* outfile = TFile::Open(Form("data/root/CD%d/%s/%dV/%s",CD_number,meas,voltage,outfile_name.c_str()),"recreate");
  TTree* tree_new = new TTree("tree","tree");

  tree_new->Branch("CD_number"        , &CD_number        ,         "CD_number/I");
  tree_new->Branch("voltage"          , &voltage          ,           "voltage/I"); 
  tree_new->Branch("event"            , &event            ,             "event/I");
  tree_new->Branch("lumi"             , &lumi             ,              "lumi/I");
  tree_new->Branch("trigger"          , &trigger          ,           "trigger/F");
  tree_new->Branch("baseline"         , &baseline         ,          "baseline/F");
  tree_new->Branch("baseline_error"   , &baseline_error   ,    "baseline_error/F");
  tree_new->Branch("charge"           , &charge           ,            "charge/F");
  tree_new->Branch("amp"              , &amp              ,               "amp/F");
  tree_new->Branch("sampling"         , &sampling         ,          "sampling/F");
  tree_new->Branch("ctrl_double"      , &ctrl_double      ,       "ctrl_double/I");
  tree_new->Branch("ctrl_width"       , &ctrl_width       ,        "ctrl_width/I");
  tree_new->Branch("ctrl_trigger"     , &ctrl_trigger     ,      "ctrl_trigger/I");
  tree_new->Branch("cosmic_rays"      , &cosmic_rays      ,       "cosmic_rays/I");
  tree_new->Branch("electronics"      , &electronics      ,       "electronics/I");
 
  //riempimento del tree
  for(unsigned iEntry=0; iEntry<nentries; iEntry++){

    tree->GetEntry(iEntry);

    cosmic_rays = 0;
    electronics = 0;
    
    float width_min = Get_width_min(CD_number,voltage,meas);
    float width_max = Get_width_max(CD_number,voltage,meas);
    float charge_min = Get_charge_min(CD_number,voltage,meas);
    float base_err_max = Get_base_err_max(CD_number,voltage,meas);
    
   if( (ctrl_width<width_min && charge<charge_min) ||     //condizione 1
	((baseline_error>base_err_max) &&                  //condizione 2 
	 (ctrl_width<width_min && ctrl_width>width_max) && 
	 (ctrl_double>3))){
     electronics = 1; }


    if ( ctrl_double==1 && ctrl_width>width_max &&
	 baseline_error<base_err_max ){
      cosmic_rays = 1; }

    
    tree_new->Fill();
    
  } //for on tree entries

  outfile->cd();
 tree_new->Write();

 std::cout << "tree saved in " << outfile_name << std::endl;
 outfile->Close();
   
    
  return(0);
}
	       	       

