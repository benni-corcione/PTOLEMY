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

void Input_Param_Ctrls(int argc, char* argv[]);
void InitializeVectors(std::vector<float>& reso_fwhmpaper,
		       std::vector<float>& reso_fwhm_errpaper,
                       std::vector<float>& mu_paper,
		       std::vector<float>& mu_errpaper,
		       int CD_number, char* choice);


int main(int argc, char* argv[]) {

  //controlli iniziali su variabili da input
  Input_Param_Ctrls(argc,argv);

  //parametri da input
  int   CD_number = (atoi(argv[1])); //cooldown
  char* misura    =       argv[2]  ;
  char* choice    =       argv[3]  ;
  int   nbins     = (atoi(argv[4]));

  std::vector<int> volts = GetVoltages(CD_number,misura);
  std::vector<float> syst_fwhm;
  std::vector<float> syst_fwhm_err;
  std::vector<float> reso_fwhmpaper;
  std::vector<float> reso_fwhm_errpaper;
  std::vector<float> mu_paper;
  std::vector<float> mu_errpaper;

  InitializeVectors(reso_fwhmpaper, reso_fwhm_errpaper, mu_paper, mu_errpaper, CD_number, choice);
  
  Settings settings;
  TCanvas* c1 = new TCanvas( "c1", "", 1500, 1500 );
  settings.Margin(c1);
  c1->SetLeftMargin(0.155);
  c1->cd();

  float phi_tes = GetPhiTes();

  //scrittura su file
  std::string filename = "data/systfwhm_CD" + std::to_string(CD_number)
    + std::string(misura) + std::string(choice) + ".txt";
  std::ofstream ofs(filename);
  ofs << "V (V) E (eV) E_err (V) mu (V)  mu_err (V) fwhm (V) fwhm_err (V) reso_fwhm (eV)reso_fwhm_err (eV)" << std::endl;

  
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
    float binsize = settings.Binsize(1./nbins);
    const char* x_amp = "Amplitude [V]";
    std::string y_amp = "Counts / " + to_str(binsize) + " V" ;
 
    float hist_min  = h_var.get_hist_min();
    float hist_max  = h_var.get_hist_max();
    float range_min;
    float range_max;

    if(CD_number==204){ range_min = h_var.get_range_min(); range_max = h_var.get_range_max(); }
    if(CD_number==188 && strcmp(choice,"amp_100kHz")==0){
      range_min = 0.2; range_max = 0.8; }
     if(CD_number==188 && strcmp(choice,"amp")==0){
      range_min = 0.2; range_max = 1; }
    	     
    //creo un histo per ogni tree
    std::string histname(Form("%d", volts[i]));

    if(nbins==92  && volts[i]>99 ){nbins=202;}
    if(nbins==202 && volts[i]<100){nbins=92 ;}
    if(nbins==150 && volts[i]>99 ){nbins=350;}
    if(nbins==350 && volts[i]<100){nbins=150;}
    TH1F* histo = new TH1F( histname.c_str(), "", nbins, hist_min, hist_max);
    Long64_t nentries = tree->GetEntries();

     
    for(int iEntry=0; iEntry<nentries; iEntry++){
      tree->GetEntry(iEntry);
      histo->Fill(variable);
    }
    
    histo->GetXaxis()->SetTitle("Amplitude (V)");
    histo->SetMarkerSize(2);  
    histo->SetNdivisions(510);
    histo->Draw(); //"pe" per avere points+errors
    settings.graphic_histo(histo,range_min,range_max,x_amp,y_amp.c_str(),volts[i]);
    settings.general_style();
    histo->SetLineWidth(3);

    //risoluzione energetica gaussiana cb
    float energy_ele = volts[i] -phi_tes;
    float energy_err = sqrt(0.0009 + pow((0.04+0.0015*volts[i]),2));

    //aggiunta fwhm
    int first_bin = nbins/1.7; //cd 204
    int last_bin  = nbins/1.4; //cd 204
    
    //aggiunta fwhm
    int first_bin_min, last_bin_min, first_bin_max, last_bin_max;
    //paper filtrato
    if(nbins==92 && strcmp(choice,"amp_100kHz")==0){
      first_bin_min = 40 ; last_bin_min = 60 ;
      first_bin_max = 60 ; last_bin_max = 90 ; }
    if(nbins==202 && strcmp(choice,"amp_100kHz")==0){
      first_bin_min = 105; last_bin_min = 120;
      first_bin_max = 120; last_bin_max = 180; }
    if(nbins==250 && strcmp(choice,"amp_100kHz")==0){
      first_bin_min = 115; last_bin_min = 160;
      first_bin_max = 160; last_bin_max = 250;
      if(volts[i]==104)   first_bin_min = 135; }
    if(nbins==150 && strcmp(choice,"amp_100kHz")==0){
      first_bin_min = 75 ; last_bin_min = 95 ;
      first_bin_max = 95 ; last_bin_max = 150; }
    if(nbins==350 && strcmp(choice,"amp_100kHz")==0){
      first_bin_min = 180; last_bin_min = 200;
      first_bin_max = 250; last_bin_max = 350;
      if(volts[i]==105)   first_bin_min = 190; }

    if(nbins==200){
      first_bin_min = 100; last_bin_min  = 130; 
      first_bin_max = 150; last_bin_max  = 200; }
    if(nbins==100){
      first_bin_min = 50 ; last_bin_min  = 65 ; 
      first_bin_max = 75 ; last_bin_max  = 100; }
    if(nbins==300){
      first_bin_min = 170; last_bin_min  = 195; 
      first_bin_max = 225; last_bin_max  = 300;
      if(volts[i]<97 || volts[i]==98)   first_bin_min = 150  ; }
    
    int bin1, bin2;
    
    float h_mezzi  = histo->GetMaximum()/2;
    if(CD_number==204){
      bin1 = histo->FindFirstBinAbove(h_mezzi,1,first_bin,last_bin);
      bin2 = histo->FindLastBinAbove (h_mezzi,1,first_bin,last_bin);
    }
    if(CD_number==188){
      bin1 = histo->FindFirstBinAbove(h_mezzi,1,first_bin_min,last_bin_min);
      bin2 = histo->FindLastBinAbove (h_mezzi,1,first_bin_max,last_bin_max);
    }

   
    
    float fwhm     = histo->GetBinCenter(bin2) - histo->GetBinCenter(bin1);
    float fwhm_err = 1./(nbins*sqrt(3)); //errore statistico = 1 bin con 700 bin
     
    float piece1_f  = fwhm_err*fwhm_err*energy_ele*energy_ele/(mu_paper[i]*mu_paper[i]);
    float piece2_f  = fwhm*fwhm*mu_errpaper[i]*mu_errpaper[i]*energy_ele*energy_ele/(mu_paper[i]*mu_paper[i]*mu_paper[i]*mu_paper[i]);
    float piece3_f  = fwhm*fwhm*energy_err*energy_err/(mu_paper[i]*mu_paper[i]);
    float reso_fwhm = fwhm/mu_paper[i]*(energy_ele);
    float reso_fwhm_err = sqrt(piece1_f+piece2_f+piece3_f);

    
    
    std::cout << "fwhm: " << fwhm << std::endl;
    std::cout << "E = " << volts[i]-phi_tes << " DE_fwhm = " << reso_fwhm << " +/- " << reso_fwhm_err << std::endl;
    

    TLine *l1 = new TLine(histo->GetBinCenter(bin1), 0, histo->GetBinCenter(bin1), histo->GetMaximum());
    l1->SetLineWidth(4);
    l1->SetLineColor(64);
    l1->Draw("same");

    TLine *l2 = new TLine(histo->GetBinCenter(bin2), 0, histo->GetBinCenter(bin2), histo->GetMaximum());
    l2->SetLineWidth(4);
    l2->SetLineColor(64);
    l2->Draw("same");

    
    TLine *l3 = new TLine(range_min, h_mezzi, range_max, h_mezzi);
    l3->SetLineWidth(4);
    l3->SetLineColor(96);
    l3->Draw("same");

    std::string outdir( Form("plots/CD%d/%s/%dV", CD_number, misura, volts[i]));
    system( Form("mkdir -p %s", outdir.c_str()) );
    c1->SaveAs(Form("plots/CD%d/%s/%dV/systfwhm_%s%dbins.png",CD_number,misura,volts[i],choice,nbins));
    
    //scrittura su file
    ofs << volts[i] << " " << energy_ele << " "
	<< energy_err  << " " 
	<< mu_paper[i] << " " << mu_errpaper[i]  << " "
	<< fwhm << " " << fwhm_err << " "
	<< reso_fwhm << " " << reso_fwhm_err << std::endl;

    float systfwhm = abs(reso_fwhm-reso_fwhmpaper[i])/(1.*reso_fwhmpaper[i]);
    syst_fwhm.push_back(systfwhm);
    float systfwhm_err = sqrt(((reso_fwhm_err*reso_fwhm_err)/(1.*reso_fwhmpaper[i]*reso_fwhmpaper[i]))+(reso_fwhm*reso_fwhm*reso_fwhm_errpaper[i]*reso_fwhm_errpaper[i])/(1.*reso_fwhmpaper[i]*reso_fwhmpaper[i]*reso_fwhmpaper[i]*reso_fwhmpaper[i]));
    syst_fwhm_err.push_back(systfwhm_err);
  } //for sui voltaggi vari

  ofs.close();
    
  std::cout << std::endl;
  std::cout << "DISCREPANZA FWHM SU NBINS = " << nbins << std::endl;

  float mean = 0;
  for(int j=0; j<syst_fwhm.size(); j++){
    std::cout << volts[j] << " -> " << syst_fwhm[j]*100 << " +- " << syst_fwhm_err[j]*100 << std::endl;
    mean += syst_fwhm[j]*100;
  }
  std::cout << std::endl;
  std::cout << "DISCREPANZA MEDIA: " << mean/volts.size() << std::endl;
  

  return 0;

}

//+++++++++++++++++++++++++++++++++++++++++++//
//++++++++++++++FUNZIONI UTILI+++++++++++++++//
//+++++++++++++++++++++++++++++++++++++++++++//

//funzione di controllo sui parametri iniziali
void Input_Param_Ctrls(int argc, char* argv[]){
  //ctrl sull'input inserito
  if (argc!=5){
    std::cout << "USAGE: ./SystematicFWHM [CD_number] [misura] [choice] [nbins]" << std::endl;
    std::cout << "[choice] : amp/charge" << std::endl; 
    std::cout << "EXAMPLE: ./SystematicFWHM 204 B60_preamp_post_cond1 amp 200" << std::endl;
    exit(1);
  }

  //ctrl su scelta amp/charge
  if(strcmp(argv[3],"amp"   )!=0 && (atoi(argv[1]))==204){
    std::cout << "Wrong choice! Choose 'amp' for CD 204" << std::endl;
    exit(1);
  }

  //ctrl su scelta amp/charge
  if(strcmp(argv[3],"amp"                    )!=0 &&
     strcmp(argv[3],"amp_100kHz"             )!=0 &&
     (atoi(argv[1]))==188){
    std::cout << "Wrong choice! Choose between 'amp' and 'amp_100kHz'" << std::endl;
    exit(1);
  }
  
  if((atoi(argv[4])!=92  && atoi(argv[4])!=202 && atoi(argv[4])!=250 &&
      atoi(argv[4])!=150 && atoi(argv[4])!=350) && strcmp(argv[3], "amp_100kHz")==0){
    std::cout<< "Wrong choice. CD188 filtered can only accept 92, 202, 250, 150 or 350 bins in thi program" << std::endl;
    exit(1);
  }

  if((atoi(argv[4])!=200  && atoi(argv[4])!=100 && atoi(argv[4])!=300)
     && strcmp(argv[3], "amp")==0 && atoi(argv[1])==188){
    std::cout<< "Wrong choice. CD188 can only accept 100, 200 and 300 bins in thi program" << std::endl;
    exit(1);
  }

  if((atoi(argv[4])!=350  && atoi(argv[4])!=700 && atoi(argv[4])!=1400)
      && atoi(argv[1])==204){
    std::cout<< "Wrong choice. CD204 can only accept 350, 700 and 1400 bins in thi program" << std::endl;
    exit(1);
  }
  
  
}




void InitializeVectors(std::vector<float>& reso_fwhmpaper,
		       std::vector<float>& reso_fwhm_errpaper,
                       std::vector<float>& mu_paper,
		       std::vector<float>& mu_errpaper,
		       int CD_number, char* choice){
  
  if(CD_number == 204){
    //vettore fwhm paper
    reso_fwhmpaper.push_back(1.28376);
    reso_fwhmpaper.push_back(1.29258);
    reso_fwhmpaper.push_back(1.52224);
    reso_fwhmpaper.push_back(1.09801);
    reso_fwhmpaper.push_back(1.32711);
    reso_fwhmpaper.push_back(1.33562);
    reso_fwhmpaper.push_back(1.12533);
    reso_fwhmpaper.push_back(1.13325);

    //vettore fwhm error paper
    reso_fwhm_errpaper.push_back(0.12356) ;
    reso_fwhm_errpaper.push_back(0.124408);
    reso_fwhm_errpaper.push_back(0.125593);
    reso_fwhm_errpaper.push_back(0.126808);
    reso_fwhm_errpaper.push_back(0.12773) ;
    reso_fwhm_errpaper.push_back(0.12855) ;
    reso_fwhm_errpaper.push_back(0.129962);
    reso_fwhm_errpaper.push_back(0.130877);

    //vettore mu paper
    mu_paper.push_back(0.611738);
    mu_paper.push_back(0.614193);
    mu_paper.push_back(0.615021);
    mu_paper.push_back(0.615534);
    mu_paper.push_back(0.617589);
    mu_paper.push_back(0.62007) ;
    mu_paper.push_back(0.619636);
    mu_paper.push_back(0.621606);

    //vettore mu error paper
    mu_errpaper.push_back(0.000398273);
    mu_errpaper.push_back(0.000322736);
    mu_errpaper.push_back(0.000322736);
    mu_errpaper.push_back(0.000255192);
    mu_errpaper.push_back(0.000256268);
    mu_errpaper.push_back(0.000283282);
    mu_errpaper.push_back(0.000296391);
    mu_errpaper.push_back(0.000243065);

  } // ciclo CD 204

  
  if(CD_number == 188 && strcmp(choice,"amp_100kHz")==0){
    //vettore fwhm paper
    reso_fwhmpaper.push_back(41.2505);
    reso_fwhmpaper.push_back(40.1976);
    reso_fwhmpaper.push_back(38.5528);
    reso_fwhmpaper.push_back(32.987 );
    reso_fwhmpaper.push_back(30.9088);
    reso_fwhmpaper.push_back(28.7483);
    reso_fwhmpaper.push_back(32.3922);
    reso_fwhmpaper.push_back(36.7662);
    reso_fwhmpaper.push_back(37.1332);
    reso_fwhmpaper.push_back(34.8901);
    reso_fwhmpaper.push_back(37.7904);

    //vettore fwhm error paper
    reso_fwhm_errpaper.push_back(1.01395 );
    reso_fwhm_errpaper.push_back(1.0261  );
    reso_fwhm_errpaper.push_back(1.02779 );
    reso_fwhm_errpaper.push_back(0.983074);
    reso_fwhm_errpaper.push_back(0.983289);
    reso_fwhm_errpaper.push_back(0.528369);
    reso_fwhm_errpaper.push_back(0.541597);
    reso_fwhm_errpaper.push_back(0.566814);
    reso_fwhm_errpaper.push_back(0.57095);
    reso_fwhm_errpaper.push_back(0.557835);
    reso_fwhm_errpaper.push_back(0.562146);

    //vettore mu paper
    mu_paper.push_back(0.6304);
    mu_paper.push_back(0.634231);
    mu_paper.push_back(0.626726);
    mu_paper.push_back(0.641663);
    mu_paper.push_back(0.638879);
    mu_paper.push_back(0.632298);
    mu_paper.push_back(0.649731);
    mu_paper.push_back(0.662483);
    mu_paper.push_back(0.662653);
    mu_paper.push_back(0.667175);
    mu_paper.push_back(0.706514);
    
    //vettore mu error paper
    mu_errpaper.push_back(0.00695614);
    mu_errpaper.push_back(0.00747296);
    mu_errpaper.push_back(0.00708774);
    mu_errpaper.push_back(0.00683898);
    mu_errpaper.push_back(0.00650729);
    mu_errpaper.push_back(0.00656163);
    mu_errpaper.push_back(0.0066055);
    mu_errpaper.push_back(0.0067054);
    mu_errpaper.push_back(0.00666634);
    mu_errpaper.push_back(0.00673959);
    mu_errpaper.push_back(0.00711128);
    
  } // ciclo CD 188 filtrato

  if(CD_number == 188 && strcmp(choice,"amp")==0){
    //vettore fwhm paper
    reso_fwhmpaper.push_back(29.4695);
    reso_fwhmpaper.push_back(32.7276);
    reso_fwhmpaper.push_back(26.3284);
    reso_fwhmpaper.push_back(33.6719);
    reso_fwhmpaper.push_back(31.4944);
    reso_fwhmpaper.push_back(22.357);
    reso_fwhmpaper.push_back(32.1226);
    reso_fwhmpaper.push_back(29.2829);
    reso_fwhmpaper.push_back(33.4843);
    reso_fwhmpaper.push_back(34.0894);
    reso_fwhmpaper.push_back(34.1084);

    //vettore fwhm error paper
    reso_fwhm_errpaper.push_back(0.425749);
    reso_fwhm_errpaper.push_back(0.463836);
    reso_fwhm_errpaper.push_back(0.418015);
    reso_fwhm_errpaper.push_back(0.473697);
    reso_fwhm_errpaper.push_back(0.464701);
    reso_fwhm_errpaper.push_back(0.413437);
    reso_fwhm_errpaper.push_back(0.477945);
    reso_fwhm_errpaper.push_back(0.467423);
    reso_fwhm_errpaper.push_back(0.529964);
    reso_fwhm_errpaper.push_back(0.565427);
    reso_fwhm_errpaper.push_back(0.62932);

    //vettore mu paper
    mu_paper.push_back(0.876398);
    mu_paper.push_back(0.839851);
    mu_paper.push_back(0.844301);
    mu_paper.push_back(0.848019);
    mu_paper.push_back(0.841225);
    mu_paper.push_back(0.812633);
    mu_paper.push_back(0.827168);
    mu_paper.push_back(0.83343);
    mu_paper.push_back(0.809957);
    mu_paper.push_back(0.818258);
    mu_paper.push_back(0.826012);
    
    //vettore mu error paper
    mu_errpaper.push_back(0.00884935);
    mu_errpaper.push_back(0.00857017);
    mu_errpaper.push_back(0.00858064);
    mu_errpaper.push_back(0.00865788);
    mu_errpaper.push_back(0.00871586);
    mu_errpaper.push_back(0.00840933);
    mu_errpaper.push_back(0.00856269);
    mu_errpaper.push_back(0.00903257);
    mu_errpaper.push_back(0.00945701);
    mu_errpaper.push_back(0.0105063);
    mu_errpaper.push_back(0.0125323);
    
  } // ciclo CD 188 paper
}
