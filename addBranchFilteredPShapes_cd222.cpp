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
    int CD_number = 222;

    //per commenti su programma aprire addBranchFilteredPShapes_cd188.cpp che ha la stessa struttura
    std::string path_root   = "/Users/massimo/Documents/phd/PTOLEMY/CD data/data/root/CD222/E100_squidfilter_ER_031025_tr12/";
    std::string path_pshape = "/Users/massimo/Documents/phd/PTOLEMY/CD data/data/pshape_filtered_CD222/";

    std::string root_file_path    = path_root + std::to_string(voltage) + "V/CD" + std::to_string(CD_number) + "_" + std::to_string(voltage) + "V_treeraw.root";
    std::string root_file_path2   = path_root + std::to_string(voltage) + "V/CD" + std::to_string(CD_number) + "_" + std::to_string(voltage) + "V_tree.root";
    std::string pshape_file1_path  = path_pshape + std::to_string(voltage) + "V_filteredpshapes_100kHz.txt";
    std::string pshape_file2_path  = path_pshape + std::to_string(voltage) + "V_filteredpshapes_10kHz.txt";
    std::string pshape_file3_path  = path_pshape + std::to_string(voltage) + "V_filteredpshapes_10kHz_10kHz.txt";
 

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

    float pshape_filtered_100kHz[2502];
    float pshape_filtered_10kHz[2502];
    float pshape_filtered_10kHz_10kHz[2502];

    TBranch* br100   = tree_raw->Branch("pshape_filtered_100kHz", pshape_filtered_100kHz, "pshape_filtered_100kHz[2502]/F");
    TBranch* br10    = tree_raw->Branch("pshape_filtered_10kHz", pshape_filtered_10kHz, "pshape_filtered_10kHz[2502]/F");
    TBranch* br10_10 = tree_raw->Branch("pshape_filtered_10kHz_10kHz", pshape_filtered_10kHz_10kHz, "pshape_filtered_10kHz_10kHz[2502]/F");
   
    std::ifstream pshape100(pshape_file1_path);
    if(!pshape100.is_open()){
      std::cout << "Error opening file with filtered pshapes @ 100 kHz. Exit program" << std::endl;
      return 1; }

    for(Long64_t i = 0; i < nentries; ++i) {
      tree_raw->GetEntry(i);
      std::string line;
      int count = 0;
	
      while(count < 2502 && std::getline(pshape100, line)) {
	if(line.find("pshape nm") != std::string::npos) continue;
	std::istringstream iss(line);
	float x, y;
	if(!(iss >> x >> y)) y = 0;
	pshape_filtered_100kHz[count++] = y;
      }
      br100->Fill();
    }
    pshape100.close();

    std::ifstream pshape10(pshape_file2_path);
    if(!pshape10.is_open()){
      std::cout << "Error opening file with filtered pshapes @ 100 kHz. Exit program" << std::endl;
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

     std::ifstream pshape10_10(pshape_file3_path);
    if(!pshape10_10.is_open()){
      std::cout << "Error opening file with filtered pshapes @ 100 kHz. Exit program" << std::endl;
      return 1; }

    for(Long64_t i = 0; i < nentries; ++i) {
      tree_raw->GetEntry(i);
      std::string line;
      int count = 0;
	
      while(count < 2502 && std::getline(pshape10_10, line)) {
	if(line.find("pshape nm") != std::string::npos) continue;
	std::istringstream iss(line);
	float x, y;
	if(!(iss >> x >> y)) y = 0;
	pshape_filtered_10kHz_10kHz[count++] = y;
      }
      br10_10->Fill();
    }
    pshape10_10.close();

    

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

    tree_raw2->SetBranchAddress("pshape_filtered_100kHz", pshape_filtered_100kHz);
    tree_raw2->SetBranchAddress("pshape_filtered_10kHz", pshape_filtered_10kHz);
    tree_raw2->SetBranchAddress("pshape_filtered_10kHz_10kHz", pshape_filtered_10kHz_10kHz);

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

    float amp_100kHz, amp_10kHz, amp_10kHz_10kHz;
    float baseline_100kHz, baseline_10kHz, baseline_10kHz_10kHz;

    TBranch* brA100   = tree->Branch("amp_100kHz"     , &amp_100kHz     ,  "amp_100kHz/F"     );
    TBranch* brA10    = tree->Branch("amp_10kHz"      , &amp_10kHz      ,  "amp_10kHz/F"      );
    TBranch* brA10_10 = tree->Branch("amp_10kHz_10kHz", &amp_10kHz_10kHz,  "amp_10kHz_10kHz/F");
 
    for(Long64_t i = 0; i < nentries; i++) {

        tree_raw2->GetEntry(i);
        tree->GetEntry(i);

        baseline_100kHz      = GetBaseline(pshape_filtered_100kHz);
        amp_100kHz           = GetAmp(pshape_filtered_100kHz, baseline_100kHz);

	baseline_10kHz       = GetBaseline(pshape_filtered_10kHz);
        amp_10kHz            = GetAmp(pshape_filtered_10kHz, baseline_10kHz);

	baseline_10kHz_10kHz = GetBaseline(pshape_filtered_10kHz_10kHz);
        amp_10kHz_10kHz      = GetAmp(pshape_filtered_10kHz_10kHz, baseline_10kHz_10kHz);
	
        brA100->Fill();
	brA10->Fill();
	brA10_10->Fill();
    }

    tree->Write("", TObject::kOverwrite);

    file_raw->Close();
    file2->Close();

    std::cout << "Branches successfully added" << std::endl;
    return 0;
}

