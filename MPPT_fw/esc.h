#define ESC_pin     9
#define initpulse   1000 //algunos ESC se inician con un pulso de 1000 otros cn 1500 otros cn 2000
//#define inc_steps   1

void init_ESC();
void vel_ESC(char percent); //0 a 100
