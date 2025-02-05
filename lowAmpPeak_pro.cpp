#include <iostream>
#include <fstream>
#include "TFile.h"
#include "TTree.h"
#include "TH2D.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLine.h"
#include "TPaveText.h"
#include "TLatex.h"
#include "TArrow.h"
#include "TF1.h"
#include "TLegend.h"

#include "graphics.h"
#include "AndCommon.h"
#include "CD_details.h"

float GetFitMin(int v);
float GetFitMax(int v);
Double_t fgauss(Double_t *x, Double_t *par);
//Double_t fexponential1(Double_t *x, Double_t *par);
//Double_t fexponential2(Double_t *x, Double_t *par);
Double_t fexponential(Double_t *x, Double_t *par);
Double_t fpol2(Double_t *x, Double_t *par);
Double_t sumf(Double_t *x, Double_t *par);
Double_t sumfpol(Double_t *x, Double_t *par);

int main(int argc, char* argv[]) {

  //controlli iniziali su variabili da input
  if (argc!=3){
    std::cout << "USAGE: ./drawFinalFits [CD_number] [misura]" << std::endl;
    std::cout << "EXAMPLE: ./drawFinalFits 204 B60_preamp_post_cond1" << std::endl;
    exit(1);
  }

  //parametri da input
  int   CD_number = (atoi(argv[1])); //cooldown
  char* misura    =       argv[2]  ;

  std::vector<int> volts = GetVoltages(CD_number,misura);

  TGraphErrors* gr_mu = new TGraphErrors(0);
 
  Settings settings;
  TCanvas* c = new TCanvas( "c", "", 1500, 1500 );
  settings.Margin(c);
  c->SetLeftMargin(0.155);
  c->cd();

  int nbins = 800;
  TH1D* h_var = new TH1D("h_var","",nbins,0,1);
  float binsize = settings.Binsize(1./nbins);
  std::string x_name("Amplitude [V]");
  std::string y_name("Counts / " + to_str(binsize) + " V");

  std::vector<TH1F*> histos;
   
  for(unsigned i=0; i<volts.size(); i++) {
    std::cout << std::endl << std::endl;
    std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
    std::cout << "V = " << volts[i] << std::endl << std::endl;
    
    //leggo tutti i tree per i voltaggi
    TFile* file = TFile::Open( Form("data/root/CD%d/%s/%dV/CD%d_%dV_tree.root", CD_number, misura, volts[i], CD_number, volts[i]), "read" );

    TTree* tree = (TTree*)file->Get("tree");
    
    //lettura del tree in amp o charge
    float amp;
    tree->SetBranchAddress("amp", &amp);
  
    float hist_min  = 0;
    float hist_max  = 1;
    float range_min = 0.05;
    float range_max = 0.15;

    
    //if(volts[i]==105 ||volts[i]==107 || volts[i]==110){nbins=400;}
    //if(volts[i]==104 || volts[i]==109){nbins=360;}
    if(volts[i]>108 ){nbins=600;}
    //creo un histo per ogni tree
    std::string histname(Form("%d", volts[i]));
    TH1F* histo = new TH1F( histname.c_str(), "", nbins, hist_min, hist_max);
    // TH1F* histo = new TH1F( histname.c_str(), "", 200, 0., 1.);
    Long64_t nentries = tree->GetEntries();
    
    for(int iEntry=0; iEntry<nentries; iEntry++){
      tree->GetEntry(iEntry);
      histo->Fill(amp);
    }
    
    histos.push_back(histo);

    //FITTING PART
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
    double par[7];
    float fit_min = GetFitMin(volts[i]);
    float fit_max = GetFitMax(volts[i]);
    double center = 0.5*(fit_max-fit_min);
    //gaussiana 
    TF1 *gaussian = new TF1(Form("gaussian_%d", volts[i]),fgauss,fit_min,fit_max,3);
    gaussian->SetParNames("#mu","#sigma", "A");
  
    //expo
    //TF1 *fondo1 = new TF1(Form("fondo1_%d", volts[i]),fexponential1,0.08,fit_min,2);
    //TF1 *fondo2 = new TF1(Form("fondo2_%d", volts[i]),fexponential2,fit_max,0.15,2);
    TF1 *fondo = new TF1(Form("fondo_%d", volts[i]),fpol2,0.08,0.15,2);
    
    //Sfrutto i parametri dati dai fit
    gaussian->GetParameters(&par[0]);
    fondo   ->GetParameters(&par[3]);
    //fondo2  ->GetParameters(&par[5]);

    TF1 *sumfu = new TF1("sumfu",sumfpol,0.08,0.15,7);
    sumfu->SetParameters(par);
    
    sumfu->SetLineColor(kRed-4);
    sumfu->SetLineWidth(4);
    sumfu->SetParLimits(0, 0.1, 0.125);
    sumfu->SetParLimits(1, 0.001, 0.01); 
    sumfu->SetParLimits(2, 10,    10000);
    //sumfu->SetParLimits(3, -30, -5);
    //sumfu->SetParLimits(4, 1, 20);
    sumfu->SetParLimits(5, 0, 2000);
    // sumfu->SetParLimits(5, -4, -1);
    //sumfu->SetParLimits(6, 1, 20);
    //sumfu->FixParameter(4, 5.5);
    //sumfu->FixParameter(3, -16);
    sumfu->SetNpx(1000);
    sumfu->SetParameters(0.1, 0.1, 100,-15,5.5,1,1); //parametri iniziali
    //PARTE GRAFICA
    settings.graphic_histo(histo,range_min,range_max,x_name.c_str(),y_name.c_str(),volts[i]);
    settings.general_style();
   
    histo->GetXaxis()->SetTitle("Amplitude (V)");
    histo->SetMarkerSize(2);
    histo->SetLineWidth(3);
  
    histo->Fit("sumfu","RmX");
    //legend->AddEntry(fitFcn,"best fit","l");
    histo->SetNdivisions(510);
    histo->Draw("same"); //"pe" per avere points+errors
           
    //drawing of fitted functions
    TF1 *gaus1 = new TF1("gaus1",fgauss,0,0.15,3);
    gaus1->SetParameters(sumfu->GetParameter(0),sumfu->GetParameter(1),sumfu->GetParameter(2));
    gaus1->SetLineColor(kBlue-7);
    gaus1->SetLineStyle(2);
    gaus1->SetNpx(1000);
    //legend->AddEntry(gaus1,"gaussian 1","l");

    /*
    TF1 *expo1 = new TF1("expo1",fexponential,0.08,0.15,2);
    expo1->SetParameters(sumfu->GetParameter(3),sumfu->GetParameter(4));
    expo1->SetLineColor(kViolet-9);
    expo1->SetLineStyle(2);
    */


    TF1 *expo1 = new TF1("expo1",fpol2,0.08,0.15,4);
    expo1->SetParameters(sumfu->GetParameter(3),sumfu->GetParameter(4), sumfu->GetParameter(5), sumfu->GetParameter(6));
    expo1->SetLineColor(kViolet-9);
    expo1->SetLineStyle(2);
    
    //legend->AddEntry(expo,"expo","l");
    /*
    TF1 *expo2 = new TF1("expo2",fexponential2,0.08,0.15,2);
    expo2->SetParameters(sumfu->GetParameter(5),sumfu->GetParameter(6));
    expo2->SetLineColor(kViolet-9);
    expo2->SetLineStyle(2);
    */
  
    //legend->Draw("same");
    gaus1->Draw("same");
    expo1->Draw("same");
    //expo2->Draw("same");
    
    c->SaveAs(Form("plots/CD%d/%s/%dlowamp_pol.png",CD_number,misura,volts[i]));
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
    
    //estrazione parametri dal fit
    float mu       = sumfu->GetParameter(0);
    float muerr    = sumfu->GetParError(0);
    muerr = sqrt(muerr*muerr+mu*0.005*mu*0.005);
    //grafico media vs voltaggio
    gr_mu->SetPoint( i, volts[i], mu );
    gr_mu->SetPointError( i, 0.,muerr);
    
  } //for sui voltaggi vari

  c->Clear();
  c->SetLeftMargin(0.185);
  
  c->cd();

  /* DISEGNO SIMULTANEO
  //graphic settings
  TH2D* h2_axes = new TH2D( "axes", "", 10, 0.08, 0.15, 10, 0., 1.1 );
  h2_axes->SetXTitle("Amplitude (V)");
  
  h2_axes->SetYTitle(y_name.c_str());
  h2_axes->GetXaxis()->SetNdivisions(505);
  gStyle->SetOptStat(0);

  h2_axes->GetXaxis()->SetLabelSize(0.04);
  h2_axes->GetXaxis()->SetTitleSize(0.04);
  h2_axes->GetYaxis()->SetLabelSize(0.04);
  h2_axes->GetYaxis()->SetTitleSize(0.04);
  //h2_axes->SetTitle(Form("working point - %s",choice));
  
  //h2_axes->SetYTitle( Form("Counts / (%.3f V)", (double)(histos[0]->GetXaxis()->GetBinWidth(13))) );

  h2_axes->Draw();

  std::vector<int> colours = {kRed+1, kRed-7, kOrange+8, kOrange+1, kOrange-2,kTeal+2, kGreen+2,kAzure+10,kAzure+7,kBlue-4,kViolet-1,kViolet-4,kPink+9,kPink+1,kPink+4};
  TLegend* legend = new TLegend(0.75, 0.90, 1.02, 0.42);
  legend->SetBorderSize(0);
  legend->SetFillStyle(0);
  
  for( unsigned i=0; i<histos.size(); ++i ) {
   
    histos[i]->SetLineWidth(4);
    histos[i]->SetLineColor(colours[i]);
    legend->AddEntry(histos[i],(Form("V = %d V",volts[i])),"lp");
    histos[i]->Draw("same pl");
    legend->Draw("same");
  }
  c->SaveAs(Form("plots/CD%d/%s/low_amp_peak.png",CD_number,misura));
  delete(c);

  */

  //grafico mu dei picchi
  gr_mu->GetXaxis()->SetLabelSize(0.04);
  gr_mu->GetXaxis()->SetTitleSize(0.04);
  gr_mu->GetYaxis()->SetLabelSize(0.04);
  gr_mu->GetYaxis()->SetTitleSize(0.04);
 
  gr_mu->SetTitle("#mu");
  gr_mu->GetXaxis()->SetTitle("Voltage (V)");
  gr_mu->GetYaxis()->SetTitle("low energy peak amplitude (V)");
  gr_mu->SetMarkerSize(3);
  gr_mu->SetLineWidth(4);
  gr_mu->SetMarkerStyle(20);
  gr_mu->SetMarkerColor(46);
  gr_mu->SetLineColor(46);
  gStyle->SetTitleFontSize(0.07);
  gr_mu->Draw("APE");
  c->SaveAs(Form("plots/CD%d/%s/low_amp_peak_new.png",CD_number,misura));
  c->Clear();

  gr_mu->GetYaxis()->SetRangeUser(0,(gr_mu->GetHistogram()->GetMaximum())+0.01);
  gr_mu->Draw("APE");
  c->SaveAs(Form("plots/CD%d/%s/low_amp_peak_fp_new.png",CD_number,misura));
  c->Clear();

  
  return(0);

}
//+++++++++++++++++++++++++++++++++++++++++++//
//++++++++++++++FUNZIONI UTILI+++++++++++++++//
//+++++++++++++++++++++++++++++++++++++++++++//


float GetFitMin(int v){
  float fit_min;

  if(v==96 ){fit_min = 0.09;}
  if(v==97 ){fit_min = 0.103;}
  if(v==98 ){fit_min = 0.106;}
  if(v==99 ){fit_min = 0.104;}
  if(v==100){fit_min = 0.106;}
  if(v==101){fit_min = 0.105;}
  if(v==102){fit_min = 0.104;}
  if(v==103){fit_min = 0.105;}
  if(v==104){fit_min = 0.105;}
  if(v==105){fit_min = 0.106;}
  if(v==106){fit_min = 0.107;}
  if(v==107){fit_min = 0.104;}
  if(v==108){fit_min = 0.109;}
  if(v==109){fit_min = 0.105;}
  if(v==110){fit_min = 0.110;}

  return fit_min;
}
  
float GetFitMax(int v){
  float fit_max;

  if(v==96 ){fit_max = 0.12; }
  if(v==97 ){fit_max = 0.111;}
  if(v==98 ){fit_max = 0.1125;}
  if(v==99 ){fit_max = 0.115;}
  if(v==100){fit_max = 0.116;}
  if(v==101){fit_max = 0.1145;}
  if(v==102){fit_max = 0.116;}
  if(v==103){fit_max = 0.117;}
  if(v==104){fit_max = 0.121;}
  if(v==105){fit_max = 0.123;}
  if(v==106){fit_max = 0.121;}
  if(v==107){fit_max = 0.12;}
  if(v==108){fit_max = 0.120;}
  if(v==109){fit_max = 0.122;}
  if(v==110){fit_max = 0.122;}

  return fit_max;
}

Double_t fgauss(Double_t *x, Double_t *par){
  Double_t arg    = 0;
  Double_t num    = pow((x[0]-par[0]),2);
  Double_t den    = 2*par[1]*par[1];
  if ( den != 0 ){ arg = -num/(den); }
  return par[2]*exp(arg);
}

Double_t fexponential(Double_t *x, Double_t *par) {
  return exp((*x)*par[0]+par[1]);
}

Double_t fpol2(Double_t *x, Double_t *par) {
  return exp(par[0]+par[1]*(*x)+par[2]*(*x)*(*x)+par[3]*(*x)*(*x)*(*x));
}

/*
Double_t fexponential1(Double_t *x, Double_t *par) {
  double fitval;
  if((*x)<0.105){fitval=exp((*x)*par[0]+par[1]);}
  if((*x)>0.105){fitval=0;}
  return fitval;
}

Double_t fexponential2(Double_t *x, Double_t *par) {
  double fitval;
  if((*x)>0.105){fitval=exp((*x)*par[0]+par[1]);}
  if((*x)<0.105){fitval=0;}
  return fitval;
}
*/

Double_t sumf(Double_t *x, Double_t *par) {
  return fgauss(x,par)+fexponential(x,&par[3]);
}

Double_t sumfpol(Double_t *x, Double_t *par) {
  return fgauss(x,par)+fpol2(x,&par[3]);
}
