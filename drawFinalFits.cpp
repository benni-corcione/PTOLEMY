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
#include "AndCommon.h"
#include "CD_details.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//++PROGRAMMA SETTATO SOLO SULLO STUDIO DI: +++++++++++++++++++//
// cd_188, conteggi, amp//
// cd_204, preamp_post_cond1_18dic, amp//
// cd_204, preamp_post_cond1_18dic, charge//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

void MuErrPercentage(char* meas, std::vector<int> volts, float muerr_perc[volts.size()]);
//aggiornare le funzioni Get in base allo studio sui fit singoli
void Input_Param_Ctrls(int argc, char* argv[]);
Double_t Cruijff(Double_t *x, Double_t *par);

int main(int argc, char* argv[]) {

  //controlli iniziali su variabili da input
  Input_Param_Ctrls(argc,argv);

  //parametri da input
  int   CD_number = (atoi(argv[1])); //cooldown
  char* choice    =       argv[3]  ;
  char* misura    =       argv[2]  ;

  std::vector<int> volts = GetVoltages(CD_number,misura);

  TGraphErrors* gr_mu = new TGraphErrors(0);
  TGraphErrors* gr_reso = new TGraphErrors(0);
  //TGraphErrors* gr_reso_fwhm = new TGraphErrors(0);

  float muerr_perc[volts.size()];
  MuErrPercentage(misura, volts, muerr_perc);

     
  Settings settings;
  TCanvas* c1 = new TCanvas( "c1", "", 1500, 1500 );
  settings.Margin(c1);
  c1->SetLeftMargin(0.155);
  c1->cd();

  std::string filename = "data/params_CD" + std::to_string(CD_number)
    + std::string(misura) + std::string(choice) + ".txt";
  std::ofstream ofs(filename);
  ofs << "V (V)  mu (V)  mu_err (V) sigma (V) sigma_err (V)" << std::endl;

  //impostazioni grafiche
  float magnifying = 1; //per avere numeri in carica più comodi (sono in E-6 sennò)
  if      (strcmp(choice,"amp"   )==0){ magnifying = 1e+0;}
  else if (strcmp(choice,"charge")==0){ magnifying = 1e+6;}

  
  
  float phi_carbon = 4.5;
  float phi_tes = GetPhiTes();
  
  for(unsigned i=0; i<volts.size(); i++) {
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
    //impostazione per gli histos
    int nbins = h_var.get_nbins();
    std::string x_name = h_var.get_x_name();
    std::string y_name = h_var.get_y_name();
  
    float hist_min  = h_var.get_hist_min();
    float hist_max  = h_var.get_hist_max();
    float range_min = h_var.get_range_min();
    float range_max = h_var.get_range_max();
    
		       
    //creo un histo per ogni tree
    std::string histname(Form("%d", volts[i]));
    TH1F* histo = new TH1F( histname.c_str(), "", nbins, hist_min, hist_max);
    // TH1F* histo = new TH1F( histname.c_str(), "", 200, 0., 1.);
    Long64_t nentries = tree->GetEntries();
    
    for(int iEntry=0; iEntry<nentries; iEntry++){
      tree->GetEntry(iEntry);
      histo->Fill(variable*magnifying);
    }
    
    Fit f_var = SetFitValues(CD_number,choice,histo,volts[i],misura);
    
    //assegnazione parametri di fit
    float mu_min     = f_var.get_mu_min();
    float mu_max     = f_var.get_mu_max();
    float mu_        = f_var.get_mu();
    float sigmaL_min = f_var.get_sigmaL_min();
    float sigmaL_max = f_var.get_sigmaL_max();
    float sigmaL_    = f_var.get_sigmaL();
    float sigmaR_min = f_var.get_sigmaR_min();
    float sigmaR_max = f_var.get_sigmaR_max();
    float sigmaR_    = f_var.get_sigmaR();
    float alfa_min   = f_var.get_alfa_min();
    float alfa_max   = f_var.get_alfa_max();
    float alfa_      = f_var.get_alfa();
    float A_min      = f_var.get_A_min();
    float A_max      = f_var.get_A_max();
    float A_         = f_var.get_A();
    float fit_min    = f_var.get_fit_min();
    float fit_max    = f_var.get_fit_max();

    //std::cout << mu_min << " " << mu_max << " " << mu_ << std::endl;
    //std::cout << fit_min << " " << fit_max << std::endl;
    
    TF1 *cruijff  = new TF1(Form("cru_%d", volts[i]), Cruijff, fit_min, fit_max, 5);

    //parametri del fit
    cruijff->SetParNames("#mu","#sigma_{L}", "#sigma_{R}", "#alpha", "A");
    cruijff->SetParLimits(0,    mu_min,    mu_max);
    cruijff->SetParLimits(1,sigmaL_min,sigmaL_max); 
    cruijff->SetParLimits(2,sigmaR_min,sigmaR_max);
    cruijff->SetParLimits(3,  alfa_min,  alfa_max);
    cruijff->SetParLimits(4,     A_min,     A_max);

    //gaussiana asimmetrica e non cruijff
    cruijff->FixParameter(3,0); //alfa value
    cruijff->SetNpx(1000);
    cruijff->SetParameters(mu_, sigmaL_, sigmaR_, alfa_, A_); //parametri iniziali

    //disegno dell'histo con funzione definita in graphics.h
    
    settings.graphic_histo(histo,range_min,range_max,x_name.c_str(),y_name.c_str(),volts[i]);
    settings.general_style();
    histo->SetLineWidth(3);
    //gStyle->SetOptFit(1111);

    if(strcmp(choice,"amp"   )==0){ histo->GetXaxis()->SetTitle("Amplitude (V)");}
  if(strcmp(choice,"charge")==0){ histo->GetXaxis()->SetTitle("Charge (uC)");}
    histo->SetMarkerSize(2);
    histo->SetLineWidth(3);
    histo->Fit(cruijff,"rx");
    histo->SetNdivisions(510);
    histo->Draw(); //"pe" per avere points+errors
    
    cruijff->SetLineColor(kRed-4);
    cruijff->SetLineWidth(5);
    cruijff->SetNpx(1000);
    cruijff->Draw("same");
    
    std::string outdir( Form("plots/CD%d/%s/%dV", CD_number, misura, volts[i]));
    system( Form("mkdir -p %s", outdir.c_str()) );
    c1->SaveAs(Form("plots/CD%d/%s/%dV/%s_fit_%dbins_final.png",CD_number,misura,volts[i],choice,nbins));

      
    //estrazione parametri dal fit
    float mu       = cruijff->GetParameter(0);
    float muerr    = cruijff->GetParError(0);
    float sigmaR    = cruijff->GetParameter(2);
    float sigmaRerr = cruijff->GetParError(2);
    float sigmaL    = cruijff->GetParameter(1);
    float sigmaLerr = cruijff->GetParError(1);
      
    //l'errore sulla stabilità di ites-ibias è insito nei segnali che otteniamo
   
    //grafico media vs voltaggio
    gr_mu->SetPoint( i, volts[i], mu );
    gr_mu->SetPointError( i, 0., muerr );
    
    //scrittura su file param.txt
    ofs << volts[i] << " " << mu << " " << muerr
	<< " " << sigmaR << " " << sigmaRerr 
        << " " << sigmaL << " " << sigmaLerr <<std::endl;
    
    float reso = sigmaR/mu*(volts[i]-phi_tes);
    //correggo risoluzione togliendo in quadratura enegy spread di 0.1eV
    //float reso_corr = sqrt( reso*reso - 0.1*0.1 );
    float reso_err = sqrt( sigmaRerr*sigmaRerr/(mu*mu) + sigmaR*sigmaR*muerr*muerr/(mu*mu*mu*mu) )*(volts[i]-phi_tes);
    //reso_err = sqrt( reso_err*reso_err + 0.15*0.15 );
    // the 0.15 V added in quadrature comes from the precision of the Keithley power supply

    float energy_ele = volts[i] - phi_carbon + (phi_carbon-phi_tes);

    gr_reso->SetPoint( i, energy_ele, reso );
    gr_reso->SetPointError( i, 0., reso_err );

    //gr_reso_corr->SetPoint( i, energy_ele, reso_corr );
    //gr_reso_corr->SetPointError( i, 0., reso_err );
    
    std::cout << "E = " << volts[i]-phi_tes << " sigma = " << reso << " +/- " << reso_err << std::endl;
    
    //gr_reso_fwhm->SetPoint( i, energy_ele, reso*2*sqrt(2*log(2)) );
    //gr_reso_fwhm->SetPointError( i, 0., reso_err*2*sqrt(2*log(2)) );
    
    
  } //for sui voltaggi vari

  ofs.close();

  c1->Clear();
  c1->SetLeftMargin(0.185);
  
  c1->cd();

  //grafico mu dei picchi
  gr_mu->GetXaxis()->SetLabelSize(0.04);
  gr_mu->GetXaxis()->SetTitleSize(0.04);
  gr_mu->GetYaxis()->SetLabelSize(0.04);
  gr_mu->GetYaxis()->SetTitleSize(0.04);
 
  gr_mu->SetTitle("#mu");
  gr_mu->GetXaxis()->SetTitle("Voltage (V)");
  if(strcmp(choice,"amp"   )==0){ gr_mu->GetYaxis()->SetTitle("Amplitude (V)");}
  if(strcmp(choice,"charge")==0){ gr_mu->GetYaxis()->SetTitle("Charge (uC)");}
  gr_mu->SetMarkerSize(3);
  gr_mu->SetLineWidth(4);
  gr_mu->SetMarkerStyle(20);
  gr_mu->SetMarkerColor(46);
  gr_mu->SetLineColor(46);
  //gr_mu->GetYaxis()->SetRangeUser(0.643,0.665);
  gStyle->SetTitleFontSize(0.07);
  gr_mu->Draw("APE");
  c1->SaveAs(Form("plots/CD%d/%s/mu_%s.png",CD_number,misura,choice));
  c1->Clear();

  //gr_mu->GetYaxis()->SetRangeUser(0,(gr_mu->GetHistogram()->GetMaximum())+0.12);
  gr_mu->GetYaxis()->SetRangeUser(0,(gr_mu->GetHistogram()->GetMaximum())+0.05);
  gr_mu->Draw("APE");
  c1->SaveAs(Form("plots/CD%d/%s/mu_%s_fp.png",CD_number,misura,choice));
  c1->Clear();
  
  //grafico risoluzione
  //preparazine tela
  float xmin_reso = volts[0] - phi_tes - 1.;
  float xmax_reso = volts[volts.size()-1] - phi_tes + 1.;
  float ymax_reso = GetYMax(CD_number, choice, misura);
 
  
  TH2D* h2_axes = new TH2D("axes_reso", "", 10, xmin_reso, xmax_reso, 10, 0., ymax_reso);
  h2_axes->GetXaxis()->SetLabelSize(0.046);
  h2_axes->GetXaxis()->SetTitleSize(0.048);
  h2_axes->GetYaxis()->SetLabelSize(0.046);
  h2_axes->GetYaxis()->SetTitleSize(0.048);
  h2_axes->GetYaxis()->SetTitleOffset(1.5);
  h2_axes->GetXaxis()->SetTitleOffset(1.4);
  //h2_axes->SetTitle("Energy resolution");
  h2_axes->GetXaxis()->SetTitle("Electron kinetic energy (eV)");
  h2_axes->GetYaxis()->SetTitle("TES Gaussian energy resolution (eV)");
  h2_axes->Draw();

  gr_reso->SetMarkerSize(4);
  gr_reso->SetLineWidth(4);
  gr_reso->SetMarkerStyle(20);
  gr_reso->SetMarkerColor(46);
  gr_reso->SetLineColor(46);
  gStyle->SetTitleFontSize(0.07);
  gr_reso->Draw("PESame");

  //TF1* f = new TF1("f","pol1",92,106,2);
  //gr_reso->Fit("pol1","m");
  //f->Draw("same");

  c1->SaveAs(Form("plots/CD%d/%s/reso_%s_nofit.png",CD_number,misura,choice));
  c1->Clear();

  /*
  title = "Energy resolution (FWHM)";
  yname = "Resolution (eV)";
  MakeGraphErrors(gr_reso_fwhm,title,xname,yname);
  c1->SaveAs(Form("plots/CD%d/reso_fwhm.png",CD_number));
  c1->Clear();
  */
  
  return 0;

}

//+++++++++++++++++++++++++++++++++++++++++++//
//++++++++++++++FUNZIONI UTILI+++++++++++++++//
//+++++++++++++++++++++++++++++++++++++++++++//

//funzione di controllo sui parametri iniziali
void Input_Param_Ctrls(int argc, char* argv[]){
  //ctrl sull'input inserito
 if (argc!=4){
    std::cout << "USAGE: ./drawFinalFits [CD_number] [misura] [choice]" << std::endl;
    std::cout << "[choice] : amp/charge" << std::endl; 
    std::cout << "EXAMPLE: ./drawFinalFits 204 B60_preamp_post_cond1 amp" << std::endl;
    exit(1);
  }

  //ctrl su scelta amp/charge
  if(strcmp(argv[3],"amp"   )!=0 &&
     strcmp(argv[3],"charge")!=0 &&
     (atoi(argv[1]))==188 && (atoi(argv[1]))==204){
    std::cout << "Wrong choice! Choose between 'amp' and 'charge'" << std::endl;
    exit(1);
    }

  //ctrl su scelta amp/charge
  if(strcmp(argv[3],"amp"                    )!=0 &&
     strcmp(argv[3],"charge"                 )!=0 &&
     strcmp(argv[3],"amp_10kHz"              )!=0 &&
     strcmp(argv[3],"amp_100kHz"             )!=0 &&
     strcmp(argv[3],"amp_10kHz_100kHz"       )!=0 &&
     strcmp(argv[3],"amp_10kHz_160kHz"       )!=0 &&
     strcmp(argv[3],"amp_10kHz_100kHz_160kHz")!=0 &&
     (atoi(argv[1]))==188){
    std::cout << "Wrong choice! Choose between 'amp','amp_10kHz','amp_100kHz','amp_10kHz_100kHz','amp_10kHz_160kHz' and 'charge'" << std::endl;
    exit(1);
    }

  //ctrl su scelta amp/charge
  if(strcmp(argv[3],"amp"             )!=0 &&
     strcmp(argv[3],"charge"          )!=0 &&
     strcmp(argv[3],"amp_10kHz"       )!=0 &&
     strcmp(argv[3],"amp_10kHz_160kHz")!=0 &&
     (atoi(argv[1]))==204){
    std::cout << "Wrong choice! Choose between 'amp','amp_10kHz', 'amp_10kHz_160kHz' and 'charge'" << std::endl;
    exit(1);
    }

   //ctrl su scelta amp/charge
  if(strcmp(argv[3],"amp"             )!=0 &&
     strcmp(argv[3],"charge"          )!=0 &&
     strcmp(argv[3],"amp_10kHz"       )!=0 &&
     strcmp(argv[3],"amp_100kHz"      )!=0 &&
     strcmp(argv[3],"amp_10kHz_10kHz" )!=0 &&
     (atoi(argv[1]))==222){
    std::cout << "Wrong choice! Choose between 'amp','amp_10kHz', 'amp_10kHz_10kHz', 'amp_100kHz' and 'charge'" << std::endl;
    exit(1);
    }
  
}


//cruiff (che può diventare una gaussiana) da fittare
Double_t Cruijff(Double_t *x, Double_t *par){
  //par[0]=mean      par[1]=sigma_L    par[2]=sigma_R
  //par[3]=alpha_L   par[4]=A
  Double_t fitval = 0;
  Double_t arg_L  = 0;
  Double_t arg_R  = 0;
  Double_t num    = pow((x[0]-par[0]),2);
  Double_t denL   = 2*par[1]*par[1];
  Double_t denR   = 2*par[2]*par[2];
  Double_t asym   = par[3]*num;
  if ( denL + asym != 0 ){ arg_L = -num/(denL+asym); }
  if ( par[2]      != 0 ){ arg_R = -num/ denR      ; }

  if       ( x[0] < par[0] ){ fitval = par[4] * exp(arg_L); } //LEFT
  else if  ( x[0] > par[0] ){ fitval = par[4] * exp(arg_R); } //RIGHT

  return(fitval);
}

void MuErrPercentage(char* meas, std::vector<int> volts, float muerr_perc[volts.size()]){
  if(strcmp(meas,"B60_preamp_post_cond1")==0){
    muerr_perc[0]=0.0005;
    muerr_perc[1]=0.0044;
    muerr_perc[2]=0.0044;
    muerr_perc[3]=0.0034;
    muerr_perc[4]=0.0068;
  }

  if(strcmp(meas,"B60_post_cond3_moku")==0){
    muerr_perc[0]=0.0045;  muerr_perc[5]=0.0065;  muerr_perc[10]=0.0071; 
    muerr_perc[1]=0.0046;  muerr_perc[6]=0.0064;  muerr_perc[11]=0.0025;
    muerr_perc[2]=0.0026;  muerr_perc[7]=0.0027;  muerr_perc[12]=0.0019; 
    muerr_perc[3]=0.0047;  muerr_perc[8]=0.0085;  muerr_perc[13]=0.0036; 
    muerr_perc[4]=0.0033;  muerr_perc[9]=0.0091;  muerr_perc[14]=0.0119; 
  }

  else if (strcmp(meas,"B60_preamp_post_cond1")!=0 && strcmp(meas,"B60_post_cond3_moku")!=0){
    for(int i=0; i<volts.size(); i++){
      muerr_perc[i] = 0.001;
    }
  }
}
    


