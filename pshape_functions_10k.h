//libreria di funzioni che ricavano info leggendo le pshapes

//BASELINE
float GetBaseline_10k(float pshape[10000]){
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
float GetBaselineError_10k(float pshape[10000], float baseline){
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
float GetCharge_10k(float pshape[10000], float dt, float baseline){
  float charge = 0;
  
  for(int i=0; i<10000; i++){
    charge += (pshape[i]-baseline);
  }
  
  charge *= dt;
  return (charge*(-1)); //lavoro con carica>0
}


//AMPLITUDE
float GetAmp_10k(float pshape[10000], float baseline){
  float amp = 0;
  float min = 0;
  
  //sto lavorando sempre con segnali ingiù
  for(int i=0; i<10000; i++){
    if (pshape[i]<min){
      min = pshape[i];
    }
  }
  amp = baseline - min; //ampiezza>0
  return abs(amp);
}



void DynamicMean_10k(float delta[9999], float dyn_mean[9999], int j_max){
  //preparazione array media dinamica
  //se qualcuno non si fida, provi l'algoritmo a parte
  //spiegarlo è un macello :')
  //grazie per la fiducia
  int counter [10000];
  int index   [10000] = {0};
 
  
  for(int l=0; l<10000; l++){
    counter[l] = int(j_max/2);

    if(l < (     int(j_max/2)+1)){
      counter[l] -= (int(j_max/2)-l);
      index[l]   += (int(j_max/2)-l);
    }
    if(l > (10000-int(j_max/2)-1)){
      counter[l] -= (int(j_max/2)+l-9999);
    }
  }

  //leggo tutta la pshape
    for(int i=0; i<10000; i++){ //leggo la pshape i+1

    //realizzo media dinamica
    for(int j=0; j<(int(j_max/2)+counter[i]); j++){ 
	dyn_mean[i] += delta[j+i-int(j_max/2)+index[i]];
      }
      //divisione che tiene conto delle condizioni al contorno (#punti usati)
      dyn_mean[i]/=(int(j_max/2)+1+counter[i]); 
    }
}

int Check_Double_10k(float threshold, float dyn_delta[9999]){
  int check=0;
  
   //leggo vettore dei delta
    for(int i=0; i<9999; ++i ){
      if(i < 9989){
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
      else if(i > 9989){
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


//CLASS OF CONTROLS OVER PSHAPE
class Controls_10k{
 public:
  //constructors and destructos
  Controls_10k(){}
  ~Controls_10k(){}

  //getters
  int get_ctrl_double_10k  (void) const { return ctrl_double_10k;  }
  int get_ctrl_trigger_10k (void) const { return ctrl_trigger_10k; }
  int get_ctrl_width_10k   (void) const { return ctrl_width_10k;   }

  //setters
  void set_ctrl_double_10k  (int a) { ctrl_double_10k  = a; }
  void set_ctrl_trigger_10k (int c) { ctrl_trigger_10k = c; }
  void set_ctrl_width_10k   (int e) { ctrl_width_10k   = e; }

 private:
  int ctrl_double_10k;  //ctrl sul superamento soglia doppie/triple
  int ctrl_trigger_10k; //ctrl sull'indice della pshape in cui supera trigger
  int ctrl_width_10k;   //ctrl su larghezza onda a metà ampiezza

};


//CONTROL OVER PSHAPE 
Controls_10k Ctrl_pshape_10k(float pshape[10000], float amp, float trigger, float baseline){
  Controls_10k ctrl;
  int ctrl_trigger = 0;
  int ctrl_width   = 0;

  //multiple event & width & trigger thresholds
  float threshold_mid      = amp * 0.5; //50% -> width
  float trigger_threshold  = abs(trigger); //pshape studiata in positivo
 
  //width indexes
  int index_1 = 0;
  int index_2 = 0;

  //controllo sulle doppie
  //parametri che sembrano ottimali
  int n_sum = 45;
  
  //array in cui sommo deltay di punti a coppia, tipo (y1-y2)+(y1-y3)+(y1-y4)+...+(y1-y_nsum)
  float delta[9999] = {0};
  float dyn_delta[9999] = {0};
  
  //leggo tutta la pshape
  for(int i=0; i<9999; i++){ //leggo la pshape i+1
    
    //controllo doppie
    
    //riempimento array delta
    //attenzione alle condizioni al contorno
    if(i < 9999-n_sum) {
      for(int l=0; l<n_sum; l++){ delta[i] += (pshape[i]-pshape[i+n_sum-l]); }
    }
    else{
	//fattore di scala per aggiustare il contorno
      float scale = n_sum/(10000-i);
      for(int l=0; l<(10000-i); l++){ delta[i] += (pshape[i]-pshape[i+l+1])*scale; }	
    }

    
    //controllo del trigger -> dove passo il trigger
    if( (-1)*pshape[i]   < trigger_threshold &&
	(-1)*pshape[i+1] > trigger_threshold   ){
      if      (ctrl_trigger == 0) { ctrl_trigger = i+1; }
      else if (ctrl_trigger != 0) { ; }
    }

    //controllo sulla width 
    //superamento soglia in discesa (prima volta)
    if( (-1)*pshape[i]   < threshold_mid &&
	(-1)*pshape[i+1] > threshold_mid   ){
      if ( index_1 == 0 && index_2 == 0 ) { index_1 = i+1; }
      else                                { ; }
    }
    //superamento soglia in salita (prima volta)
    if( (-1)*pshape[i]   > threshold_mid &&
	(-1)*pshape[i+1] < threshold_mid   ){
      if ( index_1 != 0 && index_2 == 0 ) { index_2 = i+1; }
      else                                { ; }
    }
  
    
  }//for sulla lettura pshape
   
  //calcolo della larghezza a metà ampiezza
  if (index_1 != 0 && index_2 != 0 ){
    ctrl_width = index_2 - index_1;
  }
  
  //media dinamica sulla delta per renderla più liscia
  int dyn_sum = 8;
  float thresh_double = 1.;
  DynamicMean_10k(delta,dyn_delta,dyn_sum);

  int ctrl_double = Check_Double_10k(thresh_double, dyn_delta);

  ctrl.set_ctrl_double_10k(ctrl_double);
  ctrl.set_ctrl_width_10k(ctrl_width);
  ctrl.set_ctrl_trigger_10k(ctrl_trigger);
 
  return(ctrl);
}


