#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TMultiGraph.h"
#include "TAxis.h"
#include "TLegend.h"

#include "graphics.h"
#include "AndCommon.h"
#include "CD_details.h"

bool ReadParams(const std::string& filename,
                std::vector<float>& volts,
		std::vector<float>& energy,
                std::vector<float>& mu,
                std::vector<float>& mu_err,
                std::vector<float>& sigmaR,
                std::vector<float>& sigmaR_err,
                std::vector<float>& sigmaL,
                std::vector<float>& sigmaL_err,
		std::vector<float>& reso_gaus,
		std::vector<float>& reso_gaus_err,
		std::vector<float>& reso_fwhm,
		std::vector<float>& reso_fwhm_err){

  std::ifstream in(filename);
  if(!in.is_open()){
    return false; }

  float V, mu_, energy_, muerr_, sigR_, sigRerr_, sigL_, sigLerr_, reso_, reso_err_, fwhm_, fwhm_err_;
  std::string header;
  std::getline(in, header); // riga header

  while(in >> V >> energy_ >> mu_ >> muerr_ >> sigR_ >> sigRerr_ >> sigL_ >> sigLerr_ >> reso_ >> reso_err_ >> fwhm_ >> fwhm_err_) {
    volts.push_back(V);
    energy.push_back(energy_);
    mu.push_back(mu_);
    mu_err.push_back(muerr_);
    sigmaR.push_back(sigR_);
    sigmaR_err.push_back(sigRerr_);
    sigmaL.push_back(sigL_);
    sigmaL_err.push_back(sigLerr_);
    reso_gaus.push_back(reso_);
    reso_gaus_err.push_back(reso_err_);
    reso_fwhm.push_back(fwhm_);
    reso_fwhm_err.push_back(fwhm_err_);
  }

  return true;
}


int main() {
  //per dettagli sul programma vedere drawResolutionCFR_filters_c188.cpp che è analogo
  std::string f1 = "data/params_CD204B60_post_cond3_mokuamp.txt";
  std::string f2 = "data/params_CD204B60_post_cond3_mokuamp_10kHz.txt";
  std::string f3 = "data/params_CD204B60_post_cond3_mokuamp_10kHz_160kHz.txt";
  
  std::vector<float> V1, energy1, mu1, muerr1, sR1, sRerr1, sL1, sLerr1, res1, res_err1, fwhm1, fwhm_err1;
  std::vector<float> V2, energy2, mu2, muerr2, sR2, sRerr2, sL2, sLerr2, res2, res_err2, fwhm2, fwhm_err2;
  std::vector<float> V3, energy3, mu3, muerr3, sR3, sRerr3, sL3, sLerr3, res3, res_err3, fwhm3, fwhm_err3;

 if(!ReadParams(f1, V1, energy1, mu1, muerr1, sR1, sRerr1, sL1, sLerr1, res1, res_err1, fwhm1, fwhm_err1))
    std::cout << "File 1 non trovato\n" << std::endl;
  if(!ReadParams(f2, V2, energy2, mu2, muerr2, sR2, sRerr2, sL2, sLerr2, res2, res_err2, fwhm2, fwhm_err2))
    std::cout << "File 2 non trovato\n" << std::endl;
  if(!ReadParams(f3, V3, energy3, mu3, muerr3, sR3, sRerr3, sL3, sLerr3, res3, res_err3, fwhm3, fwhm_err3))
    std::cout << "File 3 non trovato\n" << std::endl;
  
  float phi_tes = GetPhiTes();

  //GRAFICI RISOLUZIONE SIGMA
  // Creazione dei grafici con marker personalizzati
  TGraphErrors* g1 = new TGraphErrors(energy1.size(), energy1.data(), res1.data(), nullptr, res_err1.data());
  TGraphErrors* g2 = new TGraphErrors(energy2.size(), energy2.data(), res2.data(), nullptr, res_err2.data());
  TGraphErrors* g3 = new TGraphErrors(energy3.size(), energy3.data(), res3.data(), nullptr, res_err3.data());
  
  // Impostazione dei marker 
  g1->SetMarkerStyle(20);   g2->SetMarkerStyle(21);   g3->SetMarkerStyle(21); 
  g1->SetMarkerSize(2);     g2->SetMarkerSize(2);     g3->SetMarkerSize(2);
  g1->SetMarkerColor(46);   g2->SetMarkerColor(91);   g3->SetMarkerColor(38);
  g1->SetLineWidth(3);      g2->SetLineWidth(3);      g3->SetLineWidth(3);
  g1->SetLineColor(46);     g2->SetLineColor(91);     g3->SetLineColor(38);

  // Creazione della canvas
  TCanvas* c = new TCanvas("c","Resolution",800,800);
  TMultiGraph* mg = new TMultiGraph();
  c->SetLeftMargin(0.135);
  c->SetBottomMargin(0.135);

  // Aggiungi grafici al multi-grafico
  mg->Add(g1, "P");
  mg->Add(g2, "P");
  mg->Add(g3, "P");

  // Aggiungi legenda
  auto leg = new TLegend(0.15, 0.77, 0.36, 0.89);
  leg->AddEntry(g1, "Unfiltered (100kHz online)", "P");
  leg->AddEntry(g2, "Filtered 10 kHz", "P");
  leg->AddEntry(g3, "Filtered 160 kHz + 10 kHz", "P");
  leg->SetBorderSize(0);
  leg->SetFillStyle(0); 
  leg->SetTextSize(0.040); 

  mg->Draw("A");
  leg->Draw("same");

  // Personalizzazione degli assi
  mg->GetXaxis()->SetTitle("Energy (eV)");
  mg->GetYaxis()->SetTitle("Gaussian energy resolution (eV)");
  mg->GetXaxis()->SetTitleSize(0.05);  
  mg->GetYaxis()->SetTitleSize(0.05);  
  mg->GetXaxis()->SetLabelSize(0.05); 
  mg->GetYaxis()->SetLabelSize(0.05);
  mg->GetYaxis()->SetRangeUser(0,1.6);

  c->SaveAs("plots/CD204/reso_CFR_filters.pdf");

  //GRAFICI RISOLUZIONE FWHM
  // Creazione dei grafici con marker personalizzati
  TGraphErrors* g1_fwhm = new TGraphErrors(energy1.size(), energy1.data(), fwhm1.data(), nullptr, fwhm_err1.data());
  TGraphErrors* g2_fwhm = new TGraphErrors(energy2.size(), energy2.data(), fwhm2.data(), nullptr, fwhm_err2.data());
  TGraphErrors* g3_fwhm = new TGraphErrors(energy3.size(), energy3.data(), fwhm3.data(), nullptr, fwhm_err3.data());

  // Impostazione dei marker 
  g1_fwhm->SetMarkerStyle(20);   g2_fwhm->SetMarkerStyle(21);   g3_fwhm->SetMarkerStyle(21); 
  g1_fwhm->SetMarkerSize(2);     g2_fwhm->SetMarkerSize(2);     g3_fwhm->SetMarkerSize(2);
  g1_fwhm->SetMarkerColor(46);   g2_fwhm->SetMarkerColor(91);   g3_fwhm->SetMarkerColor(38);
  g1_fwhm->SetLineWidth(3);      g2_fwhm->SetLineWidth(3);      g3_fwhm->SetLineWidth(3);
  g1_fwhm->SetLineColor(46);     g2_fwhm->SetLineColor(91);     g3_fwhm->SetLineColor(38);

  // Creazione della canvas
  TCanvas* c_fwhm = new TCanvas("c_fwhm","Resolution",800,800);
  TMultiGraph* mg_fwhm = new TMultiGraph();
  c_fwhm->SetLeftMargin(0.135);
  c_fwhm->SetBottomMargin(0.135);

  // Aggiungi grafici al multi-grafico
  mg_fwhm->Add(g1_fwhm, "P");
  mg_fwhm->Add(g2_fwhm, "P");
  mg_fwhm->Add(g3_fwhm, "P");

  // Aggiungi legenda
  auto leg_fwhm = new TLegend(0.15, 0.77, 0.36, 0.89);
  leg_fwhm->AddEntry(g1_fwhm, "Unfiltered (100kHz online)", "P");
  leg_fwhm->AddEntry(g2_fwhm, "Filtered 10 kHz", "P");
  leg_fwhm->AddEntry(g3_fwhm, "Filtered 160 kHz + 10 kHz", "P");
  leg_fwhm->SetBorderSize(0);
  leg_fwhm->SetFillStyle(0); 
  leg_fwhm->SetTextSize(0.040); 

  mg_fwhm->Draw("A");
  leg_fwhm->Draw("same");

  // Personalizzazione degli assi
  mg_fwhm->GetXaxis()->SetTitle("Energy (eV)");
  mg_fwhm->GetYaxis()->SetTitle("fwhm energy resolution (eV)");
  mg_fwhm->GetXaxis()->SetTitleSize(0.05);  
  mg_fwhm->GetYaxis()->SetTitleSize(0.05);  
  mg_fwhm->GetXaxis()->SetLabelSize(0.05); 
  mg_fwhm->GetYaxis()->SetLabelSize(0.05);
  mg_fwhm->GetYaxis()->SetRangeUser(0,5);
  mg_fwhm->GetYaxis()->SetTitleOffset(1.3);

  c_fwhm->SaveAs("plots/CD204/resofwhm_CFR_filters.pdf");

  return 0;
}
