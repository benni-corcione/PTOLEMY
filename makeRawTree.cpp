#include <iostream>
#include <stdlib.h>

#include "lecroyparser.h"
#include "AndCommon.h"

#include "TFile.h"
#include "TTree.h"
#include "TGraphErrors.h"




int main( int argc, char* argv[] ) {

  if( argc != 2 ) {

    std::cout << "USAGE: ./makeRawTree [fullpath_to_trcfile]" << std::endl;
    std::cout << "EXAMPLE: ./makeRawTree data/trc/CD188/104V/C1--PTL4eD_CNT_StrongEtching_mini_40_mK_51p841uA_m166mV_trg_104V--00057.trc" << std::endl;
    exit(1);
  }

  std::string fullPath(argv[1]);
  std::vector<std::string> dirs = AndCommon::splitString( fullPath, "/" );
  //ogni elemento di questo vettore è una parola del path e splitString dà sempre vettore di stringhe

  const char* meas = dirs[3].c_str();
  int cd = atoi(AndCommon::splitString(dirs[2],"CD")[0].c_str());
  int v  = atoi(AndCommon::splitString(dirs[4], "V")[0].c_str());


  std::string dir_trc(Form("data/trc/CD%d/%s/%dV", cd, meas, v));
  std::string fileName_trc(dirs[dirs.size()-1]);

  //std::cout << dir_trc << std::endl;
  std::string fileName = AndCommon::splitString( fileName_trc, "." )[0];

  //numero del file? tipo --00057? sì
  int lumiblock = atoi(AndCommon::splitString(fileName,"--")[2].c_str());

  std::string outdir( Form("data/root/CD%d/%s/%dV", cd, meas, v));
  system( Form("mkdir -p %s", outdir.c_str()) );
  //-p crea anche le parent se prima non esistono

  TFile* outfile = TFile::Open( Form("%s/%s.root", outdir.c_str(), fileName.c_str()), "recreate" );

  TTree* tree = new TTree("treeraw", "");

  tree->Branch( "CD_number", &cd, "CD_number/I" );
  tree->Branch( "voltage"  , &v , "voltage/I"   );

  int nEvent = 0;
  tree->Branch( "event", &nEvent,  "nEvent/I" );
  tree->Branch( "lumi" , &lumiblock, "lumi/I" );

  //tempo di giorni,ore,minuti,secondi
  double trigtime_d;
  tree->Branch( "trigtime_d", &trigtime_d, "trigtime_d/D" );
  double trigtime_h;
  tree->Branch( "trigtime_h", &trigtime_h, "trigtime_h/D" );
  double trigtime_m;
  tree->Branch( "trigtime_m", &trigtime_m, "trigtime_m/D" );
  double trigtime_s;
  tree->Branch( "trigtime_s", &trigtime_s, "trigtime_s/D" );
  float sampling_time;
  tree->Branch( "sampling_time", &sampling_time, "sampling_time/F" );

  float pshape[2502];
  tree->Branch( "pshape", pshape, "pshape[2502]/F" );

  lecroyparser::lecroy_wavedesc_2_3 header;
  int16_t* waveform = nullptr;

  std::cout << "-> Reading trc file: " << fullPath << std::endl;

  bool res = lecroyparser::read(Form("%s/%s", dir_trc.c_str(), fileName_trc.c_str()), header, waveform);

  //se non ho problemi ai puntatori daje
  if(res && waveform) {

    int nsamples = ((header.wave_array_count % 2502)==0 ) ? 2502 : 2501;

    //quanti eventi nello stesso file (segmentazione)
    int nEvents_thisFile = header.wave_array_count/nsamples;

    std::cout << "-> This file has: " << nEvents_thisFile << " events" << std::endl;

    //ciclo sugli eventi
    for( unsigned iEv=0; iEv<nEvents_thisFile; ++iEv ) {

      for(uint16_t i = 0; i < nsamples; ++i ) {

        pshape[i] = waveform[i + iEv*nsamples]*header.vertical_gain-header.vertical_offset;
        //gr->SetPoint(i, i, waveform[i]*header.vertical_gain-header.vertical_offset);
        //std::cout << i << "\t" << waveform[i] << "\t" << i*header.horiz_offset+header.horiz_interval << " " << waveform[i]*header.vertical_gain-header.vertical_offset << " " << std::endl;
      }

      trigtime_d = header.trigger_time.days*24.*60.*60.;
      trigtime_h = header.trigger_time.hours*60.*60.;
      trigtime_m = header.trigger_time.minutes*60.;
      trigtime_s = header.trigger_time.seconds;
      sampling_time = header.horiz_interval;

      tree->Fill();
      nEvent++;

      //std::cout << std::endl;

    }

    delete[] waveform;

  } else  {

    return 1;

  }

  outfile->cd();
  tree->Write();
  outfile->Close();

  std::cout << "-> Saved raw tree in: " << outfile->GetName() << std::endl;
                 
  return 0;

}

