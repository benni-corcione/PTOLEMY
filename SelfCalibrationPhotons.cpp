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
float GetRLoad(int CD_number);
float GetRpar(int CD_number);
float GetIbias(int CD_number);
float GetI0(int CD_number,char* meas);
float GetVtes(int CD_number);
float GetGain(int CD_number);
float IntegrateSignal(float func[2502], float dx);

int main(int argc, char* argv[]) {

  //controlli iniziali su variabili da input
  Input_Param_Ctrls(argc,argv);

  //parametri da input
  int   CD_number = (atoi(argv[1])); //cooldown
  char* misura    =       argv[2]  ;
     
  Settings settings;
  TCanvas* c1 = new TCanvas( "c1", "", 1500, 1500 );
  settings.Margin(c1);
  c1->SetLeftMargin(0.155);
  c1->cd();
  
  float Rload = GetRLoad(CD_number);
  float gain  = GetGain(CD_number);

  int nbins  = 70;
  float Emax = 10;
  float conversion = 6.242e+18;
  

  

  TH1F* E_ETF = new TH1F("E_ETF", "", nbins, -Emax, Emax);
    
  float I0   = GetI0(CD_number,misura);
  float Vtes = GetVtes(CD_number);

    
  //leggo tutti i tree per i voltaggi
  TFile* file = TFile::Open( Form("data/root/CD%d/%s/0mW/CD%d_0mW_treeraw.root", CD_number, misura,  CD_number), "read" );
  TTree* treeraw = (TTree*)file->Get("treeraw");
  int nentries = treeraw->GetEntries();

  for(int iEntry=0; iEntry<nentries; iEntry++){

    treeraw->GetEntry(iEntry);
      
    float pshape [2502];
    float deltaI [2500];
    float deltaI2[2500];
    float dt;
      
    //lettura del tree in amp o charge
    treeraw->SetBranchAddress("pshape", &pshape);
    treeraw->SetBranchAddress("sampling_time",&dt);

    //riempimento shape di variazione corrente
    for(int i=0; i<2500; i++){
      deltaI [i] = pshape[i]*gain;
      deltaI2[i] = deltaI[i]*deltaI[i];
    }
      
    float integral_deltaI  = IntegrateSignal(deltaI , dt);
    float integral_deltaI2 = IntegrateSignal(deltaI2, dt);

    //std::cout << integral_deltaI << " " <<  integral_deltaI2 << std::endl;
    float energy = (I0*Rload-Vtes)*integral_deltaI + Rload*integral_deltaI2;
    //std::cout << energy << std::endl;
    E_ETF->Fill(energy*conversion);
      
  } //for sulle entries dello stesso voltaggio 

  c1->cd();
  gStyle->SetOptStat(0);
  E_ETF->SetLineWidth(3);
  E_ETF->GetXaxis()->SetTitle("Energy ETF (eV)");
  E_ETF->GetYaxis()->SetTitle("Counts");
  E_ETF->SetTitle("ETF Autocalibration");
  E_ETF->Draw();
    
  std::string outdir( Form("plots/CD%d/%s", CD_number, misura));
  system( Form("mkdir -p %s", outdir.c_str()) );
  c1->SaveAs(Form("plots/CD%d/%s/etf.png",CD_number,misura));
  c1->Clear();
    
  

  return 0;

}

//+++++++++++++++++++++++++++++++++++++++++++//
//++++++++++++++FUNZIONI UTILI+++++++++++++++//
//+++++++++++++++++++++++++++++++++++++++++++//

//funzione di controllo sui parametri iniziali
void Input_Param_Ctrls(int argc, char* argv[]){
  //ctrl sull'input inserito
  if (argc!=3){
    std::cout << "USAGE: ./drawFinalFits [CD_number] [misura]" << std::endl;
    std::cout << "EXAMPLE: ./drawFinalFits 204 B60_preamp_post_cond1" << std::endl;
    exit(1);
  }
  
}

float GetRpar(int CD_number){
  float Rpar = 0;
  if(CD_number==188){Rpar = 0.01 ;} //30mOhm
  if(CD_number==204){Rpar = 0.012;} //30mOhm
  return Rpar;
}

float GetRLoad(int CD_number){
  float RL = 0;
  float Rsh = 0.02; //shunt fissa
  float Rpar = GetRpar(CD_number);
  RL = Rsh + Rpar;
 
  return RL;
}

float GetIbias(int CD_number){
  float Ibias;
  if(CD_number==204){ Ibias = 28.5; }
  
  return Ibias*1E-06;
}


float GetI0(int CD_number, char* meas){
  float I0;
  float Rtes;
  float Rsh = 0.02;
  
  float Rpar  = GetRpar(CD_number);
  float Ibias = GetIbias(CD_number);
  
  if(strcmp(meas,"B60_post_cond3_moku"  )==0){ Rtes = 0.215*0.38; } //Rn*wp% 
  
  I0 = Ibias*Rsh/(Rtes+Rsh+Rpar);
  return I0;
}


float GetVtes(int CD_number){
  float V;
  float Rsh = 0.02;
  float Ibias = GetIbias(CD_number);
 
  V = Ibias*Rsh;
  return V;
}

float GetGain(int CD_number){
  float G;
  float channel1 = 269443;
  float channel2 = 268275;
  if(CD_number==188){G=1./channel1;}
  if(CD_number==204){G=1./channel2;}
  return G;
}
		    
float IntegrateSignal(float func[2502], float dx){
  float integral = 0;

  //calcolo baseline
  float baseline = 0;
  int   i_min    = 0;
  int   i_max    = 250;

  for(int i=i_min; i<i_max; i++){
    baseline += func[i];
  }
  baseline /= (i_max-i_min);

  //calcolo integrale
  for(int i=0; i<2500; i++){
    integral += (func[i]-baseline);
  }

  integral*=dx;
  
  return (integral); //integrali>0 così
}

