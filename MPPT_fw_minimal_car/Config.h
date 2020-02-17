

// Definición de pines de Arduino Nano
#define LED1 4
#define LED2 5
#define BTN 2
#define PPM_OUT_1 9
#define PPM_OUT_2 8
#define PPM_OUT_3 7
#define PPM_OUT_4 6
#define PPM_IN_1 A0
#define PPM_IN_2 A1
#define PPM_IN_3 A2
#define PPM_IN_4 A3

// Parámetros de configuración xerais
#define CICLE_TIMER 100   //tempo de ciclo en ms
#define CICLE_SAMPLES 10  //mostras por ciclo
#define PPM_DECREMENTO 3   //cantidade decrementada en caso de limitar
#define PPM_INCREMENTO 2  //aceleración
#define UMBRAL_VOLT 12    //voltaxe por debaixo da cal se limita
#define REV_SPEED 40    //velocidad fija en reversa

// Estados FSM
#define AVANCE 0
#define LIMITADO 1
#define REVERSA 2
