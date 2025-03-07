//libreria di funzioni che ricavano info leggendo le pshapes

//BASELINE
float GetBaseline(float pshape[2502]){
  float baseline = 0;
  int   i_min    = 0;
  int   i_max    = 450; 		     

  for(int i=i_min; i<i_max; i++){
    baseline += pshape[i];
  }
  baseline /= (i_max-i_min);
  return baseline;
}


//STD_DEV BASELINE
float GetBaselineError(float pshape[2502], float baseline){
  float baseline_error = 0;
  int   i_min          = 0;
  int   i_max          = 450;	

  for(int i=i_min; i<i_max; i++){
    baseline_error += pow((pshape[i]-baseline),2);
  }
  baseline_error /= (i_max-i_min-1);
  return sqrt(baseline_error);
}


//CHARGE
float GetCharge(float pshape[2502], float dt, float baseline){
  float charge = 0;
  
  for(int i=0; i<2500; i++){
    charge += (pshape[i]-baseline);
  }
  
  charge *= dt;
  return (charge*(-1)); //lavoro con carica>0
}


//AMPLITUDE
float GetAmp(float pshape[2502], float baseline){
  float amp = 0;
  float min = 100;
  
  //sto lavorando sempre con segnali ingiù
  for(int i=0; i<2500; i++){
    if (pshape[i]<min){
      min = pshape[i];
    }
  }
  amp = baseline - min; //ampiezza>0
  return (amp);
}



void DynamicMean(float delta[2502], float dyn_mean[2502], int j_max){
  //preparazione array media dinamica
  //se qualcuno non si fida, provi l'algoritmo a parte
  //spiegarlo è un macello :')
  //grazie per la fiducia
  int counter [2500];
  int index   [2500] = {0};
 
  
  for(int l=0; l<2500; l++){
    counter[l] = int(j_max/2);

    if(l < (     int(j_max/2)+1)){
      counter[l] -= (int(j_max/2)-l);
      index[l]   += (int(j_max/2)-l);
    }
    if(l > (2500-int(j_max/2)-1)){
      counter[l] -= (int(j_max/2)+l-2499);
    }
  }

  //leggo tutta la pshape
    for(int i=0; i<2500; i++){ //leggo la pshape i+1

    //realizzo media dinamica
    for(int j=0; j<(int(j_max/2)+counter[i]+1); j++){ 
	dyn_mean[i] += delta[j+i-int(j_max/2)+index[i]];
      }
      //divisione che tiene conto delle condizioni al contorno (#punti usati)
      dyn_mean[i]/=(int(j_max/2)+1+counter[i]); 
    }
}

int Check_Double(float threshold, float dyn_delta[2502]){
  int check=0;
  
   //leggo vettore dei delta
    for(int i=0; i<2499; ++i ){
      if(i < 2489){
	if(dyn_delta[i]  <threshold &&
	   dyn_delta[i+1]>threshold &&
	   dyn_delta[i+2]>threshold &&
	   dyn_delta[i+3]>threshold &&
	   dyn_delta[i+4]>threshold &&
	   dyn_delta[i+5]>threshold &&
	   dyn_delta[i+6]>threshold &&
	   dyn_delta[i+7]>threshold &&
	   dyn_delta[i+8]>threshold &&
	   dyn_delta[i+9]>threshold &&
	   dyn_delta[i+10]>threshold ) 
	  { check++;}
	else{;}
      }
      else if(i > 2489){
	if(dyn_delta[i-10]<threshold &&
	    dyn_delta[i-9]>threshold &&
	    dyn_delta[i-8]>threshold &&
	    dyn_delta[i-7]>threshold &&
	    dyn_delta[i-6]>threshold &&
	    dyn_delta[i-5]>threshold &&
	    dyn_delta[i-4]>threshold &&
	    dyn_delta[i-3]>threshold &&
	    dyn_delta[i-2]>threshold &&
	    dyn_delta[i-1]>threshold &&
	    dyn_delta[i]>threshold  )
	  { check++;}
	else{;}
      }
    } //ho finito di leggere il vettore delta
    return check;
}

//check sui bump in positivo delle pshape
int Check_Up(float pshape[2502], float dyn_delta[2502], float threshold, int CD_number, char* meas, float base, float base_err){
  int check_up   =  0;
  
  //std::cout << "soglia " << base-(4*base_err) << std::endl;
  for(int i=0; i<2499; i++){
   
     //controllo su bumpetti
    if(i>35 && i<2464){
      if(   pshape[i]>(pshape[i-35]+(4*base_err))
	    && pshape[i]>(pshape[i+35]+(4*base_err))){
	if(dyn_delta[i]>threshold && pshape[i]>(base-(4*base_err)))
	{ check_up++;}
	if(dyn_delta[i]>threshold && abs(pshape[i]-pshape[i-10])>8*base_err)
	  { check_up++;}
      }
    }
  }
  //controllo su errore baseline
  //scarto dalle doppie le doppie che non hanno triggerato eh
  if(CD_number==204 && base_err>0.006){check_up++;}
  
  return check_up;

}

//CLASS OF CONTROLS OVER PSHAPE
class Controls{
 public:
  //constructors and destructos
  Controls(){}
  ~Controls(){}

  //getters
  int get_ctrl_double  (void) const { return ctrl_double;  }
  int get_ctrl_trigger (void) const { return ctrl_trigger; }
  int get_ctrl_width   (void) const { return ctrl_width;   }

  //setters
  void set_ctrl_double  (int a) { ctrl_double  = a; }
  void set_ctrl_trigger (int c) { ctrl_trigger = c; }
  void set_ctrl_width   (int e) { ctrl_width   = e; }

 private:
  int ctrl_double;  //ctrl sul superamento soglia doppie/triple
  int ctrl_trigger; //ctrl sull'indice della pshape in cui supera trigger
  int ctrl_width;   //ctrl su larghezza onda a metà ampiezza

};


//CONTROL OVER PSHAPE 
Controls Ctrl_pshape(float pshape[2502], float amp, float trigger, float baseline, float base_error, int CD_number, char* meas){
  Controls ctrl;
  int ctrl_trigger = 0;
  int ctrl_width   = 0;

  //multiple event & width & trigger thresholds
  float threshold_mid      = baseline - (amp * 0.5); //50% -> width
  //float trigger_threshold  = abs(trigger); //pshape studiata in positivo
 
  //width indexes
  int index_1 = 0;
  int index_2 = 0;

  //controllo sulle doppie
  //parametri che sembrano ottimali
  int n_sum = 45;
  
  //array in cui sommo deltay di punti a coppia, tipo (y1-y2)+(y1-y3)+(y1-y4)+...+(y1-y_nsum)
  float delta[2502] = {0};
  float dyn_delta[2502] = {0};
  
  float smooth_shape[2502] = {0};
  DynamicMean(pshape,smooth_shape,10);
  //leggo tutta la pshape
  for(int i=0; i<2499; i++){ //leggo la pshape i+1
    
    //controllo doppie
    
    //riempimento array delta
    //attenzione alle condizioni al contorno
    if(i < 2499-n_sum) {
      for(int l=0; l<n_sum; l++){ delta[i] += (pshape[i]-pshape[i+n_sum-l]); }
    }
    else{
	//fattore di scala per aggiustare il contorno
      float scale = n_sum/(2500-i);
      for(int l=0; l<(2500-i); l++){ delta[i] += (pshape[i]-pshape[i+l+1])*scale; }	
    }

    /*
    //controllo del trigger -> dove passo il trigger
    if( (-1)*pshape[i]   < trigger_threshold &&
	(-1)*pshape[i+1] > trigger_threshold   ){
      if      (ctrl_trigger == 0) { ctrl_trigger = i+1; }
      else if (ctrl_trigger != 0) { ; }
    }
    */

    
     
    //controllo sulla width 
    //superamento soglia in discesa (prima volta)
    if( smooth_shape[i]   > threshold_mid &&
	smooth_shape[i+1] < threshold_mid   ){
      if ( index_1 == 0 && index_2 == 0 ) { index_1 = i+1; }
      else                                { ; }
    }
    //superamento soglia in salita (prima volta)
    if( smooth_shape[i]   < threshold_mid &&
        smooth_shape[i+1] > threshold_mid   ){
      if ( index_1 != 0 && index_2 == 0 ) { index_2 = i+1; }
      else                                { ; }
    }

    
  }//for sulla lettura pshape

  //std::cout << index_1 << " " << index_2 << std::endl;
  //calcolo della larghezza a metà ampiezza
  if (index_1 != 0 && index_2 != 0 ){
    ctrl_width = index_2 - index_1;
  }

  if(index_1!=0 && index_2==0){
    ctrl_width = 2500-index_1;
  }

  //std::cout << ctrl_width << std::endl;
  //media dinamica sulla delta per renderla più liscia
  int dyn_sum = 8;
  float thresh_double = 1.;
  DynamicMean(delta,dyn_delta,dyn_sum);

  int ctrl_double = Check_Double(thresh_double, dyn_delta);
  int check_up = Check_Up(pshape, dyn_delta, thresh_double, CD_number, meas, baseline, base_error);

  if(check_up>0){ctrl_double=1;}
  
  //std::cout << ctrl_width << " "  << std::endl;
  ctrl.set_ctrl_double(ctrl_double);
  ctrl.set_ctrl_width(ctrl_width);
  ctrl.set_ctrl_trigger(ctrl_trigger);
 
  return(ctrl);
}


void search_and_remove(const std::filesystem::path& directory, const std::filesystem::path& file_name){
     std::stringstream stream;  
     auto d = std::filesystem::directory_iterator(directory);

    auto found = std::find_if(d, end(d), [&file_name](const auto& dir_entry)
    {
        return dir_entry.path().filename() == file_name;
    });

    if (found != end(d))
    {
        //elimino tutti i tree singoli inutili
      stream << "cd " << directory.c_str() << std::endl;
      stream << "rm -f " << file_name.c_str() << std::endl;
      system(stream.str().c_str());
      
    }
}  


float Get_width_min(int CD_number,int voltage, char* meas){
  float width_min;

  if(CD_number==188){
    if(voltage>94  && voltage<102){ width_min = 80; }
    if(voltage>101 && voltage<105){ width_min = 100;}
    if(voltage==105)              { width_min = 120;}
  }

  if(CD_number==204 && strcmp(meas,"B60_post_cond3_moku")==0){ width_min = 60; }

  if(CD_number==204 && strcmp(meas,"B60_preamp_post_cond1")==0){ width_min = 40; }
  
  return width_min;
}

float Get_width_max(int CD_number,int voltage, char* meas){
  float width_max;

 if(CD_number==188){
    if(voltage>94   && voltage<98 ){ width_max = 200;}
    if(voltage==98 || voltage==100){ width_max = 210;}
    if(voltage==99 || voltage==101){ width_max = 220;}
    if(voltage==102|| voltage==103){ width_max = 240;}
    if(voltage==104)               { width_max = 250;}
    if(voltage==105)               { width_max = 320;}
 }

 if(CD_number==204 && strcmp(meas,"B60_post_cond3_moku")==0){
    if(voltage==96)               { width_max = 300;}
    if(voltage==97)               { width_max = 250;}
    if(voltage>97  && voltage<101){ width_max = 260;}
    if(voltage==104)              { width_max = 260;}
    if(voltage>100 && voltage<104){ width_max = 270;}
    if(voltage==105)              { width_max = 270;}
    if(voltage>105 && voltage<110){ width_max = 280;}
    if(voltage==110)              { width_max = 320;}
  }

 
 if(CD_number==204 && strcmp(meas,"B60_preamp_post_cond1")==0){
    if(voltage==97)                 { width_max = 340;}
    if(voltage==99  || voltage==101){ width_max = 280;}
    if(voltage==103 || voltage==106){ width_max = 290;}
 }
 
  return width_max;
}

float Get_charge_min(int CD_number,int voltage, char* meas){
  float charge_min;

  if(CD_number==188){ charge_min = 1.0*1E-6; }
  if(CD_number==204 && strcmp(meas,"B60_post_cond3_moku")==0){ charge_min = 0.2*1E-6; }
  if(CD_number==204 && strcmp(meas,"B60_preamp_post_cond1")==0){ charge_min = 0; }
  return charge_min;
}

float Get_base_err_max(int CD_number,int voltage, char* meas){
  float base_err_max;

  if(CD_number==188){
    if(voltage==95 || voltage==96 || voltage==102){
     base_err_max = 0.011; }
    if(voltage==99 || voltage>102){
      base_err_max = 0.012; }
    else{ base_err_max = 0.0125; }
  }

  if(CD_number==204 && strcmp(meas,"B60_post_cond3_moku")==0){ base_err_max = 0.007; }
  if(CD_number==204 && strcmp(meas,"B60_preamp_post_cond1")==0){ base_err_max = 0.007; }
  
  return base_err_max;
}


