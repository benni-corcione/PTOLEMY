#include <iostream>
#include <fstream>
#include <vector>
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


int main(void) {

  std::string file1 = "params_CD188conteggiamp_paper.txt";
  std::string file2 = "params_CD188conteggiamp_100kHz.txt";
  std::string file3 = "params_CD204B60_post_cond3_mokuamp.txt";
 
 
  std::vector<TGraphErrors*> gr_reso(3);
  std::vector<TGraphErrors*> gr_reso_fwhm(3);
  
  for(int j=0; j<3; j++){
    gr_reso[j]      = new TGraphErrors();
    gr_reso_fwhm[j] = new TGraphErrors();
  }
  
  Settings settings;
  TCanvas* c1 = new TCanvas( "c1", "", 1500, 1500 );
  settings.Margin(c1);
  c1->SetLeftMargin(0.155);

  std::vector<std::string> filename;
  filename.push_back("data/" + std::string(file1));
  filename.push_back("data/" + std::string(file2));
  filename.push_back("data/" + std::string(file3));
 
  std::vector<std::vector<int>>           volts(3);
  std::vector<std::vector<float>>        energy(3);
  std::vector<std::vector<float>>    energy_err(3);
  std::vector<std::vector<float>>            mu(3);
  std::vector<std::vector<float>>        mu_err(3);
  std::vector<std::vector<float>>            sR(3);
  std::vector<std::vector<float>>        sR_err(3);
  std::vector<std::vector<float>>            sL(3);
  std::vector<std::vector<float>>        sL_err(3);
  std::vector<std::vector<float>>     reso_gaus(3);
  std::vector<std::vector<float>> reso_gaus_err(3);
  std::vector<std::vector<float>>     reso_fwhm(3);
  std::vector<std::vector<float>> reso_fwhm_err(3);
  
  float phi_tes = GetPhiTes();

  //FOR PER LEGGERE E RIEMPIRE VETTORI
  for(int j=0; j<3; j++){
    std::ifstream infile(filename[j]); 
    std::string line;
  
    if(!infile.is_open()) {
      std::cerr << "cannot open input file" << std::endl;
      return -1;
    }

    getline(infile,line); //salto prima riga
   
    while(!infile.eof()) {

      int val0;
      float val1, val1b, val2, val3, val4, val5, val6, val7, val8, val9, val10, val11;

      getline(infile,line);
      if( line == "\n" || line == "") continue;
      sscanf(line.c_str(),"%d %f %f %f %f %f %f %f %f %f %f %f %f", &val0, &val1, &val1b, &val2, &val3, &val4, &val5, &val6, &val7, &val8, &val9, &val10, &val11);

      volts        [j].push_back(val0 );
      energy       [j].push_back(val1 );
      energy_err   [j].push_back(val1b);
      mu           [j].push_back(val2 );
      mu_err       [j].push_back(val3 );
      sR           [j].push_back(val4 );
      sR_err       [j].push_back(val5 );
      sL           [j].push_back(val6 );
      sL_err       [j].push_back(val7 );
      reso_gaus    [j].push_back(val8 );
      reso_gaus_err[j].push_back(val9 );
      reso_fwhm    [j].push_back(val10);
      reso_fwhm_err[j].push_back(val11);
    
    } // !eof
    infile.close();
  }
  
  int size[3] = {static_cast<int>(volts[0].size()),
		 static_cast<int>(volts[1].size()),
                 static_cast<int>(volts[2].size())};
   
  
   //calcolo di energia, risoluzione_sigma, errore risoluzione, risoluzione fwhm, errore risoluzione;
  for(int j=0; j<3; j++){
    for(int i=0; i<size[j]; i++) {
      float stat_gaus, syst_gaus, stat_fwhm, syst_fwhm;

      
      if(j==0){ //paper
	stat_gaus = reso_gaus_err[j][i];
	syst_gaus = 0;
	stat_fwhm = reso_fwhm_err[j][i];
	//syst_fwhm = 0;
	//syst_fwhm = 0.04 * reso_fwhm[j][i]; //4% da binnaggio e range - metodo 1
	syst_fwhm = 0.115 * reso_fwhm[j][i]; //11.5% da binning per fwhm - metodo 2
      }
      
      if(j==1){ //paper filtrato
	stat_gaus = reso_gaus_err[j][i];
	syst_gaus = 0;
	//syst_gaus = 0.02 * reso_gaus[j][i];
	stat_fwhm = reso_fwhm_err[j][i];
	//syst_fwhm = 0;
	//syst_fwhm = 0.04 * reso_fwhm[j][i]; //4% da binnaggio e range - metodo 1
	syst_fwhm = 0.115 * reso_fwhm[j][i]; //11.5% da binning per fwhm - metodo 2
      }

      if(j==2){ //new data
	stat_gaus = reso_gaus_err[j][i];
	syst_gaus = 0.11 * reso_gaus[j][i];
	stat_fwhm = reso_fwhm_err[j][i];
	//syst_fwhm = 0.102 * reso_fwhm[j][i]; //quadratura binning&range e CB fit - metodo 1
	syst_fwhm = 0.2 * reso_fwhm[j][i]; //20% da binning per fwhm - metodo 2
      }

      float reso_gaus_err_tot = sqrt(stat_gaus*stat_gaus+syst_gaus*syst_gaus);
      float reso_fwhm_err_tot = sqrt(stat_fwhm*stat_fwhm+syst_fwhm*syst_fwhm);
      gr_reso     [j]->SetPoint(i,energy[j][i],reso_gaus[j][i]);
      gr_reso     [j]->SetPointError(i,0,reso_gaus_err_tot);
      gr_reso_fwhm[j]->SetPoint(i,energy[j][i],reso_fwhm[j][i]);
      gr_reso_fwhm[j]->SetPointError(i,0,reso_fwhm_err_tot);
    
    }
  }

  c1->cd();

  gPad->SetTickx(1); // tick anche in alto
  gPad->SetTicky(1); // tick anche a destra
  //grafico risoluzione
  //preparazine tela
  
  float xmin_reso = 89.5 ;
  float xmax_reso = 102;
  float ymax_reso = 2.3;
  float ymax_fwhm = 70 ;

  TLegend* legend = new TLegend(0.5,0.70,1.23,0.89);
  legend->SetTextSize(0.038);
  legend->SetMargin   (0.1);
  legend->SetBorderSize (0);
  legend->SetFillStyle  (0);
  
  TH2D* h2_axes = new TH2D("axes_reso", "", 10, xmin_reso, xmax_reso, 10, 0, ymax_reso);
  gStyle->SetOptStat(0);
  h2_axes->GetXaxis()->SetLabelSize(0.042);
  h2_axes->GetXaxis()->SetTitleSize(0.042);
  h2_axes->GetYaxis()->SetLabelSize(0.042);
  h2_axes->GetYaxis()->SetTitleSize(0.042);
  h2_axes->GetYaxis()->SetTitleOffset(1.5);
  h2_axes->GetXaxis()->SetTitleOffset(1.3);
  h2_axes->GetXaxis()->SetTitle("Electron kinetic energy E_{e} (eV)");
  h2_axes->GetYaxis()->SetTitle("TES Gaussian energy resolution #DeltaE_{gaus} (eV)");
  h2_axes->GetYaxis()->SetNdivisions(510);
  h2_axes->Draw();
  h2_axes->GetYaxis()->SetNdivisions(510);
 
  gStyle->SetTitleFontSize(0.07);
  
  for(int i=0; i<3; i++){
    gr_reso[i]->SetMarkerSize  (5);
    gr_reso[i]->SetLineWidth   (3);
    gr_reso[i]->SetMarkerStyle(20);
    
    gr_reso[i]->GetYaxis()->SetNdivisions(510);
  }

  gr_reso[1]->SetMarkerStyle(89);
  gr_reso[2]->SetMarkerStyle(21);
  gr_reso[0]->SetMarkerStyle(20);
  gr_reso[0]->SetMarkerColor(46);
  gr_reso[0]->SetLineColor  (46);
  gr_reso[2]->SetMarkerColor(38);
  gr_reso[2]->SetLineColor  (38);
  gr_reso[1]->SetMarkerColor(kGray+1);
  gr_reso[1]->SetLineColor  (kGray+1);
 
  gr_reso[0]->Draw("PSame");
  gr_reso[1]->Draw("PSame");
  gr_reso[2]->Draw("PSame");
  legend->AddEntry(gr_reso[0], "Pepe et al. (2024)", "pe"  );
  //legend->AddEntry((TObject*)0, "Pepe et al (2024)", "");
  //legend->AddEntry(gr_reso[0], "T100, CNTs: 9 mm^{2}", "pe"  );
  legend->AddEntry(gr_reso[1], "Pepe et al. (2024)", "pe");
  legend->AddEntry((TObject*)0, "+ low-pass filter", "");
  legend->AddEntry(gr_reso[2], "This work", "pe");
  legend->Draw("same");

  //gPad->RedrawAxis();
   
  c1->SaveAs("/Users/massimo/Documents/phd/PTOLEMY/Articoli/Firmati/new article/graphs/reso_cfrall_metodo2.pdf"); 
  c1->Clear();
  legend->Clear();

  TH2D* h2_axes_fwhm = new TH2D("axes_fwhm", "", 10, xmin_reso, xmax_reso, 10, 0, ymax_fwhm);
  gStyle->SetOptStat(0);
  h2_axes_fwhm->GetXaxis()->SetLabelSize(0.042);
  h2_axes_fwhm->GetXaxis()->SetTitleSize(0.042);
  h2_axes_fwhm->GetYaxis()->SetLabelSize(0.042);
  h2_axes_fwhm->GetYaxis()->SetTitleSize(0.042);
  h2_axes_fwhm->GetYaxis()->SetTitleOffset(1.5);
  h2_axes_fwhm->GetXaxis()->SetTitleOffset(1.3);
  h2_axes_fwhm->GetXaxis()->SetTitle("Electron kinetic energy E_{e} (eV)");
  h2_axes_fwhm->GetYaxis()->SetTitle("TES FWHM energy resolution #DeltaE_{fwhm} (eV)");
  h2_axes_fwhm->Draw();
 
  
  gStyle->SetTitleFontSize(0.07);
  for(int i=0; i<3; i++){
    gr_reso_fwhm[i]->SetMarkerSize  (5);
    gr_reso_fwhm[i]->SetLineWidth   (3);
    gr_reso_fwhm[i]->SetMarkerStyle(20);
  }

  gr_reso_fwhm[1]->SetMarkerStyle(89);
  gr_reso_fwhm[2]->SetMarkerStyle(21);
  gr_reso_fwhm[0]->SetMarkerStyle(20);
  gr_reso_fwhm[0]->SetMarkerColor(46);
  gr_reso_fwhm[0]->SetLineColor  (46);
  gr_reso_fwhm[2]->SetMarkerColor(38);
  gr_reso_fwhm[2]->SetLineColor  (38);
  gr_reso_fwhm[1]->SetMarkerColor(kGray+1);
  gr_reso_fwhm[1]->SetLineColor  (kGray+1);
  
  gr_reso_fwhm[2]->Draw("PSame");
  gr_reso_fwhm[1]->Draw("PSame");
  gr_reso_fwhm[0]->Draw("PSame");
  legend->AddEntry(gr_reso_fwhm[0], "Pepe et al. (2024)", "pe"  );
  //legend->AddEntry((TObject*)0, "Pepe et al (2024)", "");
  //legend->AddEntry(gr_reso_fwhm[0], "T100, CNTs: 9 mm^{2}", "pe"  );
  legend->AddEntry(gr_reso_fwhm[1], "Pepe et al. (2024)", "pe");
  legend->AddEntry((TObject*)0, "+ low-pass filter", "");
  legend->AddEntry(gr_reso_fwhm[2], "This work", "pe");
  legend->Draw("same");

  TLine* line = new TLine(89.5, 1.4,102, 1.4 );
  line->SetLineColor(38);
  line->SetLineStyle( 2 );
  line->SetLineWidth( 3 );
  //line->Draw("same");

  TLatex* labelpepe = new TLatex( 99, 2.5, "1.5 eV" );
  labelpepe->SetTextSize( 0.042 );
  labelpepe->SetTextFont(42);   // font standard non bold
  labelpepe->SetTextColor(38);
  //labelpepe->Draw("same");

  gPad->RedrawAxis();
   
  c1->SaveAs("/Users/massimo/Documents/phd/PTOLEMY/Articoli/Firmati/new article/graphs/reso_cfrall_fwhm_metodo2.pdf"); 
  c1->Clear();

  // stampa media e deviazione standard per i 6 dataset

for(int j = 0; j < 3; j++){

    // calcolo media per gauss
    float sum_sigma  = 0;
    float mean_sigma = 0;
    
    for(size_t i = 0; i < reso_gaus[j].size(); i++){
      sum_sigma += reso_gaus[j][i];
    }
    mean_sigma = sum_sigma / reso_gaus[j].size();

    //calcolo std_dev per gauss
    float var_sigma = 0;
    float std_sigma = 0;
    for(size_t i = 0; i < reso_gaus[j].size(); i++){
      var_sigma += (reso_gaus[j][i] - mean_sigma)*(reso_gaus[j][i] - mean_sigma);
    }
    std_sigma = sqrt(var_sigma / (reso_gaus[j].size()-1));


    //calcolo media per fwhm
    float sum_fwhm = 0;
    float mean_fwhm = 0;
    for(size_t i = 0; i < reso_fwhm[j].size(); i++){
      sum_fwhm += reso_fwhm[j][i];
    }
    mean_fwhm = sum_fwhm / reso_fwhm[j].size();

    //calcolo std dev per fwhm
    float var_fwhm = 0;
    float std_fwhm = 0;
    for(size_t i = 0; i < reso_fwhm[j].size(); i++){
      var_fwhm += (reso_fwhm[j][i] - mean_fwhm)*(reso_fwhm[j][i] - mean_fwhm);
    }
    std_fwhm = sqrt(var_fwhm / (reso_fwhm[j].size()-1));


    std::cout << "Dataset " << j
              << "  sigma: mean=" << mean_sigma << " std=" << std_sigma
              << "  |  fwhm: mean=" << mean_fwhm << " std=" << std_fwhm
              << std::endl;
}

 
return 0;

}


//funzione di controllo sui parametri iniziali
void Input_Param_Ctrls(int argc, char* argv[]){
  //ctrl sull'input inserito
  if (argc!=3){
    std::cout << "USAGE: ./drawResolutionCFR [CD_number 1] [CD_number 2]" << std::endl; 
    std::cout << "EXAMPLE: ./drawResolutionCFR 188 204" << std::endl;
    std::cout << "[Per ora settato su 188 204 oppure 188 222]" << std::endl;
    
    exit(1);
  }
}

