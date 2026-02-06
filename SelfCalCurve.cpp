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
#include <sstream>
#include <filesystem>

#include "graphics.h"
#include "AndCommon.h"
#include "CD_details.h"


void Input_Param_Ctrls(int argc, char* argv[]);
float GetRLoad(int CD_number);
float GetRpar(int CD_number);
float GetIbias(int CD_number, int v);
float GetI0(int CD_number, int v, char* meas);
float GetVtes(int CD_number, int v);
float GetGain(int CD_number);
float IntegrateSignal(float func[2502], float dx);

int main(int argc, char* argv[]) {

  //controlli iniziali su variabili da input
  Input_Param_Ctrls(argc,argv);

  //parametri da input
  int   CD_number = (atoi(argv[1])); //cooldown
  char* misura    =       argv[2]  ;

  std::vector<int> volts = GetVoltages(CD_number,misura);
     
  Settings settings;
  TCanvas* c1 = new TCanvas( "c1", "", 1500, 1500 );
  settings.Margin(c1);
  c1->SetLeftMargin(0.155);
  c1->cd();
  
  float Rload = GetRLoad(CD_number);
  float gain  = GetGain(CD_number);

  //int nbins  = 70;
  //float Emax = 106;
  float conversion = 6.242e+18;
  int iPoint = 0;
  int E1=0;
  int E2=0;
  int npoints = 30;
  float e_range = 106;
  TGraph* cal = new TGraph();

  std::stringstream stream;
  std::string filename = "data/selfcal" + std::to_string(CD_number)
    + std::string(misura) + ".txt";

  stream << "rm -f " << filename << std::endl;
  system(stream.str().c_str());
  
  std::ofstream ofs(filename);
  ofs << "V (V)  E(eV)" << std::endl;
    
  for(int step=0; step<npoints; step++){

    
      
    E1=(step)*e_range*1./npoints;
    E2=(step+1)*e_range*1./npoints;

    std::cout << "step: " << step << "  E1: " << E1 << "  E2: " << E2 << std::endl;
    
    float amp_mean = 0.;
    float energy_sum = 0.;
    int count = 0;
    
    for(unsigned i=0; i<volts.size(); i++) {

      std::cout << "volts: " << volts[i] << std::endl; 
      
      //TH1F* E_ETF = new TH1F("E_ETF", "", nbins, 0., Emax);
      
      float I0   = GetI0(CD_number,volts[i],misura);
      float Vtes = GetVtes(CD_number,volts[i]);
      
      
      //leggo tutti i tree per i voltaggi
      TFile* file = TFile::Open( Form("data/root/CD%d/%s/%dV/CD%d_%dV_treeraw.root", CD_number, misura, volts[i], CD_number, volts[i]), "read" );
      TFile* file2 = TFile::Open( Form("data/root/CD%d/%s/%dV/CD%d_%dV_tree.root", CD_number, misura, volts[i], CD_number, volts[i]), "read" );
      TTree* treeraw = (TTree*)file ->Get("treeraw");
      TTree* tree    = (TTree*)file2->Get("tree");
      int nentries = treeraw->GetEntries();
      
      for(int iEntry=0; iEntry<nentries; iEntry++){
	
	treeraw->GetEntry(iEntry);
	tree   ->GetEntry(iEntry);
	
	float pshape [2502];
	float deltaI [2500];
	float deltaI2[2500];
	float dt;
	float amp;
	
	//lettura del tree in amp o charge
	tree   ->SetBranchAddress("amp", &amp);
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
	energy *= conversion;
	if(E1<=energy && energy<E2){ amp_mean+=amp; energy_sum+=energy; count++; }

	
      } //for sulle entries dello stesso voltaggio

      file->Close();
      file2->Close();
    }//voltaggi

    float amp_point = 1.*amp_mean/count;
    float energy_point = 1.*energy_sum/count;
    cal->SetPoint(iPoint,amp_point,energy_point);
    iPoint++;
    //attribuire errore ai punti ?

    //scrittura su file
    ofs << amp_point << " " << energy_point << " " << count << std::endl;

    std::cout << " " << std::endl; 
    
  }//step energetici

  ofs.close();

  cal->SetPoint(iPoint,0,-20);
  cal->SetPoint(iPoint+1,0,+120);
  cal->SetTitle("Overall self calibration curve");
  cal->GetXaxis()->SetTitle("Signal amplitude (V)");
  cal->GetYaxis()->SetRangeUser(0,110);
  cal->GetYaxis()->SetTitle("Electron energy (eV)");
  cal->SetMarkerStyle(8);
  cal->SetMarkerSize(2);
  cal->SetMarkerColor(30);

  c1->cd();
  cal->Draw("ap");
  c1->SaveAs(Form("plots/CD%d/%s/self_calibration_curve.png",CD_number,misura));
  
  //graficare curva totale ottenuta
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

float GetIbias(int CD_number, int v){
  float Ibias;
  if(CD_number==188){
    if(v==95 ){Ibias = 56.7;}
    if(v==96 ){Ibias = 56.7;}
    if(v==97 ){Ibias = 56.7;}
    if(v==98 ){Ibias = 56.5;}
    if(v==99 ){Ibias = 56.4;}
    if(v==100){Ibias = 56.2;}
    if(v==101){Ibias = 55.7;}
    if(v==102){Ibias = 54.9;}
    if(v==103){Ibias = 53.7;}
    if(v==104){Ibias = 51.8;}
    if(v==105){Ibias = 48.4;}
  }

  if(CD_number==204){ Ibias = 28.5; }
  
  return Ibias*1E-06;
}


float GetI0(int CD_number, int v, char* meas){
  float I0;
  float Rtes;
  float Rsh = 0.02;
  
  float Rpar  = GetRpar(CD_number);
  float Ibias = GetIbias(CD_number, v);
  
  if(CD_number==188){ Rtes = 0.246*0.35; } //Rn*wp% 
  if(strcmp(meas,"B60_preamp_post_cond1")==0){ Rtes = 0.215*0.36; } //Rn*wp% 
  if(strcmp(meas,"B60_post_cond3_moku"  )==0){ Rtes = 0.215*0.38; } //Rn*wp% 
  
  I0 = Ibias*Rsh/(Rtes+Rsh+Rpar);
  return I0;
}


float GetVtes(int CD_number, int v){
  float V;
  float Rsh = 0.02;
  float Ibias = GetIbias(CD_number,v);
 
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

