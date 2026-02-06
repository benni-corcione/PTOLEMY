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

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//++PROGRAMMA SETTATO SOLO SULLO STUDIO DI: +++++++++++++++++++//
// cd_188, conteggi, amp//
// cd_204, preamp_post_cond1_18dic, amp//
// cd_204, preamp_post_cond1_18dic, charge//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

void Input_Param_Ctrls(int argc, char* argv[]);

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

int main(int argc, char* argv[]) {

   std::vector<int> colors = {800,834,868,879};
   std::vector<TH1F*> histos;
   
  //controlli iniziali su variabili da input
  Input_Param_Ctrls(argc,argv);

  //parametri da input
  int   CD_number = (atoi(argv[1])); //cooldown
  char* choice    =       argv[3]  ;
  char* misura    =       argv[2]  ;

  int voltage;
  if(CD_number==188) voltage=97;
  //if(CD_number==188) voltage=105;
  //if(CD_number==204) voltage=98;
  if(CD_number==204) voltage=103;
    
  Settings settings;
  TCanvas* c1 = new TCanvas( "c1", "", 1500, 1500 );
  settings.Margin(c1);
  c1->SetLeftMargin(0.155);
  c1->cd();

  float phi_carbon = 4.5;
  float phi_tes = GetPhiTes();
  
  int nbins[3];
  if(CD_number == 204){ nbins[0]=600; nbins[1]=700; nbins[2]=800;}
  if(CD_number == 188){ nbins[0]=92 ; nbins[1]=100; nbins[2]=105;}
  //if(CD_number == 188){ nbins[0]=202 ; nbins[1]=210; nbins[2]=250;}
   
  for(int i=0; i<3; i++){
  std::cout << std::endl << std::endl;
  std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
  std::cout << "V = " << voltage << std::endl << std::endl;
    
  //leggo tutti i tree per i voltaggi
  TFile* file = TFile::Open( Form("data/root/CD%d/%s/%dV/CD%d_%dV_tree.root", CD_number, misura, voltage, CD_number, voltage), "read" );

  TTree* tree = (TTree*)file->Get("tree");
    
  //lettura del tree in amp o charge
  float variable;
  tree->SetBranchAddress(choice, &variable);

  Histo h_var = SetHistoValues(CD_number,choice,misura,voltage);
    
  std::string x_name = h_var.get_x_name();
  std::string y_name = h_var.get_y_name();
  
  float hist_min  = h_var.get_hist_min();
  float hist_max  = h_var.get_hist_max();
  float range_min = h_var.get_range_min();
  float range_max = h_var.get_range_max();

   std::string histname(Form("%d", i));
   TH1F* h1_amp = new TH1F( histname.c_str(), "", nbins[i], 0., hist_max );
   Long64_t nentries = tree->GetEntries();
    
  for(int iEntry=0; iEntry<nentries; iEntry++){
    tree->GetEntry(iEntry);
    h1_amp->Fill(variable);
  }
    
  Fit f_var = SetFitValues(CD_number,choice,h1_amp,voltage,misura);
    
  //assegnazione parametri di fit
  float mu_min     = f_var.get_mu_min();
  float mu_max     = f_var.get_mu_max();
  float mu_        = f_var.get_mu();
  float sigmaL_min = f_var.get_sigmaL_min();
  float sigmaL_max = f_var.get_sigmaL_max();
  float sigmaL_    = f_var.get_sigmaL();
  float sigmaR_min = f_var.get_sigmaR_min();
  float sigmaR_max = f_var.get_sigmaR_max();
  float sigmaR_    = f_var.get_sigmaR();
  float alfa_min   = f_var.get_alfa_min();
  float alfa_max   = f_var.get_alfa_max();
  float alfa_      = f_var.get_alfa();
  float A_min      = f_var.get_A_min();
  float A_max      = f_var.get_A_max();
  float A_         = f_var.get_A();
  float fit_min    = f_var.get_fit_min();
  float fit_max    = f_var.get_fit_max();

  //std::cout << mu_min << " " << mu_max << " " << mu_ << std::endl;
  //std::cout << fit_min << " " << fit_max << std::endl;

   //fit function definition
   TF1 *cruijff  = new TF1(Form("cru_%d", i), Cruijff, fit_min, fit_max, 5);
   cruijff->SetParNames("#mu","sigma_{L}", "#sigma_{R}", "#alpha", "A");

   //parametri del fit
   cruijff->SetParNames("#mu","#sigma_{L}", "#sigma_{R}", "#alpha", "A");
   cruijff->SetParLimits(0,    mu_min,    mu_max);
   cruijff->SetParLimits(1,sigmaL_min,sigmaL_max); 
   cruijff->SetParLimits(2,sigmaR_min,sigmaR_max);
   cruijff->SetParLimits(3,  alfa_min,  alfa_max);
   cruijff->SetParLimits(4,     A_min,     A_max);

   
   //gaussiana asimmetrica e non cruijff
   cruijff->FixParameter(3,0); //alfa value
   cruijff->SetNpx(1000);
   cruijff->SetParameters(mu_, sigmaL_, sigmaR_, alfa_, A_); //parametri iniziali
   cruijff->SetLineColor(colors[i]);
   cruijff->SetLineWidth(5);
   cruijff->SetNpx(1000);
   
   h1_amp->SetXTitle("Amplitude (V)");
   h1_amp->SetYTitle(Form("Counts / (%.3f V)", (double)(h1_amp->GetXaxis()->GetBinWidth(13))) );
   //h1_amp->GetXaxis()->SetTitleSize(.07);
   //h1_amp->GetYaxis()->SetTitleSize(.07);
   //h1_amp->GetXaxis()->SetLabelSize(0.065);
   //h1_amp->GetYaxis()->SetLabelSize(0.065);
   //h1_amp->GetYaxis()->SetTitleOffset(+1.2);
   //h1_amp->GetXaxis()->SetTitleOffset(+1.0);
   //h1_amp->GetXaxis()->SetNdivisions(107);
   h1_amp->Fit( cruijff, "RX+" );
   
   histos.push_back(h1_amp);

    //estrazione parametri dal fit
    float mu       = cruijff->GetParameter(0);
    float muerr    = cruijff->GetParError(0);
    float sigmaR    = cruijff->GetParameter(2);
    float sigmaRerr = cruijff->GetParError(2);
    float sigmaL    = cruijff->GetParameter(1);
    float sigmaLerr = cruijff->GetParError(1);

   
    //l'errore sulla stabilità di ites-ibias è insito nei segnali che otteniamo

    float energy_ele = voltage - phi_carbon + (phi_carbon-phi_tes);
    float energy_err = sqrt(0.0009 + pow((0.04+0.0015*voltage),2));
    
    float reso = sigmaR/mu*(voltage-phi_tes);
     if(CD_number==188){
      muerr = sqrt( muerr*muerr + 0.01*0.01*mu*mu );
      reso = sigmaR/mu*(voltage);
    }
    float reso_err = sqrt( sigmaRerr*sigmaRerr*energy_ele*energy_ele/(mu*mu) + sigmaR*sigmaR*energy_ele*energy_ele*muerr*muerr/(mu*mu*mu*mu) + sigmaR*sigmaR*energy_err*energy_err/(mu*mu));
   
    //aggiunta fwhm 
    float fwhm = (sigmaR+sigmaL)*sqrt(2*log(2));
    float fwhm_err = sqrt(2*log(2))*sqrt(sigmaRerr*sigmaRerr+sigmaLerr*sigmaLerr);
    float piece1_f = fwhm_err*fwhm_err*energy_ele*energy_ele/(mu*mu);
    float piece2_f = fwhm*fwhm*muerr*muerr*energy_ele*energy_ele/(mu*mu*mu*mu);
    float piece3_f = fwhm*fwhm*energy_err*energy_err/(mu*mu);
    float reso_fwhm = fwhm/mu*(energy_ele);
    float reso_fwhm_err = sqrt(piece1_f+piece2_f+piece3_f);

    std::cout << "nbins = " << nbins[i] << std::endl;
    std::cout << "E = " << voltage-phi_tes << " DE_gaus = " << reso << " +/- " << reso_err << std::endl;
    std::cout << "E = " << voltage-phi_tes << " DE_fwhm = " << reso_fwhm << " +/- " << reso_fwhm_err << std::endl;
    std::cout << "P: " << cruijff->GetProb()*100 << "%" << std::endl;

  
  }

   //drawing of superimposed fits histogram
   for( unsigned i=0; i<histos.size(); ++i ) {

    TF1* f = histos[i]->GetFunction(Form("cru_%s", histos[i]->GetName()));
   
    histos[i]->SetMarkerSize(3);
    histos[i]->SetLineWidth(3);
    histos[i]->SetFillStyle(3004);
    histos[i]->SetFillColor(colors[i]);
    histos[i]->SetTitle(Form("%d V",voltage));
    histos[i]->SetLineColor(colors[i]);
    if(CD_number==204){
    histos[i]->GetXaxis()->SetRangeUser(0.56,0.65);
    histos[i]->GetYaxis()->SetRangeUser(0,330);
    }
    if(CD_number==188){
    histos[i]->GetXaxis()->SetRangeUser(0.3,0.9);
    histos[i]->GetYaxis()->SetRangeUser(0,50);
    }
    gStyle->SetOptStat(0);
    f->SetLineColor(colors[i]);
    f->SetLineWidth(5);
    if(i!=1){
    histos[i]->Draw("same");
    f->Draw("same");
    }
    
  }

  std::string outdir( Form("plots/CD%d/%s/%dV", CD_number, misura, voltage));
  system( Form("mkdir -p %s", outdir.c_str()) );
  c1->SaveAs(Form("plots/CD%d/%s/%dV/binning_%s_fit_final.png",CD_number,misura,voltage,choice,nbins)); 



    
  
  return 0;

}

//+++++++++++++++++++++++++++++++++++++++++++//
//++++++++++++++FUNZIONI UTILI+++++++++++++++//
//+++++++++++++++++++++++++++++++++++++++++++//

//funzione di controllo sui parametri iniziali
void Input_Param_Ctrls(int argc, char* argv[]){
  //ctrl sull'input inserito
  if (argc!=4){
    std::cout << "USAGE: ./drawFinalFits [CD_number] [misura] [choice]" << std::endl;
    std::cout << "[choice] : amp/charge" << std::endl; 
    std::cout << "EXAMPLE: ./drawFinalFits 204 B60_preamp_post_cond1 amp" << std::endl;
    exit(1);
  }

  //ctrl su scelta amp/charge
  if(strcmp(argv[3],"amp"   )!=0 &&
     strcmp(argv[3],"charge")!=0 &&
     (atoi(argv[1]))==188 && (atoi(argv[1]))==204){
    std::cout << "Wrong choice! Choose between 'amp' and 'charge'" << std::endl;
    exit(1);
  }

  //ctrl su scelta amp/charge
  if(strcmp(argv[3],"amp"                    )!=0 &&
     strcmp(argv[3],"charge"                 )!=0 &&
     strcmp(argv[3],"amp_10kHz"              )!=0 &&
     strcmp(argv[3],"amp_100kHz"             )!=0 &&
     strcmp(argv[3],"amp_10kHz_100kHz"       )!=0 &&
     strcmp(argv[3],"amp_10kHz_160kHz"       )!=0 &&
     strcmp(argv[3],"amp_10kHz_100kHz_160kHz")!=0 &&
     (atoi(argv[1]))==188){
    std::cout << "Wrong choice! Choose between 'amp','amp_10kHz','amp_100kHz','amp_10kHz_100kHz','amp_10kHz_160kHz' and 'charge'" << std::endl;
    exit(1);
  }

  //ctrl su scelta amp/charge
  if(strcmp(argv[3],"amp"             )!=0 &&
     strcmp(argv[3],"charge"          )!=0 &&
     strcmp(argv[3],"amp_10kHz"       )!=0 &&
     strcmp(argv[3],"amp_10kHz_160kHz")!=0 &&
     (atoi(argv[1]))==204){
    std::cout << "Wrong choice! Choose between 'amp','amp_10kHz', 'amp_10kHz_160kHz' and 'charge'" << std::endl;
    exit(1);
  }

  //ctrl su scelta amp/charge
  if(strcmp(argv[3],"amp"             )!=0 &&
     strcmp(argv[3],"charge"          )!=0 &&
     strcmp(argv[3],"amp_10kHz"       )!=0 &&
     strcmp(argv[3],"amp_100kHz"      )!=0 &&
     strcmp(argv[3],"amp_10kHz_10kHz" )!=0 &&
     (atoi(argv[1]))==222){
    std::cout << "Wrong choice! Choose between 'amp','amp_10kHz', 'amp_10kHz_10kHz', 'amp_100kHz' and 'charge'" << std::endl;
    exit(1);
  }

  //ctrl su scelta amp/charge
  if(strcmp(argv[3],"amp")==0 && (atoi(argv[1]))==188){
    std::cout << "Per studiare questi dati vai nella cartella Electron TES old e usa drawFits.cpp" << std::endl;
    exit(1);
  }
  
}


