// This Arduino code is written for MaM Sense Board ECG output. For more info visit: https://mamhightech.com/MamSense.html
// The program measures the heart rate and sends it to serial communication.

const short Fs = 250;    // Sampling frequency(Hz)
const int   Ts = 1e6/Fs;  // Sampling period (us)

//Create the coefficient matrices for low pass filter at 35 Hz.
const float nm[7] = {0.00086, 0.00515, 0.01286,  0.01715,  0.01286,  0.00515, 0.00086};         
const float dn[7] = {1,-3.09854,4.41644,-3.55659,1.68514,-0.44112,0.04956};

unsigned long start_time=0;
unsigned long current_time=0;
short raw[200] =  {0};
short ecg[200] = {0};
unsigned long cnt=2;
short pk_thrs=60; 
short peak = pk_thrs;
short pk_n=0;  
boolean pk_reached;
unsigned long pk_loc[5]={0};
short pk_cnt=0;
float rate;
void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
start_time = micros();
}

void loop() {
  
  current_time = micros();
  
  if(current_time - start_time>= Ts){
 
    start_time = current_time;
//    Serial.println(start_time);
    raw[cnt%200] = analogRead(A2)-350;
    // Filter the raw signal with LPF
    ecg[cnt%200] = round( raw[cnt%200]*nm[0] +raw[(cnt-1)%200]*nm[1] +raw[(cnt-2)%200]*nm[2] +raw[(cnt-3)%200]*nm[3] +raw[(cnt-4)%200]*nm[4] +raw[(cnt-5)%200]*nm[5]+raw[(cnt-6)%200]*nm[6]-ecg[(cnt-1)%200]*dn[1] -ecg[(cnt-2)%200]*dn[2] -ecg[(cnt-3)%200]*dn[3] -ecg[(cnt-4)%200]*dn[4] -ecg[(cnt-5)%200]*dn[5] -ecg[(cnt-6)%200]*dn[6] );
 
    if(ecg[cnt%200]-ecg[(cnt+188)%200]>pk_thrs){    // Detects the R peaks.
      if(ecg[cnt%200]>peak){
      pk_n = cnt%200;
      peak = ecg[cnt%200];
      }
      pk_reached = true;
    }
   
    if(cnt%200==(20+pk_n)%200 && pk_reached ){
      pk_reached =false;
     
      if((peak - ecg[(pk_n+10)%200]) >= pk_thrs){             // Measures the heart rate using the distance between previous peaks.
       pk_loc[pk_cnt] = cnt;
       
       for (int i=1;i<5;i++){
      
        rate+= pk_loc[(pk_cnt+i+1)%5] - pk_loc[(pk_cnt+i)%5];
       }
       rate = rate/4;
       rate = 60*Fs/rate;
       Serial.print("Heart Rate: ");
       Serial.println(round(rate));
       rate=0; 
       pk_cnt++;
       if (pk_cnt==5){ pk_cnt=0;}
      }
      peak = -200;
    }
    //Uncomment the line if you want to monitor the ECG signal
//    Serial.println(ecg[cnt%200]);
  
    cnt++;
  }
}
