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
#include "TFitResult.h"


#include "graphics.h"
#include "AndCommon.h"
#include "CD_details.h"

void SetParamRanges(int v, int j, TF1* cb);
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

  //parametri da input
  int   CD_number     = 204; //cooldown
  char  misura_char[] = "B60_post_cond3_moku";
  char* misura        = misura_char;
  char  choice_char[] = "amp";
  char* choice        = choice_char;

  std::vector<int> volts;
  volts.push_back(96); volts.push_back(100); volts.push_back(103); 

  std::vector<std::array<float,5>> reso_gaus;
  std::vector<std::array<float,5>> reso_gaus_err;

 
  Settings settings;
  TCanvas* c1 = new TCanvas( "c1", "", 1500, 1500 );
  settings.Margin(c1);
  c1->SetLeftMargin(0.155);
  c1->cd();

  float phi_tes = GetPhiTes();

  //paper, bin1, bin2, fitrange1, fitrange2
  //int nbins[5] = {700, 600, 800, 700, 700};
  int nbins[5] = {800, 700, 900, 800, 800};
  
  for(unsigned i=0; i<volts.size(); i++) {
  //for(unsigned i=2; i<3; i++) {
    std::cout << std::endl << std::endl;
    std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
    std::cout << "V = " << volts[i] << std::endl << std::endl;
    
    //leggo tutti i tree per i voltaggi
    TFile* file = TFile::Open( Form("data/root/CD%d/%s/%dV/CD%d_%dV_tree.root", CD_number, misura, volts[i], CD_number, volts[i]), "read" );

    TTree* tree = (TTree*)file->Get("tree");
    
    //lettura del tree in amp o charge
    float variable;
    tree->SetBranchAddress(choice, &variable);
    Histo h_var = SetHistoValues(CD_number,choice,misura,volts[i]);

    float hist_min  = h_var.get_hist_min();
    float hist_max  = h_var.get_hist_max();
    float range_min = h_var.get_range_min();
    float range_max = h_var.get_range_max();

    std::array<float,5> reso_temp;
    std::array<float,5> reso_temp_err;
    
    
    //for sui binnaggi
    for(int j=0; j<5; j++){
    //creo un histo per ogni tree
      
    std::string histname(Form("%d", volts[i]));
    TH1F* histo = new TH1F( histname.c_str(), "", nbins[j], hist_min, hist_max);
    Long64_t nentries = tree->GetEntries();

    float binsize = settings.Binsize(1./nbins[j]);
    const char* x_amp = "Amplitude [V]";
    std::string y_amp = "Counts / " + to_str(binsize) + " V" ;

    
    for(int iEntry=0; iEntry<nentries; iEntry++){
      tree->GetEntry(iEntry);
      histo->Fill(variable);
    }

    float fitmin_cb;
    float fitmax_cb = 0.65;

    if(j==0 || j==1 || j==2){
    if(volts[i]==96 ) fitmin_cb = 0.595;   
    if(volts[i]==100) fitmin_cb = 0.592;
    if(volts[i]==103) { fitmin_cb = 0.59; fitmax_cb = 0.65;}
    }
    if(j==3){
      if(volts[i]==96 ){ fitmin_cb = 0.59; fitmax_cb = 0.63; } //range1
      if(volts[i]==100){ fitmin_cb = 0.6 ; fitmax_cb = 0.63; }
      if(volts[i]==103){ fitmin_cb = 0.598 ; fitmax_cb = 0.63; }}
    if(j==4){
      if(volts[i]==96 ){ fitmin_cb = 0.6; fitmax_cb = 0.66; } //range1
      if(volts[i]==100){ fitmin_cb = 0.605; fitmax_cb = 0.63; }
      if(volts[i]==103){ fitmin_cb = 0.605; fitmax_cb = 0.63; }}
    
    TF1 *cb  = new TF1(Form("cb_%d", volts[i]), Crystal_Ball, fitmin_cb, fitmax_cb, 5);
    cb->SetParNames("#mu","sigma", "#alpha", "n", "A");
    
    cb->SetParLimits(0, 0.609, 0.72); // mu
    cb->SetParLimits(1, 0.001,  0.5); // sigma
    cb->SetParLimits(2,  0.01,  100); // alpha
    cb->SetParLimits(3,  0.01,  150); // n
    cb->SetParLimits(4,     1, 3000); // A
    
    cb->SetParameter(0, 0.62); // mu
    cb->SetParameter(1, 0.01); // sigma_L
    cb->SetParameter(2, 0.04); // alpha
    cb->SetParameter(3,  90 ); // n
    cb->SetParameter(4, (double)(histo->Integral()) ); // A
    
    SetParamRanges(volts[i],j,cb);
 
    histo->GetXaxis()->SetTitle("Amplitude (V)");
    histo->SetMarkerSize(2);
    histo->SetLineWidth(3);
    histo->Fit(cb, "RX");
    histo->SetNdivisions(510);
    cb->SetLineColor(kRed-4);
    cb->SetLineWidth(5);
    cb->SetNpx(1000);
    settings.graphic_histo(histo,range_min,range_max,x_amp,y_amp.c_str(),volts[i]);
    settings.general_style();
    histo->SetLineWidth(3);
    histo->Draw(); //"pe" per avere points+errors
    cb->Draw("same");
    
    //estrazione parametri dal fit
    float mu_cb        = cb->GetParameter(0);
    float muerr_cb     = cb->GetParError(0);
    float sigmaR_cb    = cb->GetParameter(1);
    float sigmaRerr_cb = cb->GetParError(1);
    
    //l'errore sulla stabilità di ites-ibias è insito nei segnali che otteniamo
    //risoluzione energetica gaussiana cb
    float energy_ele = volts[i] -phi_tes;
    float energy_err = sqrt(0.0009 + pow((0.04+0.0015*volts[i]),2));
    float reso_cb    = sigmaR_cb/mu_cb*(volts[i]-phi_tes);
    float reso_errcb = sqrt( sigmaRerr_cb*sigmaRerr_cb*energy_ele*energy_ele/(mu_cb*mu_cb) + sigmaR_cb*sigmaR_cb*energy_ele*energy_ele*muerr_cb*muerr_cb/(mu_cb*mu_cb*mu_cb*mu_cb) + sigmaR_cb*sigmaR_cb*energy_err*energy_err/(mu_cb*mu_cb));

    /*
    float cb_max      = cb->GetMaximum(fitmin_cb, fitmax_cb);
    float Ytarget     = cb_max*0.5;
    float x1          = cb->GetX(Ytarget, fitmin_cb, mu_cb);
    float x2          = cb->GetX(Ytarget, mu_cb, fitmax_cb);
    
    float fwhm_cb     = x2-x1;
    float fwhm_errcb  = 0;
    float piece1_fcb  = fwhm_errcb*fwhm_errcb*energy_ele*energy_ele/(mu_cb*mu_cb);
    float piece2_fcb  = fwhm_cb*fwhm_cb*muerr_cb*muerr_cb*energy_ele*energy_ele/(mu_cb*mu_cb*mu_cb*mu_cb);
    float piece3_fcb  = fwhm_cb*fwhm_cb*energy_err*energy_err/(mu_cb*mu_cb);
    float reso_fwhm_cb = fwhm_cb/mu_cb*(energy_ele);
    float reso_fwhm_err_cb = sqrt(piece1_fcb+piece2_fcb+piece3_fcb);
    */
    
    std::cout << "-----CRYSTAL  BALL-----" << std::endl; 
    std::cout << "E = " << volts[i]-phi_tes << " DE_gaus = " << reso_cb << " +/- " << reso_errcb << std::endl;
    //std::cout << "E = " << volts[i]-phi_tes << " DE_fwhm = " << reso_fwhm_cb << " +/- " << reso_fwhm_err_cb << std::endl;
    std::cout << "P: " << cb->GetProb()*100 << "%" << std::endl;
   
    std::string outdir( Form("plots/CD%d/%s/%dV", CD_number, misura, volts[i]));
    system( Form("mkdir -p %s", outdir.c_str()) );
    c1->SaveAs(Form("plots/CD%d/%s/%dV/syst_gaus_cb_fit_test%d.png",CD_number,misura,volts[i],j));


    reso_temp[j] = reso_cb;
    reso_temp_err[j] = reso_errcb;

    } //for sui binnaggi

    reso_gaus.push_back(reso_temp); //ho le reso per uno stesso voltaggio
    reso_gaus_err.push_back(reso_temp_err);

  }//for sui voltaggi

  //reso_gaus è un vettore a tre componenti. Ogni componente è un array con le risoluzioni di: paper, bin1, bin2, range1, range2

  for(int v=0; v<volts.size(); v++){ //ciclo sui voltaggi
    std::cout << "+++++++++ V = " << volts[v] << " +++++++++" << std::endl;
    float media = 0;
    for(int j=0; j<5; j++){ //ciclo sui fit
      
    float systgaus = abs(reso_gaus[v][j]-reso_gaus[v][0])/(1.*reso_gaus[v][0]);
    float systgaus_err = sqrt(((reso_gaus_err[v][j]*reso_gaus_err[v][j])/(1.*reso_gaus[v][0]*reso_gaus[v][0]))+(reso_gaus[v][j]*reso_gaus[v][j]*reso_gaus_err[v][0]*reso_gaus_err[v][0])/(1.*reso_gaus[v][0]*reso_gaus[v][0]*reso_gaus[v][0]*reso_gaus[v][0]));

    media+=(systgaus*100);
    std::cout << systgaus * 100 << " +- " << systgaus_err << std::endl;
    }
    std::cout << "MEDIA --> " << media/4. << std::endl;
    std::cout << std::endl;
  }
  
  
  return 0;

}

void SetParamRanges(int v, int j, TF1* cb){

   if(v==96){
      if(j==2){
        cb->SetParLimits(0, 0.61, 0.62);
	cb->SetParLimits(1, 0.002, 0.004 );}
      if(j==4){
        cb->SetParLimits(0, 0.61, 0.62);}
      
    }
   
  if(v==100){cb->SetParameter(0, 0.62 );
      cb->SetParameter(1, 0.035);
      cb->SetParLimits(2, 0, 2 );
      cb->SetParameter(3, 0.3  );
      cb->SetParameter(4, 300  );
      if(j==3){cb->SetParLimits(3, 0, 3);
        cb->SetParLimits(0, 0.61, 0.62);}
    }


    if(v==103){cb->SetParameter(0, 0.62 );
    cb->SetParameter(1, 0.035);
    cb->SetParameter(3, 0.3  ); 
    cb->SetParLimits(0, 0.618, 0.625);
    cb->SetParLimits(2, 0, 2 );}
}
