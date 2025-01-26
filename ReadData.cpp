#include "TTree.h"
#include "TFile.h"

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
  if( argc!= 6 ) {
    std::cout << "USAGE: ./ReadData [folder_name] [output_name] [cnt_radius] [voltage] [distance]" << std::endl;
    std::cout << "cnt_radius in mm, voltage in V, distance in um" << std::endl;
    std::cout << "EXAMPLE: ./ReadData rimbalzi test 0.5 -100 1000" << std::endl;
    exit(1);
  }

  char* folder     =      argv[1] ; 
  char* subfolder  =      argv[2] ;
  float cnt_radius = atof(argv[3]);
  float distance   = atof(argv[5]);
  int voltage      = atoi(argv[4]);
  
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

      //lettura del file di dati
      std::ifstream input_file;
      input_file.open(filenames[i]);

      //verifica sull'apertura
      if (!input_file.is_open()) {
	std::cout << "Error opening the file!" << std::endl;
        exit(1);
      }

      std::string line;
      float x_i, y_i, z_i;
      float x_f, y_f, z_f;
      float theta1_i, theta2_i;
      float theta1_f, theta2_f;
      float ke_i, ke_f;
      int line_count=0;

      //creazione del tree
      int event;
      int sim_number = i+1;
     
      std::string outfile_name = Form("%s_sim%d.root",subfolder,i+1);
      TFile* outfile = TFile::Open(Form("root/%s/%s/%s",folder,subfolder,outfile_name.c_str()),"recreate");
      TTree* tree = new TTree("tree","tree");

      tree->Branch("event", &event, "event/I");
      tree->Branch("sim_number", &sim_number, "sim_number/I");
      tree->Branch("cnt_radius", &cnt_radius, "cnt_radius/F");
      tree->Branch("distance", &distance, "distance/F");
      tree->Branch("voltage", &voltage, "voltage/I");
      tree->Branch("x_i", &x_i, "x_i/F");
      tree->Branch("y_i", &y_i, "y_i/F");
      tree->Branch("z_i", &z_i, "z_i/F");
      tree->Branch("x_f", &x_f, "x_i/F");
      tree->Branch("y_f", &y_f, "y_i/F");
      tree->Branch("z_f", &z_f, "z_i/F");
      tree->Branch("theta1_i", &theta1_i, "theta1_i/F");
      tree->Branch("theta2_i", &theta2_i, "theta2_i/F");
      tree->Branch("theta1_f", &theta1_f, "theta1_f/F");
      tree->Branch("theta2_f", &theta2_f, "theta2_f/F");
      tree->Branch("ke_i", &ke_i, "ke_i/F");
      tree->Branch("ke_f", &ke_f, "ke_f/F");
      
      while(!input_file.eof()){
	//get current line
	getline(input_file,line);
	if(line == "\n" || line[0]=='F' || line[0]=='-'){ continue; }
	else{
	  line_count+=1;
	  
	  //read lines
	  //righe dispari: variabili iniziali
	  if(line_count%2!=0){
	    sscanf(line.c_str(),"%f %f %f %f %f %f", &x_i, &y_i, &z_i, &theta1_i, &theta2_i, &ke_i);
	  }

	   if(line_count%2==0){
	    sscanf(line.c_str(),"%f %f %f %f %f %f", &x_f, &y_f, &z_f, &theta1_f, &theta2_f, &ke_f);
	   

	   event = round(line_count/2);

	   /*
	   std::cout << event << " " << sim_number << " " << x_i << " " << y_i << " " << z_i << " " << theta1_i << " " << theta2_i << " " << ke_i << std::endl;
	   std::cout << event << " " << sim_number << " " << x_f << " " << y_f << " " << z_f << " " << theta1_f << " " << theta2_f << " " << ke_f << std::endl;
	   std::cout << std::endl;
	   */
	   
	   
	   
	   tree->Fill();
	   }
		
	}//else su scansione file intero
      }//while su scansione file
      
    
    //salvataggio tree su file
    outfile->cd();
    tree->Write();

    std::cout << "line_counts: " << line_count << std::endl;
    std::cout << "tree entries: " << tree->GetEntries() << std::endl;
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

  
 
        
