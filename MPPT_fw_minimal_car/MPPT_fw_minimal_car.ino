#include "esc.h"
#include "Ticker.h"
#include "current.h"
#include "Config.h"

//---------------DEBUG---------------
bool DEBUG = true;

//-------------------P&O Variables--------------
Ticker cicle_timer;
float v = 0;
float i = 0;
float p = 0;
float p_prev = 0;
float v_prev = 0;
int state = 0;

float i_media=0;
float v_media=0;
int cnt_po_ina_samples=0;
void PO_timer_callback();

//-------------------RC SIGNAL-----------------
static byte rcOld;
volatile unsigned long rcRises[4];
volatile unsigned long rcTimes[4];
volatile unsigned int rcChange =0;
void init_RC();

//-----------------ESC----------------------
int PPM1 = 50;    
int speed = 50;   // 50 para configuración bidireccional, 0 para unidireccional

//-----------------INA219---------------
#define Ki 270    // coeficiente de axuste de corrente, pola modificación da resistencia Shunt


ISR (PCINT1_vect){
  byte rcNew = PINC & 15; //PINC & 1111 pilla el estado de los pines A0-A3
  byte changes = rcNew^rcOld; //saca el pin que ha cambiado comparando el estado de los 4 anteriores cn s 4 nuevos
  byte channel =0;
  unsigned long now=micros();
  while(changes){ // de 0000 a 1111
    if((changes &1)){ //Ha cambiado este canal ?
      if((rcNew & (1<<channel))){ //si es flanco subida
        rcRises[channel] =  now;  //t0=ini
      }else{                      //si es flanco bajada
        rcTimes[channel] = now -rcRises[channel];//t1=nw-t0;
      }
    }
    changes >>=1;
    ++channel;
    ++rcChange; //activamos la flag de nuevos cambios para el loop
  }
  rcOld = rcNew;
}


void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  Serial.begin(9600);
  digitalWrite(LED1,HIGH);
  digitalWrite(LED2,HIGH);
  
  init_ESC();
  delay(1000);
  init_RC();

  digitalWrite(LED1,HIGH);
  digitalWrite(LED2,HIGH);
  
  init_INA219(0);

  cicle_timer.setCallback(PO_timer_callback);
  cicle_timer.setInterval(CICLE_TIMER/CICLE_SAMPLES);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
  cicle_timer.start();

  Serial.println("TX_PPM,ESC_PPM,Voltage,Current,Power,State");

  delay(1000);

}


void loop() {
  unsigned long rcT[4];
  unsigned int rcN;

  if(rcChange){
    cli();
    rcN=rcChange;
    rcChange =0; //borramos la flag de nuevos cambios
    //leemos los registros de tiempos d cada puerto
    rcT[0] = rcTimes[0];
    rcT[1] = rcTimes[1];
    rcT[2] = rcTimes[2];
    rcT[3] = rcTimes[3];
    sei();

    PPM1 = map(rcT[0],1000,2000,0,100);

    if(PPM1 >100) PPM1=100;
    if(PPM1 <0) PPM1 = 0;
  }
  sei();
  cicle_timer.update();
}

void print_telemetry(){
    Serial.print(PPM1);
    Serial.print(",");
    Serial.print(speed);
    Serial.print(",");
    Serial.print(v);
    Serial.print(",");
    Serial.print(i);
    Serial.print(",");
    Serial.print(p);
    Serial.print(",");
    switch (state) {
      case AVANCE:
        Serial.println("AVANCE");          
      break;
      case LIMITADO:
        Serial.println("LIMITADO");          
      break;
      case REVERSA:
        Serial.println("REVERSA");          
      break;
    }
    

}

void PO_timer_callback(){ 

        
        if(cnt_po_ina_samples<CICLE_SAMPLES){ //samplea cada 150/10 --> 15ms
            v_media += get_LoadVolts();
            i_media += get_LoadCurrent()/Ki;
            cnt_po_ina_samples++;

        }
        else{ // Al los n samples se ejecuta la máquina de estados

            v = v_media/CICLE_SAMPLES;
            i = i_media/CICLE_SAMPLES;
            p = i*v;

            switch (state) {
              case AVANCE:
                if (PPM1<50) {
                  state=REVERSA;
                  break;
                }
                if (v<UMBRAL_VOLT) state=LIMITADO;
                if (speed < PPM1) speed = speed + PPM_INCREMENTO;
                else speed = PPM1;
                digitalWrite(LED1,HIGH);
                digitalWrite(LED2,LOW);
              break;
              case LIMITADO:
                if (PPM1<50) {
                  state=REVERSA;
                  break;
                }
                if (v>UMBRAL_VOLT) state=AVANCE;
                else speed = speed - PPM_DECREMENTO;
                if (speed<50) speed=50;
                digitalWrite(LED1,LOW);
                digitalWrite(LED2,HIGH);
              break;
              case REVERSA:
                if (PPM1<45) speed=REV_SPEED; //hacia atrás, velocidad fija
                if (PPM1>=50) state=AVANCE;
                digitalWrite(LED1,LOW);
                digitalWrite(LED2,LOW);
              break;
              default:
              // statements
              break;
            }
            
            
         
         vel_ESC(speed); 
         p_prev=p;    
         v_prev=v;

         v_media = 0;
         i_media = 0;
         cnt_po_ina_samples = 0; //reseteamos contador de samples para k comience con los sig 10

         if(DEBUG){
            print_telemetry();
         }
      }//end if vel o P&or



} //end callback

void init_RC(){
  rcOld =0;
  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  pinMode(A2,INPUT);
  pinMode(A3,INPUT);
  PCMSK1 |= 0x0F; //pin-change interrupt en los 4 primeros pines del PORTC (A0-A3)
  PCIFR |= 0x02; //clear pin-change interrupt
  PCICR |= 0x02; //pin-change interrupt enable
}
