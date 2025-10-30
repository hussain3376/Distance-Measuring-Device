#define F_CPU 1000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd (A0, A1, A2, A3, A4,A5);  // initialise LCD pins

//declare timers
  
void T10msDelay();  // for 10ms delay
void T1sDelay();  // for 1s delay

// defines pins numbers
const int trigPin = PINB0;  // connect to arduino pin 8
const int echoPin = PIND7;  // connect to arduino pin 7
const int buzzer = PINB2;   // connect to arduino pin 10
const int ledPin = PINB3;  // connect to arduino pin 11

// defines variables
long duration;
int distance;
int closeDistance;
int flag = 1;

void setup() {
  DDRB |= 0xFF;  
  DDRD &= 0B01111111; 
  Serial.begin(9600);  // serial begin at 9600 baud rate
   lcd.begin(16,2);   // lcd begin

      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Distance Measure");
      lcd.setCursor(0,1);
      lcd.print("     Device     ");

  //interrupt configuration
  //DDRD &= ~(1 << 2);
  PORTD |= (1 << 2) | (1 << 3); 
  EICRA = (0x00);
  EIMSK |= (1 << INT0) | (1 << INT1); 
  sei(); 
}

void loop()
{

  PORTB &= 0B1111110; //pin0 low trig pin
  T10msDelay(); 
  PORTB |= 0B00000001; //pin0 high trig pin
  T10msDelay(); 
  PORTB &= 0B1111110; //pin0 low trig pin

  duration = pulseIn(echoPin, HIGH);
  distance = ((duration * 0.034 / 2) + 1);  // measuring distance in cm

  closeDistance = distance;
  
  if (flag == 1) {
    if (closeDistance <= 5)   // When distance is less than or equals to 5
    {
      PORTB |= 0B00001100;  // LED and Buzzer turns On
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Distance Measure"); // Print Distance Measure on lcd 
      lcd.setCursor(4,1);
      lcd.print("cm");
      lcd.setCursor(8,1);
      lcd.print("LOW");
       T1sDelay();  // 1s delay
     
    }
    else
    {
      PORTB &= ~(0B00001100); 
     
    }
    // Prints the distance on the Serial Monitor
    Serial.print("Distance: ");
    Serial.println(distance);
        lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Distance Measure");
      lcd.setCursor(4,1);
      lcd.print("cm");
      lcd.setCursor(0,1);
      lcd.print(distance);
      T1sDelay();
  } //if1 end
  else {
    PORTB &= ~(0B00001100); 
  }
  return 0;
} //loop end

//timers configuration + steps

void T10msDelay()
{
// 1/8 MHz = 0.125 us
// 2- 256 x 0.125 = 32 µs
//  (10000ms/32) = 312.5
//  1 + FFFF – 312.5 = 0xFEC8

  TCNT1H = 0xFE;
  TCNT1L = 0xC8;
  TCCR1A = 0x00; //normal mode
  TCCR1B = 0x04; //normal mode and prescaler 256
  while ((TIFR1 & (0x1 << TOV1)) == 0);
  TCCR1B = 0x00;
  TIFR1 = 0x1 << TOV1;
}

void T1sDelay()
{
  //  (1/8MHz) = 0.125 µs
  // 256 x 0.125 = 32 µs
  // (1000000ms/32) = 31250
  // 1 + FFFF – 31250 = 0x85EE

  TCNT1H = 0x85;
  TCNT1L = 0xEE;
  TCCR1A = 0x00; // Normal mode
  TCCR1B = 0x04; // Normal mode and prescaler 256
  while ((TIFR1 & (0x1 << TOV1)) == 0);
  TCCR1B = 0x00;
  TIFR1 = 0x1 << TOV1;
}
//off button
ISR(INT0_vect)
{
  flag = 0;
}
//on button
ISR(INT1_vect)
{
  flag = 1;
}
