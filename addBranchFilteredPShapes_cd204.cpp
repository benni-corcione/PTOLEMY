#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm> // per std::fill

#include "TFile.h"
#include "TTree.h"
#include "pshape_functions.h"

int main(int argc, char* argv[]) {

    if(argc != 2) {
        std::cout << "USAGE: ./addBranchFilteredPShapes_cd204 [voltage]" << std::endl;
        return 1;
    }

    int voltage   = std::atoi(argv[1]);
    int CD_number = 204;

    //per commenti su programma aprire addBranchFilteredPShapes_cd188.cpp che ha la stessa struttura
    std::string path_root   = "/Users/massimo/Documents/phd/PTOLEMY/CD data/data/root/CD204/B60_post_cond3_moku/";
    std::string path_pshape = "/Users/massimo/Documents/phd/PTOLEMY/CD data/data/pshape_filtered_CD204/";

    std::string root_file_path    = path_root + std::to_string(voltage) + "V/CD" + std::to_string(CD_number) + "_" + std::to_string(voltage) + "V_treeraw.root";
    std::string root_file_path2   = path_root + std::to_string(voltage) + "V/CD" + std::to_string(CD_number) + "_" + std::to_string(voltage) + "V_tree.root";
    std::string pshape_file_path  = path_pshape + std::to_string(voltage) + "V_filteredpshapes_10kHz.txt";

     // ===== PRIMA FASE: riempio i branch nel treeraw =====
    TFile* file = TFile::Open(root_file_path.c_str(), "UPDATE");
    if(!file || file->IsZombie()){
      std::cout << "Error opening treeraw file. Exit program" << std::endl;
      return 1; }
    
    TTree* tree_raw = (TTree*)file->Get("treeraw");
    if(!tree_raw){
      std::cout << "Error getting treeraw from file. Exit program" << std::endl;
      return 1; }
    
    Long64_t nentries = tree_raw->GetEntries();

    int event;
    tree_raw->SetBranchAddress("event", &event);

    float pshape_filtered_10kHz[2502];

    TBranch* br10 = tree_raw->Branch("pshape_filtered_10kHz", pshape_filtered_10kHz, "pshape_filtered_10kHz[2502]/F");
 
    std::ifstream pshape10(pshape_file_path);
    if(!pshape10.is_open()){
      std::cout << "Error opening file with filtered pshapes @ 10 kHz. Exit program" << std::endl;
      return 1; }

    for(Long64_t i = 0; i < nentries; ++i) {
      tree_raw->GetEntry(i);
      std::string line;
      int count = 0;
	
      while(count < 2502 && std::getline(pshape10, line)) {
	if(line.find("pshape nm") != std::string::npos) continue;
	std::istringstream iss(line);
	float x, y;
	if(!(iss >> x >> y)) y = 0;
	pshape_filtered_10kHz[count++] = y;
      }
      br10->Fill();
    }
    pshape10.close();

    tree_raw->Write("", TObject::kOverwrite);
    file->Close();

   
    // ===== SECONDA FASE: calcolo ampiezze e salvo nel tree =====
    TFile* file_raw = TFile::Open(root_file_path.c_str(), "UPDATE");
    if(!file_raw || file_raw->IsZombie()){
      std::cout << "Error opening treeraw file. Exit program" << std::endl;
      return 1; }

    TTree* tree_raw2 = (TTree*)file_raw->Get("treeraw");
    if(!tree_raw2){
      std::cout << "Error getting treeraw from file. Exit program" << std::endl;
      return 1; }

    tree_raw2->SetBranchAddress("pshape_filtered_10kHz", pshape_filtered_10kHz);

    TFile* file2 = TFile::Open(root_file_path2.c_str(), "UPDATE");
    if(!file2 || file2->IsZombie()){
      std::cout << "Error opening tree file. Exit program" << std::endl;
      return 1; }

    TTree* tree = (TTree*)file2->Get("tree");
    if(!tree){
      std::cout << "Error getting tree from file. Exit program" << std::endl;
      return 1; }

    float amp;
    tree->SetBranchAddress("amp", &amp);

    float amp_10kHz;
    float baseline_10kHz;

    TBranch* brA10  = tree->Branch("amp_10kHz",  &amp_10kHz,  "amp_10kHz/F");

    for(Long64_t i = 0; i < nentries; i++) {

        tree_raw2->GetEntry(i);
        tree->GetEntry(i);

        baseline_10kHz = GetBaseline(pshape_filtered_10kHz);
        amp_10kHz      = GetAmp(pshape_filtered_10kHz, baseline_10kHz);

        brA10->Fill();
    }

    tree->Write("", TObject::kOverwrite);

    file_raw->Close();
    file2->Close();

    std::cout << "Branches successfully added" << std::endl;
    return 0;
}

