#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <string>
#include <sstream>

#include "TLegend.h"
#include "TStyle.h"
#include "TColor.h"
#include "TCanvas.h"
#include "TH1F.h"

//stampa float in stringa come 0.0tot e non 0.0tot0000
template < typename Type > std::string to_str (const Type & t){
  std::ostringstream os;
  os << t;
  return os.str ();
}               


class Settings {
 public:

  Settings(){}
  ~Settings(){}
  
  static std::vector<int> colour(void);
  static void     general_style (void);
  static void     graphic_histo(TH1F* histo, float range_min, float range_max, const char* x_name, const char* y_name, int volt);
  static TLegend* setLegend(int      quadrant);
  static void     Margin   (TCanvas* c       );
  static double   Maximum  (TH1F*    histo   );
  static double   Binsize  (float    binsize );
};


//array of colours to be used with multiple lines graphs
std::vector<int> Settings::colour(void) {
  std::vector<int> colour;
  colour.push_back(899); //kPink-1
  colour.push_back(808); //kOrange+8
  colour.push_back(800); //korange0
  colour.push_back(416); //kGreen0
  colour.push_back(834); //kTeal-6
  colour.push_back(868); //kAzure+8
  colour.push_back(593); //kBlue-7
  colour.push_back(879); //kViolet-1
  colour.push_back(907); //kPink+7
  colour.push_back(901); //kPink+1;
  colour.push_back(806); //kOrange+6
  colour.push_back(834); //kTeal-6
  colour.push_back(597); //kBlue-3
  colour.push_back(871); //kViolet-9
  colour.push_back(612); //kMagenta-4
  colour.push_back(625); //kRed-7
  return colour;
}

//posizione e grandezza legenda
TLegend* Settings::setLegend(int quadrant) {
  TLegend* legend;
  //x1,y1 (su sx); x2,y2 (giù dx)
  //quadranti come circonferenza goniometrica
  if(quadrant==1) legend = new TLegend(0.72, 0.90, 1.06, 0.67);
  if(quadrant==2) legend = new TLegend(0.18, 0.90, 0.43, 0.65);
  if(quadrant==3) legend = new TLegend(0.12, 0.22, 0.29, 0.12);
  if(quadrant==4) legend = new TLegend(0.70, 0.25, 0.99, 0.12);
  legend->SetBorderSize(0);
  legend->SetFillStyle(0);

  return legend;
}


double Settings::Maximum(TH1F* histo){
  return (histo->GetBinContent(histo->GetMaximumBin()))*1.1;
}


//margine della tela
void Settings::Margin(TCanvas* c){
  c->SetTopMargin(0.09);
  c->SetBottomMargin(0.13);//0.13);
  c->SetLeftMargin(0.14);//0.15);
  c->SetRightMargin(0.06);//0.02);
}


//nomi, font e punti
void Settings::graphic_histo(TH1F* histo, float range_min, float range_max, const char* x_name, const char* y_name, int volt){
  histo->GetXaxis()->SetRangeUser(range_min, range_max);
  histo->GetXaxis()->SetTitle(x_name);
  histo->GetYaxis()->SetTitle(y_name);
  gStyle->SetTitleFontSize(0.06);

  histo->GetXaxis()->SetLabelSize(0.04);
  histo->GetXaxis()->SetTitleSize(0.04);
  histo->GetYaxis()->SetLabelSize(0.04);
  histo->GetYaxis()->SetTitleSize(0.04);
  histo->SetTitle(Form("%d V",volt));
 
  histo->SetMarkerSize(1.5);
  histo->SetMarkerStyle(20);
  histo->SetLineColor(kBlack);
  histo->SetLineWidth(2);

}


void Settings::general_style(void){
  gStyle->SetFrameLineWidth(2);
  gStyle->SetOptStat(0);
}

//credo per avere una cifra significativa alla binsize in legenda in y
double Settings::Binsize (float binsize){
  float size = 0;
  float factor = 1;
  if (binsize > 0.0001 && binsize < 0.000999){ factor = 1e+4;}
  if (binsize > 0.0010 && binsize < 0.009990){ factor = 1e+3;}
  if (binsize > 0.0100 && binsize < 0.099900){ factor = 1e+2;}
  if (binsize > 0.1000 && binsize < 0.999000){ factor = 1e+1;}
  size = round(binsize*factor);
  size/= factor;
  return(size);
}


//CLASS OF HISTO VARIABLES
class Histo{
 public:
  //constructors and destructos
  Histo(){}
  ~Histo(){}

  //getters
  int   get_nbins     (void) const { return nbins;     }
  float get_hist_min  (void) const { return hist_min;  }
  float get_hist_max  (void) const { return hist_max;  }
  float get_range_min (void) const { return range_min; }
  float get_range_max (void) const { return range_max; }
  std::string get_x_name (void) const { return x_name; }
  std::string get_y_name (void) const { return y_name; }

  //setters
  void set_nbins     (int   a) { nbins     = a; }
  void set_range_min (float b) { range_min = b; }
  void set_hist_max  (float d) { hist_max = d; }
  void set_hist_min  (float e) { hist_min = e; }
  void set_range_max (float c) { range_max = c; }
  void set_x_name (std::string f) { x_name = f; }
  void set_y_name (std::string g) { y_name = g; }

 private:
  int   nbins;
  float range_min;
  float range_max;
  float hist_min;
  float hist_max;
  std::string x_name;
  std::string y_name;
};



//CLASS OF FIT VARIABLES
class Fit{
 public:
  //constructors and destructos
  Fit(){}
  ~Fit(){}

  //getters
  float get_mu_min    (void) const { return mu_min;    }
  float get_mu_max    (void) const { return mu_max;    }
  float get_mu        (void) const { return mu;        }
  float get_sigmaL_min (void) const { return sigmaL_min;}
  float get_sigmaL_max (void) const { return sigmaL_max;}
  float get_sigmaL     (void) const { return sigmaL;    }
  float get_sigmaR_min (void) const { return sigmaR_min; }
  float get_sigmaR_max (void) const { return sigmaR_max; }
  float get_sigmaR     (void) const { return sigmaR;     }
  float get_alfa_min  (void) const { return alfa_min;  } 
  float get_alfa_max  (void) const { return alfa_max;  }
  float get_alfa      (void) const { return alfa;      }
  float get_A_min     (void) const { return A_min;     }
  float get_A_max     (void) const { return A_max;     }
  float get_A         (void) const { return A;         }
  float get_fit_min   (void) const { return fit_min;   }
  float get_fit_max   (void) const { return fit_max;   }

  //setters
  void set_mu_min    (float a) { mu_min    = a; }
  void set_mu_max    (float b) { mu_max    = b; }
  void set_mu        (float c) { mu        = c; }
  void set_sigmaL_min (float d) { sigmaL_min = d; }
  void set_sigmaL_max (float e) { sigmaL_max = e; }
  void set_sigmaL     (float f) { sigmaL     = f; }
  void set_sigmaR_min (float g) { sigmaR_min = g; }
  void set_sigmaR_max (float h) { sigmaR_max = h; }
  void set_sigmaR     (float i) { sigmaR     = i; }
  void set_alfa_min  (float j) { alfa_min  = j; }
  void set_alfa_max  (float k) { alfa_max  = k; }
  void set_alfa      (float l) { alfa      = l; }
  void set_A_min     (float m) { A_min     = m; }
  void set_A_max     (float n) { A_max     = n; }
  void set_A         (float o) { A         = o; }
  void set_fit_min   (float p) { fit_min   = p; }
  void set_fit_max   (float q) { fit_max   = q; }

 private:
  float    mu_min; float    mu_max; float mu   ;
  float sigmaL_min; float sigmaL_max; float sigmaL;
  float sigmaR_min; float sigmaR_max; float sigmaR;
  float  alfa_min; float  alfa_max; float alfa ;
  float     A_min; float     A_max; float A    ;
  float fit_min;
  float fit_max;
 
};

