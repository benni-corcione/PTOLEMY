#include <iostream>
#include <fstream>
#include <vector>
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


void Input_Param_Ctrls(int argc, char* argv[]);

int main(int argc, char* argv[]) {

  //controlli iniziali su variabili da input
  Input_Param_Ctrls(argc,argv);

  //parametri da input
  int CD_number1 = atoi(argv[1]);
  int CD_number2 = atoi(argv[2]);

  if(CD_number1!=188 || (CD_number2!=204 && CD_number2!=222))
    {std::cout << "Il programma per ora non lo fa" << std::endl; exit(1);}
  std::string file1;
  std::string file2;
  if(CD_number1 == 188){file1 = "params_CD188conteggiamp.txt";}
  if(CD_number2 == 204){file2 = "params_CD204B60_post_cond3_mokuamp.txt";}
  if(CD_number2 == 222){file2 = "params_CD222E100_squidfilter_ER_031025_tr12amp.txt";}

  TGraphErrors* gr_reso_1      = new TGraphErrors(0);
  TGraphErrors* gr_reso_fwhm_1 = new TGraphErrors(0);
  TGraphErrors* gr_reso_2      = new TGraphErrors(0);
  TGraphErrors* gr_reso_fwhm_2 = new TGraphErrors(0);
   
  Settings settings;
  TCanvas* c1 = new TCanvas( "c1", "", 1500, 1500 );
  settings.Margin(c1);
  c1->SetLeftMargin(0.155);
  c1->cd();

  std::string filename1 = "data/" + std::string(file1);
  std::string filename2 = "data/" + std::string(file2);

  std::vector<int>    volts_1;    std::vector<int>    volts_2;
  std::vector<float>     mu_1;    std::vector<float>     mu_2;
  std::vector<float> mu_err_1;    std::vector<float> mu_err_2;
  std::vector<float>     sR_1;    std::vector<float>     sR_2;
  std::vector<float> sR_err_1;    std::vector<float> sR_err_2;
  std::vector<float>     sL_1;    std::vector<float>     sL_2;
  std::vector<float> sL_err_1;    std::vector<float> sL_err_2;
  
  float phi_tes = GetPhiTes();

  //FOR PER LEGGERE E RIEMPIRE VETTORI
  std::ifstream infile1(filename1); 
  std::string line1;
  
  if(!infile1.is_open()) {
    std::cerr << "cannot open input file" << std::endl;
    return -1;
  }

  getline(infile1,line1); //salto prima riga
   
  while(!infile1.eof()) {

    int val1;
    float val2, val3, val4, val5, val6, val7;

    getline(infile1,line1);
    if( line1 == "\n" || line1 == "") continue;
    sscanf(line1.c_str(),"%d %f %f %f %f %f %f", &val1, &val2, &val3, &val4, &val5, &val6, &val7);

    volts_1.push_back(val1);
    mu_1.push_back(val2);
    mu_err_1.push_back(val3);
    sR_1.push_back(val4);
    sR_err_1.push_back(val5);
    sL_1.push_back(val6);
    sL_err_1.push_back(val7);
    
  } // !eof
  infile1.close();


  //FOR PER LEGGERE E RIEMPIRE VETTORI
  std::ifstream infile2(filename2); 
  std::string line2;
  
  if(!infile2.is_open()) {
    std::cerr << "cannot open input file" << std::endl;
    return -1;
  }
  
  getline(infile2,line2); //salto prima riga
  
  while(!infile2.eof()) {

    int val1;
    float val2, val3, val4, val5, val6, val7;
    
    getline(infile2,line2);
    if( line2 == "\n" || line2 == "") continue;
    sscanf(line2.c_str(),"%d %f %f %f %f %f %f", &val1, &val2, &val3, &val4, &val5, &val6, &val7);

    volts_2.push_back(val1);
    mu_2.push_back(val2);
    mu_err_2.push_back(val3);
    sR_2.push_back(val4);
    sR_err_2.push_back(val5);
    sL_2.push_back(val6);
    sL_err_2.push_back(val7);
    
  } // !eof
  infile2.close();

  
  int size1 = volts_1.size();
  int size2 = volts_2.size();
  
  float energy_1[size1]        ;    float energy_2[size2]        ;
  float reso_sigma_1[size1]    ;    float reso_sigma_2[size2]    ;
  float reso_sigma_err_1[size1];    float reso_sigma_err_2[size2];
  float reso_fwhm_1[size1]     ;    float reso_fwhm_2[size2]     ;
  float reso_fwhm_err_1[size1] ;    float reso_fwhm_err_2[size2] ;
  
  //calcolo di energia, risoluzione_sigma, errore risoluzione, risoluzione fwhm, errore risoluzione;
  for(int i=0; i<size1; i++) {
    float fwhm     = (sR_1[i]+sL_1[i])*sqrt(2*log(2));
    float fwhm_err = sqrt(2*log(2))*sqrt(sR_err_1[i]*sR_err_1[i]+sL_err_1[i]*sL_err_1[i]);
    float piece1_s = sR_err_1[i]*sR_err_1[i]/(mu_1[i]*mu_1[i]);
    float piece2_s = sR_1[i]*sR_1[i]*mu_err_1[i]*mu_err_1[i]/(mu_1[i]*mu_1[i]*mu_1[i]*mu_1[i]);
    float piece1_f = fwhm_err*fwhm_err/(mu_1[i]*mu_1[i]);
    float piece2_f = fwhm*fwhm*mu_err_1[i]*mu_err_1[i]/(mu_1[i]*mu_1[i]*mu_1[i]*mu_1[i]);
    
    energy_1[i]         = volts_1[i]-phi_tes;
    reso_sigma_1[i]     = sR_1[i]/mu_1[i]*(energy_1[i]);
    reso_fwhm_1[i]      = fwhm/mu_1[i]*(energy_1[i]);
    reso_sigma_err_1[i] = sqrt(piece1_s+piece2_s)*(energy_1[i]);
    reso_fwhm_err_1[i]  = sqrt(piece1_f+piece2_f)*(energy_1[i]);

    gr_reso_1->SetPoint(i,energy_1[i],reso_sigma_1[i]);
    gr_reso_1->SetPointError(i,0,reso_sigma_err_1[i]);
    gr_reso_fwhm_1->SetPoint(i,energy_1[i],reso_fwhm_1[i]);
    gr_reso_fwhm_1->SetPointError(i,0,reso_fwhm_err_1[i]);
    
  }

   for(int i=0; i<size2; i++) {
    float fwhm     = (sR_2[i]+sL_2[i])*sqrt(2*log(2));
    float fwhm_err = sqrt(2*log(2))*sqrt(sR_err_2[i]*sR_err_2[i]+sL_err_2[i]*sL_err_2[i]);
    float piece1_s = sR_err_2[i]*sR_err_2[i]/(mu_1[i]*mu_2[i]);
    float piece2_s = sR_2[i]*sR_2[i]*mu_err_2[i]*mu_err_2[i]/(mu_2[i]*mu_2[i]*mu_2[i]*mu_2[i]);
    float piece1_f = fwhm_err*fwhm_err/(mu_2[i]*mu_2[i]);
    float piece2_f = fwhm*fwhm*mu_err_2[i]*mu_err_2[i]/(mu_2[i]*mu_2[i]*mu_2[i]*mu_2[i]);
    
    energy_2[i]         = volts_2[i]-phi_tes;
    reso_sigma_2[i]     = sR_2[i]/mu_2[i]*(energy_2[i]);
    reso_fwhm_2[i]      = fwhm/mu_2[i]*(energy_2[i]);
    reso_sigma_err_2[i] = sqrt(piece1_s+piece2_s)*(energy_2[i]);
    reso_fwhm_err_2[i]  = sqrt(piece1_f+piece2_f)*(energy_2[i]);

    gr_reso_2->SetPoint(i,energy_2[i],reso_sigma_2[i]);
    gr_reso_2->SetPointError(i,0,reso_sigma_err_2[i]);
    gr_reso_fwhm_2->SetPoint(i,energy_2[i],reso_fwhm_2[i]);
    gr_reso_fwhm_2->SetPointError(i,0,reso_fwhm_err_2[i]);
    
  }
   
    
 
  c1->cd();
  
  //grafico risoluzione
  //preparazine tela
  float energy_min = 0;
  if(energy_1[0]<energy_2[0]){energy_min=energy_1[0];}
  else{energy_min=energy_2[0];}

  float energy_max = 0;
  if(energy_1[size1-1]>energy_2[size2-1]){energy_max=energy_1[size1-1];}
  else{energy_max=energy_2[size2-1];}
  
  float xmin_reso;
  if(CD_number1==188 && CD_number2==204){ xmin_reso= energy_min - 1;}
  if(CD_number1==188 && CD_number2==222){ xmin_reso= energy_min - 2;}
  float xmax_reso;
  if(CD_number1==188 && CD_number2==204){ xmax_reso= energy_max + 1;}
  if(CD_number1==188 && CD_number2==222){ xmax_reso= energy_max + 2;}
  float ymax_reso = 0;
  float ymax_fwhm = 0;
  if(CD_number1==188 && CD_number2==204){ ymax_reso = 2.3; ymax_fwhm = 44; }
  if(CD_number1==188 && CD_number2==222){ ymax_reso = 2.3; ymax_fwhm = 44; }

  gPad->SetGrid(1, 1);
  gStyle->SetGridColor(kGray+1);   // grigio chiaro
  gStyle->SetGridStyle(3);         // tratteggiata (default=3, continua=1)
  gStyle->SetGridWidth(1);
 
  TLegend* legend = new TLegend(0.35,0.83,0.95,0.9);
  legend->SetTextSize(0.03);
  legend->SetMargin(0.1);
  legend->SetBorderSize(0);
  legend->SetFillStyle(0);
  TH2D* h2_axes = new TH2D("axes_reso", "", 10, xmin_reso, xmax_reso, 10, 0., ymax_reso);
  gStyle->SetOptStat(0);
  h2_axes->GetXaxis()->SetLabelSize(0.046);
  h2_axes->GetXaxis()->SetTitleSize(0.048);
  h2_axes->GetYaxis()->SetLabelSize(0.046);
  h2_axes->GetYaxis()->SetTitleSize(0.048);
  h2_axes->GetYaxis()->SetTitleOffset(1.5);
  h2_axes->GetXaxis()->SetTitleOffset(1.4);
  h2_axes->GetXaxis()->SetTitle("Electron kinetic energy (eV)");
  h2_axes->GetYaxis()->SetTitle("TES Gaussian energy resolution (eV)");
  h2_axes->Draw();

  gStyle->SetTitleFontSize(0.07);
  gr_reso_1->SetMarkerSize(3);
  gr_reso_1->SetLineWidth(4);
  gr_reso_1->SetMarkerStyle(20);
  gr_reso_1->SetMarkerColor(46);
  gr_reso_1->SetLineColor(46);
  gr_reso_2->SetMarkerSize(3);
  gr_reso_2->SetLineWidth(4);
  if(CD_number2==204){
    gr_reso_2->SetMarkerStyle(21); gr_reso_2->SetMarkerColor(38); gr_reso_2->SetLineColor(38);}
  if(CD_number2==222){
    gr_reso_2->SetMarkerStyle(22); gr_reso_2->SetMarkerColor(30); gr_reso_2->SetLineColor(30);}
  
  
  gr_reso_1->Draw("PESame");
  gr_reso_2->Draw("PESame");
  if(CD_number1==188 && CD_number2==204){
 legend->AddEntry(gr_reso_1, "TES: (100 x 100) #mum^{2}, CNTs: (3 x 3) mm^{2}", "pe");
  legend->AddEntry(gr_reso_2, "TES: (60 x 60) #mum^{2}, CNTs: (1 x 1) mm^{2}", "pe");
  }
  if(CD_number1==188 && CD_number2==222){
  legend->AddEntry(gr_reso_1, "TES side: 100um, CNT side: 3 mm", "pe");
  legend->AddEntry(gr_reso_2, "TES side: 100um, CNT side: 1 mm", "pe");
  }
  legend->Draw("same");

  if(CD_number1==188 && CD_number2==204){ c1->SaveAs("plots/articolo2/reso_cfr188_204.png"); }
  if(CD_number1==188 && CD_number2==222){ c1->SaveAs("plots/articolo2/reso_cfr188_222.png"); }
  c1->Clear();
  legend->Clear();

  TH2D* h2_axes_fwhm = new TH2D("axes_fwhm", "", 10, xmin_reso, xmax_reso, 10, 0., ymax_fwhm);
  gStyle->SetOptStat(0);
  h2_axes_fwhm->GetXaxis()->SetLabelSize(0.046);
  h2_axes_fwhm->GetXaxis()->SetTitleSize(0.048);
  h2_axes_fwhm->GetYaxis()->SetLabelSize(0.046);
  h2_axes_fwhm->GetYaxis()->SetTitleSize(0.048);
  h2_axes_fwhm->GetYaxis()->SetTitleOffset(1.5);
  h2_axes_fwhm->GetXaxis()->SetTitleOffset(1.4);
  h2_axes_fwhm->GetXaxis()->SetTitle("Electron kinetic energy (eV)");
  h2_axes_fwhm->GetYaxis()->SetTitle("TES FWHM energy resolution (eV)");
  h2_axes_fwhm->Draw();
  
  gStyle->SetTitleFontSize(0.07);
  gr_reso_fwhm_1->SetMarkerSize(3);
  gr_reso_fwhm_1->SetLineWidth(4);
  gr_reso_fwhm_1->SetMarkerStyle(20);
  gr_reso_fwhm_1->SetMarkerColor(46);
  gr_reso_fwhm_1->SetLineColor(46);
  gr_reso_fwhm_2->SetMarkerSize(3);
  gr_reso_fwhm_2->SetLineWidth(4);
  gr_reso_fwhm_2->SetMarkerStyle(20);
  gr_reso_fwhm_2->SetMarkerColor(38);
  gr_reso_fwhm_2->SetLineColor(38);
  
  gr_reso_fwhm_1->Draw("PESame");
  gr_reso_fwhm_2->Draw("PESame");
  if(CD_number1==188 && CD_number2==204){
  legend->AddEntry(gr_reso_fwhm_1, "TES: (100 x 100) #mum^{2}, CNTs: (3 x 3) mm^{2}", "pe");
  legend->AddEntry(gr_reso_fwhm_2, "TES: (60 x 60) #mum^{2}, CNTs: (1 x 1) mm^{2}", "pe");
  }
  if(CD_number1==188 && CD_number2==222){
  legend->AddEntry(gr_reso_1, "TES side: 100um, CNT side: 3 mm", "pe");
  legend->AddEntry(gr_reso_2, "TES side: 100um, CNT side: 1 mm", "pe");
  }
  legend->Draw("same");

  if(CD_number1==188 && CD_number2==204){ c1->SaveAs("plots/articolo2/reso_cfr188_204_fwhm.png"); }
  if(CD_number1==188 && CD_number2==222){ c1->SaveAs("plots/articolo2/reso_cfr188_222_fwhm.png"); }
 
  c1->Clear();
 
  
  return 0;

}


//funzione di controllo sui parametri iniziali
void Input_Param_Ctrls(int argc, char* argv[]){
  //ctrl sull'input inserito
 if (argc!=3){
    std::cout << "USAGE: ./drawResolutionCFR [CD_number 1] [CD_number 2]" << std::endl; 
    std::cout << "EXAMPLE: ./drawResolutionCFR 188 204" << std::endl;
    std::cout << "[Per ora settato su 188 204 oppure 188 222]" << std::endl;
    
    exit(1);
  }
}

