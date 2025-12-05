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

// Legge un file e riempie i vettori
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
  std::string f1 = "data/params_CD188conteggiamp_10kHz.txt";
  std::string f2 = "data/params_CD188conteggiamp_100kHz.txt";
  std::string f3 = "data/params_CD188conteggiamp_paper.txt";
  std::string f4 = "data/params_CD188conteggiamp_10kHz_100kHz.txt";
  std::string f5 = "data/params_CD188conteggiamp_10kHz_160kHz.txt";
  std::string f6 = "data/params_CD188conteggiamp_10kHz_100kHz_160kHz.txt";
  
  std::vector<float> V1, energy1, mu1, muerr1, sR1, sRerr1, sL1, sLerr1, res1, res_err1, fwhm1, fwhm_err1;
  std::vector<float> V2, energy2, mu2, muerr2, sR2, sRerr2, sL2, sLerr2, res2, res_err2, fwhm2, fwhm_err2;
  std::vector<float> V3, energy3, mu3, muerr3, sR3, sRerr3, sL3, sLerr3, res3, res_err3, fwhm3, fwhm_err3;
  std::vector<float> V4, energy4, mu4, muerr4, sR4, sRerr4, sL4, sLerr4, res4, res_err4, fwhm4, fwhm_err4;
  std::vector<float> V5, energy5, mu5, muerr5, sR5, sRerr5, sL5, sLerr5, res5, res_err5, fwhm5, fwhm_err5;
  std::vector<float> V6, energy6, mu6, muerr6, sR6, sRerr6, sL6, sLerr6, res6, res_err6, fwhm6, fwhm_err6;

  if(!ReadParams(f1, V1, energy1, mu1, muerr1, sR1, sRerr1, sL1, sLerr1, res1, res_err1, fwhm1, fwhm_err1))
    std::cout << "File 1 non trovato\n" << std::endl;
  if(!ReadParams(f2, V2, energy2, mu2, muerr2, sR2, sRerr2, sL2, sLerr2, res2, res_err2, fwhm2, fwhm_err2))
    std::cout << "File 2 non trovato\n" << std::endl;
  if(!ReadParams(f3, V3, energy3, mu3, muerr3, sR3, sRerr3, sL3, sLerr3, res3, res_err3, fwhm3, fwhm_err3))
    std::cout << "File 3 non trovato\n" << std::endl;
  if(!ReadParams(f4, V4, energy4, mu4, muerr4, sR4, sRerr4, sL4, sLerr4, res4, res_err4, fwhm4, fwhm_err4))
    std::cout << "File 4 non trovato\n" << std::endl;
  if(!ReadParams(f5, V5, energy5,  mu5, muerr5, sR5, sRerr5, sL5, sLerr5, res5, res_err5, fwhm5, fwhm_err5))
    std::cout << "File 5 non trovato\n" << std::endl;
  if(!ReadParams(f6, V6, energy6, mu6, muerr6, sR6, sRerr6, sL6, sLerr6, res6, res_err6, fwhm6, fwhm_err6))
    std::cout << "File 6 non trovato\n" << std::endl;

  float phi_tes = GetPhiTes();

 
  //GRAFICI RISOLUZIONE SIGMA
  // Creazione dei grafici senza errori sulle x
  TGraphErrors* g1 = new TGraphErrors(energy1.size(), energy1.data(), res1.data(), nullptr, res_err1.data());
  TGraphErrors* g2 = new TGraphErrors(energy2.size(), energy2.data(), res2.data(), nullptr, res_err2.data());
  TGraphErrors* g3 = new TGraphErrors(energy3.size(), energy3.data(), res3.data(), nullptr, res_err3.data());
  TGraphErrors* g4 = new TGraphErrors(energy4.size(), energy4.data(), res4.data(), nullptr, res_err4.data());
  TGraphErrors* g5 = new TGraphErrors(energy5.size(), energy5.data(), res5.data(), nullptr, res_err5.data());
  TGraphErrors* g6 = new TGraphErrors(energy6.size(), energy6.data(), res6.data(), nullptr, res_err6.data());

  // impostazioni grafiche
  g1->SetMarkerStyle(71);   g2->SetMarkerStyle(72);   g3->SetMarkerStyle(73);  
  g1->SetMarkerColor(46);   g2->SetMarkerColor(91);   g3->SetMarkerColor(38); 
  g1->SetLineColor(46);     g2->SetLineColor(91);     g3->SetLineColor(38); 
  g1->SetMarkerSize(2);     g2->SetMarkerSize(2);     g3->SetMarkerSize(2);
  g1->SetLineWidth(3);      g2->SetLineWidth(3);      g3->SetLineWidth(3);

  g4->SetMarkerStyle(71);   g5->SetMarkerStyle(71);   g6->SetMarkerStyle(71);  
  g4->SetMarkerColor(30);   g5->SetMarkerColor(94);   g6->SetMarkerColor(63);   
  g4->SetLineColor(30);     g5->SetLineColor(94);     g6->SetLineColor(63);
  g4->SetMarkerSize(2);     g5->SetMarkerSize(2);     g6->SetMarkerSize(2); 
  g4->SetLineWidth(3);      g5->SetLineWidth(3);      g6->SetLineWidth(3);      
  
  // Creazione della canvas
  TCanvas* c = new TCanvas("c","Resolution",900,900);
  TMultiGraph* mg = new TMultiGraph();
  c->SetLeftMargin(0.135);
  c->SetTopMargin(0.300);
  c->SetBottomMargin(0.135);
  
  // Aggiungi grafici al multi-grafico
  mg->Add(g1, "P");
  mg->Add(g2, "P");
  mg->Add(g3, "P");
  mg->Add(g4, "P");
  mg->Add(g5, "P");
  mg->Add(g6, "P");

  // Aggiungi legenda
  auto leg = new TLegend(0.1, 0.71, 0.4, 0.96);
  leg->AddEntry(g3, "Unfiltered", "P");
  leg->AddEntry(g1, "Filtered 10 kHz", "P");
  leg->AddEntry(g2, "Filtered 100 kHz", "P");
  leg->AddEntry(g4, "Filtered 100 kHz + 10 kHz", "P");
  leg->AddEntry(g5, "Filtered 160 kHz + 10 kHz", "P");
  leg->AddEntry(g6, "Filtered 160 kHz + 100kHz + 10 kHz", "P");
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
  //mg->GetYaxis()->SetRangeUser(-0.05,4); 

  c->SaveAs("plots/CD188/reso_CFR_filters.pdf");

  //GRAFICI RISOLUZIONE FWHM
  // Creazione dei grafici senza errori sulle x
  TGraphErrors* g1_fwhm = new TGraphErrors(energy1.size(), energy1.data(), fwhm1.data(), nullptr, fwhm_err1.data());
  TGraphErrors* g2_fwhm = new TGraphErrors(energy2.size(), energy2.data(), fwhm2.data(), nullptr, fwhm_err2.data());
  TGraphErrors* g3_fwhm = new TGraphErrors(energy3.size(), energy3.data(), fwhm3.data(), nullptr, fwhm_err3.data());
  TGraphErrors* g4_fwhm = new TGraphErrors(energy4.size(), energy4.data(), fwhm4.data(), nullptr, fwhm_err4.data());
  TGraphErrors* g5_fwhm = new TGraphErrors(energy5.size(), energy5.data(), fwhm5.data(), nullptr, fwhm_err5.data());
  TGraphErrors* g6_fwhm = new TGraphErrors(energy6.size(), energy6.data(), fwhm6.data(), nullptr, fwhm_err6.data());

  // impostazioni grafiche
  g1_fwhm->SetMarkerStyle(71);   g2_fwhm->SetMarkerStyle(21);   g3_fwhm->SetMarkerStyle(22);  
  g1_fwhm->SetMarkerColor(46);   g2_fwhm->SetMarkerColor(91);   g3_fwhm->SetMarkerColor(38); 
  g1_fwhm->SetLineColor(46);     g2_fwhm->SetLineColor(91);     g3_fwhm->SetLineColor(38); 
  g1_fwhm->SetMarkerSize(2);     g2_fwhm->SetMarkerSize(2);     g3_fwhm->SetMarkerSize(2);
  g1_fwhm->SetLineWidth(3);      g2_fwhm->SetLineWidth(3);      g3_fwhm->SetLineWidth(3);

  g4_fwhm->SetMarkerStyle(71);   g5_fwhm->SetMarkerStyle(71);   g6_fwhm->SetMarkerStyle(71);  
  g4_fwhm->SetMarkerColor(30);   g5_fwhm->SetMarkerColor(94);   g6_fwhm->SetMarkerColor(63);   
  g4_fwhm->SetLineColor(30);     g5_fwhm->SetLineColor(94);     g6_fwhm->SetLineColor(63);
  g4_fwhm->SetMarkerSize(2);     g5_fwhm->SetMarkerSize(2);     g6_fwhm->SetMarkerSize(2); 
  g4_fwhm->SetLineWidth(3);      g5_fwhm->SetLineWidth(3);      g6_fwhm->SetLineWidth(3);      
  
  // Creazione della canvas
  TCanvas* c_fwhm = new TCanvas("c_fwhm","Resolution",900,900);
  TMultiGraph* mg_fwhm = new TMultiGraph();
  c_fwhm->SetLeftMargin(0.135);
  c_fwhm->SetTopMargin(0.300);
  c_fwhm->SetBottomMargin(0.135);
  
  // Aggiungi grafici al multi-grafico
  mg_fwhm->Add(g1_fwhm, "P");
  mg_fwhm->Add(g2_fwhm, "P");
  mg_fwhm->Add(g3_fwhm, "P");
  mg_fwhm->Add(g4_fwhm, "P");
  mg_fwhm->Add(g5_fwhm, "P");
  mg_fwhm->Add(g6_fwhm, "P");

  // Aggiungi legenda
  auto leg_fwhm = new TLegend(0.1, 0.71, 0.4, 0.96);
  leg_fwhm->AddEntry(g3_fwhm, "Unfiltered", "P");
  leg_fwhm->AddEntry(g1_fwhm, "Filtered 10 kHz", "P");
  leg_fwhm->AddEntry(g2_fwhm, "Filtered 100 kHz", "P");
  leg_fwhm->AddEntry(g4_fwhm, "Filtered 100 kHz + 10 kHz", "P");
  leg_fwhm->AddEntry(g5_fwhm, "Filtered 160 kHz + 10 kHz", "P");
  leg_fwhm->AddEntry(g6_fwhm, "Filtered 160 kHz + 100kHz + 10 kHz", "P");
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
  mg_fwhm->GetYaxis()->SetTitleOffset(1.3);
  mg_fwhm->GetYaxis()->SetRangeUser(0,110); 

  c_fwhm->SaveAs("plots/CD188/resofwhm_CFR_filters.pdf");

  return 0;
}
