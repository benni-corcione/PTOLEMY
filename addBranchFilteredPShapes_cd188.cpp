#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "TFile.h"
#include "TTree.h"
#include "pshape_functions.h"

int main(int argc, char* argv[]) {

    if(argc != 2) {
        std::cout << "USAGE: ./addBranchFilteredPShapes_cd188 [voltage]" << std::endl;
        return 1;
    }

    int voltage   = std::atoi(argv[1]);
    int CD_number = 188;

    std::string path_root   = "/Users/massimo/Documents/phd/PTOLEMY/CD data/data/root/CD188/conteggi/";
    std::string path_pshape = "/Users/massimo/Documents/phd/PTOLEMY/CD data/data/pshape_filtered_CD188/";

    std::string root_file_path    = path_root + std::to_string(voltage) + "V/CD" + std::to_string(CD_number) + "_" + std::to_string(voltage) + "V_treeraw.root";
    std::string root_file_path2   = path_root + std::to_string(voltage) + "V/CD" + std::to_string(CD_number) + "_" + std::to_string(voltage) + "V_tree.root";

    std::string pshape_file1_path = path_pshape + std::to_string(voltage) + "V_filteredpshapes_100kHz.txt";
    std::string pshape_file2_path = path_pshape + std::to_string(voltage) + "V_filteredpshapes_10kHz.txt";
    std::string pshape_file3_path = path_pshape + std::to_string(voltage) + "V_filteredpshapes_10kHz_100kHz.txt";
    std::string pshape_file4_path = path_pshape + std::to_string(voltage) + "V_filteredpshapes_10kHz_160kHz.txt";
    std::string pshape_file5_path = path_pshape + std::to_string(voltage) + "V_filteredpshapes_10kHz_100kHz_160kHz.txt";

    // ===== PRIMA FASE: riempio i branch nel treeraw ===== 
    TFile* file = TFile::Open(root_file_path.c_str(), "UPDATE");
    if(!file || file->IsZombie()){
      std::cout << "Error opening treeraw file. Exit program" << std::endl;
      return 1; }
    
    TTree* tree_raw = (TTree*)file->Get("treeraw");
    if(!tree_raw){
      std::cout << "Error getting treeraw in the file. Exit program" << std::endl;
      return 1; }
    
    Long64_t nentries = tree_raw->GetEntries();

    //branch per allinearsi al treeraw esistente
    int event;
    tree_raw->SetBranchAddress("event", &event);

    //creazione di nuovi branch
    float pshape_filtered_100kHz[2502];
    float pshape_filtered_10kHz[2502];
    float pshape_filtered_10kHz_100kHz[2502];
    float pshape_filtered_10kHz_160kHz[2502];
    float pshape_filtered_10kHz_100kHz_160kHz[2502];
    TBranch* br100    = tree_raw->Branch("pshape_filtered_100kHz",       pshape_filtered_100kHz,       "pshape_filtered_100kHz[2502]/F"      );
    TBranch* br10     = tree_raw->Branch("pshape_filtered_10kHz",        pshape_filtered_10kHz,        "pshape_filtered_10kHz[2502]/F"       );
    TBranch* br10_100 = tree_raw->Branch("pshape_filtered_10kHz_100kHz", pshape_filtered_10kHz_100kHz, "pshape_filtered_10kHz_100kHz[2502]/F");
    TBranch* br10_160 = tree_raw->Branch("pshape_filtered_10kHz_160kHz", pshape_filtered_10kHz_160kHz, "pshape_filtered_10kHz_160kHz[2502]/F");
    TBranch* br10_100_160 = tree_raw->Branch("pshape_filtered_10kHz_100kHz_160kHz", pshape_filtered_10kHz_100kHz_160kHz, "pshape_filtered_10kHz_100kHz_160kHz[2502]/F");

    //lettura della parte a 100kHz
    std::ifstream pshape100(pshape_file1_path);
    if(!pshape100.is_open()){
      std::cout << "Error opening filtered shapes file @ 100kHz. Exit program" << std::endl;
      return 1; }

    for(Long64_t i = 0; i < nentries; i++) {
      tree_raw->GetEntry(i);
      std::string line;
      int cnt = 0; //counter

      //ciclo di lettura file finché non arrivo a 2502 righe o il file non finisce
      while(cnt < 2502 && std::getline(pshape100, line)) {
	//salto riga se contiene "pshape nm"
	if(line.find("pshape nm") != std::string::npos) continue;
	//trasformo riga in stream per poter estrarre numeri
	std::istringstream iss(line);
	float x, y;
	if(!(iss >> x >> y)) y = 0; //se non riesco a leggere due numeri, y=0
	pshape_filtered_100kHz[cnt] = y;
	cnt++;
      }

      br100->Fill();
    }
    
    pshape100.close();

    //lettura della parte a 10kHz
    std::ifstream pshape10(pshape_file2_path);
    if(!pshape10.is_open()) return 1;

    for(Long64_t i = 0; i < nentries; i++) {
      tree_raw->GetEntry(i);
      std::string line;
      int cnt = 0;

      while(cnt < 2502 && std::getline(pshape10, line)) {
	if(line.find("pshape nm") != std::string::npos) continue;
	std::istringstream iss(line);
	float x, y;
	if(!(iss >> x >> y)) y = 0;
	pshape_filtered_10kHz[cnt++] = y;
      }

      br10->Fill();
    }
    
    pshape10.close();

    //lettura della parte a 10kHz+100kHz
    std::ifstream pshape10_100(pshape_file3_path);
    if(!pshape10_100.is_open()){
      std::cout << "Error opening filtered shapes file @ 10 + 100kHz. Exit program" << std::endl;
      return 1; }

    for(Long64_t i = 0; i < nentries; i++) {
      tree_raw->GetEntry(i);
      std::string line;
      int cnt = 0;

      //ciclo di lettura file finché non arrivo a 2502 righe o il file non finisce
      while(cnt < 2502 && std::getline(pshape10_100, line)) {
	//salto riga se contiene "pshape nm"
	if(line.find("pshape nm") != std::string::npos) continue;
	//trasformo riga in stream per poter estrarre numeri
	std::istringstream iss(line);
	float x, y;
	if(!(iss >> x >> y)) y = 0; //se non riesco a leggere due numeri, y=0
	pshape_filtered_10kHz_100kHz[cnt] = y;
	cnt++;
      }

      br10_100->Fill();
    }
    
    pshape10_100.close();

    //lettura della parte a 10kHz+160kHz
    std::ifstream pshape10_160(pshape_file4_path);
    if(!pshape10_160.is_open()){
      std::cout << "Error opening filtered shapes file @ 10 + 160kHz. Exit program" << std::endl;
      return 1; }

    for(Long64_t i = 0; i < nentries; i++) {
      tree_raw->GetEntry(i);
      std::string line;
      int cnt = 0;

      //ciclo di lettura file finché non arrivo a 2502 righe o il file non finisce
      while(cnt < 2502 && std::getline(pshape10_160, line)) {
	//salto riga se contiene "pshape nm"
	if(line.find("pshape nm") != std::string::npos) continue;
	//trasformo riga in stream per poter estrarre numeri
	std::istringstream iss(line);
	float x, y;
	if(!(iss >> x >> y)) y = 0; //se non riesco a leggere due numeri, y=0
	pshape_filtered_10kHz_160kHz[cnt] = y;
	cnt++;
      }

      br10_160->Fill();
    }
    
    pshape10_160.close();


     //lettura della parte a 10kHz+100kHz+160kHz
    std::ifstream pshape10_100_160(pshape_file5_path);
    if(!pshape10_100_160.is_open()){
      std::cout << "Error opening filtered shapes file @ 10 + 100 + 160kHz. Exit program" << std::endl;
      return 1; }

    for(Long64_t i = 0; i < nentries; i++) {
      tree_raw->GetEntry(i);
      std::string line;
      int cnt = 0;

      //ciclo di lettura file finché non arrivo a 2502 righe o il file non finisce
      while(cnt < 2502 && std::getline(pshape10_100_160, line)) {
	//salto riga se contiene "pshape nm"
	if(line.find("pshape nm") != std::string::npos) continue;
	//trasformo riga in stream per poter estrarre numeri
	std::istringstream iss(line);
	float x, y;
	if(!(iss >> x >> y)) y = 0; //se non riesco a leggere due numeri, y=0
	pshape_filtered_10kHz_100kHz_160kHz[cnt] = y;
	cnt++;
      }

      br10_100_160->Fill();
    }
    
    pshape10_100_160.close();

    
    
    //sovrascrittura nel tree_raw in caso il branch esistesse già
    tree_raw->Write("", TObject::kOverwrite);
    file->Close();

    
    // ===== SECONDA FASE: calcolo ampiezze e salvo nel tree, non nel treeraw =====
    // ===== se non divido in queste due fasi, viene tutto scritto nel tree, non nel treeraw =====

    //riapro file raw per estrarre pshape su cui calcolare ampiezza
    TFile* file_raw = TFile::Open(root_file_path.c_str(), "UPDATE");
    if(!file_raw || file_raw->IsZombie()){
      std::cout << "Error opening treeraw file. Exit program" << std::endl;
      return 1; }

    TTree* tree_raw2 = (TTree*)file_raw->Get("treeraw");
    if(!tree_raw2){
       std::cout << "Error getting treeraw from file. Exit program" << std::endl;
       return 1; }

    //setto i branch da cui devo estrarre data
    tree_raw2->SetBranchAddress("pshape_filtered_100kHz",        pshape_filtered_100kHz      );
    tree_raw2->SetBranchAddress("pshape_filtered_10kHz",         pshape_filtered_10kHz       );
    tree_raw2->SetBranchAddress("pshape_filtered_10kHz_100kHz",  pshape_filtered_10kHz_100kHz);
    tree_raw2->SetBranchAddress("pshape_filtered_10kHz_160kHz",  pshape_filtered_10kHz_160kHz);
    tree_raw2->SetBranchAddress("pshape_filtered_10kHz_100kHz_160kHz",  pshape_filtered_10kHz_100kHz_160kHz);

    //apro file del tree
    TFile* file2 = TFile::Open(root_file_path2.c_str(), "UPDATE");
    if(!file2 || file2->IsZombie()){
       std::cout << "Error opening tree file. Exit program" << std::endl;
       return 1; }

    TTree* tree = (TTree*)file2->Get("tree");
    if(!tree){
       std::cout << "Error getting tree from file. Exit program" << std::endl;
       return 1; }

    //seleziono branch per allinearmi al tree gia esistente
    float amp;
    tree->SetBranchAddress("amp", &amp);

    float amp_100kHz, amp_10kHz, amp_10kHz_100kHz, amp_10kHz_160kHz, amp_10kHz_100kHz_160kHz;
    float baseline_100kHz, baseline_10kHz, baseline_10kHz_100kHz, baseline_10kHz_160kHz, baseline_10kHz_100kHz_160kHz;

    TBranch* brA100     = tree->Branch("amp_100kHz",        &amp_100kHz,        "amp_100kHz/F"      );
    TBranch* brA10      = tree->Branch("amp_10kHz",         &amp_10kHz,         "amp_10kHz/F"       );
    TBranch* brA10_100  = tree->Branch("amp_10kHz_100kHz",  &amp_10kHz_100kHz,  "amp_10kHz_100kHz/F");
    TBranch* brA10_160  = tree->Branch("amp_10kHz_160kHz",  &amp_10kHz_160kHz,  "amp_10kHz_160kHz/F");
    TBranch* brA10_100_160  = tree->Branch("amp_10kHz_100kHz_160kHz",  &amp_10kHz_100kHz_160kHz,  "amp_10kHz_100kHz_160kHz/F");

    for(Long64_t i = 0; i < nentries; i++) {
        tree_raw2->GetEntry(i);
        tree->GetEntry(i);

	//funzioni definite nel pshape_functions.h
        baseline_100kHz       = GetBaseline(pshape_filtered_100kHz);
        amp_100kHz            = GetAmp(pshape_filtered_100kHz, baseline_100kHz);

        baseline_10kHz        = GetBaseline(pshape_filtered_10kHz);
        amp_10kHz             = GetAmp(pshape_filtered_10kHz, baseline_10kHz);

	baseline_10kHz_100kHz = GetBaseline(pshape_filtered_10kHz_100kHz);
        amp_10kHz_100kHz      = GetAmp(pshape_filtered_10kHz_100kHz, baseline_10kHz_100kHz);

	baseline_10kHz_160kHz = GetBaseline(pshape_filtered_10kHz_160kHz);
        amp_10kHz_160kHz      = GetAmp(pshape_filtered_10kHz_160kHz, baseline_10kHz_160kHz);

	baseline_10kHz_100kHz_160kHz = GetBaseline(pshape_filtered_10kHz_100kHz_160kHz);
        amp_10kHz_100kHz_160kHz      = GetAmp(pshape_filtered_10kHz_100kHz_160kHz, baseline_10kHz_100kHz_160kHz);

        brA100->Fill();
        brA10->Fill();
	brA10_100->Fill();
	brA10_160->Fill();
	brA10_100_160->Fill();
    }

    //sovrascrittura su tree in caso esistano già i branch
    tree->Write("", TObject::kOverwrite);

    file_raw->Close();
    file2->Close();

    std::cout << "Branches successfully added" << std::endl;
     
    return 0;
}
