#include "TTree.h"
#include "TFile.h"

#include <fstream>
#include <string>
#include <stdlib.h>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <iostream>
#include <cmath>

void FillVectors(int x_max, int y_max, int n_events, float x_f[n_events], float y_f[n_events], int N, float x[N], float y[N], int counts[N]);

int main(int argc, char* argv[]){
  
  //parametri da input
  if( argc!= 4 ) {
    std::cout << "USAGE: ./PositionMap [folder] [subfolder] [numero_sim]" << std::endl;
    std::cout << "EXAMPLE: ./PositionMap pinhole test 1" << std::endl;
    exit(1);
  }

  char* folder    =      argv[1];
  char* subfolder =      argv[2];
  int   number    = atoi(argv[3]);   

  //apertura file con il tree
  TFile run(Form("root/%s/%s/%s_sim%d.root",folder,subfolder,subfolder, number));
  TTree *tree = (TTree*)run.Get(Form("tree"));
  Long64_t nentries = tree->GetEntries();
  
  //preparazione per la lettura del tree_raw
  int   event;
  float pos_x;
  float pos_y;
  float pos_z;
  float pos_x_array[nentries];
  float pos_y_array[nentries];
  float pos_z_array[nentries];

  tree->SetBranchAddress("event", &event);
  tree->SetBranchAddress("x_f"  , &pos_x);
  tree->SetBranchAddress("y_f"  , &pos_y);
  tree->SetBranchAddress("z_f"  , &pos_z);

  //step di 10um
  int x_max=500;
  int y_max=500;

  float fractPart, intPart;
  
  for(int iEntry=0; iEntry<nentries; iEntry++){
    tree->GetEntry(iEntry);
    
    //quantizzazione risultati
    fractPart = modf(pos_y, &intPart);
    if(fractPart>0.49){pos_y=intPart+1;}
    if(fractPart<0.50){pos_y=intPart  ;}
    fractPart = modf(pos_z, &intPart);
    if(fractPart>0.49){pos_z=intPart+1;}
    if(fractPart<0.50){pos_z=intPart  ;}

    pos_x_array[iEntry]=pos_x;
    pos_y_array[iEntry]=pos_y;
    pos_z_array[iEntry]=pos_z;
  }//for sulle entries

 
  int N = x_max*y_max;
  float z[N], y[N];
  int counts[N];

  /*
  for(int n=0; n<10; n++){
  std::cout << pos_y_array[n] << " " << pos_z_array[n] << " " << std::endl;
  }
  */
  FillVectors(x_max,y_max,nentries,pos_y_array,pos_z_array,N,y,z,counts);
  //Print(N,x,y,pos_z,counts);
  run.Close();

  
  //scrittura su file per python
  std::string filename = "data/" + std::string(folder)
    + "/" + std::string(subfolder)
    + "/" + std::string(subfolder)
    + "_py.txt";
  std::ofstream ofs(filename);

  //prime righe del file
  //ofs << "CNT voltage: " << cnt_voltage      << std::endl;
  for(int n=0; n<N; n++){
  ofs << y[n] << " " << z[n] << " " << counts[n] << std::endl;
  }
  ofs.close();

  std::cout << "output saved in " << filename << std::endl;
  return(0);
}

void FillVectors(int x_max, int y_max, int n_events, float x_f[n_events], float y_f[n_events], int N, float x[N], float y[N], int counts[N]){
  
  int index=0;
  
  for(int i=0; i<x_max; i++){
    for(int j=0; j<y_max; j++){
      x[index]=(i+1)-(x_max/2); y[index]=(j+1)-(x_max/2);
      index++;
    }
  }

  for(int n=0; n<N; n++){
    counts[n]=0;
    for(int m=0; m<n_events; m++){
      if(x_f[m]==x[n] && y_f[m]==y[n]){
	counts[n]++;
      }
    }
  }
	
}

