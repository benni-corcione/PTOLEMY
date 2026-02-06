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

  //controlli iniziali su variabili da input
  Input_Param_Ctrls(argc,argv);

  //parametri da input
  int   CD_number = (atoi(argv[1])); //cooldown
  char* choice    =       argv[3]  ;
  char* misura    =       argv[2]  ;

  int voltage;
  //if(CD_number==188) voltage=97;
  if(CD_number==188) voltage=105;
  //if(CD_number==204) voltage=98;
  if(CD_number==204) voltage=103;
  
  Settings settings;
  TCanvas* c1 = new TCanvas( "c1", "", 1500, 1500 );
  settings.Margin(c1);
  c1->SetLeftMargin(0.155);
  c1->cd();

  float phi_carbon = 4.5;
  float phi_tes = GetPhiTes();
  
 
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

  //impostazione per gli histos
  int nbins = h_var.get_nbins();;
    
  std::string x_name = h_var.get_x_name();
  std::string y_name = h_var.get_y_name();
  
  float hist_min  = h_var.get_hist_min();
  float hist_max  = h_var.get_hist_max();
  float range_min = h_var.get_range_min();
  float range_max = h_var.get_range_max();
    	      
  //creo un histo per ogni tree
  TH1F* histo = new TH1F("histo", "", nbins, hist_min, hist_max);
  Long64_t nentries = tree->GetEntries();
    
  for(int iEntry=0; iEntry<nentries; iEntry++){
    tree->GetEntry(iEntry);
    histo->Fill(variable);
  }
    
  float fitmin1, fitmin2;
  float fitmax1, fitmax2;

  /*
   //per V=97
  if(CD_number==188){
    fitmin1 = 0.45;
    fitmin2 = 0.43;
    fitmax1 = 0.8;
    fitmax2 = 0.81;
  }
  */
  /*
  //per V=98
  if(CD_number==204){
    fitmin1 = 0.604;
    fitmin2 = 0.61;
    fitmax1 = 0.63;
    fitmax2 = 0.63;
  }
  */

  //per V=105
  if(CD_number==188){
    fitmin1 = 0.45;
    fitmin2 = 0.43;
    fitmax1 = 0.8;
    fitmax2 = 0.81;
  }

  //per V=103
  if(CD_number==204){
    fitmin1 = 0.6132;
    fitmin2 = 0.616;
    fitmax1 = 0.635;
    fitmax2 = 0.63;
  }
  
  Fit f_var = SetFitValues(CD_number,choice,histo,voltage,misura);
    
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
    
  TF1 *cruijff1  = new TF1("cruijff1", Cruijff, fit_min, fit_max, 5);
  TF1 *cruijff2  = new TF1("cruijff2", Cruijff, fitmin1, fitmax1, 5);
  TF1 *cruijff3  = new TF1("cruijff3", Cruijff, fitmin2, fitmax2, 5);

  //parametri del fit
  cruijff1->SetParNames("#mu","#sigma_{L}", "#sigma_{R}", "#alpha", "A");
  cruijff1->SetParLimits(0,    mu_min,    mu_max);
  cruijff1->SetParLimits(1,sigmaL_min,sigmaL_max); 
  cruijff1->SetParLimits(2,sigmaR_min,sigmaR_max);
  cruijff1->SetParLimits(3,  alfa_min,  alfa_max);
  cruijff1->SetParLimits(4,     A_min,     A_max);
    
  cruijff2->SetParNames("#mu","#sigma_{L}", "#sigma_{R}", "#alpha", "A");
  cruijff2->SetParLimits(0,    mu_min,    mu_max);
  cruijff2->SetParLimits(1,sigmaL_min,sigmaL_max); 
  cruijff2->SetParLimits(2,sigmaR_min,sigmaR_max);
  cruijff2->SetParLimits(3,  alfa_min,  alfa_max);
  cruijff2->SetParLimits(4,     A_min,     A_max);

  cruijff3->SetParNames("#mu","#sigma_{L}", "#sigma_{R}", "#alpha", "A");
  cruijff3->SetParLimits(0,    mu_min,    mu_max);
  cruijff3->SetParLimits(1,sigmaL_min,sigmaL_max); 
  cruijff3->SetParLimits(2,sigmaR_min,sigmaR_max);
  cruijff3->SetParLimits(3,  alfa_min,  alfa_max);
  cruijff3->SetParLimits(4,     A_min,     A_max);

  //if(CD_number==204){cruijff3->SetParameter(0,0.61);}
 

  //gaussiana asimmetrica e non cruijff
  cruijff1->FixParameter(3,0); //alfa value
  cruijff1->SetNpx(1000);
  cruijff1->SetParameters(mu_, sigmaL_, sigmaR_, alfa_, A_); //parametri iniziali
  cruijff1->SetLineColor(kRed);
  cruijff1->SetLineWidth(5);
  cruijff1->SetNpx(1000);

  cruijff2->FixParameter(3,0); //alfa value
  cruijff2->SetNpx(1000);
  cruijff2->SetParameters(mu_, sigmaL_, sigmaR_, alfa_, A_); //parametri iniziali
  cruijff2->SetLineColor(kMagenta+2);
  cruijff2->SetLineWidth(5);
  cruijff2->SetNpx(1000);

  cruijff3->FixParameter(3,0); //alfa value
  cruijff3->SetNpx(1000);
  cruijff3->SetParameters(mu_, sigmaL_, sigmaR_, alfa_, A_); //parametri iniziali
  cruijff3->SetLineColor(kOrange);
  cruijff3->SetLineWidth(5);
  cruijff3->SetNpx(1000);
    
    
  histo->GetXaxis()->SetTitle("Amplitude (V)");
  histo->SetMarkerSize(2);
  histo->SetLineWidth(3);
  histo->Fit(cruijff1,"RX");
  histo->Fit(cruijff2,"RX+");
  histo->Fit(cruijff3,"RX+");
  histo->SetNdivisions(510);
  histo->Draw(); //"pe" per avere points+errors

 
  cruijff1->Draw("same");
  cruijff3->Draw("same");
   cruijff2->Draw("same");

  if(CD_number==188) range_min = 0.3;
  settings.graphic_histo(histo,range_min,range_max,x_name.c_str(),y_name.c_str(),voltage);
  settings.general_style();
  histo->SetLineWidth(3);
  //gStyle->SetOptFit(1111);

  std::string outdir( Form("plots/CD%d/%s/%dV", CD_number, misura, voltage));
  system( Form("mkdir -p %s", outdir.c_str()) );
  c1->SaveAs(Form("plots/CD%d/%s/%dV/range_%s_fit_%dbins_final.png",CD_number,misura,voltage,choice,nbins)); 

  //estrazione parametri dal fit
    float mu1       = cruijff1->GetParameter(0);
    float muerr1    = cruijff1->GetParError(0);
    float sigmaR1    = cruijff1->GetParameter(2);
    float sigmaRerr1 = cruijff1->GetParError(2);
    float sigmaL1    = cruijff1->GetParameter(1);
    float sigmaLerr1 = cruijff1->GetParError(1);

    //estrazione parametri dal fit
    float mu2       = cruijff2->GetParameter(0);
    float muerr2    = cruijff2->GetParError(0);
    float sigmaR2    = cruijff2->GetParameter(2);
    float sigmaRerr2 = cruijff2->GetParError(2);
    float sigmaL2    = cruijff2->GetParameter(1);
    float sigmaLerr2 = cruijff2->GetParError(1);

    //estrazione parametri dal fit
    float mu3       = cruijff3->GetParameter(0);
    float muerr3    = cruijff3->GetParError(0);
    float sigmaR3    = cruijff3->GetParameter(2);
    float sigmaRerr3 = cruijff3->GetParError(2);
    float sigmaL3    = cruijff3->GetParameter(1);
    float sigmaLerr3 = cruijff3->GetParError(1);

   
    //l'errore sulla stabilità di ites-ibias è insito nei segnali che otteniamo

    float energy_ele = voltage- phi_carbon + (phi_carbon-phi_tes);
    float energy_err = sqrt(0.0009 + pow((0.04+0.0015*voltage),2));
    
    float reso1 = sigmaR1/mu1*(voltage-phi_tes);
    if(CD_number==188){
      muerr1 = sqrt( muerr1*muerr1 + 0.01*0.01*mu1*mu1 );
      reso1 = sigmaR1/mu1*(voltage);
    }
    float reso_err1 = sqrt( sigmaRerr1*sigmaRerr1*energy_ele*energy_ele/(mu1*mu1) + sigmaR1*sigmaR1*energy_ele*energy_ele*muerr1*muerr1/(mu1*mu1*mu1*mu1) + sigmaR1*sigmaR1*energy_err*energy_err/(mu1*mu1));
   
    //aggiunta fwhm 
    float fwhm1 = (sigmaR1+sigmaL1)*sqrt(2*log(2));
    float fwhm_err1 = sqrt(2*log(2))*sqrt(sigmaRerr1*sigmaRerr1+sigmaLerr1*sigmaLerr1);
    float piece1_f1 = fwhm_err1*fwhm_err1*energy_ele*energy_ele/(mu1*mu1);
    float piece2_f1 = fwhm1*fwhm1*muerr1*muerr1*energy_ele*energy_ele/(mu1*mu1*mu1*mu1);
    float piece3_f1 = fwhm1*fwhm1*energy_err*energy_err/(mu1*mu1);
    float reso_fwhm1 = fwhm1/mu1*(energy_ele);
    float reso_fwhm_err1 = sqrt(piece1_f1+piece2_f1+piece3_f1);

    float reso2 = sigmaR2/mu2*(voltage-phi_tes);
     if(CD_number==188){
      muerr2 = sqrt( muerr2*muerr2 + 0.01*0.01*mu2*mu2 );
      reso2 = sigmaR2/mu2*(voltage);
    }
    float reso_err2 = sqrt( sigmaRerr2*sigmaRerr2*energy_ele*energy_ele/(mu2*mu2) + sigmaR2*sigmaR2*energy_ele*energy_ele*muerr2*muerr2/(mu2*mu2*mu2*mu2) + sigmaR2*sigmaR2*energy_err*energy_err/(mu2*mu2));
   
    //aggiunta fwhm 
    float fwhm2 = (sigmaR2+sigmaL2)*sqrt(2*log(2));
    float fwhm_err2 = sqrt(2*log(2))*sqrt(sigmaRerr2*sigmaRerr2+sigmaLerr2*sigmaLerr2);
    float piece1_f2 = fwhm_err2*fwhm_err2*energy_ele*energy_ele/(mu2*mu2);
    float piece2_f2 = fwhm2*fwhm2*muerr2*muerr2*energy_ele*energy_ele/(mu2*mu2*mu2*mu2);
    float piece3_f2 = fwhm2*fwhm2*energy_err*energy_err/(mu2*mu2);
    float reso_fwhm2 = fwhm2/mu2*(energy_ele);
    float reso_fwhm_err2 = sqrt(piece1_f2+piece2_f2+piece3_f2);


    float reso3 = sigmaR3/mu3*(voltage-phi_tes);
     if(CD_number==188){
      muerr3 = sqrt( muerr3*muerr3 + 0.01*0.01*mu3*mu3 );
      reso3 = sigmaR3/mu3*(voltage);
    }
    float reso_err3 = sqrt( sigmaRerr3*sigmaRerr3*energy_ele*energy_ele/(mu3*mu3) + sigmaR3*sigmaR3*energy_ele*energy_ele*muerr3*muerr3/(mu3*mu3*mu3*mu3) + sigmaR3*sigmaR3*energy_err*energy_err/(mu3*mu3));
   
    //aggiunta fwhm 
    float fwhm3 = (sigmaR3+sigmaL3)*sqrt(2*log(2));
    float fwhm_err3 = sqrt(2*log(2))*sqrt(sigmaRerr3*sigmaRerr3+sigmaLerr3*sigmaLerr3);
    float piece1_f3 = fwhm_err3*fwhm_err3*energy_ele*energy_ele/(mu3*mu3);
    float piece2_f3 = fwhm3*fwhm3*muerr3*muerr3*energy_ele*energy_ele/(mu3*mu3*mu3*mu3);
    float piece3_f3 = fwhm3*fwhm3*energy_err*energy_err/(mu3*mu3);
    float reso_fwhm3 = fwhm3/mu3*(energy_ele);
    float reso_fwhm_err3 = sqrt(piece1_f3+piece2_f3+piece3_f3);

    std::cout << "PAPER: " << std::endl;
    std::cout << "E = " << voltage-phi_tes << " DE_gaus = " << reso1 << " +/- " << reso_err1 << std::endl;
    std::cout << "E = " << voltage-phi_tes << " DE_fwhm = " << reso_fwhm1 << " +/- " << reso_fwhm_err1 << std::endl;
    std::cout << "E = " << voltage-phi_tes << " DE_gaus = " << reso2 << " +/- " << reso_err2 << std::endl;
     std::cout << "E = " << voltage-phi_tes << " DE_fwhm = " << reso_fwhm2 << " +/- " << reso_fwhm_err2 << std::endl;
    std::cout << "E = " << voltage-phi_tes << " DE_gaus = " << reso3 << " +/- " << reso_err3 << std::endl;
    
    std::cout << "E = " << voltage-phi_tes << " DE_fwhm = " << reso_fwhm3 << " +/- " << reso_fwhm_err3 << std::endl;
   
   

    
  
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


