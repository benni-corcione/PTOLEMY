#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <filesystem>

#include "AndCommon.h"
std::vector<std::string> get_filenames( std::filesystem::path path);
std::string RoundNumber(float result);

int main(int argc, char *argv[]){

  //parametri da input
  if (argc!=4){
    std::cout << "USAGE: ./makeAllrawTreePhotons [cooldown] [measure] [power]" << std::endl;
     std::cout << "EXAMPLE: ./makeAllrawTree 188 counts 2.5" << std::endl;
    exit(1);
  }
  
  
  int   CD_number = atoi(argv[1]);
  float power     = atof(argv[3]); 
  char* meas      =      argv[2] ;
  char* pw_char   =      argv[3] ;
  
  for(int i=0; i<4; i++){ //i<4 è per sicurezza ma il punto dovrebbe essere trovato prima
    if(pw_char[i]=='.'){pw_char[i]='p';} }
  
  std::stringstream stream;    

  std::string path1 = "data/trc/CD";
  std::string path  = path1 + std::to_string(CD_number)
                      + "/" + std::string(meas)
                      + "/" + std::string(pw_char) 
                      + "mW";

  std::vector<std::string> filenames = get_filenames(path);
  
  for(int i=0; i<filenames.size(); i++){  
  
    stream << "./makerawTreePhotons " << filenames[i] << " " << power <<  std::endl;
    
    //senza if riesegue per ogni ciclo anche tutti i programmi prima dell'i-esimo
    if(i==filenames.size()-1){
      system(stream.str().c_str());
    }
    
  }
  
  return 0;
}

std::string RoundNumber(float result){
    std::ostringstream out;
    out<<std::setprecision(1)<<std::fixed<<std::showpoint<< result;
    std::string RoundResult =  out.str();
    return RoundResult;
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

  
 

       

       

      
