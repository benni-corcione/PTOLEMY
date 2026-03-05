#include <iostream>
#include <filesystem>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <cstdlib>

int main() {
    namespace fs = std::filesystem;
    std::string folder = "root/geom0/";

    // raccoglie tutti i file .root
    std::map<std::string, std::vector<std::string>> groups;
    for (const auto& entry : fs::directory_iterator(folder)) {
        if (entry.path().extension() == ".root") {
            std::string fname = entry.path().filename().string();

            // trova prefisso senza _runN
            size_t run_pos = fname.rfind("_run");
            if (run_pos != std::string::npos) {
                std::string prefix = fname.substr(0, run_pos);
                groups[prefix].push_back(fname);
            }
        }
    }

    // esegue hadd per ogni gruppo
    for (auto& [prefix, files] : groups) {
        if (files.empty()) continue;

        std::string final_file = folder + prefix + ".root";
	

        // dividiamo i file da unire in nuovi vs già uniti
        std::vector<std::string> to_merge;
        if (fs::exists(final_file)) {
	  for (auto& f : files) {
	    to_merge.push_back(f);
	  }
	  
	  if (!to_merge.empty()) {
	    std::string temp_file = folder + prefix + "_temp.root";
	    
	    std::stringstream cmd;
	    cmd << "hadd -f " << temp_file
		<< " " << final_file;
	    
	    for (auto& f : to_merge) {
	      cmd << " " << folder << f;
	    }
	    
	    std::cout << "Appending to existing file: " << cmd.str() << std::endl;
	    system(cmd.str().c_str());
	    
	    fs::rename(temp_file, final_file);
	  }
	  
        } else {
	  // file finale non esiste → unisci tutti
            std::stringstream cmd;
            cmd << "hadd -f " << final_file;
            for (auto& f : files) {
                cmd << " " << folder << f;
            }
            std::cout << "Creating new merged file: " << cmd.str() << std::endl;
            system(cmd.str().c_str());
        }

        // elimina i file parziali
        for (auto& f : files) {
            fs::remove(folder + f);
        }
    }

    return 0;
}
