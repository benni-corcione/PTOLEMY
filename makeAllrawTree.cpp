#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <filesystem>

#include "AndCommon.h"
std::vector<std::string> get_filenames( std::filesystem::path path );

int main(int argc, char *argv[]){

  //parametri da input
  if (argc!=4){
    std::cout << "USAGE: ./makeAllrawTree [cooldown] [measure] [voltage]" << std::endl;
     std::cout << "EXAMPLE: ./makeAllrawTree 188 counts 101" << std::endl;
     std::cout << "EXAMPLE: ./makeAllrawTree 204 workingpoint 134" << std::endl;
    exit(1);
  }
  
  
  int   CD_number = atoi(argv[1]);
  int   voltage   = atoi(argv[3]); 
  char* meas      =      argv[2] ;

  std::stringstream stream;    
  
  std::string path1 =  "data/trc/CD";
  std::string path  = path1 + std::to_string(CD_number)
                    + "/" + std::string(meas)
                    + "/" + std::to_string(voltage)
                    + "V";
   
  std::vector<std::string> filenames = get_filenames(path);
  
  for(int i=0; i<filenames.size(); i++){  
  
    stream << "./makeRawTree " << filenames[i] <<  std::endl;
    
    //senza if riesegue per ogni ciclo anche tutti i programmi prima dell'i-esimo
    if(i==filenames.size()-1){
      system(stream.str().c_str());
    }
    
  }
  
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

  
 
        

       

       

      
