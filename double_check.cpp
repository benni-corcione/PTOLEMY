#include <iostream>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <filesystem>

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLine.h"
#include "TStyle.h"
#include "pshape_functions.h"

//prendo deltaY 
//ciclare sul rawtree e disegnare eventi triggeranti

//servono sia evento sia lumi!
int main(int argc, char* argv[]){

  //parametri da input
  if (argc!=4){
    std::cout << "USAGE: ./data_fit [CD_number] [misura] [voltage]" << std::endl;
    exit(1);
  }

  int CD_number = atoi(argv[1]);
  int voltage   = atoi(argv[3]);
  char* meas    =      argv[2] ; 
  
  std::string fileName = "CD"+ std::to_string(CD_number)
                       + "_" + std::to_string(voltage) + "V_treeraw.root";

   std::stringstream stream;
   stream << "rm -r plots/CD" << std::to_string(CD_number)
	  << "/" << std::string(meas)
	  << "/" << std::to_string(voltage)
	  << "V/check" << std::endl;
  system(stream.str().c_str());

  //preparazione lettura del tree
  TFile* file = TFile::Open(Form("data/root/CD%d/%s/%dV/%s",CD_number, meas, voltage, fileName.c_str())); 
  TTree* tree = (TTree*)file->Get("treeraw");
  int nentries = tree->GetEntries();
  

  //settings tree 
  float pshape[2502];
  int   ev;
  int   lum;
  float sampling;

  tree->SetBranchAddress( "event" ,         &ev       );
  tree->SetBranchAddress( "lumi"  ,         &lum      );
  tree->SetBranchAddress( "pshape",         &pshape   );
  tree->SetBranchAddress( "sampling_time",  &sampling );

  //differenza y di un punto con quello dopo n_sum punti
  TCanvas* c1 = new TCanvas("c1", "c1", 1800, 850); //1800 850
  c1->cd();

  int conteggio = 0;
  //questi sembrano i parametri ottimali
  int n_sum = 45;
  
  //FOR SU netries
  for(unsigned iEntry=0; iEntry<nentries; iEntry++){
   
    //array in cui sommo deltay di punti a coppia, tipo (y1-y2)+(y1-y3)+(y1-y4)+...+(y1-y_nsum)
    float delta[2502] = {0};
    float dyn_delta[2502] = {0};
    
    
    tree->GetEntry(iEntry);
    
    //////////////////////////////////
    //if pe studiare la singola pshape
    //if (ev==74 && lum==169){
      
    TH1D* wave = new TH1D("wave", "", 2499,0.,2499*sampling*1.E+6);
    TH1D* delta_histo = new TH1D("delta_histo", "", 2499,0.,2499*sampling*1.E+6);
    TH1D* dyn_histo = new TH1D("dyn_histo", "", 2499,0.,2499*sampling*1.E+6);

    //ciclo sulla pshape
    for(int i=0; i<2499; ++i ){
      wave->SetBinContent(i+1,pshape[i]);
     
      //if(pshape[i]>(base+(4*base_err)) && pshape[i+70]<){check_up++;}
      //riempimento array delta
      //attenzione alle condizioni al contorno
      if(i < 2499-n_sum) {
	for(int l=0; l<n_sum; l++){ delta[i] += (pshape[i]-pshape[i+n_sum-l]); }
      }
      else{
	//fattore di scala per aggiustare il contorno
	float scale = n_sum/(2500-i);
	for(int l=0; l<(2500-i); l++){ delta[i] += (pshape[i]-pshape[i+l+1])*scale; }	
      }
      delta_histo->SetBinContent (i+1,delta[i]);
    }//ho finito di leggere la forma d'onda

    
    //medi dinamica sulla delta per renderla più liscia
    int check = 0;
    int dyn_sum = 8;
    std::vector<int> index;
    float smooth_shape[2502];
    DynamicMean(delta,dyn_delta,dyn_sum);
    DynamicMean(pshape,smooth_shape,10);
    
    //per disegnare la media dinamica della delta
    for(int i=0; i<2499; ++i ){
      dyn_histo->SetBinContent(i+1,dyn_delta[i]);
    }

    //soglia: se la media dinamica la supera per almeno un tot allora era una doppia
    float threshold = 1.0;
    
    //leggo vettore dei delta

    for(int i=0; i<2499; ++i ){
      if(i < 2489){
	if(dyn_delta[i]  <threshold &&
	   dyn_delta[i+1]>threshold &&
	   dyn_delta[i+2]>threshold &&
	   dyn_delta[i+3]>threshold &&
	   dyn_delta[i+4]>threshold &&
	   dyn_delta[i+5]>threshold &&
	   dyn_delta[i+6]>threshold &&
	   dyn_delta[i+7]>threshold &&
	   dyn_delta[i+8]>threshold &&
	   dyn_delta[i+9]>threshold &&
	   dyn_delta[i+10]>threshold ) 
	  { check++; index.push_back(i);}
	else{;}
      }
      else if(i > 2489){
	if(dyn_delta[i-10]<threshold &&
	    dyn_delta[i-9]>threshold &&
	    dyn_delta[i-8]>threshold &&
	    dyn_delta[i-7]>threshold &&
	    dyn_delta[i-6]>threshold &&
	    dyn_delta[i-5]>threshold &&
	    dyn_delta[i-4]>threshold &&
	    dyn_delta[i-3]>threshold &&
	    dyn_delta[i-2]>threshold &&
	    dyn_delta[i-1]>threshold &&
	    dyn_delta[i]>threshold  )
	  { check++; index.push_back(i);}
	else{;}
      }
    }//ho finito di leggere il vettore delta

     float base     = GetBaseline(pshape);
     float base_err = GetBaselineError(pshape, base);
      
    int check_up = Check_Up(pshape, dyn_delta, threshold, CD_number, meas, base, base_err);
    if(check_up>0){check=1;}
   
   
    //ho incontrato una doppia almeno
    if(check > 1 ){
     
      
      conteggio++;
      gStyle->SetOptStat(0);
      wave->SetLineColor(kBlack);
      wave->SetLineWidth(5);
      wave->GetXaxis()->SetTitle("Time (#mus)");
      wave->GetYaxis()->SetTitle("Amplitude (V)");
      wave->GetXaxis()->SetTitleSize(.07); //0.06 su singola, //0.08 su mini
      wave->GetYaxis()->SetTitleSize(.07);
      wave->GetXaxis()->SetLabelSize(0.08); //0.05 su singola, //0.07 su mini
      wave->GetYaxis()->SetLabelSize(0.08);
      wave->GetYaxis()->SetTitleOffset(+0.75);
      wave->GetXaxis()->SetTitleOffset(+0.9);
      
      wave->GetYaxis()->SetRangeUser(-1.,1.2);
    
      c1->SetBottomMargin(0.2);
      c1->SetLeftMargin(3);
      c1->SetRightMargin(0.04);
      gStyle->SetFrameLineWidth(1);
      wave->Draw("l");
      
      dyn_histo->SetLineWidth(3);
      delta_histo->SetLineWidth(3);
      delta_histo->SetLineColor(kTeal-8);
      dyn_histo->SetLineColor(kCyan+3);
     
      delta_histo->Draw("l same");
      dyn_histo->Draw("l same");

      TLine *line = new TLine(0,threshold,100,threshold);
      line->SetLineWidth(5);
      line->SetLineColor(kPink-5);
      line->SetLineStyle(9);
      line->Draw("same");
       wave->Draw("l same");
  
       std::string outdir( Form("plots/CD%d/%s/%dV/check/double_sum%d_thresh%.1f", CD_number, meas, voltage,n_sum,threshold));
      system( Form("mkdir -p %s", outdir.c_str()) );
      //-p crea anche le parent se prima non esistono
      
      c1->SaveAs(Form("%s/event%dlumi%d.png",outdir.c_str(),ev,lum));
      std::cout << std::endl;
      c1->Clear();
          delete(line);
   
    }
    
    delete(wave);
    delete(delta_histo);
    delete(dyn_histo);
   
    //per scriverlo nel tree assegno check_double (1,2,3 ecc) alla forma d'onda

    ////////////////////
    //termine dell'if per singola pshape
    //}
    
  } //prossima entry del tree

  //std::cout << "numero di eventi: " << conteggio << std::endl;
  file->Close();
  delete(c1);
  //ho finito di leggere tutte le pshape
  
  return 0;
}



