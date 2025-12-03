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

Double_t Cruijff(Double_t *x, Double_t *par){
  //par[0]=mean      par[1]=costante 0<k<1    par[2]=sigma_R
  //par[3]=alpha_L   par[4]=A
  Double_t fitval = 0;
  Double_t arg_L  = 0;
  Double_t arg_R  = 0;
  Double_t num    = pow((x[0]-par[0]),2);
  Double_t denL   = 2*par[1]*par[1] + par[3]*num;
  //Double_t denL   = 2*par[2]*par[2]*par[1]*par[1];
  Double_t denR   = 2*par[2]*par[2];
  //Double_t asym   = par[3]*num;
  if ( denL > 0 ){ arg_L = -num/denL ;}
  if ( denR > 0 ){ arg_R = -num/denR ;}

  if       ( x[0] < par[0] ){ fitval = par[4] * exp(arg_L); } //LEFT
  else if  ( x[0] > par[0] ){ fitval = par[4] * exp(arg_R); } //RIGHT

  return(fitval);
}

//correzioni date dal cambio di temperatura del TES
float getCorr( double v, TGraphErrors* gr_temp_vs_V, TGraphErrors* gr_corr );


int main(int argc, char* argv[]) {

  if(argc!=2){
    std::cout << "Serve inserire il numero di cooldown" << std::endl; exit(1);
  }
  int CD_number = atoi(argv[1]);
  
  const char* measure = nullptr;
  //std::vector<int> colors{kBlack, 46, 38, 30};
  TStyle* style = setStyle();
  style->SetPadLeftMargin(0.17);
  style->SetPadRightMargin(0.04);
  style->SetPadTopMargin(0.04);
  style->SetPadBottomMargin(0.1);

  std::vector<int> colors; 

  std::vector<int> volts;
  if(CD_number==188){volts.push_back(105); volts.push_back(103); volts.push_back(101); volts.push_back(97); measure = "conteggi"; colors.push_back(1); colors.push_back(46); colors.push_back(38); colors.push_back(30); }
  if(CD_number==204){volts.push_back(105); volts.push_back(97); measure = "B60_post_cond3_moku"; colors.push_back(1); colors.push_back(30);}
  if(CD_number==222){volts.push_back(123);  volts.push_back(128); measure = "E100_squidfilter_ER_031025_tr12"; colors.push_back(46); colors.push_back(38);}

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
  float xmin = 0;
  float xmax = 0;
  int nbins;
  float scale_factor = 1.0;
    if(CD_number == 204) scale_factor = 1.61;
    if(CD_number == 222) scale_factor = 8.63;
    
  if(CD_number==188){ xmin = 0.73*scale_factor; xmax = 0.93999*scale_factor; nbins = 200*2;}
  //if(CD_number==204){ xmin = 0.55*scale_factor;  xmax = 0.67*scale_factor; nbins = 896;}
  //if(CD_number==222){ xmin = 0.09*scale_factor; xmax = 0.13*scale_factor; nbins = 650;}

  if(CD_number==204){ xmin = 0.81;  xmax = 1.12; nbins = 404;}
  if(CD_number==222){ xmin = 0.81;  xmax = 1.12; nbins = 404;}
  

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
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
    //TH1F* h1_amp = new TH1F( histname.c_str(), "", nbins, 0., 1. );
    //tree->Project(histname.c_str(), "amp");

    //chatgpt
    TH1F* h1_amp = new TH1F(histname.c_str(), "", nbins, 0, 2.);

    
    float amp_val;
    tree->SetBranchAddress("amp", &amp_val);

    for(Long64_t k = 0; k < tree->GetEntries(); k++){
      tree->GetEntry(k);
      
      float new_amp = scale_factor * amp_val;   // riscalamento richiesto
      h1_amp->Fill(new_amp);
    }
    //fine chatgpt
    
    Fit f_var = SetFitValues(CD_number,"amp",h1_amp,volts[i],(char*)measure);
    
    //assegnazione parametri di fit
    float mu_min     = f_var.get_mu_min()*scale_factor;
    float mu_max     = f_var.get_mu_max()*scale_factor;
    float mu_        = f_var.get_mu()*scale_factor;
    float sigmaL_min = f_var.get_sigmaL_min()*scale_factor;
    float sigmaL_max = f_var.get_sigmaL_max()*scale_factor;
    float sigmaL_    = f_var.get_sigmaL()*scale_factor;
    float sigmaR_min = f_var.get_sigmaR_min()*scale_factor;
    float sigmaR_max = f_var.get_sigmaR_max()*scale_factor;
    float sigmaR_    = f_var.get_sigmaR()*scale_factor;
    float alfa_min   = f_var.get_alfa_min()*scale_factor;
    float alfa_max   = f_var.get_alfa_max()*scale_factor;
    float alfa_      = f_var.get_alfa()*scale_factor;
    float A_min      = f_var.get_A_min();
    float A_max      = f_var.get_A_max();
    float A_         = f_var.get_A();
    float fit_min    = f_var.get_fit_min()*scale_factor;
    float fit_max    = f_var.get_fit_max()*scale_factor;
   
   TF1 *cruijff  = new TF1(Form("cru_%d", volts[i]), Cruijff, fit_min, fit_max, 5);

    //parametri del fit
    cruijff->SetParNames("#mu","#sigma_{L}", "#sigma_{R}", "#alpha", "A");
    if(CD_number==222){mu_ = 0.985;}
    if(CD_number==222){sigmaR_max = 0.03;}
    if(CD_number==204){sigmaL_min = 0.0097;}
    cruijff->SetParLimits(0,    mu_min,    mu_max);
    cruijff->SetParLimits(1,sigmaL_min,sigmaL_max); 
    cruijff->SetParLimits(2,sigmaR_min,sigmaR_max);
    cruijff->SetParLimits(3,  alfa_min,  alfa_max);
    cruijff->SetParLimits(4,     A_min,     A_max);

    //gaussiana asimmetrica e non cruijff
    cruijff->FixParameter(3,0); //alfa value
    cruijff->SetNpx(1000);
    cruijff->SetParameters(mu_, sigmaL_, sigmaR_, alfa_, A_); //parametri iniziali

   
    //quindi di fatto è una gaussiana doppiamente definitita e basta
    cruijff->FixParameter( 3, 0.  ); // alpha

  
    cruijff->SetLineColor(colors[i]);
    cruijff->SetLineWidth(3); //8 single (metà per multipli insieme)
    h1_amp->SetLineWidth(4); //4 single
    h1_amp->Fit(cruijff, "RX");
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
    cruijff->Draw("Lsame");
    latex->DrawLatex( 0.3, h1_amp->GetMaximum()-0.05*(h1_amp->GetMaximum()), Form("V_{CNT} = %s V", h1_amp->GetName()));
   
      
    histos.push_back(h1_amp);

  } //for sui voltaggi vari

  int ymax;
  if(CD_number==188){ymax = 280;}
  if(CD_number==204){ymax = 560;}
  if(CD_number==222){ymax = 120;}
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
  if(CD_number==204){h2_axes->GetYaxis()->SetNdivisions(510);}
  if(CD_number==222){h2_axes->GetYaxis()->SetNdivisions(510);}
  h2_axes->GetXaxis()->SetNdivisions(510);
  //h2_axes->GetXaxis()->SetNdivisions(505);
  h2_axes->GetYaxis()->SetTitleOffset(1.5);
  h2_axes->SetYTitle( Form("Counts", (double)(histos[0]->GetXaxis()->GetBinWidth(13))) );
  if(CD_number==204){h2_axes->SetTitle("T60, CNTs: 1 mm^{2}");}
  if(CD_number==222){h2_axes->SetTitle("T100b, CNTs: 1 mm^{2}");}
  h2_axes->Draw();
  
  TLatex* latex = new TLatex();
  latex->SetTextSize(0.05);
  //style->SetOptTitle(1);
  
  for( unsigned i=0; i<histos.size(); ++i ) {

    histos[i]->SetLineWidth(3);
    histos[i]->SetLineColor(colors[i]);
    //histos[i]->GetXaxis()->SetNdivisions(0);
    TF1* f = histos[i]->GetFunction(Form("cru_%s", histos[i]->GetName()));
    f->SetLineColor(colors[i]);
    histos[i]->Draw("same");
    
    latex->SetTextColor(colors[i]);
    float fzero = f->Eval(xmin);
    float nsigma = (volts[i]==105.) ? 4.8 : 3.;
    if(CD_number==188){
    latex->DrawLatex( 0.74, fzero + nsigma*sqrt(fzero), Form("#it{V}_{CNT} = %s V", histos[i]->GetName()) );
    }
    if(CD_number==204){
      
      if(i==0){ latex->DrawLatex( 1.005, 450, Form("#it{V}_{CNT} = %s V", histos[i]->GetName()));}
      if(i==1){ latex->DrawLatex( 0.875, 500, Form("#it{V}_{CNT} = %s V", histos[i]->GetName()) );}}
    
  
  
  if(CD_number==222){
    histos[i]->SetTitle("T100b, CNTs: 1 mm^{2}");
    if(i==0){ latex->DrawLatex( 0.855, 100, Form("#it{V}_{CNT} = %s V", histos[i]->GetName()));}
    if(i==1){ latex->DrawLatex( 0.115*scale_factor, 110, Form("#it{V}_{CNT} = %s V", histos[i]->GetName()) );}}
  

  }

  /*
  TLine* line = new TLine(0.9995, 0, 0.9995, 102 );
  line->SetLineColor(kBlack);
  line->SetLineStyle( 2 );
  line->SetLineWidth( 2 );
  line->Draw("same");
  */
  gPad->RedrawAxis();
  
  //c1->SaveAs(Form("/Users/massimo/Documents/phd/PTOLEMY/Articoli/Firmati/new article/graphs/fits_CD%d.pdf",CD_number));
  c1->SaveAs(Form("/Users/massimo/Documents/phd/PTOLEMY/CD Data/plots/articolo2/fits_CD%d.pdf",CD_number));

  c1->Clear();

    
  
  

  return 0;

}

