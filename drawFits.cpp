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


float getCorr( double v, TGraphErrors* gr_temp_vs_V, TGraphErrors* gr_corr );


int main() {

  //TStyle* style = setStyle();
  //style->SetPadLeftMargin(0.17);
  //style->SetPadRightMargin(0.04);
  //style->SetPadTopMargin(0.04);
  //style->SetPadBottomMargin(0.15);

  std::vector<int> colors = get_colors();

  //std::vector<int> volts = {96, 99, 103, 105};
  std::vector<int> volts = {105, 104, 103, 102, 101, 100, 99, 98, 97, 96, 95};

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

  std::string filename = "/Users/massimo/Documents/phd/PTOLEMY/CD data/data/params_CD188conteggiamp_paper.txt";
  std::ofstream ofs(filename);
  ofs << "V (V) E (eV) mu (V)  mu_err (V) sigmaR (V) sigmaR_err (V) sigmaL (V) sigmaL_err (V) reso_gaus (eV) reso_gaus_err (eV) reso_fwhm (eV) reso_fwhm_err (eV)" << std::endl;

  
  std::vector<TH1D*> histos;

  // for the fit plot of the paper
  float xmin = 0.73;
  float xmax = 0.939999;

  TGraphErrors* gr_mu = new TGraphErrors(0);
  TGraphErrors* gr_mucorr = new TGraphErrors(0);
  TGraphErrors* gr_reso = new TGraphErrors(0);
  TGraphErrors* gr_reso_corr = new TGraphErrors(0);
  TGraphErrors* gr_reso_fwhm = new TGraphErrors(0);

  TFile* file_temp = TFile::Open("temp_vs_V.root", "read" );
  TGraphErrors* gr_temp_vs_V = (TGraphErrors*)file_temp->Get("temp_vs_V");

  TFile* file_corr = TFile::Open("corr_VvsT.root", "read" );
  TGraphErrors* gr_corr = (TGraphErrors*)file_corr->Get("corr");

  TCanvas* c1 = new TCanvas( "c1", "", 1500, 1500 );
  c1->SetLeftMargin(0.13);
  c1->cd();

  for( unsigned i=0; i<volts.size(); i++ ) {

    std::cout << std::endl << std::endl;
    std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
    std::cout << "V = " << volts[i] << std::endl << std::endl;

    TFile* file = TFile::Open( Form("tree/CD188_%dV_tree.root", volts[i]), "read" );
    //if( volts[i] == 100. ) file = TFile::Open( "tree/100Vb_tree.root", "read" );
    TTree* tree = (TTree*)file->Get("tree");

    std::string histname(Form("%d", volts[i]));
    int nbins = 200;
    float binsize = 1./nbins;
    TH1D* h1_amp = new TH1D( histname.c_str(), "", nbins, 0., 1. );
    tree->Project(histname.c_str(), "amp");

    float fitmin = (volts[i]<100. && volts[i]!=97.) ? 0.5 : 0.6;
    float fitmax = 0.96;
    //if( volts[i] == 96 ) {
    //  fitmin = 0.79;
    //  fitmax = 0.86;
    //} else if( volts[i] == 99 ) {
    //  fitmin = 0.75;
    //  fitmax = 0.86;
    //} else if( volts[i] == 103 ) {
    //  fitmin = 0.75;
    //  fitmax = 0.87;
    //} else if( volts[i] == 105 ) {
    //  fitmin = 0.5;
    //  //fitmax = 0.92;
    //}

    TF1 *cruijff  = new TF1(Form("cru_%d", volts[i]), Cruijff, fitmin, fitmax, 5);
    cruijff->SetParNames("#mu","sigma_{L}", "#sigma_{R}", "#alpha", "A");
    cruijff->SetParLimits(0,   xmin,   xmax); // mu
    cruijff->SetParLimits(1,   0.01,   0.5); // sigma_L
    cruijff->SetParLimits(2,  0.001,   0.1); // sigma_R
    cruijff->SetParLimits(3,      0,     1 ); // alpha
    cruijff->SetParLimits(4,      1,   3000); // A

    cruijff->SetParameter( 0, 0.8  ); // mu
    cruijff->SetParameter( 1, 0.1  ); // sigma_L
    cruijff->SetParameter( 2, 0.02 ); // sigma_R
    cruijff->SetParameter( 3, 0.1  ); // alpha
    cruijff->SetParameter( 4, (double)(h1_amp->Integral()) ); // A

    cruijff->FixParameter( 3, 0.  ); // alpha

    if(volts[i]==101){cruijff->SetParameter( 0, 0.82);} // mu}
    h1_amp->Fit( cruijff, "RX" );
    h1_amp->SetLineWidth(2);
    
    c1->Clear();
    cruijff->SetLineColor(kRed-4);
    cruijff->SetLineWidth(3);
    cruijff->SetNpx(1000);
    h1_amp->SetLineColor(kBlack);
    h1_amp->GetYaxis()->SetTitle(Form("Counts/%.3f", binsize));
    h1_amp->GetXaxis()->SetTitle("Amplitude [V]");
    h1_amp->SetTitle(Form("%d V", volts[i]));

    gStyle->SetOptStat(0);
    h1_amp->Draw();
    cruijff->Draw("Lsame");
    c1->SaveAs(Form("plots/CD188/conteggi/%dV/%d_final.pdf", volts[i],volts[i]));

    float mu = cruijff->GetParameter(0);
    float muerr = cruijff->GetParError(0);
    float sigma = cruijff->GetParameter(2);
    float sigmaerr = cruijff->GetParError(2);
    float sigmaL = cruijff->GetParameter(1);
    float sigmaLerr = cruijff->GetParError(1);

    muerr = sqrt( muerr*muerr + 0.01*0.01*mu*mu );

    gr_mu->SetPoint( i, volts[i], mu );
    gr_mu->SetPointError( i, 0., muerr );

    
    float corr = getCorr( volts[i], gr_temp_vs_V, gr_corr );
    gr_mucorr->SetPoint( i, volts[i], mu*corr );
    gr_mucorr->SetPointError( i, 0., muerr*corr );

    float reso = sigma/mu*volts[i];
    float reso_corr = sqrt( reso*reso - 0.1*0.1 );
    float reso_err = sqrt( sigmaerr*sigmaerr/(mu*mu) + sigma*sigma*muerr*muerr/(mu*mu*mu*mu) )*volts[i];

    //aggiunta fwhm
    
    float energy_ele = volts[i] - phi + (phi-phi_tes);
     float energy_err = sqrt(0.0009 + pow((0.04+0.0015*volts[i]),2));
    
    float fwhm = (sigma+sigmaL)*sqrt(2*log(2));
    float fwhm_err = sqrt(2*log(2))*sqrt(sigmaerr*sigmaerr+sigmaLerr*sigmaLerr);
    float piece1_f = fwhm_err*fwhm_err/(mu*mu);
    float piece2_f = fwhm*fwhm*muerr*muerr/(mu*mu*mu*mu);
    float reso_fwhm = fwhm/mu*(energy_ele);
    float reso_fwhm_err = sqrt(piece1_f+piece2_f)*(energy_ele);
   
    //scrittura su file param.txt
    ofs << volts[i] << " " << energy_ele << " " 
      	<< energy_err << " " 
	<< mu << " " << muerr << " "
	<< sigma << " " << sigmaerr << " "
	<< sigmaL << " " << sigmaLerr << " "
	<< reso << " " << reso_err << " "
	<< reso_fwhm << " " << reso_fwhm_err << std::endl;

    gr_reso->SetPoint( i, energy_ele, reso );
    gr_reso->SetPointError( i, 0., reso_err );

    gr_reso_corr->SetPoint( i, energy_ele, reso_corr );
    gr_reso_corr->SetPointError( i, 0., reso_err );

std::cout << "E = " << volts[i] << " sigma = " << reso << " +/- " << reso_err << std::endl;

    gr_reso_fwhm->SetPoint( i, energy_ele, reso*2*sqrt(2*log(2)) );
    gr_reso_fwhm->SetPointError( i, 0., reso_err*2*sqrt(2*log(2)) );

    if( (volts[i] == 97. ) ||
        //(volts[i] == 99. ) ||
        (volts[i] == 100.) ||
        (volts[i] == 101.) ||
        (volts[i] == 103.) ||
        (volts[i] == 105.)   ) histos.push_back(h1_amp);
    

  }

  ofs.close();

  TH2D* h2_axes = new TH2D( "axes", "", 10, xmin, xmax, 10, 0., 280. );
  h2_axes->SetXTitle( "TES amplitude (V)" );
  h2_axes->GetXaxis()->SetNdivisions(505);
  h2_axes->GetYaxis()->SetTitleOffset(1.5);
  h2_axes->SetYTitle( Form("Counts / (%.3f V)", (double)(histos[0]->GetXaxis()->GetBinWidth(13))) );

  h2_axes->Draw();
  
  TLatex* latex = new TLatex();
  latex->SetTextSize(0.035);

  for( unsigned i=0; i<histos.size(); ++i ) {

    histos[i]->SetLineWidth(2);
    histos[i]->SetLineColor(colors[i]);
    TF1* f = histos[i]->GetFunction(Form("cru_%s", histos[i]->GetName()));
    f->SetLineColor(colors[i]);
    histos[i]->Draw("same");
    
    latex->SetTextColor(colors[i]);
    float fzero = f->Eval(xmin);
    float nsigma = (volts[i]==105.) ? 4.8 : 3.;
    latex->DrawLatex( 0.74, fzero + nsigma*sqrt(fzero), Form("#it{V}_{cnt} = %s V", histos[i]->GetName()) );

  }

  gPad->RedrawAxis();

  c1->SaveAs("fits.pdf");


  c1->Clear();


  float xmin_resomu = 94.00001 - phi;
  float xmax_resomu = 105.9999 - phi;

  TGraphErrors* gr_photonreso = new TGraphErrors(0);
  TGraphErrors* gr_photonreso_fwhm = new TGraphErrors(0);
  
  float e_photon, reso_photon, reso_photon_err;
  int n_photon;
  std::ifstream file_photons("data/photonreso.dat");
  while( file_photons >> e_photon >> reso_photon >> reso_photon_err >> n_photon ) {
    int n = gr_photonreso->GetN();
    gr_photonreso->SetPoint( n, e_photon, reso_photon );
    gr_photonreso->SetPointError( n, 0., reso_photon_err ); 
    gr_photonreso_fwhm->SetPoint( n, e_photon, reso_photon*2*sqrt(2*log(2)) );
    gr_photonreso_fwhm->SetPointError( n, 0., reso_photon_err*2*sqrt(2*log(2)) ); 
  }
  
  


  TH2D* h2_axes_reso = new TH2D( "axes_reso", "", 10, xmin_resomu, xmax_resomu, 10, 0., 2.3 );
  h2_axes_reso->SetXTitle( "Energy (eV)" );
  h2_axes_reso->GetYaxis()->SetTitleOffset(1.5);
  h2_axes_reso->GetXaxis()->SetNdivisions(3, 5, 1);
  //h2_axes_reso->GetXaxis()->SetNdivisions(706);
  h2_axes_reso->SetYTitle( "TES Gaussian energy resolution (eV)" );
  //h2_axes_reso->SetYTitle( "TES electron energy resolution #sigma_{e} (eV)" );
  h2_axes_reso->Draw();

  gr_photonreso->SetMarkerSize(2);
  gr_photonreso->SetLineWidth(3);
  gr_photonreso->SetMarkerStyle(21);
  gr_photonreso->SetMarkerColor(kBlack);
  gr_photonreso->SetLineColor  (kBlack);
  gr_photonreso->Draw("PSame");

  //gr_reso_corr->SetMarkerSize(2);
  //gr_reso_corr->SetLineWidth(3);
  //gr_reso_corr->SetMarkerStyle(24);
  //gr_reso_corr->SetMarkerColor(kBlue);
  //gr_reso_corr->SetLineColor(46);
  //gr_reso_corr->Draw("PSame");

  gr_reso->SetMarkerSize(2);
  gr_reso->SetLineWidth(3);
  gr_reso->SetMarkerStyle(20);
  gr_reso->SetMarkerColor(46);
  gr_reso->SetLineColor(46);
  gr_reso->Draw("PSame");

  TLegend* legend = new TLegend( 0.6, 0.75, 0.9, 0.9 );
  legend->SetTextSize( 0.04 );
  legend->SetFillColor( 0 );
  legend->AddEntry( gr_photonreso, "Photons"  , "PL" );
  legend->AddEntry( gr_reso      , "Electrons", "PL" );
  legend->Draw("same");

  gPad->RedrawAxis();

  c1->SaveAs("reso.pdf");

  c1->Clear();

  TH2D* h2_axes_reso_fwhm = new TH2D( "axes_reso_fwhm", "", 10, xmin_resomu, xmax_resomu, 10, 0., 2.3*2*sqrt(2*log(2)) );
  h2_axes_reso_fwhm->SetXTitle( "Energy (eV)" );
  h2_axes_reso_fwhm->GetYaxis()->SetTitleOffset(1.5);
  h2_axes_reso_fwhm->GetXaxis()->SetNdivisions(3, 5, 1);
  h2_axes_reso_fwhm->SetYTitle( "#Delta#it{E}_{fwhm} (eV)" );
  h2_axes_reso_fwhm->Draw();

  gr_photonreso_fwhm->SetMarkerSize(2);
  gr_photonreso_fwhm->SetLineWidth(3);
  gr_photonreso_fwhm->SetMarkerStyle(21);
  gr_photonreso_fwhm->SetMarkerColor(kBlack);
  gr_photonreso_fwhm->SetLineColor  (kBlack);
  gr_photonreso_fwhm->Draw("PSame");

  gr_reso_fwhm->SetMarkerSize(2);
  gr_reso_fwhm->SetLineWidth(3);
  gr_reso_fwhm->SetMarkerStyle(20);
  gr_reso_fwhm->SetMarkerColor(46);
  gr_reso_fwhm->SetLineColor(46);
  gr_reso_fwhm->Draw("PSame");

  legend->Draw("same");

  gPad->RedrawAxis();

  c1->SaveAs("reso_fwhm.pdf");

  c1->Clear();

  TH2D *h2_axes_mu = new TH2D( "axes_mu", "", 10, xmin_resomu, xmax_resomu, 10, 0., 4. );
  //TH2D *h2_axes_mu = new TH2D( "axes_mu", "", 10, xmin_resomu, xmax_resomu, 10, 0.79, 0.89 );
  h2_axes_mu->SetXTitle( "Electron energy #it{E}_{e} (eV)" );
  h2_axes_mu->SetYTitle( "Position of absorption peak #mu (V)" );
  h2_axes_mu->GetYaxis()->SetTitleOffset(1.5);
  h2_axes_mu->GetXaxis()->SetNdivisions(3, 5, 1);
  h2_axes_mu->GetYaxis()->SetNdivisions(5, 5, 1);
  h2_axes_mu->GetYaxis()->SetAxisColor (colors[0]);
  h2_axes_mu->GetYaxis()->SetTitleColor(colors[0]);
  h2_axes_mu->GetYaxis()->SetLabelColor(colors[0]);
  h2_axes_mu->Draw();

  gr_mu->SetMarkerSize(2);
  gr_mu->SetLineWidth(3);
  gr_mu->SetMarkerStyle(21);
  gr_mu->SetMarkerColor(kBlack);
  gr_mu->SetLineColor(kBlack);
  gr_mu->Draw("PSame");

  gr_mucorr->SetMarkerSize(2);
  gr_mucorr->SetLineWidth(3);
  gr_mucorr->SetMarkerStyle(21);
  gr_mucorr->SetMarkerColor(46);
  gr_mucorr->SetLineColor(46);
  gr_mucorr->Draw("PSame");

  TLegend* legend_mu = new TLegend( 0.25, 0.7, 0.55, 0.9 );
  legend_mu->SetFillColor(0);
  legend_mu->SetTextSize( 0.04 );
  legend_mu->AddEntry( gr_mu, "Before temp. correction", "PL" );
  legend_mu->AddEntry( gr_mucorr, "After temp. correction", "PL" );
  legend_mu->Draw("same");
  

  gPad->RedrawAxis();

  c1->SaveAs("mu.pdf");

  c1->Clear();

  TPad *pad_mu  = new TPad("pad_mu" ,"",0,0,1,1);

  TPad *pad_reso  = new TPad("pad_reso ","",0,0,1,1);
  pad_reso->SetFillStyle(4000); //will be transparent
  pad_reso->SetFrameFillStyle(0);


  h2_axes_reso->GetYaxis()->SetAxisColor (46);
  h2_axes_reso->GetYaxis()->SetTitleColor(46);
  h2_axes_reso->GetYaxis()->SetLabelColor(46);

  pad_mu->Draw();
  pad_mu->cd();
  h2_axes_mu->Draw("");

  gr_mu->SetMarkerStyle(21);
  gr_mu->SetMarkerSize(2);
  //gr_mu->SetLineWidth(2);
  gr_mu->Draw("psame");

  pad_reso->Draw("Y+");
  pad_reso->cd();

  h2_axes_reso->Draw("Y+");
  gr_reso->SetLineWidth(1);
  gr_reso->Draw("PSame Y+");

  c1->SaveAs("mureso.pdf");

  return 0;

}


float getCorr( double v, TGraphErrors* gr_temp_vs_V, TGraphErrors* gr_corr ) {

  float temp = -1.;

  for( unsigned i=0; i<gr_temp_vs_V->GetN(); ++i ) {

    double x, y;
    gr_temp_vs_V->GetPoint( i, x, y );

    if( v == x ) temp = y;

  }

  if( temp > 62. ) temp = 61.99;

  float corr = -1.;

  for( unsigned i=1; i<gr_corr->GetN(); ++i ) {

    double x0, y0;
    gr_corr->GetPoint( i-1, x0, y0 );

    double x1, y1;
    gr_corr->GetPoint( i, x1, y1 );

    if( temp>x0 && temp<x1 ) {

      TF1* line = new TF1( "line", "[0] + [1]*x" );
      line->SetParameter( 0, (y0*x1-y1*x0)/(x1-x0) );
      line->SetParameter( 1, (y1-y0)/(x1-x0) );

      corr = line->Eval( temp );

    }

  } // for points

  std::cout << "+++ for V = " << v << " V, T = " << temp << " mK, and corr = " << corr << std::endl;

  return corr;

}


