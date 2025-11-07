#include <iostream>
#include <fstream>
#include <cmath>
#include "TH1F.h"
#include "TStyle.h"
#include "TCanvas.h"
#include <TRandom3.h>

int main() {
  // Creiamo un generatore di numeri casuali
  TRandom3 random(0);
  TCanvas* c1  = new TCanvas("c1","",1000,1000);
  // Numero di coppie da generare
  int n = 10000;
  int nbins = 50;
  TH1F* sen_el = new TH1F("sen_el","",nbins,-2,2);
  TH1F* elev = new TH1F("elev","",nbins,-10*M_PI/180.,190*M_PI/180.);
  TH1F* azim = new TH1F("azim","",nbins,-10*M_PI/180.,370*M_PI/180.);
  
  // Generiamo le coppie di angoli
  for (int i = 0; i < n; ++i) {
    // Generiamo l'angolo azimutale (uniformemente distribuito tra 0 e 2*pi)
    double azimuth = random.Uniform(0, 2 * M_PI);
    
    // Generiamo l'angolo di elevazione (uniformemente distribuito in coseno tra 0 e 1)
    double elevazioneCos = random.Uniform(0, 1);
    double elevazione = acos(elevazioneCos);
    elev->Fill(elevazione);
    azim->Fill(azimuth);
    sen_el->Fill(cos(elevazione));
  }

 
  
  gStyle->SetOptStat(0);
  sen_el->SetLineWidth(3);
  sen_el->GetYaxis()->SetTitle("Counts");
  sen_el->GetXaxis()->SetTitle("sin(theta)");
  sen_el->SetTitle("Check of uniformity -> sen(theta)");
  sen_el->Draw();
  c1->SaveAs("sen_el.png");
  c1->Clear();

  elev->SetLineWidth(3);
  elev->GetYaxis()->SetTitle("Counts");
  elev->GetXaxis()->SetTitle("elev");
  elev->SetTitle("Check of uniformity -> elev");
  elev->Draw();
  c1->SaveAs("elev.png");
  c1->Clear();

  azim->SetLineWidth(3);
  azim->GetYaxis()->SetTitle("Counts");
  azim->GetXaxis()->SetTitle("azim");
  azim->SetTitle("Check of uniformity -> azim");
  azim->Draw();
  c1->SaveAs("azim.png");
  c1->Clear();
  
}
