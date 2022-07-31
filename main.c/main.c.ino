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

unsigned int value = 0;

ISR(TIMER2_OVF_vect)
{
  set_bit(ADCSRA, ADSC);
  value = (ADCH << 8 | ADCL);
}

void setup() 
{

  cli();

  Serial.begin(9600);
  
  /*!
    ADC Config, 128.
  */
  ADCSRA = 0b10000111;
  ADMUX = 0b01000000;

  /*!
    T2 OVF MODE, .
  */
  TCCR2A = 0x00;
  TCCR2B = 0x00;
  TCCR2B |= (1<<CS21)|(1 << CS20); // Configurar PreScaler

  TCNT0 = 0x00;

  set_bit(TIMSK2, TOIE2);

  sei();
}

void loop() 
{
  Serial.println(value);
}
