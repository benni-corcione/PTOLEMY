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
#include "CD_details.h"
#include "common.h"

Double_t Crystal_Ball(Double_t *x, Double_t *par) {
  // par[0] = mean (mu)
  // par[1] = sigma
  // par[2] = alpha (>0)
  // par[3] = n (>0)
  // par[4] = norm (A)

  Double_t t = (x[0] - par[0]) / par[1];
  Double_t alpha = par[2];
  Double_t n = par[3];
  Double_t A = par[4];

  if (t > -alpha) {
    return A * TMath::Exp(-0.5 * t * t);
  } else {
    Double_t absAlpha = TMath::Abs(alpha);
    Double_t factor = TMath::Power(n / absAlpha, n) * TMath::Exp(-0.5 * absAlpha * absAlpha);
    Double_t tail = TMath::Power((n / absAlpha) - absAlpha - t, -n);
    return A * factor * tail;
  }
}

int main(void) {

  int CD_number = 204;
  
  const char* measure = "B60_post_cond3_moku";
  
  TStyle* style = setStyle();
  style->SetPadLeftMargin(0.17);
  style->SetPadRightMargin(0.04);
  style->SetPadTopMargin(0.04);
  style->SetPadBottomMargin(0.1);

  std::vector<int> colors = {1, 30}; 
  std::vector<int> volts = {103, 97};
 
  // carbon WF:
  float phi = 4.5;

  TH1D* h1_tesWF = new TH1D( "tes_WF", "", 100, 4., 5.);
  h1_tesWF->Fill(4.38); // these numbers from the measurements done by Alessandro
  h1_tesWF->Fill(4.34);
  h1_tesWF->Fill(4.44);
  h1_tesWF->Fill(4.37);
  h1_tesWF->Fill(4.25);
  h1_tesWF->Fill(4.41);
  h1_tesWF->Fill(4.37);
  h1_tesWF->Fill(4.29);
  h1_tesWF->Fill(4.56);

  float phi_tes = h1_tesWF->GetMean();
  std::cout << "Phi TES: " << phi_tes << " +/- " << h1_tesWF->GetMeanError() << " eV" << std::endl; 

  std::vector<TH1F*> histos;

  // for the fit plot of the paper
  float scale_factor = 1./0.621179;  //1/mu_max dei miei histo
  //float scale_factor = 1;
  float xmin = 0.55*scale_factor;
  float xmax = 0.67*scale_factor;
  int nbins  = 1400; //HO RADDOPPIATO RANGE DI DEFINIZIONE HISTO
  
  
 
  TCanvas* c1 = new TCanvas( "c1", "", 1000, 1000 );
  c1->cd();


  for( unsigned i=0; i<volts.size(); i++ ) {

    std::cout << std::endl << std::endl;
    std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
    std::cout << "V = " << volts[i] << std::endl << std::endl;

    //leggo tutti i tree per i voltaggi
    TFile* file = TFile::Open( Form("data/root/CD%d/%s/%dV/CD%d_%dV_tree.root", CD_number, measure, volts[i], CD_number, volts[i]), "read" );
    TTree* tree = (TTree*)file->Get("tree");

    //creo un histo per ogni tree
    std::string histname(Form("%d", volts[i]));
    
    TH1F* h1_amp = new TH1F(histname.c_str(), "", nbins, 0, 2.*scale_factor);
    
    float amp_val;
    tree->SetBranchAddress("amp", &amp_val);

    for(Long64_t k = 0; k < tree->GetEntries(); k++){
      tree->GetEntry(k);
      
      float new_amp = scale_factor * amp_val;   // riscalamento richiesto
      h1_amp->Fill(new_amp);
    }
    //fine chatgpt

    float fitmin_cb = 0.6*scale_factor, fitmax_cb = 0.65*scale_factor;
    if(volts[i]==97 ) fitmin_cb = 0.59*scale_factor ;
    if(volts[i]==103) fitmin_cb = 0.61*scale_factor ;
    
    TF1 *cb  = new TF1(Form("cb_%d", volts[i]), Crystal_Ball, fitmin_cb, fitmax_cb, 5);
    cb->SetParNames("#mu","sigma", "#alpha", "n", "A");
   
    cb->SetParLimits(0, 0.609*scale_factor, 0.72*scale_factor); // mu
    cb->SetParLimits(1, 0.001*scale_factor,  0.5*scale_factor); // sigma
    cb->SetParLimits(2,  0.01,  100); // alpha
    cb->SetParLimits(3,  0.01,  150); // n
    cb->SetParLimits(4,     1, 3000); // A
    
    cb->SetParameter(0, 0.62*scale_factor); // mu
    cb->SetParameter(1, 0.01*scale_factor); // sigma_L
    cb->SetParameter(2, 0.04); // alpha
    cb->SetParameter(3,  3 ); // n
    cb->SetParameter(4, (double)(h1_amp->Integral()) ); // A
    
    if(volts[i]==97 ){cb->SetParameter(0, 0.611*scale_factor);
      cb->SetParameter(1, 0.035*scale_factor);
      cb->SetParLimits(0, 0.609*scale_factor, 0.613*scale_factor);
      cb->SetParLimits(4, 270, 290);}
    
    if(volts[i]==103){cb->SetParameter(0, 0.62*scale_factor);
      cb->SetParameter(1, 0.035*scale_factor);
      cb->SetParameter(3, 0.3 ); 
      cb->SetParLimits(0, 0.618*scale_factor, 0.625*scale_factor);
      cb->SetParLimits(2, 0, 2);}
    
    
    
    cb->SetLineColor(colors[i]);
    cb->SetLineWidth(3); //8 single (metà per multipli insieme)
    
    h1_amp->SetLineWidth(4); //4 single
    h1_amp->Fit(cb, "RX");
    h1_amp->SetXTitle("arbitrary units");
    h1_amp->SetYTitle(Form("Counts/%f", (double)(h1_amp->GetXaxis()->GetBinWidth(13))) );
    h1_amp->GetXaxis()->SetTitleSize(0.04);
    h1_amp->GetYaxis()->SetTitleSize(0.072);
    h1_amp->GetXaxis()->SetLabelSize(0.068);
    h1_amp->GetYaxis()->SetLabelSize(0.068);
    h1_amp->GetYaxis()->SetTitleOffset(+1.2);
    h1_amp->GetXaxis()->SetTitleOffset(0.6);
    
    c1->Clear();
    TLatex* latex = new TLatex();
    latex->SetTextSize(0.080);
    latex->SetTextColor(colors[i]);
    
    h1_amp->Draw();
    cb->Draw("Lsame");
    latex->DrawLatex( 0.3, h1_amp->GetMaximum()-0.05*(h1_amp->GetMaximum()), Form("V_{CNT} = %s V", h1_amp->GetName()));
   
      
    histos.push_back(h1_amp);

  } //for sui voltaggi vari

  int ymax = 299;
  TH2D* h2_axes = new TH2D( "axes", "", 10, xmin, xmax, 10, 0., ymax );
  h2_axes->SetXTitle( "arbitrary units" );
  //h2_axes->GetXaxis()->SetLabelSize(0);  
  //h2_axes->GetXaxis()->SetTickLength(0);  
  h2_axes->GetXaxis()->SetTitleSize(0.05);
  h2_axes->GetYaxis()->SetTitleSize(0.05);
  h2_axes->GetYaxis()->SetLabelSize(0.05);
  h2_axes->GetXaxis()->SetTitleSize(0.048);
  h2_axes->GetXaxis()->SetLabelSize(0.048);
  h2_axes->GetXaxis()->SetTitleOffset(1.0);
  h2_axes->GetYaxis()->SetNdivisions(510);
  h2_axes->GetXaxis()->SetNdivisions(510);
  h2_axes->GetXaxis()->SetNdivisions(505);
  h2_axes->GetYaxis()->SetTitleOffset(1.5);
  h2_axes->SetYTitle( Form("Counts / %.3f", (double)(histos[0]->GetXaxis()->GetBinWidth(13))) );
  h2_axes->SetTitle("T60, CNTs: 1 mm^{2}");
  h2_axes->Draw();
  
  TLatex* latex = new TLatex();
  latex->SetTextSize(0.05);
  //style->SetOptTitle(1);
  
  for( unsigned i=0; i<histos.size(); ++i ) {

    histos[i]->SetLineWidth(3);
    histos[i]->SetLineColor(colors[i]);
    
    TF1* f = histos[i]->GetFunction(Form("cb_%s", histos[i]->GetName()));
    f->SetLineColor(colors[i]);
    
    histos[i]->Draw("same");
    f->SetNpx(1000);
    f->Draw("Lsame");
    latex->SetTextColor(colors[i]);
    float fzero = f->Eval(xmin);
    
    if(i==0){ latex->DrawLatex( 1.01, 250, "E_{e} = 99 eV");}
    if(i==1){ latex->DrawLatex( 0.92, 250, "E_{e} = 93 eV");}
  }
 
  gPad->RedrawAxis();
  
  c1->SaveAs(Form("/Users/massimo/Documents/phd/PTOLEMY/Articoli/Firmati/new article/graphs/fits_CD%d.pdf",CD_number));
  c1->SaveAs(Form("/Users/massimo/Documents/phd/PTOLEMY/CD Data/plots/articolo2/fits_CD%d_cb.pdf",CD_number));

  c1->Clear();

    
  
  

  return 0;

}

