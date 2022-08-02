/*
 * ControlePI-ATMega328p.c
 *
 * ! Archive Description.
*
* \author Matheus Barbi. M.
  \author Rodri Jost. 
* \since 31/07/2022
* \version 1.5
*/

#include <avr/io.h>
#include <avr/interrupt.h>

///! Macros and Control Variables

#define tst_bit(Y,bit_x)  (Y&(1<<bit_x)) 
#define set_bit(Y, bit_x) (Y|=(1<<bit_x))
#define clr_bit(Y, bit_x) (Y&=~(1<<bit_x))
#define cpl_bit(Y, bit_x) (Y^=(1<<bit_x))

///!  Frequência Interna 8MHZ
#define F_CPU 8000000UL

///! Constante de Controle
/*!
    KA  coeficiente de anti wind-up
    TS  Periodo de amostragem (ms)
*/
#define acionaMotor 1 
#define KP 20 
#define KI 30.00
#define KA 0.01 
#define TS 1 

///! Variaveis de Controle
/*!
    r[]  SET POINT
*/
volatile unsigned int leituraAnalogica = 0;
volatile int r[] = {300, 425}; 
volatile float u = 0, us[2], e[2], yf[2], usat = 0, k1 = 0, k2 = 0, aw = 0;
volatile int i = 0, k = 0, j = 0, y[2];


ISR(TIMER2_COMPA_vect)
{
  set_bit(ADCSRA, ADSC); //requisita leitura do ADC
  while(tst_bit(ADCSRA, ADSC)); //aguarda termino da leitura
  leituraAnalogica = ADC; //captura leitura

  ///! Swap Var. Control
  /*!
    e[] guarda os valores anteriores
  */
  us[1] = us[0];
  e[1] = e[0]; 
  y[1] = y[0];
  y[0] = leituraAnalogica;
  yf[1] = yf[0];
  
  ///! filtro passa-baixas
  yf[0] = 0.0500*y[1] + 0.9500*yf[1];

  ///! P.I. Control
   /*!
    us[0] PWM Signal;
  */
  e[0] = r[j] - yf[0];
  us[0] = us[1] + k1*e[0] + k2*e[1]; 

  ///! Ajusts P.I
  /*!
    antiwindup;
    aw - Sinal excente;
    us[0] - KA inicialmente 0 [0.5 ate 2];
  */
  if (us[0] > 255) usat = 255;
  else if (us[0] < 0) usat = 0;
  else usat = us[0];
  aw = usat - us[0]; 
  us[0] = us[0] + KA*aw; l
    
  if(us[0] > 255) OCR1A = 255;
  if(us[0] < 0) OCR1A  = 0;
  else OCR1A = us[0]; 
  k++; 
}


int main(void)
{
  cli();

  //Serial.begin(115200);
  
  ///! ADC Config
  /*!
    Prescaler 128;
    Vref 5V;
  */
  ADCSRA = 0x00;
  ADCSRB = 0x00;
  ADMUX = 0x00;

  set_bit(ADCSRA, ADEN);
  set_bit(ADCSRA, ADPS0);
  set_bit(ADCSRA, ADPS1);
  set_bit(ADCSRA, ADPS2);
  set_bit(ADMUX, REFS0); 
  
  ///! TIMER2 Config
  /*!
      Prescaler 32;
      COMPA MODE ;
      TOP 249;
  */
  TCCR2A = 0x00;
  TCCR2B = 0x00;
  TIMSK2 = 0x00;
  set_bit(TCCR2B,CS21); 
  set_bit(TCCR2B,CS20);
  set_bit(TCCR2A, WGM21); 
  OCR2A = 249; 
  TCNT2 = 0x00;
  set_bit(TIMSK2, OCIE2A); //Habilita interrupção por comparação

  ///! TIMER1 Config
  /*!
      Prescaler 1;
      Fast PWM Mode;
      TOP 255;
      OCR1A - Ciclo ativo 0C1A
  */
  TCCR1A = 0x00;
  TCCR1B = 0x00;
  set_bit(DDRB, acionaMotor); 
  set_bit(TCCR1A, COM1A1); 
  set_bit(TCCR1A, WGM11);
  set_bit(TCCR1B, WGM12);
  set_bit(TCCR1B, WGM13);
  set_bit(TCCR1B, CS10); 
  ICR1 = 255; 
  OCR1A = 0;

  ///! PI Control
   /*!
      Inicia Varivaeis de controle
      Ganhos do controlador
  */
  for(i=0; i<2; i++) yf[i] = 0;
  for(i=0; i<2; i++) y[i] = 0;
  for(i=0; i<2; i++) e[i] = 0;
  for(i=0; i<2; i++) us[i] = 0;
  k1 = KP + KI*TS*1e-3;
  k2 = -KP;

  sei(); 

  ///! Mudança de SETPOINT
  /*!
   Sinal Degrau aplicado 
  */
    
  while (1) 
  {
    if ((k > 10000) && (j == 0))
    {
      j++; 
    }
    if ((k > 20000) && (j == 1))
    {
      j--; 
      k = 0;
    }
	}
}
