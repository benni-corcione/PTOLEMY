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
                std::vector<float>& mu,
                std::vector<float>& mu_err,
                std::vector<float>& sigmaR,
                std::vector<float>& sigmaR_err,
                std::vector<float>& sigmaL,
                std::vector<float>& sigmaL_err){
  std::ifstream in(filename);
  if(!in.is_open()){
    return false; }

  float V, mu_, muerr_, sigR_, sigRerr_, sigL_, sigLerr_;
  std::string header;
  std::getline(in, header); // riga header

  while(in >> V >> mu_ >> muerr_ >> sigR_ >> sigRerr_ >> sigL_ >> sigLerr_) {
    volts.push_back(V);
    mu.push_back(mu_);
    mu_err.push_back(muerr_);
    sigmaR.push_back(sigR_);
    sigmaR_err.push_back(sigRerr_);
    sigmaL.push_back(sigL_);
    sigmaL_err.push_back(sigLerr_);
  }

  return true;
}

int main() {
  //per dettagli sul programma vedere drawResolutionCFR_filters_c188.cpp che è analogo
  std::string f1 = "data/params_CD222E100_squidfilter_ER_031025_tr12amp.txt";
  std::string f2 = "data/params_CD222E100_squidfilter_ER_031025_tr12amp_100kHz.txt";
  std::string f3 = "data/params_CD222E100_squidfilter_ER_031025_tr12amp_10kHz.txt";
  std::string f4 = "data/params_CD222E100_squidfilter_ER_031025_tr12amp_10kHz_10kHz.txt";
  
  std::vector<float> V1, mu1, muerr1, sR1, sRerr1, sL1, sLerr1;
  std::vector<float> V2, mu2, muerr2, sR2, sRerr2, sL2, sLerr2;
  std::vector<float> V3, mu3, muerr3, sR3, sRerr3, sL3, sLerr3;
  std::vector<float> V4, mu4, muerr4, sR4, sRerr4, sL4, sLerr4;
  std::vector<float> energy;

  if(!ReadParams(f1, V1, mu1, muerr1, sR1, sRerr1, sL1, sLerr1))
    std::cout << "File 1 non trovato\n" << std::endl;
  if(!ReadParams(f2, V2, mu2, muerr2, sR2, sRerr2, sL2, sLerr2))
    std::cout << "File 2 non trovato\n" << std::endl;
  if(!ReadParams(f3, V3, mu3, muerr3, sR3, sRerr3, sL3, sLerr3))
    std::cout << "File 3 non trovato\n" << std::endl;
  if(!ReadParams(f4, V4, mu4, muerr4, sR4, sRerr4, sL4, sLerr4))
    std::cout << "File 4 non trovato\n" << std::endl;

  float phi_tes = GetPhiTes();

  std::vector<float> res1, res_err1, fwhm1, fwhm_err1;
  std::vector<float> res2, res_err2, fwhm2, fwhm_err2;
  std::vector<float> res3, res_err3, fwhm3, fwhm_err3;
  std::vector<float> res4, res_err4, fwhm4, fwhm_err4;
 
  // Calcola le risoluzioni per ogni dataset
  for(size_t i=0;i<V1.size();i++){
    energy.push_back(V1[i] - phi_tes);

    float sigmaR    = sR1[i];
    float sigmaRerr = sRerr1[i];
    float sigmaL    = sL1[i];
    float sigmaLerr = sLerr1[i];
    float mu        = mu1[i];
    float muerr     = muerr1[i];
    float factor    = (V1[i] - phi_tes);

    float reso = sigmaR/mu * factor;
    float reso_err = std::sqrt( sigmaRerr*sigmaRerr/(mu*mu)
                                + sigmaR*sigmaR*muerr*muerr/(mu*mu*mu*mu) ) * factor;

    float fwhm = (sigmaR + sigmaL) * sqrt(2*log(2));
    float fwhm_err = sqrt(2*log(2)) * sqrt(sigmaRerr*sigmaRerr+sigmaLerr*sigmaLerr);
    float reso_fwhm = (fwhm/mu)* factor;
    float piece1_f = fwhm_err*fwhm_err/(mu*mu);
    float piece2_f = fwhm*fwhm*muerr*muerr/(mu*mu*mu*mu);
    float reso_fwhm_err = sqrt(piece1_f+piece2_f) * factor;
    res1.push_back(reso);
    res_err1.push_back(reso_err);
    fwhm1.push_back(reso_fwhm);
    fwhm_err1.push_back(reso_fwhm_err);
  }

  for(size_t i=0;i<V2.size();i++){
    float sigmaR    = sR2[i];
    float sigmaRerr = sRerr2[i];
    float sigmaL    = sL2[i];
    float sigmaLerr = sLerr2[i];
    float mu        = mu2[i];
    float muerr     = muerr2[i];
    float factor    = (V2[i] - phi_tes);

    float reso = sigmaR/mu * factor;
    float reso_err = std::sqrt( sigmaRerr*sigmaRerr/(mu*mu)
                                + sigmaR*sigmaR*muerr*muerr/(mu*mu*mu*mu) ) * factor;

    float fwhm = (sigmaR + sigmaL) * sqrt(2*log(2));
    float fwhm_err = sqrt(2*log(2)) * sqrt(sigmaRerr*sigmaRerr+sigmaLerr*sigmaLerr);
    float reso_fwhm = (fwhm/mu)* factor;
    float piece1_f = fwhm_err*fwhm_err/(mu*mu);
    float piece2_f = fwhm*fwhm*muerr*muerr/(mu*mu*mu*mu);
    float reso_fwhm_err = sqrt(piece1_f+piece2_f) * factor;
    res2.push_back(reso);
    res_err2.push_back(reso_err);
    fwhm2.push_back(reso_fwhm);
    fwhm_err2.push_back(reso_fwhm_err);
    
  }

  for(size_t i=0;i<V3.size();i++){
    float sigmaL    = sL3[i];
    float sigmaLerr = sLerr3[i];
    float sigmaR    = sR3[i];
    float sigmaRerr = sRerr3[i];
    float mu        = mu3[i];
    float muerr     = muerr3[i];
    float factor    = (V3[i] - phi_tes);

    float reso = sigmaR/mu * factor;
    float reso_err = std::sqrt( sigmaRerr*sigmaRerr/(mu*mu)
                                + sigmaR*sigmaR*muerr*muerr/(mu*mu*mu*mu) ) * factor;

    float fwhm = (sigmaR + sigmaL) * sqrt(2*log(2));
    float fwhm_err = sqrt(2*log(2)) * sqrt(sigmaRerr*sigmaRerr+sigmaLerr*sigmaLerr);
    float reso_fwhm = (fwhm/mu)* factor;
    float piece1_f = fwhm_err*fwhm_err/(mu*mu);
    float piece2_f = fwhm*fwhm*muerr*muerr/(mu*mu*mu*mu);
    float reso_fwhm_err = sqrt(piece1_f+piece2_f) * factor;
    res3.push_back(reso);
    res_err3.push_back(reso_err);
    fwhm3.push_back(reso_fwhm);
    fwhm_err3.push_back(reso_fwhm_err);
  }

   for(size_t i=0;i<V4.size();i++){
    float sigmaL    = sL4[i];
    float sigmaLerr = sLerr4[i];
    float sigmaR    = sR4[i];
    float sigmaRerr = sRerr4[i];
    float mu        = mu4[i];
    float muerr     = muerr4[i];
    float factor    = (V4[i] - phi_tes);

    float reso = sigmaR/mu * factor;
    float reso_err = std::sqrt( sigmaRerr*sigmaRerr/(mu*mu)
                                + sigmaR*sigmaR*muerr*muerr/(mu*mu*mu*mu) ) * factor;

    float fwhm = (sigmaR + sigmaL) * sqrt(2*log(2));
    float fwhm_err = sqrt(2*log(2)) * sqrt(sigmaRerr*sigmaRerr+sigmaLerr*sigmaLerr);
    float reso_fwhm = (fwhm/mu)* factor;
    float piece1_f = fwhm_err*fwhm_err/(mu*mu);
    float piece2_f = fwhm*fwhm*muerr*muerr/(mu*mu*mu*mu);
    float reso_fwhm_err = sqrt(piece1_f+piece2_f) * factor;
    res4.push_back(reso);
    res_err4.push_back(reso_err);
    fwhm4.push_back(reso_fwhm);
    fwhm_err4.push_back(reso_fwhm_err);
  }

  //GRAFICI RISOLUZIONE SIGMA
  // Creazione dei grafici con marker personalizzati
  TGraphErrors* g1 = new TGraphErrors(energy.size(), energy.data(), res1.data(), nullptr, res_err1.data());
  TGraphErrors* g2 = new TGraphErrors(energy.size(), energy.data(), res2.data(), nullptr, res_err2.data());
  TGraphErrors* g3 = new TGraphErrors(energy.size(), energy.data(), res3.data(), nullptr, res_err3.data());
  TGraphErrors* g4 = new TGraphErrors(energy.size(), energy.data(), res4.data(), nullptr, res_err4.data());

  // Impostazione dei marker 
  g1->SetMarkerStyle(20);   g2->SetMarkerStyle(21);   g3->SetMarkerStyle(21); 
  g1->SetMarkerSize(2);     g2->SetMarkerSize(2);     g3->SetMarkerSize(2);
  g1->SetMarkerColor(46);   g2->SetMarkerColor(91);   g3->SetMarkerColor(38);
  g1->SetLineWidth(3);      g2->SetLineWidth(3);      g3->SetLineWidth(3);
  g1->SetLineColor(46);     g2->SetLineColor(91);     g3->SetLineColor(38);

  g4->SetMarkerStyle(22);
  g4->SetMarkerSize(2);
  g4->SetMarkerColor(30);
  g4->SetLineWidth(3);
  g4->SetLineColor(30);
  
  // Creazione della canvas
  TCanvas* c = new TCanvas("c","Resolution",800,800);
  TMultiGraph* mg = new TMultiGraph();
  c->SetLeftMargin(0.135);
  c->SetBottomMargin(0.135);

  // Aggiungi grafici al multi-grafico
  mg->Add(g1, "P");
  mg->Add(g2, "P");
  mg->Add(g3, "P");
  mg->Add(g4, "P");

  // Aggiungi legenda
  auto leg = new TLegend(0.15, 0.17, 0.36, 0.35);
  leg->AddEntry(g1, "Unfiltered (10 kHz online)", "P");
  leg->AddEntry(g2, "Filtered 100 kHz", "P");
  leg->AddEntry(g3, "Filtered 10 kHz", "P");
  leg->AddEntry(g4, "Filtered 10 kHz + 10 kHz", "P");
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
  mg->GetYaxis()->SetRangeUser(0, 2.2);

  c->SaveAs("plots/CD222/reso_CFR_filters.pdf");

  //GRAFICI RISOLUZIONE FWHM
  // Creazione dei grafici con marker personalizzati
  TGraphErrors* g1_fwhm = new TGraphErrors(energy.size(), energy.data(), fwhm1.data(), nullptr, fwhm_err1.data());
  TGraphErrors* g2_fwhm = new TGraphErrors(energy.size(), energy.data(), fwhm2.data(), nullptr, fwhm_err2.data());
  TGraphErrors* g3_fwhm = new TGraphErrors(energy.size(), energy.data(), fwhm3.data(), nullptr, fwhm_err3.data());
  TGraphErrors* g4_fwhm = new TGraphErrors(energy.size(), energy.data(), fwhm4.data(), nullptr, fwhm_err4.data());

  // Impostazione dei marker 
  g1_fwhm->SetMarkerStyle(20);   g2_fwhm->SetMarkerStyle(21);   g3_fwhm->SetMarkerStyle(21); 
  g1_fwhm->SetMarkerSize(2);     g2_fwhm->SetMarkerSize(2);     g3_fwhm->SetMarkerSize(2);
  g1_fwhm->SetMarkerColor(46);   g2_fwhm->SetMarkerColor(91);   g3_fwhm->SetMarkerColor(38);
  g1_fwhm->SetLineWidth(3);      g2_fwhm->SetLineWidth(3);      g3_fwhm->SetLineWidth(3);
  g1_fwhm->SetLineColor(46);     g2_fwhm->SetLineColor(91);     g3_fwhm->SetLineColor(38);

  g4_fwhm->SetMarkerStyle(22);
  g4_fwhm->SetMarkerSize(2);
  g4_fwhm->SetMarkerColor(30);
  g4_fwhm->SetLineWidth(3);
  g4_fwhm->SetLineColor(30);
  
  // Creazione della canvas
  TCanvas* c_fwhm = new TCanvas("c_fwhm","Resolution",800,800);
  TMultiGraph* mg_fwhm = new TMultiGraph();
  c_fwhm->SetLeftMargin(0.135);
  c_fwhm->SetBottomMargin(0.135);

  // Aggiungi grafici al multi-grafico
  mg_fwhm->Add(g1_fwhm, "P");
  mg_fwhm->Add(g2_fwhm, "P");
  mg_fwhm->Add(g3_fwhm, "P");
  mg_fwhm->Add(g4_fwhm, "P");

  // Aggiungi legenda
  auto leg_fwhm = new TLegend(0.15, 0.17, 0.36, 0.35);
  leg_fwhm->AddEntry(g1_fwhm, "Unfiltered (10 kHz online)", "P");
  leg_fwhm->AddEntry(g2_fwhm, "Filtered 100 kHz", "P");
  leg_fwhm->AddEntry(g3_fwhm, "Filtered 10 kHz", "P");
  leg_fwhm->AddEntry(g4_fwhm, "Filtered 10 kHz + 10 kHz", "P");
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
  mg_fwhm->GetYaxis()->SetRangeUser(0, 7.8);

  c_fwhm->SaveAs("plots/CD222/resofwhm_CFR_filters.pdf");

  return 0;
}
