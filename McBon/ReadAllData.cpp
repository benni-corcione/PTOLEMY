#include <cstdlib>
#include <string>
#include <iostream>

int main(int argc, char* argv[]) {

  if(argc!=3){
    std::cout << "USE: ./ReadAllData run_min run_max" << std::endl;
    std::cout << "EXAMPLE: ./ReadAllData 1 101" << std::endl;
    exit(1);
  }

  int nmin = atoi(argv[1]);
  int nmax = atoi(argv[2]);
  
  std::string command = "make ReadData";
  system(command.c_str());
  
    for (int i = nmin; i <= nmax; ++i) {
        std::string command = "./ReadData 0 " + std::to_string(i);
        system(command.c_str());
    }
    return 0;
}
