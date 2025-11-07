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
  if( argc!= 4 ) {
    std::cout << "USAGE: ./ReadData_pinhole [folder_name] [d1] [d2]" << std::endl;
    std::cout << "EXAMPLE: ./ReadData_pinhole pin1" << std::endl;
    exit(1);
  }

  char* folder = argv[1] ; 
  int d1 = atoi(argv[2]);
  int d2 = atoi(argv[3]);
  
  std::string path = "data/" + std::string(folder);
  std::vector<std::string> filenames = get_filenames(path);
  std::stringstream stream;

  std::string save_fold(Form("root/%s",folder));
  system( Form("mkdir -p %s", save_fold.c_str()) );
  
  stream << "cd " << path << std::endl;
  stream << "rm -f .DS_*" << std::endl;
  system(stream.str().c_str());
  
  for(int i=0; i<filenames.size(); i++){  

    //nome tipico name_cnt50um_pin100um_Vcnt90_Vpin10
    //nome tipico name_pin100um_Vpin90_Vpin10
    std::string  cnt_str = AndCommon::splitString(filenames[i],"_cnt" )[1].c_str(); //50um_pin100um_Vcnt90_Vpin10
    std::string  pin_str = AndCommon::splitString(filenames[i],"_pin" )[1].c_str(); //100um_Vcnt90_Vpin10
    std::string Vcnt_str = AndCommon::splitString(filenames[i],"_Vcnt")[1].c_str(); //90_Vpin10
    std::string Vpin_str = AndCommon::splitString(filenames[i],"_Vpin")[1].c_str(); //10
    std::string    name  = AndCommon::splitString(filenames[i],"/"   )[2].c_str(); //name

    //int distance   = 500;  //distanza cnt-pinhole & pinhole-tes
    int sub_radius = 4000; //substrato con raggio di 0.4 cm
    int cnt_radius = atoi(AndCommon::splitString( cnt_str,"um")[0].c_str());
    //int V_cnt      = atoi(AndCommon::splitString(Vcnt_str,"_" )[0].c_str());
    int V_pin      = atoi(AndCommon::splitString(Vpin_str,"_" )[0].c_str());
    int V_cnt = V_pin - 100;
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
    float ke_i, ke_f;
    float vx_i, vy_i, vz_i;
    float azim_i, azim_f, elev_i, elev_f;
    float vx_f, vy_f, vz_f;
    int   line_count = 0;
    
    //creazione del tree
    int event;
    
    std::string outfile_name = Form("root/%s/%s.root",folder,name.c_str());
    TFile* outfile = TFile::Open(Form("%s",outfile_name.c_str()),"recreate");
    TTree tree("tree","tree");
    
    tree.Branch("event"     , &event     ,      "event/I");
    tree.Branch("cnt_radius", &cnt_radius, "cnt_radius/I");
    tree.Branch("distance1" , &d1  ,        "distance1/I");
    tree.Branch("distance1" , &d2  ,        "distance2/I");
    tree.Branch("substrate" , &sub_radius,  "substrate/I");
    tree.Branch("V_cnt"     , &V_cnt     ,      "V_cnt/I");
    tree.Branch("V_pin"     , &V_pin     ,      "V_pin/I");
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

    
    
    while(!input_file.eof()){
      //get current line
      getline(input_file,line);
      if(line == "\n" || line[0]=='F' || line[0]=='-'){ continue; }
      else{
	line_count+=1;
	
	//read lines
	//righe dispari: variabili iniziali
	if(line_count%2!=0){
	  sscanf(line.c_str(),"%f %f %f %f %f %f %f", &x_i, &y_i, &z_i, &vx_i, &vy_i, &vz_i, &ke_i);
	}
	
	if(line_count%2==0){
	  sscanf(line.c_str(),"%f %f %f %f %f %f %f", &x_f, &y_f, &z_f, &vx_f, &vy_f, &vz_f, &ke_f);

	  azim_i = atan2(-vy_i,vz_i); //azim
	  azim_f = atan2(-vy_f,vz_f); //azim
	  elev_i = atan2(vx_i,sqrt(vz_i*vz_i+vy_i*vy_i)); //elev
	  elev_f = atan2(vx_f,sqrt(vz_f*vz_f+vy_f*vy_f)); //elev
	 
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

  
 
        
