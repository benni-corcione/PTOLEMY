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
    std::cout << "EXAMPLE: ./ReadData_pinhole pin1 0.5 1" << std::endl;
    exit(1);
  }

  char* folder = argv[1] ; 
  int d1 = atoi(argv[2]);
  int d2 = atoi(argv[3]);
  
  std::string path = std::string(folder) + "/data";
  std::vector<std::string> filenames = get_filenames(path);
  std::stringstream stream;

  std::string save_fold(Form("%s/data",folder));
  system( Form("mkdir -p %s", save_fold.c_str()) );
  
  stream << "cd " << path << std::endl;
  stream << "rm -f .DS_*" << std::endl;
  system(stream.str().c_str());

  

  for(int i=0; i<filenames.size(); i++){  

    //nome tipico simulazione_V1_-90_V2_10.txt
    std::string  V1_str = AndCommon::splitString(filenames[i],"_V1_" )[1].c_str(); //-90_V2_10.txt
    std::string  V2_str = AndCommon::splitString(filenames[i],"_V2_" )[1].c_str(); //10.txt

    int sub_radius = 4000; //substrato con raggio di 0.4 cm
    int cnt_radius = 100;
    int V1         = atoi(AndCommon::splitString(V1_str,"_"   )[0].c_str());
    int V2         = atoi(AndCommon::splitString(V2_str,".txt")[0].c_str());
    std::string name0 =   AndCommon::splitString(filenames[i],"/")[2].c_str(); //name
    std::string name =    AndCommon::splitString(name0       ,".")[0].c_str(); //name


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
    float vx_i, vy_i, vz_i;
    float vx_f, vy_f, vz_f;
    float ke_i, ke_f;
    float azim_i, azim_f, elev_i, elev_f;
    int   line_count = 0;
    
    //creazione del tree
    int event_a;
    int event;
    
    std::string outfile_name = Form("%s/root/%s.root",folder,name.c_str());
    TFile* outfile = TFile::Open(Form("%s",outfile_name.c_str()),"recreate");
    TTree tree("tree","tree");
    
    tree.Branch("event"     , &event     ,      "event/I");
    tree.Branch("cnt_radius", &cnt_radius, "cnt_radius/I");
    tree.Branch("distance1" , &d1        ,  "distance1/I");
    tree.Branch("distance2" , &d2        ,  "distance2/I");
    tree.Branch("substrate" , &sub_radius,  "substrate/I");
    tree.Branch("V1"        , &V1        ,         "V1/I");
    tree.Branch("V2"        , &V2        ,         "V2/I");
    tree.Branch("x_i"       , &x_i       ,        "x_i/F");
    tree.Branch("y_i"       , &y_i       ,        "y_i/F");
    tree.Branch("z_i"       , &z_i       ,        "z_i/F");
    tree.Branch("vx_i"      , &vx_i      ,       "vx_i/F");
    tree.Branch("vy_i"      , &vy_i      ,       "vy_i/F");
    tree.Branch("vz_i"      , &vz_i      ,       "vz_i/F");
    tree.Branch("x_f"       , &x_f       ,        "x_f/F");
    tree.Branch("y_f"       , &y_f       ,        "y_f/F");
    tree.Branch("z_f"       , &z_f       ,        "z_f/F");
    tree.Branch("ke_i"      , &ke_i      ,       "ke_i/F");
    tree.Branch("ke_f"      , &ke_f      ,       "ke_f/F");
    tree.Branch("vx_f"      , &vx_f      ,       "vx_f/F");
    tree.Branch("vy_f"      , &vy_f      ,       "vy_f/F");
    tree.Branch("vz_f"      , &vz_f      ,       "vz_f/F");
    tree.Branch("azim_i"    , &azim_i    ,     "azim_i/F");
    tree.Branch("azim_f"    , &azim_f    ,     "azim_f/F");
    tree.Branch("elev_i"    , &elev_i    ,     "elev_i/F");
    tree.Branch("elev_f"    , &elev_f    ,     "elev_f/F");

    
    
   while (std::getline(input_file, line)) {
      std::replace(line.begin(), line.end(), ',', ' ');
      while (!line.empty() && isspace(line.front())) line.erase(line.begin());
  
      // skippa righe vuote o di intestazione
      if (line.empty()) continue;
      if (line.rfind("ion", 0) == 0) continue; // salta righe che iniziano con "ion"
      if (line[0] == 'F' || line[0] == '-') continue;
      else{
	line_count += 1;
	
	//read lines
	//righe dispari: variabili iniziali
	if(line_count%2!=0){
	  sscanf(line.c_str(),"%d %f %f %f %f %f %f %f %f %f", &event_a, &x_i, &y_i, &z_i, &vx_i, &vy_i, &vz_i, &ke_i, &elev_i, &azim_i);
	}
	
	if(line_count%2==0){
	  sscanf(line.c_str(),"%d %f %f %f %f %f %f %f %f %f", &event_a, &x_f, &y_f, &z_f, &vx_f, &vy_f, &vz_f, &ke_f, &elev_f, &azim_f);
	}
	event = round(line_count/2);
	tree.Fill();
      }
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

  
 
        
