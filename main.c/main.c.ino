//! Archive Description.
/*!
* \author Matheus Barbi. M.
  \author Rodri Jost. 
* \since 31/07/2022
* \version 1.0
*/

#define tst_bit(Y,bit_x)  (Y&(1<<bit_x)) 
#define set_bit(Y, bit_x) (Y|=(1<<bit_x))
#define clr_bit(Y, bit_x) (Y&=~(1<<bit_x))
#define cpl_bit(Y, bit_x) (Y^=(1<<bit_x))

#define acionaMotor 1
#define KP 1.5 
#define KI 50.00
#define KA 0.01 //coeficiente de anti wind up ajustado para a montagem
#define TS 128 //us

unsigned int leituraAnalogica = 0;
int r[] = {350, 500}; //Operar na região linear
float u = 0, us[2], e[2], yf[2], usat = 0, k1 = 0, k2 = 0, aw = 0; //usat será, de forma importante, 0 ou 255
int i = 0, j = 0, k = 0, y[2];

ISR(TIMER2_OVF_vect)
{
  set_bit(ADCSRA, ADSC); 
  while(tst_bit(ADCSRA, ADSC));
  leituraAnalogica = ADC;

  /* Sinal Degrau
  k++;
   if(k >= 2000){ //talvez aumentar tempo - original 2000
    k = 0;
    j++; // j índice do vetor do sinal degrau
    if(j >= 2) j = 0;
  }*/

  // swap de variáveis
  us[1] = us[0];
  e[1] = e[0]; // Erro Anterior
  y[1] = y[0];
  y[0] = leituraAnalogica;
  yf[1] = yf[0];
  
  // filtro passa-baixas:
  yf[0] = 0.0500*y[1] + 0.9500*yf[1];

  // controle PI:
  e[0] = r[j] - yf[0];
  us[0] = us[1] + k1*e[0] + k2*e[1]; //calculo do sinal atual de pwm

  if (us[0] > 255) usat = 255;
  else if (us[0] < 0) usat = 0;
  else usat = us[0];

  aw = usat - us[0];
  us[0] = us[0] + KA*aw; 
    
  OCR1A = us[0];
  if(OCR1A > 255) OCR1A = 255;
  if(OCR1A < 0) OCR1A  = 0;
  
}

void setup() 
{

  cli();

  //Serial.begin(115200);
  
  /*!
    ADC Config, 128.
  */
  ADCSRA = 0b10000111;
  ADCSRB = 0b00000000;
  ADMUX = 0b01000000;

  /*!
    T2 OVF MODE, .8
  */
  TCCR2A = 0x00;
  TCCR2B = 0x00;
  TCCR2B |= (1<<CS21); // Configurar PreScaler

  TCNT2 = 0x00;

  set_bit(TIMSK2, TOIE2);

  set_bit(DDRB, acionaMotor);
  //fast PWM, TOP = ICR1, OC1A e OC1B habilitados
  TCCR1A = 0b10100010; //PWM não invertido nos pinos OC1A e OC1B
  TCCR1B = 0b00011001; //liga TC1, prescaler = 1
  ICR1 = 255; //valor máximo para contagem
  OCR1A = 0; //controle do ciclo ativo do PWM 0C1A

  for(i=0; i<2; i++) yf[i] = 0;
  for(i=0; i<2; i++) y[i] = 0;
  for(i=0; i<2; i++) e[i] = 0;
  for(i=0; i<2; i++) us[i] = 0;
    
  // ganhos do controlador:
  k1 = KP + KI*TS*0.000001;
  k2 = -KP;

  sei();

}

void loop() 
{
  
}