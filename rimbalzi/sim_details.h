std::vector<int> GetVoltages(char* folder, char* subfolder){
  std::vector<int> volts;

  if(strcmp(folder,"rimbalzi")==0 && strcmp(subfolder,"rimbalzi_1")==0){
    volts.push_back(110); volts.push_back(100); volts.push_back(70);
    volts.push_back(30) ; volts.push_back(135) ;
  }

  if(strcmp(folder,"rimbalzi")==0 && strcmp(subfolder,"rimbalzi_2")==0){
    volts.push_back(100);
  }
  
  if(strcmp(folder,"rimbalzi")==0 && strcmp(subfolder,"rimbalzi_3")==0){
    volts.push_back(100);
  }

  if(strcmp(folder,"rimbalzi")==0 && strcmp(subfolder,"presim_rimbalzi_4")==0){
    volts.push_back(100);
  }
  if(strcmp(folder,"rimbalzi")==0 && strcmp(subfolder,"rimbalzi_4")==0){
    volts.push_back(100);
  }
  return volts;
}

std::vector<int> GetSimNumber(char* folder, char* subfolder){
  std::vector<int> sim;

  if(strcmp(folder,"rimbalzi")==0 && strcmp(subfolder,"rimbalzi_1")==0){
     sim.push_back(1);
  }

   if(strcmp(folder,"rimbalzi")==0 && strcmp(subfolder,"rimbalzi_2")==0){
     sim.push_back(1);
  }
   return sim;
}

std::vector<int> GetEnergy(char* folder, char* subfolder){
  std::vector<int> energy;

  if(strcmp(folder,"rimbalzi")==0 && strcmp(subfolder,"rimbalzi_1")==0){
     energy.push_back(110);  energy.push_back(100);  energy.push_back(70 );
     energy.push_back(30 );  energy.push_back(10);  energy.push_back(2);
     energy.push_back(125);  energy.push_back(135);
   }

   if(strcmp(folder,"rimbalzi")==0 && strcmp(subfolder,"rimbalzi_2")==0){
     energy.push_back(100);  energy.push_back(80);  energy.push_back(50);
     energy.push_back(25 );  energy.push_back(10);  energy.push_back(5 );
     energy.push_back(2  ); 
   }
   return energy;
}

std::vector<int> GetCNTs(char* folder, char* subfolder){
  std::vector<int> cnt_radius;

  if(strcmp(folder,"rimbalzi")==0 && strcmp(subfolder,"rimbalzi_1")==0){
    cnt_radius.push_back(15) ; cnt_radius.push_back(40) ;
    cnt_radius.push_back(200); cnt_radius.push_back(500);
  }
  
  if(strcmp(folder,"rimbalzi")==0 && strcmp(subfolder,"rimbalzi_2")==0){
    cnt_radius.push_back(15 ); cnt_radius.push_back(40 );
    cnt_radius.push_back(100); cnt_radius.push_back(200);
    cnt_radius.push_back(300); cnt_radius.push_back(500);
  }
  
  if(strcmp(folder,"rimbalzi")==0 && strcmp(subfolder,"rimbalzi_3")==0){
    cnt_radius.push_back(15 ); cnt_radius.push_back(40 );
    cnt_radius.push_back(200); cnt_radius.push_back(500);
    cnt_radius.push_back(300); cnt_radius.push_back(500);
  }
  
  if(strcmp(folder,"rimbalzi")==0 && strcmp(subfolder,"presim_rimbalzi_4")==0){
    cnt_radius.push_back(15 );   cnt_radius.push_back(40 );
    cnt_radius.push_back(100); cnt_radius.push_back(200);
    cnt_radius.push_back(300); cnt_radius.push_back(500);
  }

  if(strcmp(folder,"rimbalzi")==0 && strcmp(subfolder,"rimbalzi_4")==0){
    cnt_radius.push_back(15 );   cnt_radius.push_back(40 );
    cnt_radius.push_back(100); cnt_radius.push_back(200);
    cnt_radius.push_back(300); cnt_radius.push_back(500);
  }
  return cnt_radius;
}

std::vector<int> Getdistance(char* folder, char* subfolder){
  std::vector<int> d;
  
  if(strcmp(folder,"rimbalzi")==0){
    d.push_back(600); //micron 
  }
  return d;
}

std::vector<int> GetSubstrate(char* folder, char* subfolder){
  std::vector<int> substrate;  //in micron
  if(strcmp(folder,"rimbalzi")==0 && strcmp(subfolder,"rimbalzi_1")==0){
    substrate.push_back(0);
  }
  if(strcmp(folder,"rimbalzi")==0 && strcmp(subfolder,"rimbalzi_2")==0){
    substrate.push_back(1000);
    substrate.push_back(3000);
    substrate.push_back(5000);
  }
  if(strcmp(folder,"rimbalzi")==0 && strcmp(subfolder,"rimbalzi_3")==0){
    substrate.push_back(4000);
  }

  if(strcmp(folder,"rimbalzi")==0 && strcmp(subfolder,"rimbalzi_4")==0){
    substrate.push_back(4000);
  }
  return substrate;
}

std::vector<int> GetDistGen(char* folder, char* subfolder){
  std::vector<int> dist_gen;  //in micron
  if(strcmp(folder,"rimbalzi")==0 && strcmp(subfolder,"rimbalzi_3")==0){
    dist_gen.push_back(0);
    dist_gen.push_back(50);
    dist_gen.push_back(100);
    dist_gen.push_back(150);
    dist_gen.push_back(200);
    dist_gen.push_back(250);
    dist_gen.push_back(300);
    dist_gen.push_back(350);
    dist_gen.push_back(400);
    dist_gen.push_back(450);
    dist_gen.push_back(500);
    dist_gen.push_back(550);
    dist_gen.push_back(600);
    dist_gen.push_back(650);
    dist_gen.push_back(700);
  }

   if(strcmp(folder,"rimbalzi")==0 && strcmp(subfolder,"rimbalzi_4")==0){
     //dist_gen.push_back(0);
    dist_gen.push_back(15);
    dist_gen.push_back(25);
    dist_gen.push_back(50);
    dist_gen.push_back(75);
    dist_gen.push_back(100);
    dist_gen.push_back(150);
    dist_gen.push_back(200);
    dist_gen.push_back(250);
    dist_gen.push_back(300);
    dist_gen.push_back(350);
    dist_gen.push_back(400);
    dist_gen.push_back(450);
    dist_gen.push_back(500);
    dist_gen.push_back(550);
    dist_gen.push_back(600);
    dist_gen.push_back(650);
    dist_gen.push_back(700);
    dist_gen.push_back(750);
    dist_gen.push_back(800);
  }
  
  return dist_gen;
}
