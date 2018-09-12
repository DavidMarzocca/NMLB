/*  ------------------------
 *    No Model Left Behind 
 *  ------------------------
 *  
 *  A lost model alarm for when everything else fails
 *  
 *  
 *  Code by David Marzocca
 *  
 *  
 *  
*/

// Load library to get the VCC voltage and for the sleep modes
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

// Routines to set and claer bits (used in the sleep code)
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit)) // sets a bit to zero
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit)) // sets a bit to one
#endif



// *** Buzzer mode: ***
//		1 > digital from flight controller; 
//		2 > PWM signal from a receiver
// Default is 1

const int bz_mode = 1;


// *********** Define I/O Pins **********************
const int STin = 2;     // Input from +5V Status
const int BZin = 3;     // Bz- Input from flight controller

const int BZout = 0;    // Buzzer Output
const int CEout = 1;    // Output to Charge-Enable pin of TP4056
const int LEDout = 4;   // LED Output

// *********** Define some flags **********************
boolean board_status = LOW;
boolean start = 0;
boolean arm_delay = 0;
boolean on_lowSig = 0;
boolean on_highSig = 0;
boolean on_lostSig = 0;

volatile boolean f_wdt = 1; // watchdog timeout flag

// *********** Define time-keeping variables **********************
unsigned long wait_start_time = 0;
unsigned long current_time = 0;
unsigned long board_on_start_time = 0;
unsigned long stdby_start_time = 0;
unsigned long last_blink_time;
const unsigned long time_delay = 180000; // delay to wait (in ms)


// *********** SETUP **********************
void setup()
{
  
  set_pins();

  // Disable charging the battery
  // (need to connect CEout pin on the attiny
  // to the CE pin, #8, on the TP4056)
  digitalWrite(CEout, LOW);

  uint16_t Vcc;

  // Read Vcc a few times.
  // This is to get a stable reading from the ADC
  Vcc = readVcc();
  delay(50);
  Vcc = readVcc();
  delay(50);
  Vcc = readVcc();
  
  // switch Analog to Digitalconverter OFF
  // -- saves power.
  cbi(ADCSRA,ADEN);
  
  double myVcc = (double)Vcc/1000;
  
  // If the battery voltage starts to be low, give a long beep
  // and start charging the battery
  if(myVcc < 4.0){
    long_beep();
    digitalWrite(CEout, HIGH);
  }
  
  // another beep if Vcc < 3.8V
  if(myVcc < 3.8){
    delay(300);
    long_beep();
  }

  // another one if Vcc < 3.5V
  if(myVcc < 3.5){
    delay(300);
    long_beep();
  }

  delay(200);
  
  // Beep and blink twice to set the start
  digitalWrite(BZout, HIGH);
  digitalWrite(LEDout, HIGH);
  delay(50);
  digitalWrite(BZout, LOW);
  digitalWrite(LEDout, LOW);
  delay(100);
  digitalWrite(BZout, HIGH);
  digitalWrite(LEDout, HIGH);
  delay(50);
  digitalWrite(BZout, LOW);
  digitalWrite(LEDout, LOW);
  delay(50);

  
}


// *********** MAIN LOOP **********************
void loop()
{

  // read the status of the board,
  // is there a +5V or not?
  board_status = digitalRead(STin);

  // update the time
  current_time = millis();

  // if it sees a +5V on the signal pin
  if(board_status == HIGH) {

    // If it is the first time in this state
    if( on_highSig ==  0){
      on_highSig = 1;

      // reser the timer for when the signal turned on
      board_on_start_time = current_time;

      // reset blink timer
      last_blink_time = current_time;

      // reset the other flags
      on_lowSig = 0;
      on_lostSig = 0;
      arm_delay = 0;

      // Beep and blink "-- - --" to give a confirmation of the start
      confirm_start();

      // if the voltage dropped below 3.9V start charging
      double myVcc = get_voltage_V();
      if(myVcc < 4.0){
        digitalWrite(CEout, HIGH);
      }
      else{
        digitalWrite(CEout, LOW);
      }
    }

    // If it was turned on for more than 5 seconds then arm the delay
    if(arm_delay == 0 && current_time - board_on_start_time > 5000){
      arm_delay = 1;

      // Beep and blink long "---" to give a confirmation of the arming
      digitalWrite(BZout, HIGH);
      digitalWrite(LEDout, HIGH);
      delay(500);
      digitalWrite(BZout, LOW);
      digitalWrite(LEDout, LOW);
    }
  
    // to show that it is working
    // blink the LED twice every 3 seconds
    if(current_time - last_blink_time >= 3000){
      
      blink_ntimes(2);

      last_blink_time = current_time;
    }

    // Play the buzzer if a bz signal is sent from betaflight.
    if(bz_mode == 1){
	    if(digitalRead(BZin) == HIGH){
    	  digitalWrite(BZout, LOW);
	    }
    	if(digitalRead(BZin) == LOW){
	      digitalWrite(BZout, HIGH);
    	}
    }
    
    // Play the buzzer if a bz PWM signal is received
    if(bz_mode == 2){
      if (pulseIn(BZin, HIGH) > 1800){
        digitalWrite(BZout, HIGH);
      }
      else{
        digitalWrite(BZout, LOW);
      }
    }
    
  }
  // End case: turned on with signal

  
  // if the delay was not armed
  // and still there is no power
  // then blink twice the LED.
  // If it was in this state for more than 1minute,
  // go in power-saving mode
  if(board_status == LOW && arm_delay == 0){

    // If it is the first time in this state
    if(on_lowSig == 0){
      on_lowSig = 1;
	    
      digitalWrite(BZout, LOW); // in case power went down while the buzzer was playing

      stdby_start_time = current_time;

      // blink once and reset timer for next blink
      blink_ntimes(1);  
      last_blink_time = current_time;

      // reset the other flags
      on_highSig = 0;
      on_lostSig = 0;
    }

    
    // to show that it is working
    // blink the LED once every 3 seconds
    if(current_time - stdby_start_time < 60000 && current_time - last_blink_time >= 3000){
      
      blink_ntimes(1);
      last_blink_time = current_time;
    } 

    // if it was in standby for more than one minute, go to sleep mode
    // -- code still to write --       Remove the check on the last_blink_time.
    if(current_time - stdby_start_time >= 60000 && current_time - last_blink_time >= 3000){

      if (f_wdt==1) {  // wait for timed out watchdog / flag is set when a watchdog timeout occurs
        setup_watchdog(8); // approximately 4 seconds sleep
        f_wdt=0;
        system_sleep();
      }
    }
  }
  // End case: turned on without signal or no signal and delay not armed
  
  
  // if the delay is armed (model was on for more than 5s)
  // but the present signal is zero then
  // activate the buzzer after a delay
  if(board_status == LOW && arm_delay == 1){

    // if it is the first time in this state
    if(on_lostSig == 0){
      on_lostSig = 1;
      
      digitalWrite(BZout, LOW); // in case power went down while the buzzer was playing
	    
      // reset the clock
      wait_start_time = current_time;

      // reset the other flags
      on_lowSig = 0;
      on_highSig = 0;

      // blink 4 times and reset timer for next blink
      blink_ntimes(4);  
      last_blink_time = current_time;
      
    } 

    // if the desired amount of time is not passed,
    // just blink 4 times every 3 seconds
    if( current_time - wait_start_time < time_delay && current_time - last_blink_time >= 3000){
     
      // blink 4 times
      blink_ntimes(4);
      last_blink_time = current_time;
    }
    
    // if the desired amount of time is passed, do not wait further
    if( current_time - wait_start_time >= time_delay && current_time - last_blink_time >= 5000){
      
      // play the SOS every 5 seconds
      // hopefully you will find the model before the battery runs out!
      sos();
      last_blink_time = current_time;

      // go briefly to sleep to save battery
      if (f_wdt==1) {  // wait for timed out watchdog / flag is set when a watchdog timeout occurs
        setup_watchdog(6); // approximately 1 second sleep
        f_wdt=0;
        system_sleep();
      }
    }
     
  }
  // End case: lost signal


} 

//*****************************************************
//*****************************************************
//******************  Other functions  ****************


void set_pins() {
  pinMode(STin, INPUT);
  pinMode(BZin, INPUT);
  
  pinMode(BZout, OUTPUT);
  pinMode(LEDout, OUTPUT);
  pinMode(CEout, OUTPUT);
}


// This is used to further reduce power consumption during sleep mode
void set_pins_as_input() {
  pinMode(STin, INPUT);
  pinMode(BZin, INPUT);
  
  pinMode(CEout, INPUT);
  pinMode(BZout, INPUT);
  pinMode(LEDout, INPUT);
}


// This function reads the input voltage
// of the ATtiny85 and gives it as output in mV
uint16_t readVcc(void) 
{
  uint16_t result;
  
  // Read 1.1V reference against Vcc
  ADMUX = _BV(MUX3) | _BV(MUX2);
  // i.e. turns on MUX3 and MUX2 bits in ADMUX:
  // (MUX3,MUX2,MUX1,MUX0) = (1,1,0,0)
  // means it reads VBG (bandgap voltage)
  delay(50); // Wait for Vref to settle

  // Start the conversion
  ADCSRA |= _BV(ADSC);
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCW;
  return 1126400L / result; // Back-calculate Vcc in mV.
  // note: 1126400 = 1.1*1024*1000
}


double get_voltage_V(){
  
  uint16_t Vcc;

  // switch Analog to Digitalconverter ON
  // -- saves power.
  sbi(ADCSRA,ADEN);
  
  // Read Vcc a few times.
  // This is to get a stable reading from the ADC
  Vcc = readVcc();
  delay(50);
  Vcc = readVcc();
  delay(50);
  Vcc = readVcc();
  
  // switch Analog to Digitalconverter OFF
  // -- saves power.
  cbi(ADCSRA,ADEN);

  return (double)Vcc/1000;
}


// This function plays an SOS on both beeper and LED
// SOS:  ". . . - - - . . ."
void sos() {
  // If doesn't see external power,
    // after a delay plays SOS
    // and show it on the LED
    //
    for(int ii = 0; ii < 3; ii++){
      digitalWrite(BZout, HIGH);
      digitalWrite(LEDout, HIGH);
      delay(100);
      digitalWrite(BZout, LOW);
      digitalWrite(LEDout, LOW);
      delay(100);
    }
    delay(200);
    for(int ii = 0; ii < 3; ii++){
      digitalWrite(BZout, HIGH);
      digitalWrite(LEDout, HIGH);
      delay(400);
      digitalWrite(BZout, LOW);
      digitalWrite(LEDout, LOW);
      delay(100);
    }
    delay(200);
    for(int ii = 0; ii < 3; ii++){
      digitalWrite(BZout, HIGH);
      digitalWrite(LEDout, HIGH);
      delay(100);
      digitalWrite(BZout, LOW);
      digitalWrite(LEDout, LOW);
      delay(100);
    }
}

// This function blinks the LED n times
void blink_ntimes(int n)
{
  for(int ii = 0; ii < n; ii++)
  {
    digitalWrite(LEDout, HIGH);
    delay(100);
    digitalWrite(LEDout, LOW);
    delay(200);
  }
}

// Beep and blink "-- - --"
void confirm_start() {
      digitalWrite(BZout, HIGH);
      digitalWrite(LEDout, HIGH);
      delay(500);
      digitalWrite(BZout, LOW);
      digitalWrite(LEDout, LOW);
      delay(200);
      digitalWrite(BZout, HIGH);
      digitalWrite(LEDout, HIGH);
      delay(200);
      digitalWrite(BZout, LOW);
      digitalWrite(LEDout, LOW);
      delay(200);
      digitalWrite(BZout, HIGH);
      digitalWrite(LEDout, HIGH);
      delay(500);
      digitalWrite(BZout, LOW);
      digitalWrite(LEDout, LOW);
}

// Plays and shows a long beep
void long_beep() {
  digitalWrite(BZout, HIGH);
  digitalWrite(LEDout, HIGH);
  delay(800);
  digitalWrite(BZout, LOW);
  digitalWrite(LEDout, LOW);
}

// set system into the sleep state 
// system wakes up when wtchdog is timed out
void system_sleep() {
  cbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter OFF
  set_pins_as_input();                 // Set all pins as input
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();

  sleep_mode();                        // System sleeps here

  sleep_disable();                     // System continues execution here when watchdog timed out 
  sbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter ON
  set_pins();                          // Set all pins as they should be
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
ISR(WDT_vect) {
  f_wdt=1;  // set global flag
}

