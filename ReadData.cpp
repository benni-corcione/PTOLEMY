#include "TTree.h"
#include "TFile.h"
#include "AndCommon.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>

std::vector<std::string> get_filenames( std::filesystem::path path );
int Set_cnt_length(int n);
int Set_pin1(int n);
int Set_pin2_thick(int n);

//programma che fa gli histo più generali possibili delle variabili

int main(int argc, char* argv[]){
  
  //parametri da input
  if( argc!= 3 ) {
    std::cout << "USAGE: ./ReadData [geom_number] [number of runs]" << std::endl;
    std::cout << "EXAMPLE: ./ReadData 0 19" << std::endl;
    std::cout << "geom_number described in file geom.txt" << std::endl;
    exit(1);
  }

  int geom = atoi(argv[1]);
  int runs = atoi(argv[2]);
 
  
  for (int runindex=runs; runindex < runs+1; runindex++){ //scanno le run
    
    std::string path = "data/geom" + std::to_string(geom) + "/run" + std::to_string(runindex);
    std::vector<std::string> filenames = get_filenames(path);
    std::stringstream stream;
      
    std::string save_fold(Form("root/geom%d/",geom));
    system( Form("mkdir -p %s", save_fold.c_str()) );
      
    stream << "cd " << path << std::endl;
    stream << "rm -f .DS_*" << std::endl;
    system(stream.str().c_str());
      
    for(int i=0; i<filenames.size(); i++){  
	
      //nome tipico mcbon0_V1_-10_V2_90.txt
      std::string mcbon_str  = AndCommon::splitString(std::filesystem::path(filenames[i]).filename().string(), "_V1_")[0];
      std::string name_str   = std::filesystem::path(filenames[i]).filename().string();
      std::string volts_str  = AndCommon::splitString(std::filesystem::path(filenames[i]).filename().string(), "_V1_")[1];
      std::string V1_longstr = volts_str;
      std::string V1_str     = AndCommon::splitString(V1_longstr, "_V2_")[0];
      std::string V2_longstr = AndCommon::splitString(V1_longstr, "_V2_")[1];
      std::string V2_str     = AndCommon::splitString(V2_longstr, ".txt")[0];
      std::string name       = AndCommon::splitString(name_str, ".txt")[0] + "_run" + std::to_string(runindex);
	
      int cnt_length = Set_cnt_length(geom);
      int cnt_hole   = 10;
      int cnt_radius = 50;
      int pin1       = Set_pin1      (geom);
      int pin2_thick = Set_pin2_thick(geom);
      int pin2       = 10;
      int V_cnt      = atoi(V1_str.c_str());
      int V_pin1     = atoi(V2_str.c_str());
	
      //lettura del file di dati
      std::ifstream input_file;
      input_file.open(filenames[i]);
	
      //verifica sull'apertura
      if (!input_file.is_open()) {
	std::cout << "Error opening the file!" << std::endl;
	std::cout << filenames[i] << std::endl;
	exit(1);
      }
	
      std::string line;
      int ni, nf;
      float x_i, y_i, z_i;
      float x_f, y_f, z_f;
      float ke_i, ke_f;
      float vx_i, vy_i, vz_i;
      float azim_i, azim_f, elev_i, elev_f;
      float azim_iF, azim_fF, elev_iF, elev_fF;
      float vx_f, vy_f, vz_f;
      int   line_count = 0;
	
      //creazione del tree
      int event;
	
      std::string outfile_name = Form("root/geom%d/%s.root",geom,name.c_str());
      TFile* outfile = TFile::Open(Form("%s",outfile_name.c_str()),"recreate");
      TTree tree("tree","tree");
	    
      tree.Branch("event"     , &event     ,      "event/I");
      tree.Branch("cnt_length", &cnt_length, "cnt_length/I");
      tree.Branch("cnt_radius", &cnt_radius, "cnt_radius/I");
      tree.Branch("cnt_hole"  , &cnt_hole  ,   "cnt_hole/I");
      tree.Branch("pin1"      , &pin1      ,       "pin1/I");
      tree.Branch("pin2_thick", &pin2_thick, "pin2_thick/I");
      tree.Branch("pin2"      , &pin2      ,       "pin2/I");
      tree.Branch("V_cnt"     , &V_cnt     ,      "V_cnt/I");
      tree.Branch("V_pin1"    , &V_pin1    ,     "V_pin1/I");
      tree.Branch("x_i"       , &x_i       ,        "x_i/F");
      tree.Branch("y_i"       , &y_i       ,        "y_i/F");
      tree.Branch("z_i"       , &z_i       ,        "z_i/F");
      tree.Branch("vx_i"      , &vx_i      ,       "vx_i/F");
      tree.Branch("vy_i"      , &vy_i      ,       "vy_i/F");
      tree.Branch("vz_i"      , &vz_i      ,       "vz_i/F");
      tree.Branch("x_f"       , &x_f       ,        "x_i/F");
      tree.Branch("y_f"       , &y_f       ,        "y_i/F");
      tree.Branch("z_f"       , &z_f       ,        "z_i/F");
      tree.Branch("ke_i"      , &ke_i      ,       "ke_i/F");
      tree.Branch("ke_f"      , &ke_f      ,       "ke_f/F");
      tree.Branch("vx_f"      , &vx_f      ,       "vx_f/F");
      tree.Branch("vy_f"      , &vy_f      ,       "vy_f/F");
      tree.Branch("vz_f"      , &vz_f      ,       "vz_f/F");
      tree.Branch("azim_i"    , &azim_i    ,     "azim_i/F");
      tree.Branch("azim_f"    , &azim_f    ,     "azim_f/F");
      tree.Branch("elev_i"    , &elev_i    ,     "elev_i/F");
      tree.Branch("elev_f"    , &elev_f    ,     "elev_f/F");

      getline(input_file,line);
	  
      while(!input_file.eof()){
	//get current line
	getline(input_file,line);
	if(line == "\n" || line[0]=='F' || line[0]=='-'){ continue; }
	else{
	  line_count+=1;
	      
	  //read lines
	  //righe dispari: variabili iniziali
	  if(line_count%2!=0){
	    sscanf(line.c_str(),"%d, %f, %f, %f, %f, %f, %f, %f, %f, %f", &ni, &x_i, &y_i, &z_i, &vx_i, &vy_i, &vz_i, &ke_i, &elev_i, &azim_i);
	  }
	      
	  if(line_count%2==0){
	    sscanf(line.c_str(),"%d, %f, %f, %f, %f, %f, %f, %f, %f, %f", &nf, &x_f, &y_f, &z_f, &vx_f, &vy_f, &vz_f, &ke_f, &elev_f, &azim_f);

	    /*
	    azim_i = atan2(-vy_i,vz_i); //azim
	    azim_f = atan2(-vy_f,vz_f)*180/M_PI; //azim
	    elev_i = atan2(vx_i,sqrt(vz_i*vz_i+vy_i*vy_i))*180/M_PI; //elev
	    elev_f = atan2(vx_f,sqrt(vz_f*vz_f+vy_f*vy_f))*180/M_PI; //elev
	    */
	    /*
	    if(azim_i_check-azim_i>1E-03){ std::cout << "Angoli errati" << std::endl; }
	    if(azim_f_check-azim_f>1E-03){ std::cout << "Angoli errati" << std::endl; }
	    if(elev_i_check-elev_i>1E-03){ std::cout << "Angoli errati" << std::endl; }
	    if(elev_f_check-elev_f>1E-03){ std::cout << "Angoli errati" << std::endl; }*/
	    
	    event = round(line_count/2); 
	    tree.Fill();
	  }
	    
	}//else su scansione file intero
      }//while su scansione file
      outfile->cd();
	  
      //salvataggio tree su file
      tree.Write();

      //std::cout << "line_counts: " << line_count << std::endl;
      //std::cout << "tree entries: " << tree.GetEntries() << std::endl;
      std::cout << "tree saved in " << outfile_name << std::endl;
      outfile->Close();

      input_file.close();


    }//for sul file di simulazione
  }//for sul runindex
 
  
  return 0;
}

std::vector<std::string> get_filenames( std::filesystem::path path )
{
  namespace stdfs = std::filesystem;

  std::vector<std::string> filenames;
    
  const stdfs::directory_iterator end{} ;
    
  for( stdfs::directory_iterator iter{path} ; iter != end ; ++iter )
    { 
      filenames.push_back( iter->path().string() ) ;
    }

  return filenames;
}

//geometry settings
int Set_cnt_length(int n){
  int cnt_length=0;

  if(n==0)                          cnt_length = 100;
  if(n==1 || n==2 || n==3 || n==4 ) cnt_length = 100;
  if(n==5 || n==6 || n==7 || n==8 ) cnt_length = 150;
  if(n==9 || n==10|| n==11|| n==12) cnt_length = 200;

  return cnt_length;
}

//geometry settings
int Set_pin1(int n){
  int pin1=0;

  if(n==0)                                           pin1 = 50;
  if(n==1 || n==2 || n==5 || n==6 || n==9 || n==10 ) pin1 = 50;
  if(n==3 || n==4 || n==7 || n==8 || n==11|| n==12 ) pin1 = 25;
  
  return pin1;
}

//geometry settings
int Set_pin2_thick(int n){
  int pin2_thick=0;

  if(n==0)   pin2_thick = 500;
  if(n%2==0) pin2_thick = 250;
  if(n%2!=0) pin2_thick = 500;
  
  return pin2_thick;
}


   
	 
