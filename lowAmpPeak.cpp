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
//aggiornare le funzioni Get in base allo studio sui fit singoli
Double_t Cruijff(Double_t *x, Double_t *par);

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

  int nbins = 500;
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

    if(volts[i]==105 ||volts[i]==107 || volts[i]==110){nbins=400;}
    if(volts[i]==104 || volts[i]==109){nbins=360;}
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
    float fit_min = GetFitMin(volts[i]);
    float fit_max = GetFitMax(volts[i]);

    TF1 *cruijff  = new TF1(Form("cru_%d", volts[i]), Cruijff, fit_min, fit_max, 5);
    
    //parametri del fit
    cruijff->SetParNames("#mu","#sigma_{L}", "#sigma_{R}", "#alpha", "A");
    cruijff->SetParLimits(0, 0.1, 0.12);
    cruijff->SetParLimits(1, 0.002, 0.05); 
    cruijff->SetParLimits(2, 0.002, 0.05); 
    cruijff->SetParLimits(3, 0   ,   1);
    cruijff->SetParLimits(4, 10,    10000);

    //gaussiana asimmetrica e non cruijff
    cruijff->FixParameter(3,0); //alfa value
    cruijff->SetNpx(1000);
    cruijff->SetParameters(0.1, 0.1, 0.1, 0, 10); //parametri iniziali

    //PARTE GRAFICA
    settings.graphic_histo(histo,range_min,range_max,x_name.c_str(),y_name.c_str(),volts[i]);
    settings.general_style();
    //gStyle->SetOptFit(1111);

    histo->GetXaxis()->SetTitle("Amplitude (V)");
    histo->SetMarkerSize(2);
    histo->SetLineWidth(3);
    histo->Fit(cruijff,"RmX");
    histo->SetNdivisions(510);
    histo->Draw(); //"pe" per avere points+errors
    
    cruijff->SetLineColor(kRed-4);
    cruijff->SetLineWidth(4);
    cruijff->Draw("same");
    
    std::string outdir( Form("plots/CD%d/%s/%dV", CD_number, misura, volts[i]));
    system( Form("mkdir -p %s", outdir.c_str()) );
    c->SaveAs(Form("plots/CD%d/%s/%dV/fit_lowamp.png",CD_number,misura,volts[i]));


    
    
    //estrazione parametri dal fit
    float mu       = cruijff->GetParameter(0);
    float muerr    = cruijff->GetParError(0);
    
    //grafico media vs voltaggio
    gr_mu->SetPoint( i, volts[i], mu );
    gr_mu->SetPointError( i, 0., muerr );
   
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
  c->SaveAs(Form("plots/CD%d/%s/low_amp_peak.png",CD_number,misura));
  c->Clear();

  gr_mu->GetYaxis()->SetRangeUser(0,(gr_mu->GetHistogram()->GetMaximum())+0.01);
  gr_mu->Draw("APE");
  c->SaveAs(Form("plots/CD%d/%s/low_amp_peak_fp.png",CD_number,misura));
  c->Clear();

  
  return(0);

}
//+++++++++++++++++++++++++++++++++++++++++++//
//++++++++++++++FUNZIONI UTILI+++++++++++++++//
//+++++++++++++++++++++++++++++++++++++++++++//

float GetFitMax(int v){
  float fit_max;

  if(v==96 ){fit_max = 0.115;} if(v==97 ){fit_max = 0.114;}
  if(v==98 ){fit_max = 0.115;} if(v==99){fit_max = 0.116;}
  if(v==100){fit_max = 0.119;} if(v==101){fit_max = 0.115;}
  if(v==102){fit_max = 0.119;} if(v==103){fit_max = 0.119;}
  if(v==104){fit_max = 0.122;} if(v==105){fit_max = 0.122;}
  if(v==106){fit_max = 0.122;} if(v==107){fit_max = 0.12;}
  if(v==108){fit_max = 0.121;} if(v==109){fit_max = 0.128;}
  if(v==110){fit_max = 0.124;}

  return fit_max;
}

float GetFitMin(int v){
  float fit_min;

  if(v==96 ){fit_min = 0.1;} if(v==97 ){fit_min = 0.1025;}
  if(v==98 ){fit_min = 0.104;} if(v==99 ){fit_min = 0.102;}
  if(v==100){fit_min = 0.105;} if(v==101){fit_min = 0.105;}
  if(v==102){fit_min = 0.105;} if(v==103){fit_min = 0.108;}
  if(v==104){fit_min = 0.105;} if(v==105){fit_min = 0.103;}
  if(v==106){fit_min = 0.107;} if(v==107){fit_min = 0.1085;}
  if(v==108){fit_min = 0.108;} if(v==109){fit_min = 0.104;}
  if(v==110){fit_min = 0.107;}

  return fit_min;
}
  


//cruiff (che può diventare una gaussiana) da fittare
Double_t Cruijff(Double_t *x, Double_t *par){
  //par[0]=mean      par[1]=sigma_L    par[2]=sigma_R
  //par[3]=alpha_L   par[4]=A
  Double_t fitval = 0;
  Double_t arg_L  = 0;
  Double_t arg_R  = 0;
  Double_t num    = pow((x[0]-par[0]),2);
  Double_t denL   = 2*par[1]*par[1];
  Double_t denR   = 2*par[2]*par[2];
  Double_t asym   = par[3]*num;
  if ( denL + asym != 0 ){ arg_L = -num/(denL+asym); }
  if ( par[2]      != 0 ){ arg_R = -num/ denR      ; }

  if       ( x[0] < par[0] ){ fitval = par[4] * exp(arg_L); } //LEFT
  else if  ( x[0] > par[0] ){ fitval = par[4] * exp(arg_R); } //RIGHT

  return(fitval);
}

