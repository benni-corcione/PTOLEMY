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

  Histo h_var = SetHistoValues(CD_number,choice,misura);
    
  //impostazione per gli histos
  int nbins = h_var.get_nbins();
  std::string x_name = h_var.get_x_name();
  std::string y_name = h_var.get_y_name();
  
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
    //gStyle->SetOptFit(1111);
    
    histo->SetMarkerSize(2);
    histo->SetLineWidth(3);
    histo->Fit(cruijff,"RmX");
    histo->SetNdivisions(510);
    histo->Draw(); //"pe" per avere points+errors
    
    cruijff->SetLineColor(kRed-4);
    cruijff->SetLineWidth(4);
    cruijff->Draw("same");
    
    std::string outdir( Form("plots/CD%d/%s/%dV", CD_number, misura, volts[i]));
    system( Form("mkdir -p %s", outdir.c_str()) );
    c1->SaveAs(Form("plots/CD%d/%s/%dV/%s_fit_%dbins_final.png",CD_number,misura,volts[i],choice,nbins));

      
    //estrazione parametri dal fit
    float mu       = cruijff->GetParameter(0);
    float muerr    = cruijff->GetParError(0);
    float sigma    = cruijff->GetParameter(2);
    float sigmaerr = cruijff->GetParError(2);
      
    //errore sulla mu (muerr)
    //0.01*mu è errore di 1% sull'effettivo posizionamento in 35%Rn come working point, visto che andata e ritorno della curva di caratterizzazione non coincidono.
    muerr = sqrt( muerr*muerr + 0.01*0.01*mu*mu );
    
    //grafico media vs voltaggio
    gr_mu->SetPoint( i, volts[i], mu );
    gr_mu->SetPointError( i, 0., muerr );
    
    //scrittura su file param.txt
    ofs << volts[i] << " " << mu << " " << muerr
	<< " " << sigma << " " << sigmaerr << std::endl;
    
    float reso = sigma/mu*volts[i];
      //correggo risoluzione togliendo in quadratura enegy spread di 0.1eV
    //float reso_corr = sqrt( reso*reso - 0.1*0.1 );
    float reso_err = sqrt( sigmaerr*sigmaerr/(mu*mu) + sigma*sigma*muerr*muerr/(mu*mu*mu*mu) )*volts[i];
    //reso_err = sqrt( reso_err*reso_err + 0.15*0.15 );
    // the 0.15 V added in quadrature comes from the precision of the Keithley power supply

    float energy_ele = volts[i] - phi_carbon + (phi_carbon-phi_tes);

    gr_reso->SetPoint( i, energy_ele, reso );
    gr_reso->SetPointError( i, 0., reso_err );

    //gr_reso_corr->SetPoint( i, energy_ele, reso_corr );
    //gr_reso_corr->SetPointError( i, 0., reso_err );
    
    std::cout << "E = " << volts[i] << " sigma = " << reso << " +/- " << reso_err << std::endl;
    
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
  gr_mu->GetYaxis()->SetTitle("Amplitude (V)");
  gr_mu->SetMarkerSize(3);
  gr_mu->SetLineWidth(4);
  gr_mu->SetMarkerStyle(20);
  gr_mu->SetMarkerColor(46);
  gr_mu->SetLineColor(46);
  gStyle->SetTitleFontSize(0.07);
  gr_mu->Draw("APE");
  c1->SaveAs(Form("plots/CD%d/%s/mu_%s.png",CD_number,misura,choice));
  c1->Clear();

  gr_mu->GetYaxis()->SetRangeUser(0,(gr_mu->GetHistogram()->GetMaximum())+0.12);
  gr_mu->Draw("APE");
  c1->SaveAs(Form("plots/CD%d/%s/mu_%s_fp.png",CD_number,misura,choice));
  c1->Clear();
  
  //grafico risoluzione
  //preparazine tela
  float xmin_reso = volts[0] - phi_tes - 1.;
  float xmax_reso = volts[volts.size()-1] - phi_tes + 1.;
  float ymax_reso = GetYMax(CD_number, choice, misura);
  TH2D* h2_axes = new TH2D("axes_reso", "", 10, xmin_reso, xmax_reso, 10, 0., ymax_reso);
  h2_axes->GetXaxis()->SetLabelSize(0.04);
  h2_axes->GetXaxis()->SetTitleSize(0.04);
  h2_axes->GetYaxis()->SetLabelSize(0.04);
  h2_axes->GetYaxis()->SetTitleSize(0.04);
  h2_axes->SetTitle("Energy resolution");
  h2_axes->GetXaxis()->SetTitle("Nominal energy (eV)");
  h2_axes->GetYaxis()->SetTitle("Resolution (eV)");
  h2_axes->Draw();

  gr_reso->SetMarkerSize(3);
  gr_reso->SetLineWidth(3);
  gr_reso->SetMarkerStyle(20);
  gr_reso->SetMarkerColor(46);
  gr_reso->SetLineColor(46);
  gStyle->SetTitleFontSize(0.07);
  gr_reso->Draw("PESame");
  c1->SaveAs(Form("plots/CD%d/%s/reso_%s.png",CD_number,misura,choice));
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
     strcmp(argv[3],"charge")!=0 ){ 
    std::cout << "Wrong choice! Choose between 'amp' and 'charge'" << std::endl;
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




