#include "TTree.h"
#include "TFile.h"
#include "AndCommon.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>

std::vector<std::string> get_filenames(std::filesystem::path path);

int main(int argc, char* argv[]) {
  std::string path = "/Users/massimo/Documents/phd/PTOLEMY/simulazioni/hyperion3/simulazioni_daniele_rifatte/data";
  std::vector<std::string> filenames = get_filenames(path);
  std::stringstream stream;
    
  //stream << "rm -r root" << std::endl;
  //system(stream.str().c_str());
    
  std::string save_fold("root");
  system(Form("mkdir -p %s", save_fold.c_str()));

  
    
  stream << "cd " << path << std::endl;
  stream << "rm -f .DS_*" << std::endl;
  system(stream.str().c_str());
     
  

  for (int i = 0; i < filenames.size(); i++) {

    std::string V1_str   = AndCommon::splitString(filenames[i], "_V1_")[1];
    std::string V2_str   = AndCommon::splitString(filenames[i], "_V2_")[1];
    std::string name_str = AndCommon::splitString(filenames[i], "/"   )[9];

    int V1 = atoi(AndCommon::splitString(V1_str, "_V2_")[0].c_str());
    int V2 = atoi(AndCommon::splitString(V2_str, ".txt")[0].c_str());
    std::string name = AndCommon::splitString(name_str, ".txt")[0];

    std::ifstream input_file(filenames[i]);
    if (!input_file.is_open()) {
      std::cout << "Error opening the file: " << filenames[i] << std::endl;
      exit(1);
    }

    // Variabili per la lettura
    std::string line;
    int event, event_i, event_f;
    float x_i, y_i, z_i, vx_i, vy_i, vz_i, ke_i, elev_i, azim_i;
    float x_f, y_f, z_f, vx_f, vy_f, vz_f, ke_f, elev_f, azim_f;
    int line_count = 0;

    std::string outfile_name = Form("root/%s.root", name.c_str());
    TFile* outfile = TFile::Open(outfile_name.c_str(), "RECREATE");
    TTree tree("tree", "tree");

    // Branches
    tree.Branch("event" , &event , "event/I");
    tree.Branch("V1"    , &V1    , "V1/I"    );
    tree.Branch("V2"    , &V2    , "V2/I"    );
    tree.Branch("x_i"   , &x_i   , "x_i/F"   );
    tree.Branch("y_i"   , &y_i   , "y_i/F"   );
    tree.Branch("z_i"   , &z_i   , "z_i/F"   );
    tree.Branch("x_f"   , &x_f   , "x_f/F"   );
    tree.Branch("y_f"   , &y_f   , "y_f/F"   );
    tree.Branch("z_f"   , &z_f   , "z_f/F"   );
    tree.Branch("vx_i"  , &vx_i  , "vx_i/F"  );
    tree.Branch("vy_i"  , &vy_i  , "vy_i/F"  );
    tree.Branch("vz_i"  , &vz_i  , "vz_i/F"  );
    tree.Branch("vx_f"  , &vx_f  , "vx_f/F"  );
    tree.Branch("vy_f"  , &vy_f  , "vy_f/F"  );
    tree.Branch("vz_f"  , &vz_f  , "vz_f/F"  );
    tree.Branch("ke_i"  , &ke_i  , "ke_i/F"  );
    tree.Branch("ke_f"  , &ke_f  , "ke_f/F"  );
    tree.Branch("azim_i", &azim_i, "azim_i/F");
    tree.Branch("azim_f", &azim_f, "azim_f/F");
    tree.Branch("elev_i", &elev_i, "elev_i/F");
    tree.Branch("elev_f", &elev_f, "elev_f/F");

    // Salta intestazione
    std::getline(input_file, line);

    while (std::getline(input_file, line)) {
      line_count++;
      if (line.empty()) continue; // salta righe vuote

      std::stringstream ss(line);
      std::string token;

      int evt;
      float x, y, z, vx, vy, vz, ke, elev, azim;

      // Legge i 10 token separati da ','
      std::getline(ss, token, ','); token.erase(0, token.find_first_not_of(" \t")); token.erase(token.find_last_not_of(" \t")+1); evt = std::stoi(token);
      std::getline(ss, token, ','); token.erase(0, token.find_first_not_of(" \t")); token.erase(token.find_last_not_of(" \t")+1); x = std::stof(token);
      std::getline(ss, token, ','); token.erase(0, token.find_first_not_of(" \t")); token.erase(token.find_last_not_of(" \t")+1); y = std::stof(token);
      std::getline(ss, token, ','); token.erase(0, token.find_first_not_of(" \t")); token.erase(token.find_last_not_of(" \t")+1); z = std::stof(token);
      std::getline(ss, token, ','); token.erase(0, token.find_first_not_of(" \t")); token.erase(token.find_last_not_of(" \t")+1); vx = std::stof(token);
      std::getline(ss, token, ','); token.erase(0, token.find_first_not_of(" \t")); token.erase(token.find_last_not_of(" \t")+1); vy = std::stof(token);
      std::getline(ss, token, ','); token.erase(0, token.find_first_not_of(" \t")); token.erase(token.find_last_not_of(" \t")+1); vz = std::stof(token);
      std::getline(ss, token, ','); token.erase(0, token.find_first_not_of(" \t")); token.erase(token.find_last_not_of(" \t")+1); ke = std::stof(token);
      std::getline(ss, token, ','); token.erase(0, token.find_first_not_of(" \t")); token.erase(token.find_last_not_of(" \t")+1); elev = std::stof(token);
      std::getline(ss, token, ','); token.erase(0, token.find_first_not_of(" \t")); token.erase(token.find_last_not_of(" \t")+1); azim = std::stof(token);

      if (line_count % 2 != 0) {
	event_i = evt; x_i = x; y_i = y; z_i = z;
	vx_i = vx; vy_i = vy; vz_i = vz; ke_i = ke;
	elev_i = elev; azim_i = azim;
      } else {
	event_f = evt; x_f = x; y_f = y; z_f = z;
	vx_f = vx; vy_f = vy; vz_f = vz; ke_f = ke;
	elev_f = elev; azim_f = azim;

	if (event_i != event_f) {
	  std::cout << "Disallineamento eventi: " << event_i << " " << event_f << std::endl;
	  exit(1);
	}
	event = event_i;
	tree.Fill();
      }
    }

    outfile->cd();
    tree.Write();
      if(i%100==0){std::cout << "File " << i << " letto" << std::endl;}
    //std::cout << "line counts: " << line_count << std::endl;
    //std::cout << "tree entries: " << tree.GetEntries() << std::endl;
    //std::cout << "tree saved in " << outfile_name << std::endl;
    outfile->Close();
    input_file.close();
  }

  return 0;
}

std::vector<std::string> get_filenames(std::filesystem::path path) {
  namespace stdfs = std::filesystem;
  std::vector<std::string> filenames;

  for (stdfs::directory_iterator iter{path}; iter != stdfs::directory_iterator{}; ++iter) {
    filenames.push_back(iter->path().string());
  }
  return filenames;
}
