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

int main(void) {

  std::ifstream ifs1 ("data/params_CD204B60_preamp_post_cond1amp.txt" );
  std::ifstream ifs2 ("data/params_CD204B60_post_cond3_mokuamp.txt" );
  
  std::vector<int> volts;
  volts.push_back(97);  volts.push_back(99); volts.push_back(101);
  volts.push_back(103); volts.push_back(106);
     
  Settings settings;
  TCanvas* c1 = new TCanvas( "c1", "", 1500, 1500 );
  settings.Margin(c1);
  c1->SetLeftMargin(0.155);
  c1->cd();

  int iPoint = 0;
  double v1,mu1,err1;
  double v2,mu2,err2;
  double sigma,sigmaerr;
  double mu1_[volts.size()];
  double muerr1_[volts.size()];
  double mu2_[volts.size()];
  double muerr2_[volts.size()];
		 
  while( ifs1 >> v1 >> mu1 >> err1 >> sigma >> sigmaerr ) {
    mu1_   [iPoint] = mu1;
    muerr1_[iPoint] = err1;
    iPoint++;
  }

  iPoint = 0;

  while( ifs2 >> v2 >> mu2 >> err2 >> sigma >> sigmaerr ) {
      
    if(v2==97 || v2==99 || v2==101 || v2==103 || v2==106){
    mu2_   [iPoint] = mu2;
    muerr2_[iPoint] = err2;
    iPoint++;
    }
  }

  float v[volts.size()];
  float v_err[volts.size()];
  float ratio[volts.size()];
  float r_err[volts.size()];

  for(int i=0; i<volts.size();i++){
    v[i]=volts[i];
    v_err[i]=0;
    ratio[i]=mu1_[i]/mu2_[i];
    r_err[i]=sqrt(muerr1_[i]*muerr1_[i]+muerr1_[2]*muerr1_[2]);
    std::cout << v[i] << " " << ratio[i] << " " << r_err[i]<< std::endl;

    
  }

  TGraphErrors* r = new TGraphErrors(volts.size(), v, ratio, v_err, r_err);
  c1->cd();
  r->SetMarkerStyle(8);
  r->GetXaxis()->SetTitle("V_cnt");
  r->GetYaxis()->SetTitle("Mu ratio");
  r->Draw();
  c1->SaveAs(Form("plots/CD204/B60_post_cond3_moku/bessel_lowpass.png"));
  

    
  

  return 0;

}


 
