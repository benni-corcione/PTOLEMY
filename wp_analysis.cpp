#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdlib.h>

#include "TFile.h"
#include "TF1.h"
#include "TTree.h"
#include "TString.h"
#include "TH2D.h"
#include "TMath.h"
#include "TAxis.h"
#include "pshape_functions.h"
#include "graphics.h"
#include "AndCommon.h"
 
//variabili da modificare di volta in volta: range_min, range_max, fit_min, fit_max, nbins

void Input_Param_Ctrls(int argc, char* argv[]);

int main(int argc, char* argv[]){ 

  //controlli iniziali su variabili da input
  Input_Param_Ctrls(argc,argv);

  //parametri da input
  int   CD_number = (atoi(argv[1])); //cooldown
  int   voltage   = (atoi(argv[2]));
  char* choice    =       argv[3]  ;

  std::vector<int> ibias = {29, 30, 31, 32, 35, 38};
  
  //impostazioni grafiche
  float magnifying = 1; //per avere numeri in carica più comodi (sono in E-6 sennò)
  float offset     = 0; //per allungare il range di disegno permesso wrt var_min/var_max
  if      (strcmp(choice,"amp"   )==0){ magnifying = 1e+0; offset = 0.05;}
  else if (strcmp(choice,"charge")==0){ magnifying = 1e+6; offset = 5  ;}
  
  //variabili per l'istogramma
  int   nbins   = 200;
  float var_min = 0;
  float var_max = 0; 
  if (strcmp(choice,"charge")==0){ var_min = 0; var_max = 20; }
  if (strcmp(choice,"amp"   )==0){ var_min = -0.1; var_max = 1;  }
  
  Settings settings;
  //funzione che tiene solo una cifra significativa per passare a stringa
  float binsize = settings.Binsize((var_max-var_min)/nbins); 
  
  const char* x_name; //x axis label
  std::string y_name; //y axis label 
  if      (strcmp(choice,"amp"   )==0){
    x_name = "Amplitude [V]";
    y_name = "Counts / " + to_str(binsize) + " V" ;
  }
  else if (strcmp(choice,"charge")==0){
    x_name = "Charge [uC]"  ;
    y_name = "Counts / " + to_str(binsize)  + " uC";
  }

  
  //tela
  TCanvas* c = new TCanvas("c","",1400,1200);
  settings.Margin(c);
  c->SetLeftMargin(0.155);
  c->cd();
  
  //range del disegno
  float range_min = 0;
  float range_max = 0;
  
  if      (strcmp(choice,"amp"   )==0){ range_min = var_min-offset; range_max = var_max+offset    ; }
  else if (strcmp(choice,"charge")==0){ range_min = var_min-offset; range_max = var_max+0.5*offset; }
  

  std::vector<TH1F*> histos;
  
  //ciclo sui file  
  for(int i=0; i<ibias.size(); ++i){
    
    std::string folder = "wp_ibias" + std::to_string(ibias[i]) + "uA";

    std::string histname(Form("%d", ibias[i]));			     
    //apertura file con il tree
   
    TFile* run = TFile::Open(Form("data/root/CD%d/%s/%dV/CD%d_%dV_tree.root",CD_number,folder.c_str(),voltage,CD_number,voltage),"read");
    TTree *tree = (TTree*)run->Get("tree");
    Long64_t nentries = tree->GetEntries();

    
    //preparazione per la lettura del tree in amp o charge
    float variable;
    tree->SetBranchAddress(choice, &variable);
    
    //creazione istogramma e lettura del tree
    TH1F* histo = new TH1F(histname.c_str(), choice, nbins, var_min-offset, var_max+offset);
    // tree->Project(histname.c_str(), choice);
    
    for(int iEntry=0; iEntry<nentries; iEntry++){
      tree->GetEntry(iEntry);
      histo->Fill(variable*magnifying);
    }
    
    
    int ymax = histo->GetBinContent(histo->GetMaximumBin());
    
    for(int iEntry=0; iEntry<nentries; iEntry++){
      histo->SetBinContent(iEntry,(histo->GetBinContent(iEntry))/ymax);
    }
   
    histos.push_back(histo);
    
  }

  //graphic settings
  TH2D* h2_axes = new TH2D( "axes", "", 10, var_min, var_max, 10, 0., 1.1 );
  h2_axes->SetXTitle(x_name);
  
  h2_axes->SetYTitle(y_name.c_str());
  h2_axes->GetXaxis()->SetNdivisions(505);
  gStyle->SetOptStat(0);

  h2_axes->GetXaxis()->SetLabelSize(0.04);
  h2_axes->GetXaxis()->SetTitleSize(0.04);
  h2_axes->GetYaxis()->SetLabelSize(0.04);
  h2_axes->GetYaxis()->SetTitleSize(0.04);
  h2_axes->SetTitle(Form("working point - %s",choice));
  
  //h2_axes->SetYTitle( Form("Counts / (%.3f V)", (double)(histos[0]->GetXaxis()->GetBinWidth(13))) );

  h2_axes->Draw();

 
  TLegend* legend;

  if (strcmp(choice,"amp"   )==0){legend = settings.setLegend(2);}
  if (strcmp(choice,"charge")==0){legend = settings.setLegend(1);}
  
  std::vector<int> colours = {899,808,800,834,868,879};
 
  for( unsigned i=0; i<histos.size(); ++i ) {
   
    histos[i]->SetLineWidth(2);
    histos[i]->SetMarkerSize(2);
    histos[i]->SetLineWidth(3);
    histos[i]->SetFillStyle(3004);
    histos[i]->SetFillColor(colours[i]+1);
    histos[i]->SetLineColor(colours[i]+1);
    legend->AddEntry(histos[i],(Form("ibias = %d uA",ibias[i])),"l");
    histos[i]->Draw("same");
    legend->Draw("same");
  }

  /*
    cruijff->SetLineColor(kRed-4);
    cruijff->SetLineWidth(4);
    cruijff->SetLineWidth(5);
    //cruijff->Draw("same");
    
  
  std::string outdir( Form("plots/CD%d/%s/%dV/", CD_number, meas, voltage));
  system( Form("mkdir -p %s", outdir.c_str()) );
  */
  c->SaveAs(Form("plots/CD%d/%s_fit_%dbins_32uA.png",CD_number,choice,nbins));
  //std::cout << "cruijff P: " << cruijff->GetProb() << std::endl;
  
  //delete(cruijff);
  //delete(histo);
  delete(c);
  
  return(0);
}


//funzione di controllo sui parametri iniziali
void Input_Param_Ctrls(int argc, char* argv[]){
  //ctrl sull'input inserito
  if (argc!=4){
    std::cout << "USAGE: ./fitAmpChargeData [CD_number] [voltage] [amp/charge]" << std::endl;
    std::cout << "[amp/charge]: variable studied" << std::endl;
     std::cout << "EXAMPLE: ./fitAmpChargeData 201 134 amp" << std::endl;
    exit(1);
  }

  //ctrl su scelta amp/charge
  if(strcmp(argv[3],"amp"   )!=0 &&
     strcmp(argv[3],"charge")!=0 ){ 
    std::cout << "Wrong choice! Choose between 'amp' and 'charge'" << std::endl;
    exit(1);
  }
}

