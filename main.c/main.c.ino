
#define tst_bit(Y,bit_x)  (Y&(1<<bit_x)) 
#define set_bit(Y, bit_x) (Y|=(1<<bit_x))
#define clr_bit(Y, bit_x) (Y&=~(1<<bit_x))
#define cpl_bit(Y, bit_x) (Y^=(1<<bit_x))

unsigned int value = 0;

void setup() 
{

  cli();

  Serial.begin(9600);
  
  ADCSRA = 0b10010111;
  ADMUX = 0b01000000;

  sei();
}

void loop() 
{
  set_bit(ADCSRA, ADSC);
  while(!(clr_bit(ADCSRA, ADIF)));
  set_bit(ADCSRA, ADIF);

  value = (ADCH << 8 | ADCL);
  Serial.println(value);
}
