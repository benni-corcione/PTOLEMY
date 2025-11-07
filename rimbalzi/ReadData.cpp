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
//programma che fa gli histo più generali possibili delle variabili
int main(int argc, char* argv[]){
  
  //parametri da input
  if( argc!= 3 ) {
    std::cout << "USAGE: ./ReadData [folder_name] [output_name]" << std::endl;
    std::cout << "EXAMPLE: ./ReadData rimbalzi test" << std::endl;
    exit(1);
  }

  char* folder    = argv[1] ; 
  char* subfolder = argv[2] ;
  
  std::string path = "data/" + std::string(folder) + "/" + std::string(subfolder);
  //std::cout << path << std::endl;
  std::vector<std::string> filenames = get_filenames(path);
  
  std::stringstream stream;

  std::string save_fold(Form("root/%s",folder));
  std::string save_subfold(Form("root/%s/%s",folder,subfolder));
  system( Form("mkdir -p %s", save_fold.c_str()) );
  system( Form("mkdir -p %s", save_subfold.c_str()));
  
  stream << "cd " << path << std::endl;
  stream << "rm -f .DS_*" << std::endl;
  system(stream.str().c_str());
  
  for(int i=0; i<filenames.size(); i++){  

    //nome tipico name_d600um_cnt15um_sub5000um_V100_E10-1
    std::string   d_str = AndCommon::splitString(filenames[i],"_d"  )[1].c_str(); //600um_cnt15um_sub5000um_V100_E10-1
    std::string cnt_str = AndCommon::splitString(filenames[i],"_cnt")[1].c_str(); //15um_sub5000um_V100_E10-1
    std::string sub_str = AndCommon::splitString(filenames[i],"_sub")[1].c_str(); //5000um_100_E10-1
    std::string   V_str = AndCommon::splitString(filenames[i],"_V"  )[1].c_str(); //100_E10-1
    std::string   E_str = AndCommon::splitString(filenames[i],"_E"  )[1].c_str(); //10-1
    std::string   name  = AndCommon::splitString(filenames[i],"/"   )[3].c_str(); //10-1

    float distance   = atof(AndCommon::splitString(  d_str,"um")[0].c_str());
    float cnt_radius = atof(AndCommon::splitString(cnt_str,"um")[0].c_str());
    int   sub_radius = atoi(AndCommon::splitString(sub_str,"um")[0].c_str());
    int   voltage    = atoi(AndCommon::splitString(  V_str,"_" )[0].c_str());
    int   energy     = atoi(AndCommon::splitString(  E_str,"-" )[0].c_str());
    int   sim_number = atoi(AndCommon::splitString(  E_str,"-" )[1].c_str());

    
    /*
    std::cout << filenames[i] << std::endl;
    std::cout << "d   : " << distance << std::endl;
    std::cout << "cnts: " << cnt_radius << std::endl;
    std::cout << "v   : " << voltage << std::endl;
    std::cout << "E   : " << energy << std::endl;
    std::cout << "sim : " << sim_number << std::endl;
    */
    
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
    float x_i, y_i, z_i;
    float x_f, y_f, z_f;
    float theta1_i, theta2_i;
    float theta1_f, theta2_f;
    float ke_i, ke_f;
    float vx, vy, vz;
    int   line_count = 0;
    float r_f;
    //creazione del tree
    int event;
    
    std::string outfile_name = Form("root/%s/%s/%s.root",folder,subfolder,name.c_str());
    TFile* outfile = TFile::Open(Form("%s",outfile_name.c_str()),"recreate");
    TTree tree("tree","tree");
    
    tree.Branch("event"     , &event     ,      "event/I");
    tree.Branch("sim_number", &sim_number, "sim_number/I");
    tree.Branch("cnt_radius", &cnt_radius, "cnt_radius/F");
    tree.Branch("distance"  , &distance  ,   "distance/F");
    tree.Branch("substrate" , &sub_radius,  "substrate/I");
    tree.Branch("voltage"   , &voltage   ,    "voltage/I");
    tree.Branch("x_i"       , &x_i       ,        "x_i/F");
    tree.Branch("y_i"       , &y_i       ,        "y_i/F");
    tree.Branch("z_i"       , &z_i       ,        "z_i/F");
    tree.Branch("vx"        , &vx        ,         "vx/F");
    tree.Branch("vy"        , &vy        ,         "vy/F");
    tree.Branch("vz"        , &vz        ,         "vz/F");
    tree.Branch("x_f"       , &x_f       ,        "x_i/F");
    tree.Branch("y_f"       , &y_f       ,        "y_i/F");
    tree.Branch("z_f"       , &z_f       ,        "z_i/F");
    tree.Branch("r_f"       , &r_f       ,        "r_i/F");
    tree.Branch("theta1_i"  , &theta1_i  ,   "theta1_i/F");
    tree.Branch("theta2_i"  , &theta2_i  ,   "theta2_i/F");
    tree.Branch("theta1_f"  , &theta1_f  ,   "theta1_f/F");
    tree.Branch("theta2_f"  , &theta2_f  ,   "theta2_f/F");
    tree.Branch("ke_i"      , &ke_i      ,       "ke_i/F");
    tree.Branch("ke_f"      , &ke_f      ,       "ke_f/F");
    tree.Branch("energy"    , &energy    ,     "energy/F");
    
    
    while(!input_file.eof()){
      //get current line
      getline(input_file,line);
      if(line == "\n" || line[0]=='F' || line[0]=='-'){ continue; }
      else{
	line_count+=1;
	
	//read lines
	//righe dispari: variabili iniziali
	if(line_count%2!=0){
	  sscanf(line.c_str(),"%f %f %f %f %f %f  %f", &x_i, &y_i, &z_i, &vx, &vy, &vz, &ke_i);
	}
	
	if(line_count%2==0){
	  sscanf(line.c_str(),"%f %f %f %f %f %f %f", &x_f, &y_f, &z_f, &vx, &vy, &vz, &ke_f);

	  x_i*=5;  y_i*=5;  z_i*=5;
	  x_f*=5;  y_f*=5;  z_f*=5;
	  r_f = sqrt(y_f*y_f+z_f*z_f);
	  event = round(line_count/2); 
	  tree.Fill();
	}
	
      }//else su scansione file intero
    }//while su scansione file
    
    
    //salvataggio tree su file
    outfile->cd();
    tree.Write();

    std::cout << "line_counts: " << line_count << std::endl;
    std::cout << "tree entries: " << tree.GetEntries() << std::endl;
    std::cout << "tree saved in " << outfile_name << std::endl;
    outfile->Close();

    input_file.close();
    
  }//for sul file di simulazione
  
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

  
 
        
