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
  std::string f1 = "data/params_CD188conteggiamp_10kHz.txt";
  std::string f2 = "data/params_CD188conteggiamp_100kHz.txt";
  std::string f3 = "data/params_CD188conteggiamp.txt";

  std::vector<float> V1, mu1, muerr1, sR1, sRerr1, sL1, sLerr1;
  std::vector<float> V2, mu2, muerr2, sR2, sRerr2, sL2, sLerr2;
  std::vector<float> V3, mu3, muerr3, sR3, sRerr3, sL3, sLerr3;
  std::vector<float> energy;

  if(!ReadParams(f1, V1, mu1, muerr1, sR1, sRerr1, sL1, sLerr1))
    std::cout << "File 1 non trovato\n" << std::endl;
  if(!ReadParams(f2, V2, mu2, muerr2, sR2, sRerr2, sL2, sLerr2))
    std::cout << "File 2 non trovato\n" << std::endl;
  if(!ReadParams(f3, V3, mu3, muerr3, sR3, sRerr3, sL3, sLerr3))
    std::cout << "File 3 non trovato\n" << std::endl;

  float phi_tes = GetPhiTes();

  std::vector<float> res1, res_err1;
  std::vector<float> res2, res_err2;
  std::vector<float> res3, res_err3;

  // Calcola le risoluzioni per ogni dataset
  for(size_t i=0; i<V1.size(); i++){
    //mi serviva di riempire il vettore di energie
    energy.push_back(V1[i] - phi_tes);

    float sigmaR    = sR1[i];
    float sigmaRerr = sRerr1[i];
    float mu        = mu1[i];
    float muerr     = muerr1[i];
    float factor    = (V1[i] - phi_tes);

    float reso = sigmaR/mu * factor;
    float reso_err = std::sqrt( sigmaRerr*sigmaRerr/(mu*mu)
				+ sigmaR*sigmaR*muerr*muerr/(mu*mu*mu*mu) ) * factor;

    res1.push_back(reso);
    res_err1.push_back(reso_err);
  }

  for(size_t i=0;i<V2.size();i++){
    float sigmaR    = sR2[i];
    float sigmaRerr = sRerr2[i];
    float mu        = mu2[i];
    float muerr     = muerr2[i];
    float factor    = (V2[i] - phi_tes);

    float reso = sigmaR/mu * factor;
    float reso_err = std::sqrt( sigmaRerr*sigmaRerr/(mu*mu)
                                + sigmaR*sigmaR*muerr*muerr/(mu*mu*mu*mu) ) * factor;

    res2.push_back(reso);
    res_err2.push_back(reso_err);
  }

  for(size_t i=0;i<V3.size();i++){
    float sigmaR    = sR3[i];
    float sigmaRerr = sRerr3[i];
    float mu        = mu3[i];
    float muerr     = muerr3[i];
    float factor    = (V3[i] - phi_tes);

    float reso = sigmaR/mu * factor;
    float reso_err = std::sqrt( sigmaRerr*sigmaRerr/(mu*mu)
                                + sigmaR*sigmaR*muerr*muerr/(mu*mu*mu*mu) ) * factor;

    res3.push_back(reso);
    res_err3.push_back(reso_err);
  }

  // Creazione dei grafici senza errori sulle x
  TGraphErrors* g1 = new TGraphErrors(energy.size(), energy.data(), res1.data(), nullptr, res_err1.data());
  TGraphErrors* g2 = new TGraphErrors(energy.size(), energy.data(), res2.data(), nullptr, res_err2.data());
  TGraphErrors* g3 = new TGraphErrors(energy.size(), energy.data(), res3.data(), nullptr, res_err3.data());

  // impostazioni grafiche
  g1->SetMarkerStyle(20);   g2->SetMarkerStyle(21);   g3->SetMarkerStyle(22);  
  g1->SetMarkerColor(46);   g2->SetMarkerColor(91);   g3->SetMarkerColor(38); 
  g1->SetLineColor(46);     g2->SetLineColor(91);     g3->SetLineColor(38); 
  g1->SetMarkerSize(2);     g2->SetMarkerSize(2);     g3->SetMarkerSize(2);
  g1->SetLineWidth(3);      g2->SetLineWidth(3);      g3->SetLineWidth(3);
  
  // Creazione della canvas
  TCanvas* c = new TCanvas("c","Resolution",800,600);
  TMultiGraph* mg = new TMultiGraph();
  c->SetLeftMargin(0.135);
  c->SetBottomMargin(0.135);
  
  // Aggiungi grafici al multi-grafico
  mg->Add(g1, "P");
  mg->Add(g2, "P");
  mg->Add(g3, "P");

  // Aggiungi legenda
  auto leg = new TLegend(0.20, 0.75, 0.45, 0.88);
  leg->AddEntry(g1, "Filtered 10 kHz", "P");
  leg->AddEntry(g2, "Filtered 100 kHz", "P");
  leg->AddEntry(g3, "Unfiltered", "P");
  leg->SetBorderSize(0);
  leg->SetFillStyle(0); 
  leg->SetTextSize(0.045);  

  mg->Draw("A");
  leg->Draw("same");

  // Personalizzazione degli assi
  mg->GetXaxis()->SetTitle("Energy (V)");
  mg->GetYaxis()->SetTitle("Resolution");
  mg->GetXaxis()->SetTitleSize(0.05);  
  mg->GetYaxis()->SetTitleSize(0.05); 
  mg->GetXaxis()->SetLabelSize(0.05); 
  mg->GetYaxis()->SetLabelSize(0.05); 

  c->SaveAs("plots/CD188/reso_CFR_filters.pdf");

  //Settings per altro grafico
  // Calcola i rapporti delle risoluzioni e gli errori
  std::vector<float> res_ratio_100kHz, res_err_ratio_100kHz;
  std::vector<float> res_ratio_10kHz, res_err_ratio_10kHz;

  for(size_t i = 0; i < V2.size(); i++) {
    float reso_10kHz          = res1[i];
    float reso_10kHz_err      = res_err1[i];
    float reso_100kHz         = res2[i];
    float reso_unfiltered     = res3[i];
    float reso_100kHz_err     = res_err2[i];
    float reso_unfiltered_err = res_err3[i];

    // Calcolo del rapporto e dell'errore per reso_100kHz / reso_unfiltered
    // incertezza in quadratura di quella percentuale
    float ratio_100kHz     = reso_100kHz / reso_unfiltered;
    float ratio_100kHz_err = ratio_100kHz * std::sqrt(std::pow(reso_100kHz_err / reso_100kHz, 2)
						      + std::pow(reso_unfiltered_err / reso_unfiltered, 2));
    //float ratio_10kHz_err = std::sqrt(std::pow((1 / reso_unfiltered) * reso_10kHz_err, 2)
    //				      + std::pow((-reso_10kHz / std::pow(reso_unfiltered, 2)) * reso_unfiltered_err, 2));
    
    res_ratio_100kHz.push_back(ratio_100kHz);
    res_err_ratio_100kHz.push_back(ratio_100kHz_err);

    // Calcolo del rapporto e dell'errore per reso_10kHz / reso_unfiltered
    float ratio_10kHz = reso_10kHz / reso_unfiltered;
    float ratio_10kHz_err = ratio_10kHz * std::sqrt(std::pow(reso_10kHz_err / reso_10kHz, 2)
						    + std::pow(reso_unfiltered_err / reso_unfiltered, 2));
    //float ratio_10kHz_err = std::sqrt(std::pow((1 / reso_unfiltered) * reso_10kHz_err, 2)
    //				      + std::pow((-reso_10kHz  std::pow(reso_unfiltered, 2)) * reso_unfiltered_err, 2));

    res_ratio_10kHz.push_back(ratio_10kHz);
    res_err_ratio_10kHz.push_back(ratio_10kHz_err);
  }

  // Creazione dei grafici per i rapporti
  TGraphErrors* g_ratio_100kHz = new TGraphErrors(V2.size(), energy.data(), res_ratio_100kHz.data(), nullptr, res_err_ratio_100kHz.data());
  TGraphErrors* g_ratio_10kHz = new TGraphErrors(V1.size(), energy.data(), res_ratio_10kHz.data(), nullptr, res_err_ratio_10kHz.data());

  // Impostazione dei marker per i grafici dei rapporti
  g_ratio_100kHz->SetMarkerStyle(20);   g_ratio_10kHz->SetMarkerStyle(21);
  g_ratio_100kHz->SetMarkerSize(2);     g_ratio_10kHz->SetMarkerSize(2);
  g_ratio_100kHz->SetMarkerColor(91);   g_ratio_10kHz->SetMarkerColor(46); 
  g_ratio_100kHz->SetLineWidth(3);      g_ratio_10kHz->SetLineWidth(3);
  g_ratio_100kHz->SetLineColor(91);     g_ratio_10kHz->SetLineColor(46); 

  // Creazione della canvas per i rapporti
  TCanvas* c2 = new TCanvas("c2", "Resolution Ratios", 800, 600);
  TMultiGraph* mg2 = new TMultiGraph();
  c2->SetLeftMargin(0.135);
  c2->SetBottomMargin(0.135);

  // Aggiungi i grafici al multi-grafico
  mg2->Add(g_ratio_10kHz, "P");
  mg2->Add(g_ratio_100kHz, "P");

  // Aggiungi legenda
  auto leg2 = new TLegend(0.15, 0.78, 0.45, 0.88);
  leg2->AddEntry(g_ratio_10kHz, "10 kHz / Unfiltered", "P");
  leg2->AddEntry(g_ratio_100kHz, "100 kHz / Unfiltered", "P");
  leg2->SetBorderSize(0);
  leg2->SetFillStyle(0);
  leg2->SetTextSize(0.045);

  // Disegna i grafici
  mg2->Draw("A");
  leg2->Draw("same");

  // Opzioni degli assi
  mg2->GetXaxis()->SetTitle("Energy (V)");
  mg2->GetYaxis()->SetTitle("Resolution Ratio");
  mg2->GetXaxis()->SetTitleSize(0.05);
  mg2->GetYaxis()->SetTitleSize(0.05);
  mg2->GetXaxis()->SetLabelSize(0.05);
  mg2->GetYaxis()->SetLabelSize(0.05);
  mg2->GetYaxis()->SetRangeUser(-0.5,3);
  mg2->GetXaxis()->SetRangeUser(95.3-phi_tes,105.5-phi_tes);

  // Aggiungi una linea orizzontale tratteggiata su y = 1
  TLine* line = new TLine(95.3-phi_tes, 1,105.5-phi_tes, 1);
  line->SetLineStyle(2);  
  line->SetLineColor(kBlack);  
  line->SetLineWidth(2); 
  line->Draw("same");  

  c2->SaveAs("plots/CD188/reso_ratios.pdf");

  return 0;
}
