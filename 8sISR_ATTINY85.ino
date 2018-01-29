/*
 ATtiny85 power circuitry
 
 Overview:
 * This code wakes up ATTINY85 IC every 8s. 
 * Every 5.2 minute it will set pin5 to high for 8s. 
 * Rest of the time IC is put to SLEEP_MODE_PWR_DOWN only to be woken up with watchdog interrupt.
 
 This is to reduce the energy consumed so it can be attached to a battery.
 This code is designed to run on the ATTiny 25/45/85.
 
 * Modified on 5 Feb 2011 by terrorgithb
 * Base version
 
 This example code is in the public domain.
 */
 
#define F_CPU 8000000  // This is used by delay.h library
 
#include <stdlib.h>
 
#include <avr/io.h>        // Adds useful constants
#include <util/delay.h>    // Adds delay_ms and delay_us functions
  
#include <avr/sleep.h>
#include <avr/interrupt.h>
 
// *********** Define I/O Pins **********************
#define pin5 0        // MOSFET Driver output
#define pin6 1
#define pin7 2        
#define pin3 4        
#define pin1 5        
#define pin2 3
 
//************ USER PARAMETERS*********************** 
char f_wdt = 0;
char timeout = 0;

void setup()  { 
  
  ADCSRA &= ~(1<<ADEN); //Disable ADC, saves ~230uA
  ACSR = (1<<ACD);      //Disable the analog comparator
  DIDR0 = 0x3F;         //Disable digital input buffers on all ADC0-ADC5 pins.
   
  // Set up IO pins
  pinMode(pin1, INPUT);
  pinMode(pin2, INPUT);
  pinMode(pin3, INPUT);
  pinMode(pin5, OUTPUT);   
  pinMode(pin6, INPUT);
  pinMode(pin7, INPUT);    

  delay(100);  // Wait a while for this to stabilise.
  
  timeout = 9; //8sec
  setup_watchdog(timeout); //Setup watchdog to go off after 8sec   
  
  digitalWrite(pin5, HIGH); // FET ON       
  
  for (;;) 
  {        
    system_sleep();  // Send the unit to sleep
  }
} 
 
// the loop routine runs over and over again forever:
void loop()  
{  
}

// set system into the sleep state 
// system wakes up when wtchdog is timed out
void system_sleep() 
{
  //setup_watchdog(timeout); //Setup watchdog to go off after 1sec 

  //ADCSRA &= ~(1<<ADEN); //Disable ADC, saves ~230uA
  //ACSR = (1<<ACD); //Disable the analog comparator
  //DIDR0 = 0x3F;    //Disable digital input buffers on all ADC0-ADC5 pins.

  /*pinMode(pin1, INPUT);
  pinMode(pin2, INPUT);
  pinMode(pin3, INPUT);
  pinMode(pin6, INPUT);
  pinMode(pin7, INPUT);*/
 
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  /*cli();
  sleep_enable();
  sei();
  sleep_cpu();
  sleep_disable();
  sei();*/
  sleep_enable();
  sleep_mode();                        // System actually sleeps here
}
 
// 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms
// 6=1 sec,7=2 sec, 8=4 sec, 9= 8sec
void setup_watchdog(int ii) {
 
  byte bb;
  int ww;
  if (ii > 9 ) ii=9;
  bb=ii & 7;
  if (ii > 7) bb|= (1<<5);
  bb|= (1<<WDCE);
  ww=bb;
 
  MCUSR &= ~(1<<WDRF);
  // start timed sequence
  WDTCR |= (1<<WDCE) | (1<<WDE);
  // set new watchdog timeout value
  WDTCR = bb;
  WDTCR |= _BV(WDIE);
}
  
// Watchdog Interrupt Service / is executed when watchdog timed out
ISR(WDT_vect) 
{
  f_wdt++;  // set global flag666

  if(f_wdt == 1)
  {
    digitalWrite(pin5, LOW);  // FET OFF 
    pinMode(pin5, INPUT);      
  }  
  if(f_wdt == 40)
  {    
    f_wdt = 0;
    pinMode(pin5, OUTPUT);    
    digitalWrite(pin5, HIGH); // FET ON 
  }
}
