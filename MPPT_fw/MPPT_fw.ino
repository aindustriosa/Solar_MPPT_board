#include <Streaming.h> //para la salida de print y endl http://arduiniana.org/libraries/streaming/
#include <EEPROM.h>
#include "esc.h"
#include "Ticker.h"
#include "current.h"
#include "Config.h"

#define LED1 4
#define LED2 5
#define BTN 2

//---------------DEBUG---------------
//#define PIN_DEBUG 8
bool DEBUG = true;
//-------------------P&O SETTINGS--------------
Ticker PO_timer;

#define PO_ENABLE_SPEED 30
#define PO_TIMER 150
#define PO_STEP 1
#define po_samples 10
float v = 0;
float i = 0;
float p = 0;
float Av = 0; //Delta Volts
float Ap = 0; //Delta Power
float p_prev = 0;
float v_prev = 0;

float i_media=0;
float v_media=0;
int cnt_po=1;
int cnt_po_ina_samples=0;
int po_speed =0; //valor d po_speed cuando entramos en po
bool po_enable= false;
void PO_timer_callback();

//-------------------RC SIGNAL-----------------
static byte rcOld;
volatile unsigned long rcRises[4];
volatile unsigned long rcTimes[4];
volatile unsigned int rcChange =0;
void init_RC();

//-----------------ESC----------------------
#define sensibilidad 3
int speed = 0;

//-----------------INA219---------------
#define Ki 270
#define Kv 

//--------------THERMISTOR--------------
#define th1 A6
#define th2 A7

int temp1,temp2 =0;


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
  Serial.begin(9600);

  //pinMode(PIN_DEBUG,INPUT);

  init_ESC();
    delay(1000);
  init_RC();
  
  init_INA219(0);

  PO_timer.setCallback(PO_timer_callback);
  PO_timer.setInterval(PO_TIMER/po_samples);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
  PO_timer.start();

  po_enable = false; //indicamos que iniciamos en speed
  Serial.println("TX_PPM,Mode,ESC_PPM,Voltage,Current,Power");
  Serial.println();

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

    speed = map(rcT[0],1000,2000,0,100);
    //speed = speed/sensibilidad;

    if(speed >100){
        speed=100;
    }
    //Serial.println(rcT[0]);
  }
  sei();
  PO_timer.update();
}

void print_telemetry(){
    Serial.print(speed);
    Serial.print(",");
    if(po_enable){
        Serial.print("10,"); //modo P
        Serial.print(po_speed);
        digitalWrite(LED1,HIGH);
        digitalWrite(LED2,LOW);
    }else{
        Serial.print("5,"); // modo s
        Serial.print(speed);
        digitalWrite(LED1,LOW);
        digitalWrite(LED2,HIGH);
    }
    Serial.print(",");
    Serial.print(v);
    Serial.print(",");
    Serial.print(i);
    Serial.print(",");
    Serial.println(p);
    //Serial.print(Ap);
    //Serial.print(Av);
    //Serial.println(analogRead(th1));
}

void PO_timer_callback(){ 

        if(cnt_po_ina_samples<po_samples){ //samplea cada 150/10 --> 15ms
            v_media += get_LoadVolts();
            i_media += get_LoadCurrent()/Ki;
            cnt_po_ina_samples++;

        }else{ // Al los 10 samples 10*15 --> 150ms se ejecuta el P&O

            v = v_media/po_samples;
            i = i_media/po_samples;
            p = i*v;
            Av = v - v_prev;
            Ap = p - p_prev;

            //DEBUG = digitalRead(PIN_DEBUG);
            if (speed<0) speed=0;
            if(speed < PO_ENABLE_SPEED){ //Si po_speed <50, vel fijada x mando
                po_enable=false; //indicamos que estamos en modo speed
                vel_ESC(speed);
                cnt_po =1; //reseteamos indicador de P&O para que cuando comience se resetee en ciclo 1 y pille speed       
            }else{ //Si po_speed >50 entra P&0
                /*
                ----------COMIENZA P&O--------------------
                */

                if(cnt_po != 1){
                    if(Ap == 0){ 
                        po_speed += PO_STEP;
                        //Serial.println("AP=0");
                    }else{
                        if(Ap>0){
                            //Serial.println("AP>0");
                            if(Av<0){
                                po_speed += PO_STEP;
                                vel_ESC(po_speed);
                                //Serial.println("B2");
                                
                            }else if(Av>=0){
                                po_speed -= PO_STEP;
                                vel_ESC(po_speed);
                                //Serial.println("B1");
                            }
                        }else{
                            //Serial.println("AP<0");
                            if(Av>0){
                                po_speed += PO_STEP;
                                vel_ESC(po_speed);
                                //Serial.println("A2");
                            }else if(Av<=0){
                                po_speed -= PO_STEP;
                                vel_ESC(po_speed);
                                //Serial.println("A1");
                            }
                        }
                    }
                }else{ //Es l primer sample de PO
                    po_enable=true;
                    po_speed = PO_ENABLE_SPEED;
                    po_speed += PO_STEP;
                }
        

                
                cnt_po++;
                /*
                ----------FIN P&O--------------------
                */
                    
            }//end if vel o P&or

                p_prev=p;    
                v_prev=v;

                v_media = 0;
                i_media = 0;
            cnt_po_ina_samples = 0; //reseteamos contador de samples para k comience con los sig 10

            if(DEBUG){
                print_telemetry();
            }

        }//end loop samples

} //end fnct

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
