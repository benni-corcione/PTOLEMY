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
#include "TCanvas.h"
#include "TH1D.h"
#include "TStyle.h"

float Get_width_min(int CD_number,int voltage,char* meas);
float Get_width_max(int CD_number,int voltage,char* meas);
float Get_charge_min(int CD_number,int voltage,char* meas);
float Get_base_err_max(int CD_number,int voltage,char* meas);

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
  TFile run(Form("data/root/CD%d/%s/%dV/CD%d_%dV_tree.root",CD_number,meas,voltage, CD_number, voltage),"update");
  TTree *tree = (TTree*)run.Get(Form("tree"));
  Long64_t nentries = tree->GetEntries();

  //apertura file con il tree_raw
  TFile file(Form("data/root/CD%d/%s/%dV/CD%d_%dV_treeraw.root",CD_number,meas,voltage,CD_number,voltage));
  TTree* treeraw = (TTree*)file.Get("treeraw");  
  
  //preparazione per la lettura del tree e la creazione di nuovi Branch
  float baseline_error;
  int ctrl_double;
  int ctrl_width;
  float charge;
  int event;
  int lumi;

  float pshape[2502];
  
  tree->SetBranchAddress("baseline_error", &baseline_error);
  tree->SetBranchAddress("ctrl_double"    , &ctrl_double   );
  tree->SetBranchAddress("ctrl_width"     , &ctrl_width    );
  tree->SetBranchAddress("charge"         , &charge        );
  tree->SetBranchAddress("event"          , &event         );
  tree->SetBranchAddress("lumi"           , &lumi          );

  treeraw->SetBranchAddress("pshape", &pshape);
  int counter_el = 0;
  int counter_cr = 0;

   TCanvas* c1 = new TCanvas("c1", "c1", 1800, 1000);
  c1->cd();

  //TBranch *b = tree->GetBranch("cosmic_ray");

  /*
  if(b!=NULL){
    tree->GetListOfBranches()->Remove(b);
    std::cout << "found" << std::endl;
    tree->Write("", TObject::kOverwrite);
    run.Close();
    run.cd();
    tree = (TTree*)run.Get("tree");
  }
*/
  
  //riempimento del tree
  for(unsigned iEntry=0; iEntry<nentries; iEntry++){
   
    tree->GetEntry(iEntry);
    treeraw->GetEntry(iEntry);
    
    TH1D* h1 = new TH1D("h1", "", 2500, 0., 2500.);

    //parametri di riferimento
    float width_min = Get_width_min(CD_number,voltage,meas);
    float width_max = Get_width_max(CD_number,voltage,meas);
    float charge_min = Get_charge_min(CD_number,voltage,meas);
    float base_err_max = Get_base_err_max(CD_number,voltage,meas);

    //elettronica
    if( (ctrl_width<width_min && charge<charge_min) ||     //condizione 1
	((baseline_error>base_err_max) &&                  //condizione 2 
	 (ctrl_width<width_min && ctrl_width>width_max) && 
	 (ctrl_double>3))){
      counter_el++;

       for(int i=0; i<2500; ++i ){ h1->SetBinContent(i+1, pshape[i] ); }

       h1->GetYaxis()->SetRangeUser(-0.5,0.5);
       // h1->GetYaxis()->SetRangeUser(-0.9,0.1);
       h1->SetLineColor(kAzure-4);
       h1->SetLineWidth(3);
       gStyle->SetOptStat(0);
       
       h1->Draw("l");

       /*
       std::cout << "b: " << baseline_error << std::endl;
       std::cout << "c: " << charge*1E+6 << std::endl;
       std::cout << "w: " << ctrl_width << std::endl;
       std::cout << "d: " << ctrl_double << std::endl;
       */
       std::string outdir_a( Form("plots/CD%d/%s/%dV/charac", CD_number, meas, voltage));
       system( Form("mkdir -p %s", outdir_a.c_str()) );
       c1->SaveAs(Form("%s/el_event%d_lumi%d.png",outdir_a.c_str(),event,lumi));
       std::cout << std::endl;
       
    } //for pshape
  
    //raggi cosmici
    if ( ctrl_double==1 && ctrl_width>width_max &&
	 baseline_error<base_err_max ){
     
      counter_cr++;

       for(int i=0; i<2500; ++i ){ h1->SetBinContent(i+1, pshape[i] ); }
       
       h1->GetYaxis()->SetRangeUser(-0.5,0.5);
       //h1->GetYaxis()->SetRangeUser(-0.9,0.1);
       h1->SetLineColor(kPink-4);
       h1->SetLineWidth(3);
       gStyle->SetOptStat(0);
       
       h1->Draw("l");
       
      
       std::string outdir_a( Form("plots/CD%d/%s/%dV/charac", CD_number, meas, voltage));
       system( Form("mkdir -p %s", outdir_a.c_str()) );
       c1->SaveAs(Form("%s/cr_event%d_lumi%d.png",outdir_a.c_str(),event,lumi));
       std::cout << std::endl;
    }
    

    if ( ctrl_double==0 ){
      //std::cout << "COSMIC RAY - event " << event << "  lumi " << lumi << std::endl;
      
      for(int i=0; i<2500; ++i ){
	h1->SetBinContent(i+1, pshape[i] );
      }
      
      h1->GetYaxis()->SetRangeUser(-0.5,0.5);
      //h1->GetYaxis()->SetRangeUser(-0.9,0.1);
      h1->SetLineColor(kBlack);
      h1->SetLineWidth(3);
      gStyle->SetOptStat(0);
      
      h1->Draw("l");
      
      
      std::string outdir_a( Form("plots/CD%d/%s/%dV/charac", CD_number, meas, voltage));
      system( Form("mkdir -p %s", outdir_a.c_str()) );
      c1->SaveAs(Form("%s/0_event%d_lumi%d.png",outdir_a.c_str(),event,lumi));
      std::cout << std::endl;
    }
    
    delete(h1);
   
  } //for on tree entries
  
  std::cout << "Cosmic rays: " << counter_cr << std::endl;
  std::cout << "Electronics: " << counter_el << std::endl;
  
  //std::cout << "tree entries: " << tree->GetEntries() << std::endl;
  //std::cout << "tree saved in " << run << std::endl;
  run.Close();
  
  
  return(0);
}
	       	       
  

void search_and_remove(const std::filesystem::path& directory, const std::filesystem::path& file_name){
     std::stringstream stream;  
     auto d = std::filesystem::directory_iterator(directory);

    auto found = std::find_if(d, end(d), [&file_name](const auto& dir_entry)
    {
        return dir_entry.path().filename() == file_name;
    });

    if (found != end(d))
    {
        //elimino tutti i tree singoli inutili
      stream << "cd " << directory.c_str() << std::endl;
      stream << "rm -f " << file_name.c_str() << std::endl;
      system(stream.str().c_str());
      
    }
}  


float Get_width_min(int CD_number,int voltage, char* meas){
  float width_min;

  if(CD_number==188){
    if(voltage>94  && voltage<102){ width_min = 80; }
    if(voltage>101 && voltage<105){ width_min = 100;}
    if(voltage==105)              { width_min = 120;}
  }

  if(CD_number==204 && strcmp(meas,"B60_post_cond3_moku")==0){ width_min = 60; }

  if(CD_number==204 && strcmp(meas,"B60_preamp_post_cond1")==0){ width_min = 40; }
  
  return width_min;
}

float Get_width_max(int CD_number,int voltage, char* meas){
  float width_max;

 if(CD_number==188){
    if(voltage>94   && voltage<98 ){ width_max = 200;}
    if(voltage==98 || voltage==100){ width_max = 210;}
    if(voltage==99 || voltage==101){ width_max = 220;}
    if(voltage==102|| voltage==103){ width_max = 240;}
    if(voltage==104)               { width_max = 250;}
    if(voltage==105)               { width_max = 320;}
 }

 if(CD_number==204 && strcmp(meas,"B60_post_cond3_moku")==0){
    if(voltage==96)               { width_max = 300;}
    if(voltage==97)               { width_max = 250;}
    if(voltage>97  && voltage<101){ width_max = 260;}
    if(voltage==104)              { width_max = 260;}
    if(voltage>100 && voltage<104){ width_max = 270;}
    if(voltage==105)              { width_max = 270;}
    if(voltage>105 && voltage<110){ width_max = 280;}
    if(voltage==110)              { width_max = 320;}
  }

 
 if(CD_number==204 && strcmp(meas,"B60_preamp_post_cond1")==0){
    if(voltage==97)                 { width_max = 340;}
    if(voltage==99  || voltage==101){ width_max = 280;}
    if(voltage==103 || voltage==106){ width_max = 290;}
 }
 
  return width_max;
}

float Get_charge_min(int CD_number,int voltage, char* meas){
  float charge_min;

  if(CD_number==188){ charge_min = 1.0*1E-6; }
  if(CD_number==204 && strcmp(meas,"B60_post_cond3_moku")==0){ charge_min = 0.2*1E-6; }
  if(CD_number==204 && strcmp(meas,"B60_preamp_post_cond1")==0){ charge_min = 0; }
  return charge_min;
}

float Get_base_err_max(int CD_number,int voltage, char* meas){
  float base_err_max;

  if(CD_number==188){
    if(voltage==95 || voltage==96 || voltage==102){
     base_err_max = 0.011; }
    if(voltage==99 || voltage>102){
      base_err_max = 0.012; }
    else{ base_err_max = 0.0125; }
  }

  if(CD_number==204 && strcmp(meas,"B60_post_cond3_moku")==0){ base_err_max = 0.007; }
  if(CD_number==204 && strcmp(meas,"B60_preamp_post_cond1")==0){ base_err_max = 0.007; }
  
  return base_err_max;
}

