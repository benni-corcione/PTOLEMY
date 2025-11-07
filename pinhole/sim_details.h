std::vector<int> GetVcnt(char* folder){
  std::vector<int> volts;
  
  if(strcmp(folder,"pin1")==0){
    volts.push_back(100); //volts.push_back(90);
    volts.push_back(80);
    //volts.push_back(70 );
    volts.push_back(60);
    volts.push_back(50);
    //volts.push_back(40 );
    volts.push_back(30);
    //volts.push_back(20);
    volts.push_back(10 );
  }

  if(strcmp(folder,"pin2d")==0){
    volts.push_back(200); 
    volts.push_back(180);
    volts.push_back(160);
    volts.push_back(140);
    volts.push_back(120);
    volts.push_back(100);
    volts.push_back(80 );
    volts.push_back(60 );
    volts.push_back(40 );
    volts.push_back(20 );
    volts.push_back(10 );
  }

  if(strcmp(folder,"pin2ddown")==0 || strcmp(folder,"pin2dthr50")==0){
    volts.push_back(100); 
    volts.push_back(80);
    volts.push_back(60);
    volts.push_back(40);
    volts.push_back(20);
    volts.push_back(10);
  }

   if(strcmp(folder,"pin2ddownthr50")==0 || strcmp(folder,"pinthr50")==0){
    volts.push_back(50); 
    volts.push_back(40);
    volts.push_back(30);
    volts.push_back(20);
    volts.push_back(10);
  }
  
  return volts;
}


std::vector<int> GetVpin(char* folder){
  std::vector<int> volts;
  
  if(strcmp(folder,"focal_1d_thr100")==0){
    volts.push_back(-97 );
    volts.push_back(-93 );
    volts.push_back(-90 );
    volts.push_back(-87 );
    volts.push_back(-83 );
    volts.push_back(-80 );
    volts.push_back(-70 );
    volts.push_back(-60 );
    volts.push_back(-50 );
    volts.push_back(-40 );
    volts.push_back(-30 );
    volts.push_back(-20 );
    volts.push_back(-10 );
    volts.push_back(0   );
    volts.push_back(+10 );
    volts.push_back(+20 );
    volts.push_back(+30 );
    volts.push_back(+40 );
    volts.push_back(+50 );
    volts.push_back(+60 );
    volts.push_back(+70 );
    volts.push_back(+80 );
    volts.push_back(+90 );
    volts.push_back(+100);
  }

  if(strcmp(folder,"focal_1d_thr50")==0){
    volts.push_back(-49 );
    volts.push_back(-48 );
    volts.push_back(-47 );
    volts.push_back(-46 );
    volts.push_back(-45 );
    volts.push_back(-44 );
    volts.push_back(-43 );
    volts.push_back(-42 );
    volts.push_back(-41 );
    volts.push_back(-40 );
    volts.push_back(-38 );
    volts.push_back(-35 );
    volts.push_back(-30 );
    volts.push_back(-25 );
    volts.push_back(-20 );
    volts.push_back(-15 );
    volts.push_back(-10 );
    volts.push_back(-5  );
    volts.push_back( 0  );
    volts.push_back( 5  );
    volts.push_back( 10 );
    volts.push_back( 15 );
    volts.push_back( 20 );
    volts.push_back( 25 );
    volts.push_back( 30 );
    volts.push_back( 35 );
    volts.push_back( 40 );
    volts.push_back( 45 );
    volts.push_back( 50 );
  }

  if(strcmp(folder,"focal_2d_thr50")==0 || strcmp(folder,"focal_2ddown_thr50")==0 || strcmp(folder,"focal_3d_thr50")==0|| strcmp(folder,"focal_4d_thr50")==0){
    volts.push_back(-49);
    volts.push_back(-48);
    volts.push_back(-47);
    volts.push_back(-46);
    volts.push_back(-45);
    volts.push_back(-43);
    volts.push_back(-41);
    volts.push_back(-40);
    volts.push_back(-38);
    volts.push_back(-35);
    volts.push_back(-30);
    volts.push_back(-25);
    volts.push_back(-20);
    volts.push_back(-10);
    volts.push_back( 0 );
    volts.push_back( 10);
    volts.push_back( 20);
    volts.push_back( 30);
    volts.push_back( 40);
    volts.push_back( 50);
  }

   if(strcmp(folder,"focal_curve_2dup")==0 || strcmp(folder,"focal_curve_2ddown")==0 || strcmp(folder,"focal_curve_dmezzi")==0){
    volts.push_back(-49);
    volts.push_back(-48);
    volts.push_back(-47);
    volts.push_back(-46);
    volts.push_back(-45);
    volts.push_back(-43);
    volts.push_back(-40);
    volts.push_back(-35);
    volts.push_back(-30);
    volts.push_back(-25);
    volts.push_back(-20);
    volts.push_back(-10);
    volts.push_back( 0 );
    volts.push_back( 10);
    volts.push_back( 20);
    volts.push_back( 30);
    volts.push_back( 40);
    volts.push_back( 50);
  }

  if(strcmp(folder,"check_energy")==0){
    volts.push_back(-19);
    volts.push_back(-18);
    volts.push_back(-17);
    volts.push_back(-16);
    volts.push_back(-15);
    volts.push_back(-13);
    volts.push_back(-10);
    volts.push_back(-8 );
    volts.push_back(-6 );
    volts.push_back(-4 );
    volts.push_back(-2 );
    volts.push_back( 0 );
    volts.push_back( 2 );
    volts.push_back( 4 );
    volts.push_back( 6 );
    volts.push_back( 8 );
    volts.push_back( 10);
    volts.push_back( 12);
    volts.push_back( 14);
    volts.push_back( 16);
    volts.push_back( 18);
    volts.push_back( 20);
  }
  
  if(strcmp(folder,"focal4dcheck3")==0 || strcmp(folder,"focal3dcheck3")==0 || strcmp(folder,"focal2dcheck3")==0 || strcmp(folder,"focal1d_low")==0 ||
     strcmp(folder,"focal2dd_low")==0 || strcmp(folder,"focaldmezzi_high")==0 ){
    volts.push_back(45);
    volts.push_back(40);
    volts.push_back(30);
    volts.push_back(20);
    volts.push_back(10);
    volts.push_back(0);
    volts.push_back(-10);
    volts.push_back(-20);
    volts.push_back(-30);
    volts.push_back(-40);
    volts.push_back(-50);
  }


  if(strcmp(folder,"focal4dcheck4")==0 || strcmp(folder,"focal3dcheck4")==0 || strcmp(folder,"focal2dcheck4")==0 || strcmp(folder,"focal1d_high")==0 ||
      strcmp(folder,"focal2d_low")==0   || strcmp(folder,"focal2dd_high")==0){
    volts.push_back(95);
    volts.push_back(90);
    volts.push_back(80);
    volts.push_back(70);
    volts.push_back(60);
    volts.push_back(50);
    volts.push_back(40);
    volts.push_back(30);
    volts.push_back(20);
    volts.push_back(10);
    volts.push_back(0);
  }
    
if(strcmp(folder,"focal2d_high")==0){
      volts.push_back(195);
      volts.push_back(190);
      volts.push_back(180);
      volts.push_back(170);
      volts.push_back(160);
      volts.push_back(150);
      volts.push_back(140);
      volts.push_back(130);
      volts.push_back(120);
      volts.push_back(110);
      volts.push_back(100);
    }
    
    if(strcmp(folder,"focaldmezzi_low")==0){
          volts.push_back(20);
          volts.push_back(15);
          volts.push_back(5);
          volts.push_back(-5);
          volts.push_back(-15);
          volts.push_back(-25);
          volts.push_back(-35);
          volts.push_back(-45);
          volts.push_back(-55);
          volts.push_back(-65);
          volts.push_back(-75);
        }
    
  if(strcmp(folder,"check_rate")==0){
    for(int i=0; i<32; i++){
      volts.push_back(+12-(i*2));
    }
   }
  return volts;
}


std::vector<int> GetCNTs(char* folder){
  std::vector<int> cnt_radius;
  
  if(strcmp(folder,"pin1")==0){
    //cnt_radius.push_back(10) ; //cnt_radius.push_back(25 );
    cnt_radius.push_back(50) ; cnt_radius.push_back(100);
    cnt_radius.push_back(500);
  }

  if(strcmp(folder,"pin2d")==0 ||strcmp(folder,"pin2ddown")==0 ||strcmp(folder,"pinthr50")==0 || strcmp(folder,"pin2dthr50")==0 || strcmp(folder,"pin2ddownthr50")==0 || strcmp(folder,"check_energy")==0 || strcmp(folder,"check_rate")==0 || strcmp(folder,"focal2dcheck3")==0 || strcmp(folder,"focal3dcheck3")==0 || strcmp(folder,"focal4dcheck3")==0 || strcmp(folder,"focal2dcheck4")==0 || strcmp(folder,"focal3dcheck4")==0 || strcmp(folder,"focal4dcheck4")==0 ||  strcmp(folder,"aiuto")==0 || strcmp(folder,"focal_curve_2dup")==0 || strcmp(folder,"focal_curve_2ddown")==0 || strcmp(folder,"focal_curve_dmezzi")==0 || strcmp(folder,"focal1d_high")==0
     || strcmp(folder,"focal1d_low")==0 || strcmp(folder,"focal2d_high")==0 || strcmp(folder,"focal2d_low")==0 || strcmp(folder,"focal2dd_high")==0
     || strcmp(folder,"focal2dd_low")==0 || strcmp(folder,"focaldmezzi_high")==0
     || strcmp(folder,"focaldmezzi_low")==0 ){
    cnt_radius.push_back(500);
  }
  
  return cnt_radius;
}

std::vector<int> GetPin(char* folder){
  std::vector<int> pin_radius;
  
  if(strcmp(folder,"pin1")==0){
    pin_radius.push_back(5 ) ;
    pin_radius.push_back(10) ; pin_radius.push_back(15 );
    pin_radius.push_back(25) ; pin_radius.push_back(35 );
    pin_radius.push_back(50) ; pin_radius.push_back(100);
  }

   
  if(strcmp(folder,"focal_1d_thr100")==0){
    pin_radius.push_back(10 );
    pin_radius.push_back(25 );
    pin_radius.push_back(50 );
    pin_radius.push_back(100); 
  }

   if(strcmp(folder,"focal_1d_thr50")==0){
    pin_radius.push_back(100);
    pin_radius.push_back(50);
    pin_radius.push_back(25);
    pin_radius.push_back(10);
  }

   if(strcmp(folder,"focal_2d_thr50")==0 || strcmp(folder,"focal_2ddown_thr50")==0  || strcmp(folder,"focal_3d_thr50")==0 || strcmp(folder,"focal_4d_thr50")==0){
    pin_radius.push_back(10);
    pin_radius.push_back(25);
    pin_radius.push_back(50);
    pin_radius.push_back(100);
    
  }
   
   if(strcmp(folder,"pin2d")==0 || strcmp(folder,"pinthr50")==0){
     pin_radius.push_back(15);
     pin_radius.push_back(25);
     pin_radius.push_back(35);
     pin_radius.push_back(50);
     pin_radius.push_back(100);
   }


   if(strcmp(folder,"pin2ddown")==0 || strcmp(folder,"pin2ddownthr50")==0 || strcmp(folder,"check_energy")==0 || strcmp(folder,"check_rate")==0){
    pin_radius.push_back(25);
  }

   if(strcmp(folder,"pin2dthr50")==0 || strcmp(folder,"focal_curve_2dup")==0 || strcmp(folder,"focal_curve_2ddown")==0 || strcmp(folder,"focal_curve_dmezzi")==0 || strcmp(folder,"focal1d_high")==0
      || strcmp(folder,"focal1d_low")==0 || strcmp(folder,"focal2d_high")==0 || strcmp(folder,"focal2d_low")==0 || strcmp(folder,"focal2dd_high")==0
      || strcmp(folder,"focal2dd_low")==0 || strcmp(folder,"focaldmezzi_high")==0
      || strcmp(folder,"focaldmezzi_low")==0 ){
     pin_radius.push_back(10);
     pin_radius.push_back(25);
     pin_radius.push_back(50);
     pin_radius.push_back(100);
   }


   if(strcmp(folder,"focal2dcheck3")==0 || strcmp(folder,"focal3dcheck3")==0 || strcmp(folder,"focal2dcheck4")==0 || strcmp(folder,"focal3dcheck4")==0){
     pin_radius.push_back(10);
     pin_radius.push_back(25);
   }

    if(strcmp(folder,"focal4dcheck3")==0 || strcmp(folder,"focal4dcheck4")==0){
     pin_radius.push_back(10); 
     pin_radius.push_back(25);
     pin_radius.push_back(50);
   }

   
   return pin_radius;
}
