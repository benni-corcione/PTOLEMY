#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <filesystem>

#include "TFile.h"
#include "TTree.h"
#include "TH2D.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLine.h"
#include "TPaveText.h"
#include "TF1.h"
#include "TMath.h"

#include "AndCommon.h"
#include "pshape_functions.h"
#include "graphics.h"
#include "CD_details.h"

Double_t Decay(Double_t *x, Double_t *par);
Double_t Poli(Double_t *x, Double_t *par);
float FitError(float q, float q_err, float tau, float tau_err, float x);
float FinalError(float rate, float fit, float rate_err, float fit_err);
float FinalError_easy(float rate, float fit, float rate_err, float fit_err);
  
int main(int argc, char* argv[]) {

  if(argc!=3){
    std::cout << "USAGE: ./compareRateFE [meas] [file]" << std::endl;
    std::cout << "EXAMPLE: ./compareRateFE B60_post_cond3_moku MICb_100to220V.dat" << std::endl;
    exit(1);
  }

  //andata e ritorno della curva
  TGraphErrors* gr_an  = new TGraphErrors(0);
  TGraphErrors* gr_re = new TGraphErrors(0);
  TCanvas* c = new TCanvas("c","",1000,1000);
  TLegend* legend = new TLegend(0.75,0.82,0.9,0.9);
  legend->SetBorderSize(0);
  legend->SetFillStyle(0);
  c->cd();
  c->SetLeftMargin(0.13);
  
  int CD_number = 204;
  char* meas     = argv[1];
  char* filename = argv[2];

  //leggere file curva
  std::ifstream input_file;
  input_file.open(Form("data/dat/CD204/%s",filename));
  
  //verifica sull'apertura
  if (!input_file.is_open()) {
    std::cout << "Error opening the file!" << std::endl;
    exit(1);
  }
  
  std::string line;
  float voltage;
  float voltage_old = 0;
  float temperature;
  float current;
  float sd;
  int point_an = 0;
  int point_re = 0;
  int count    = 0;
  
  while(!input_file.eof()){
  
    getline(input_file,line);
      if(line == "\n" || line[0]=='#' || line==""){ continue; }
      else{
	
	count++;
	if(count>1){voltage_old=voltage;}
	sscanf(line.c_str(),"%f %f %f %f", &voltage, &temperature, &current, &sd);
	//curva di andata
	if(voltage_old<voltage){
	  point_an++;
	  gr_an->SetPoint     (point_an,voltage,current);
	  gr_an->SetPointError(point_an,0      ,sd     );
	}
	//curva di ritorno
	if(voltage_old>voltage || voltage_old==voltage){
	  point_re++;
	  gr_re->SetPoint     (point_re,voltage,current);
	  gr_re->SetPointError(point_re,0      ,sd     );
	}
      }//else su lettura file
  }//while su file completo
 
  
  input_file.close();

  //correzione al primo punto della curva
  gr_an->SetPointY(1,(gr_an->GetPointY(2)));
  gr_an->SetPointError(1,0,(gr_an->GetErrorY(2)));
  
  //grafico con errori andata e ritorno
  gr_an->SetMarkerStyle(8);
  gr_an->SetLineColor(kAzure+1);
  gr_an->SetMarkerColor(kAzure+1);
  legend->AddEntry(gr_an,"andata","pe");
  
  gr_re->SetMarkerStyle(8);
  gr_re->SetLineColor(kTeal-6);
  gr_re->SetMarkerColor(kTeal-6);
  legend->AddEntry(gr_re,"ritorno","pe");
  
  TH2D *h2_axes_g = new TH2D( "axes_g", "", 10, 99, 221, 10, -1, 1);
  h2_axes_g->SetXTitle( "Nanotube voltage #it{V}_{cnt} (V)" );
  h2_axes_g->SetYTitle( "Rate [Hz]" );
  //h2_axes_g->GetYaxis()->SetRangeUser(-310,10);
  h2_axes_g->GetYaxis()->SetRangeUser(-1,0.1);
  h2_axes_g->GetXaxis()->SetRangeUser(100,160);
  //h2_axes_g->GetYaxis()->SetTitleOffset(1.2);
  
  h2_axes_g->Draw("");
  gr_an->Draw("pe same");
  gr_re->Draw("pe same");
  legend->Draw("same");
  
  int Npoints = gr_an->GetN();
  float min_base = gr_an->GetPointX(1);
  float max_base = gr_an->GetPointX(33);

  //fittare la baseline e riportarla a offset zero
  TF1 *poli  = new TF1("polin", Poli, min_base, max_base, 1);

  gr_an->Fit(poli,"RmX0");
  float offset_an = poli->GetParameter(0);
  gr_re->Fit(poli,"RmX0");
  float offset_re = poli->GetParameter(0);

  //fit esponenziale
  float v_min_fit  = 100.0;
  float v_max_fit  = 145.0;
  float fit_min;   
  float fit_max;  

  for(int i=1; i<Npoints; i++){
    gr_an->SetPointY(i,(gr_an->GetPointY(i))-offset_an);
    gr_re->SetPointY(i,(gr_re->GetPointY(i))-offset_re);
    if(gr_an->GetPointX(i)==v_min_fit){
      fit_min = gr_an->GetPointX(i);
    }
    if(gr_an->GetPointX(i)==v_max_fit){
      fit_max = gr_an->GetPointX(i);
    }
  }

   
  gr_an->Draw("same");  
  gr_re->Draw("same");
  gStyle->SetOptStat(0);
  
  TF1 *decay_an  = new TF1("decay", Decay, fit_min, fit_max, 2);
  TF1 *decay_re  = new TF1("decay", Decay, fit_min, fit_max, 2);

  //parametri del fit
  decay_an->SetParNames("tau","m");
  decay_re->SetParNames("tau","m");
  decay_an->SetNpx(1000);
  decay_re->SetNpx(1000);

  decay_an->SetLineColor(kRed-4);
  decay_an->SetLineWidth(4);
  decay_re->SetLineColor(kMagenta);
  decay_re->SetLineWidth(4);
  gr_an->Fit(decay_an,"RmX");
  gr_re->Fit(decay_re,"RmX");
  decay_an->Draw("same");
  decay_re->Draw("same");
  
  c->SaveAs(Form("plots/CD%d/%s/fe.png",CD_number,meas));
  c->Clear();
  legend->Clear();
  
  //salvare la curva
  float q_an   = decay_an->GetParameter(0);
  float tau_an = decay_an->GetParameter(1);
  float q_re   = decay_re->GetParameter(0);
  float tau_re = decay_re->GetParameter(1);

  float q_an_err   = decay_an->GetParError(0);
  float tau_an_err = decay_an->GetParError(1);
  float q_re_err   = decay_re->GetParError(0);
  float tau_re_err = decay_re->GetParError(1);

  //acquisizione dati rate
  std::vector<int> volts = GetVoltages(CD_number,meas);
  float v          [volts.size()];
  float v_err      [volts.size()];
  float rate       [volts.size()];
  float rate_err   [volts.size()];
  float rate_PU    [volts.size()];
  float rate_PU_err[volts.size()];
  float diff_an    [volts.size()];
  float diff_re    [volts.size()];
  float diff_an_err[volts.size()];
  float diff_re_err[volts.size()];

  //conversione da rate segnali a pA di corrente
  float conv = 1E+12*(1.6E-19);
  for(int i = 0; i<volts.size(); ++i) {
    //acquisizione rate presalvate
    TFile* file_rate  = TFile::Open( Form("data/root/CD%d/%s/%dV/rate_%dV.root", CD_number,meas,volts[i],volts[i]), "read");
    
    TH1D* h1_rate     = (TH1D*)file_rate ->Get( "rate"    );
    TH1D* h1_rate_PU  = (TH1D*)file_rate ->Get( "rate_PU" );
    v          [i] = volts[i];
    v_err      [i] = 0;
    rate       [i] = (h1_rate    ->GetMean()     )*conv;
    rate_err   [i] = (h1_rate    ->GetMeanError())*conv;
    rate_PU    [i] = (h1_rate_PU ->GetMean()     )*conv;
    rate_PU_err[i] = (h1_rate_PU ->GetMeanError())*conv;
    
    file_rate->Close();
  }


  //incertezza su valore finale 
  //vedere differenza curva e punti
  for( int i = 0; i<volts.size(); ++i ) {
    float fit_an     = -exp(q_an+tau_an*volts[i]);
    float fit_an_err = FitError(q_an,q_an_err,tau_an,tau_an_err,volts[i]); 
    diff_an[i] = 100*(-rate[i])/fit_an;
    diff_an_err[i] = 100*FinalError(rate[i],fit_an,rate_err[i],fit_an_err);

    /*
    std::cout << "q   : " << q_an_err << std::endl;
    std::cout << "tau : " << tau_an_err << std::endl;
    std::cout << "rate: " << rate[i] << "+/-" << rate_err[i] << "(" << 100*rate_err[i]/rate[i] << ")" << std::endl;
    std::cout << "fit : " << fit_an << "+/-" << fit_an_err << "(" << 100*fit_an_err/fit_an << ")" << std::endl;
    std::cout << "diff: " << diff_an[i] << "+/-" << diff_an_err[i] << "(" << 100*diff_an_err[i]/diff_an[i] << ")" << std::endl;
    std::cout << std::endl;
    */
    float fit_re     = -exp(q_re+tau_re*volts[i]);
    float fit_re_err = FitError(q_re,q_re_err,tau_re,tau_re_err,volts[i]);
    diff_re[i] = 100*(-rate[i])/fit_re;
    diff_re_err[i] = 100*FinalError(rate[i],fit_re,rate_err[i],fit_re_err);
    
   }

  /*

  for(int i=0; i<volts.size();i++){
    std::cout << "rate: " << rate_err[i]/rate[i]  << std::endl;
  } 
  */			   
  
  //grafico differenze relative riscontrate
  TGraphErrors* gdiff_an = new TGraphErrors(volts.size(),v,diff_an,v_err,diff_an_err);
  TGraphErrors* gdiff_re = new TGraphErrors(volts.size(),v,diff_re,v_err,diff_re_err);
  TH2D *h2_axes = new TH2D( "axes", "", 10, 95, 111, 10, -30, 35);
  h2_axes->SetXTitle( "Nanotube voltage #it{V}_{cnt} (V)" );
  h2_axes->SetYTitle( "(Rate/fe)*100" );
  //h2_axes_g->GetYaxis()->SetRangeUser(-310,10);
  //h2_axes->GetYaxis()->SetRangeUser(-1,0.1);
  //h2_axes->GetXaxis()->SetRangeUser(100,160);
  gdiff_an->SetMarkerStyle(8);
  gdiff_an->SetLineColor(kAzure+1);
  gdiff_an->SetMarkerColor(kAzure+1);
  legend->AddEntry(gr_an,"andata","pe");
   
  h2_axes->Draw("");
  gdiff_an->Draw("pesame");
  gdiff_re->SetMarkerStyle(8);
  gdiff_re->SetLineColor(kTeal-6);
  gdiff_re->SetMarkerColor(kTeal-6);
  legend->AddEntry(gr_re,"ritorno","pe");
  gdiff_re->Draw("pesame");

  c->SaveAs(Form("plots/CD%d/%s/rate_su_fe.png",CD_number,meas));
  c->Clear();
	 
  

  
   return(0);
}


Double_t Decay(Double_t *x, Double_t *par){
  //par[0]=offset    par[1]=coefficient  par[2]=constant
  return(-exp(par[0]+par[1]*x[0]));
}

Double_t Poli(Double_t *x, Double_t *par){
  //par[0]=offset    par[1]=coefficient  par[2]=constant
  return(par[0]);
}

float FitError(float q, float q_err, float tau, float tau_err, float x){
  float fiterror;
  float piece1,piece2;

  piece1 = exp(q+tau*x)*exp(q+tau*x)*q_err*q_err;
  piece2 = exp(q+tau*x)*exp(q+tau*x)*x*x*tau_err*tau_err;
  fiterror = sqrt(piece1+piece2);
  return(fiterror);
}

float FinalError(float rate, float fit, float rate_err, float fit_err){
  float finalerror;
  float piece1,piece2;

  piece1 = (1./fit)*(1./fit)*rate_err*rate_err;
  piece2 = (fit_err*fit_err*rate*rate)/(fit*fit*fit*fit);
  finalerror = sqrt(piece1+piece2);
  return(finalerror);
}


float FinalError_easy(float rate, float fit, float rate_err, float fit_err){
  float finalerror;
  float piece1,piece2;

  piece1 = rate_err*rate_err/(rate*rate);
  piece2 = fit_err*fit_err/(fit*fit);
  finalerror = sqrt(piece1+piece2)*rate/fit;
  return(finalerror);
}


