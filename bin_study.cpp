#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>

#include "TFile.h"
#include "TF1.h"
#include "TTree.h"
#include "TString.h"
#include "TH2D.h"
#include "TMath.h"
#include "TAxis.h"
#include "TGraphErrors.h"
#include "pshape_functions.h"
#include "graphics.h"

//programma che verifica quanto il binnaggio influisca sui risultati dei fit

Double_t gaussian(Double_t *x,Double_t *par);
Double_t cruijff_vinc(Double_t *x, Double_t *par);
void SetFitRange(int CD_number, int voltage, char* choice, double fit_range[2], int picco, int n_bins[]);


int main(int argc, char* argv[]){

  //parametri da input
  if (argc!=5){
    std::cout << "USAGE: ./bin_study [CD_number] [misura] [voltage] [amp/charge]" << std::endl;
    std::cout << "EXAMPLE: ./bin_study 204 preamp_post_cond1_18dic 97 amp" << std::endl;
    exit(1);
  }

  int   CD_number = (atoi(argv[1])); //cooldown
  int   voltage   = (atoi(argv[3]));
  char*  choice   =       argv[4]  ;
  char*  meas     =       argv[2]  ;
  
  //apertura file con il tree
  TFile run(Form("CD%d/%s/%dV/CD%d_%dV_tree.root",CD_number,meas,voltage,CD_number,voltage));
 
  TTree *tree = (TTree*)run.Get("tree");
  Long64_t nentries = tree->GetEntries();
 
  //preparazione per la lettura del tree
  float variable; //amp or charge
  
  tree->SetBranchAddress(choice, &variable);  

  int   magnifying = 1;
  float offset     = 0;
  if      (strcmp(choice,"charge")==0){ magnifying=1e+6; offset = 5; }
  else if (strcmp(choice,"amp"   )==0){ magnifying=1   ; offset = 0.05; }

  float var_min = tree->GetMinimum(choice)*magnifying;
  float var_max = tree->GetMaximum(choice)*magnifying;
  float range_min = var_min - offset;
  float range_max = var_max + offset;
  int tests = 50; //quanti binnaggi testo
  int acceptance = 0.05;

  float x        [tests];
  float xerror   [tests];
  float mean     [tests];
  float err_mean [tests];
  float sigma    [tests];
  float err_sigma[tests];
  float prob     [tests];
  float err_prob [tests];
  
  int n_bins[tests]; //array di binnaggi variabili
  int step;
  double fit_range[2];
  SetFitRange(CD_number,voltage,choice,fit_range,picco,n_bins);

  if(CD_number==188 && strcmp(choice, "amp"  )==0             ){ step=5; }
  if(CD_number==190 && strcmp(choice,"charge")==0 && picco==1 && voltage<146 ){ step=5;  }
  if(CD_number==190 && strcmp(choice,"charge")==0 && picco==1 && voltage>145 ){ step=20; }
  if(CD_number==190 && strcmp(choice,"charge")==0 && picco==2 && voltage<145 ){ step=2;  }
  if(CD_number==190 && strcmp(choice,"charge")==0 && picco==2 && voltage>144 ){ step=5; }
  
  for(int i=0; i<tests; i++){
    n_bins[i]=n_bins[0]+i*step;
  }

  //funzioni di fit
  TF1 *fit_cruijff = new TF1("fit_cruijff", cruijff_vinc , fit_range[0], fit_range[1], 5);
  TF1 *fit_gauss   = new TF1("fit_gauss"  , gaussian     , fit_range[0], fit_range[1], 3);

  int count=0; //quanti fit convergono
  for(int j=0; j<tests; j++){
    TH1F* histo = new TH1F("histo", "", n_bins[j], range_min, range_max);
    
    for(int iEntry=0; iEntry<nentries; iEntry++){
      tree->GetEntry(iEntry);
      histo->Fill(variable*magnifying);
    }

    if(CD_number==188){
      //parametri iniziali e limiti
      float   mean_min = 0.79; float   mean_max = 0.9 ; float mean_   = 0.85;
      float      k_min = 0.57; float      k_max = 1.13; float k_      = 0.84;
      float sigmaR_min = 0   ; float sigmaR_max = 1   ; float sigmaR_ = 0.1 ;
      float   alfa_min = 0   ; float   alfa_max = 10  ; float alfa_   = 0   ;
      float      A_min = 0   ; float      A_max = 3000; float A_      = 180 ;
      
      fit_cruijff->SetParNames("#mu","k", "#sigma_{R}", "#alpha", "A");
      fit_cruijff->SetParLimits(0,   mean_min,vvvmean_max);
      fit_cruijff->SetParLimits(1,      k_min,      k_max); 
      fit_cruijff->SetParLimits(2, sigmaR_min, sigmaR_max);
      fit_cruijff->SetParLimits(3,   alfa_min,   alfa_max);
      fit_cruijff->SetParLimits(4,      A_min,      A_max);
      fit_cruijff->SetParameters(mean_, k_, sigmaR_, alfa_, A_);
      
      histo->Fit("fit_cruijff","rq"); //in range

     //salvo solo fit con convergenza
       if(fit_cruijff->GetProb()>acceptance     &&
	 fit_cruijff->GetParameter(0)!=mean_min &&
          fit_cruijff->GetParameter(0)!=mean_max) {
           TCanvas* d = new TCanvas("d","Plot",2000,2000); d->cd();
           histo->SetLineWidth(3);
           histo->SetMarkerSize(3);
           histo->SetMarkerStyle(8);
           histo->SetMarkerColor(kBlack);
           histo->GetXaxis()->SetRangeUser(0.6, 1);
           histo->GetXaxis()->SetTitle("Amplitude");
           histo->GetYaxis()->SetTitle("Counts");
           histo->Draw("pe");
           
           fit_cruijff->SetLineColor(kRed);
           fit_cruijff->SetLineWidth(3);
           fit_cruijff->Draw("same");
           d->SaveAs(Form("CD %d/%d V/bin_study/bin_comp_bin%d.png",CD_number,voltage,n_bins[j]));
           delete(d);
           
           prob     [count]=fit_cruijff->GetProb();
           err_prob [count]=0;
           x        [count]=n_bins[j];
           xerror   [count]=0;
           mean     [count]=fit_cruijff->GetParameter(0);
           err_mean [count]=fit_cruijff->GetParError(0);
           sigma    [count]=fit_cruijff->GetParameter(2);
           err_sigma[count]=fit_cruijff->GetParError(2);
           count++;
       }
    }

    if(CD_number==190 && picco==1){
      float     A_min = 1  ; float     A_max = 3000; float A_     = 1 ;
      float  mean_min = 7  ; float  mean_max = 20  ; float mean_  = 11;
      float sigma_min = 0.1; float sigma_max = 10  ; float sigma_ = 1 ;
      //parametri iniziali e limiti

      fit_gauss->SetParNames("A","#mu","#sigma");
      fit_gauss->SetParLimits(0,     A_min,     A_max);
      fit_gauss->SetParLimits(1,  mean_min,  mean_max);
      fit_gauss->SetParLimits(2, sigma_min, sigma_max);
      fit_gauss->SetParameters(A_, mean_, sigma_);
      
      histo->Fit("fit_gauss","rq"); //in range

        
        //salvo solo fit con convergenza
      if(fit_gauss->GetProb()>acceptance      &&
	 fit_gauss->GetParameter(1)!=mean_min &&
	 fit_gauss->GetParameter(1)!=mean_max) {
	 TCanvas* d = new TCanvas("d","Plot",2000,2000); d->cd();
	 histo->SetLineWidth(3);
	 histo->SetMarkerSize(3);
	 histo->SetMarkerStyle(8);
	 histo->SetMarkerColor(kBlack);
	 histo->GetXaxis()->SetRangeUser(10, 15);
	 histo->GetXaxis()->SetTitle("Charge");
	 histo->GetYaxis()->SetTitle("Counts");
	 histo->Draw("pe");
	 
	 fit_gauss->SetLineColor(kRed);
	 fit_gauss->SetLineWidth(3);
	 fit_gauss->Draw("same");
	 d->SaveAs(Form("CD %d/%d V/bin_study/bin_comp_bin%d.png",CD_number,voltage,n_bins[j]));
	 delete(d);
       
      
	prob     [count]=fit_gauss->GetProb();
	err_prob [count]=0;
	x        [count]=n_bins[j];
	xerror   [count]=0;
	mean     [count]=fit_gauss->GetParameter(1);
	err_mean [count]=fit_gauss->GetParError(1);
	sigma    [count]=fit_gauss->GetParameter(2);
	err_sigma[count]=fit_gauss->GetParError(2);
	count++;
      }
    }

    if(CD_number==190 && picco==2){
      //parametri iniziali e limiti
      float     A_min = 1  ; float     A_max = 3000; float A_     = 22;
      float  mean_min = 10 ; float  mean_max = 25  ; float mean_  = 11;
      float sigma_min = 0.1; float sigma_max = 10  ; float sigma_ = 1 ;
      //parametri iniziali e limiti

      fit_gauss->SetParNames("A","#mu","#sigma");
      fit_gauss->SetParLimits(0,     A_min,     A_max);
      fit_gauss->SetParLimits(1,  mean_min,  mean_max);
      fit_gauss->SetParLimits(2, sigma_min, sigma_max);
      fit_gauss->SetParameters(A_, mean_, sigma_);
      
      histo->Fit("fit_gauss","rq"); //in range

        //salvo solo fit con convergenza
      if(fit_gauss->GetProb()>acceptance      &&
	 fit_gauss->GetParameter(1)!=mean_min &&
	 fit_gauss->GetParameter(1)!=mean_max) {
	 TCanvas* d = new TCanvas("d","Plot",2000,2000); d->cd();
	 histo->SetLineWidth(3);
	 histo->SetMarkerSize(3);
	 histo->SetMarkerStyle(8);
	 histo->SetMarkerColor(kBlack);
	 histo->GetXaxis()->SetRangeUser(14, 32);
	 histo->GetXaxis()->SetTitle("Charge");
	 histo->GetYaxis()->SetTitle("Counts");
	 histo->Draw("pe");
	 
	 fit_gauss->SetLineColor(kRed);
	 fit_gauss->SetLineWidth(3);
	 fit_gauss->Draw("same");
	 d->SaveAs(Form("CD %d/%d V/bin_study/bin_comp_bin%d.png",CD_number,voltage,n_bins[j]));
	 delete(d);
       
	prob     [count]=fit_gauss->GetProb();
	err_prob [count]=0;
	x        [count]=n_bins[j];
	xerror   [count]=0;
	mean     [count]=fit_gauss->GetParameter(1);
	err_mean [count]=fit_gauss->GetParError(1);
	sigma    [count]=fit_gauss->GetParameter(2);
	err_sigma[count]=fit_gauss->GetParError(2);
	count++;
      }
    }

    delete(histo);
  }

  //resolution settings
  float resol    [count];
  float err_resol[count];

  for(int l=0; l<count; l++){
    float d1 = pow(err_sigma[l],2)/pow(mean[l],2);
    float d2 = pow(err_mean[l],2)*pow(sigma[l],2)/pow(mean[l],4);
    resol[l]     = voltage*(sigma[l]/mean[l]);
    err_resol[l] = voltage*sqrt(d1+d2);
  }

  float y     [count];
  float yerror[count];
  char* ytitle[4]={(char*)"mean",(char*)"sigma",(char*)"resolution"};
  
  TCanvas* c = new TCanvas("c","Plot",2000,2000); c->cd();
  
  for(int k=0; k<3; k++){
    
    if(k==0){ for(int p=0; p<count; p++){ y[p] = mean[p] ; yerror[p] = err_mean[p] ; }}
    if(k==1){ for(int p=0; p<count; p++){ y[p] = sigma[p]; yerror[p] = err_sigma[p]; }}
    if(k==2){ for(int p=0; p<count; p++){ y[p] = resol[p]; yerror[p] = err_resol[p]; }}
    
    TGraphErrors* g = new TGraphErrors(count, x, y , xerror, yerror);
    g->SetMarkerStyle(8);
    g->SetMarkerSize(4);
    g->SetLineWidth(3);
    g->SetMarkerColor(kAzure-2);
    g->GetYaxis()->SetTitle(ytitle[k]);
    g->SetTitle(Form("%s_%dV_picco%d",ytitle[k],voltage,picco));

    if(k==2){g->SetMinimum(0);}

    g->Draw("AP");
    c->SaveAs(Form("CD %d/%d V/bin_comparison_%s%d.png",CD_number,voltage,ytitle[k],picco));
    delete(g);
  }
  
  delete(fit_cruijff);
  delete(fit_gauss);
  delete(c);
  
  return(0);
}


//funzioni usate

Double_t cruijff_vinc(Double_t *x, Double_t *par){
  //par[0]=mean      par[1]=costante 0<k<1    par[2]=sigma_R
  //par[3]=alpha_L   par[4]=A
  Double_t fit   = 0;
  Double_t arg_L = 0;
  Double_t arg_R = 0;
  Double_t num   = pow((x[0]-par[0]),2);
  Double_t denL  = 2*par[2]*par[2]*par[1]*par[1];
  Double_t denR  = 2*par[2]*par[2];
  Double_t asym  = par[3]*num;
  if ( denL + asym != 0 ){ arg_L = -num/(denL+asym); }
  if ( par[2]      != 0 ){ arg_R = -num/ denR      ; }

  if       ( x[0] < par[0] ){ fit = par[4] * exp(arg_L); } //LEFT
  else if  ( x[0] > par[0] ){ fit = par[4] * exp(arg_R); } //RIGHT

  return(fit);
}
 

Double_t gaussian(Double_t *x,Double_t *par) {
  Double_t arg = 0;
  if (par[2]!=0) arg = (x[0] - par[1])/par[2];
  Double_t fitval = par[0]*exp(-0.5*arg*arg);
  return fitval;
}


void SetFitRange(int CD_number, int voltage, char* choice, double fit_range[2], int picco, int n_bins[]){
  if(CD_number==188 && strcmp(choice,"amp")==0){  
    if     (voltage == 95 ){ fit_range[0]=0.795; fit_range[1]=0.86; }
    else if(voltage == 96 ){ fit_range[0]=0.79 ; fit_range[1]=0.85; }
    else if(voltage == 97 ){ fit_range[0]=0.785; fit_range[1]=0.85; }
    else if(voltage == 98 ){ fit_range[0]=0.79 ; fit_range[1]=0.86; }
    else if(voltage == 99 ){ fit_range[0]=0.79 ; fit_range[1]=0.86; }
    else if(voltage == 100){ fit_range[0]=0.79 ; fit_range[1]=0.84; }
    else if(voltage == 101){ fit_range[0]=0.79 ; fit_range[1]=0.86; }
    else if(voltage == 102){ fit_range[0]=0.80 ; fit_range[1]=0.88; }
    else if(voltage == 103){ fit_range[0]=0.80 ; fit_range[1]=0.88; }
    else if(voltage == 105){ fit_range[0]=0.85 ; fit_range[1]=0.91; }
    n_bins[0]=130;
  }
  
  if(CD_number==190 && strcmp(choice,"charge")==0){  
    if     (voltage == 130){ if     (picco==1) { fit_range[0]=10.8; fit_range[1]=12 ;  }
      else { std::cout << "invalide choice" << std::endl; exit(1); }}
    else if(voltage == 135){ if     (picco==1) { fit_range[0]=11.2; fit_range[1]=12.6; }
      else { std::cout << "invalide choice" << std::endl; exit(1); }}
    else if(voltage == 140){ if     (picco==1) { fit_range[0]=11.4; fit_range[1]=12.6; }
                             else if(picco==2) { fit_range[0]=16.5; fit_range[1]=23  ; }}
    else if(voltage == 141){ if     (picco==1) { fit_range[0]=9.1 ; fit_range[1]=10  ; }
                             else if(picco==2) { fit_range[0]=16.5; fit_range[1]=23  ; }}
    else if(voltage == 141){ if     (picco==1) { fit_range[0]=9.1 ; fit_range[1]=10  ; }
                             else if(picco==2) { fit_range[0]=14  ; fit_range[1]=19  ; }}
    else if(voltage == 142){ if     (picco==1) { fit_range[0]=11.8; fit_range[1]=13  ; }
                             else if(picco==2) { fit_range[0]=16.5; fit_range[1]=23  ; }}
    else if(voltage == 143){ if     (picco==1) { fit_range[0]=12  ; fit_range[1]=13.2; }
                             else if(picco==2) { fit_range[0]=16  ; fit_range[1]=27  ; }}
    else if(voltage == 144){ if     (picco==1) { fit_range[0]=12  ; fit_range[1]=13.2; }
                             else if(picco==2) { fit_range[0]=17  ; fit_range[1]=27  ; }}
    else if(voltage == 145){ if     (picco==1) { fit_range[0]=12.2; fit_range[1]=13.6; }
                             else if(picco==2) { fit_range[0]=19  ; fit_range[1]=25  ; }}
    else if(voltage == 146){ if     (picco==1) { fit_range[0]=14  ; fit_range[1]=15.5; }
                             else if(picco==2) { fit_range[0]=18  ; fit_range[1]=29  ; }}
    else if(voltage == 147){ if     (picco==1) { fit_range[0]=12.2; fit_range[1]=13.5; }
                             else if(picco==2) { fit_range[0]=19  ; fit_range[1]=25  ; }}
    else if(voltage == 148){ if     (picco==1) { fit_range[0]=12.4; fit_range[1]=13.4; }
                             else if(picco==2) { fit_range[0]=16  ; fit_range[1]=25  ; }}
    else if(voltage == 149){ if     (picco==1) { fit_range[0]=12.8; fit_range[1]=14  ; }
                             else if(picco==2) { fit_range[0]=18.5; fit_range[1]=28  ; }}
    else if(voltage == 150){ if     (picco==1) { fit_range[0]=12.7; fit_range[1]=14.2; }
                             else if(picco==2) { fit_range[0]=18.5; fit_range[1]=30  ; }}
    else { std::cout << "voltaggio non presente!" << std::endl; exit(1); }
  if(voltage < 146 && picco==1){ n_bins[0]=700; }
  if(voltage > 145 && picco==1){ n_bins[0]=950; }
  if(voltage < 145 && picco==2){ n_bins[0]=30;  }
  if(voltage > 144 && picco==2){ n_bins[0]=100; }
  }
  
}




 
