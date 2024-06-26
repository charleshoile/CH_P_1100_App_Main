/*  
 
Project 1100/1101
Sketch to measure current. 
Charles Hoile
With some help from Rob Williams
 
Project Notes **************************************************************************************************
 
  Version 1.0  - 19/06/2024 - Initial Release
  Version 2.0  - 24/06/2024 - Some improvements after Rob has helped me. It's very smooth now. 
  Version 3.0  - 25/06/2024 - Rob continues to be a godsend
 
 
Hardware Pinouts **************************************************************************************************
   =======================Digitals
  D0    - PD0   - n/c (Reserved for Serial Comms)
  D1    - PD1   - n/c (Reserved for Serial Comms)
  D2    - PD2   - n/c
  D3    - PD3   - TM1637 Controller for the 4 digit 7 segment displayBAR
  D4    - PD4   - TM1637 Controller for the 4 digit 7 segment displayBAR
  D5    - PD5   - LED 02 on the bar graph
  D6    - PD6   - LED 01 on the bar graph
  D7    - PD7   - TM1637 Controller for the BAR GRAPH
  D8    - PB0   - TM1637 Controller for the BAR GRAPH
  D9    - PB1   - LED 51 on the bar graph
  D10   - PB2   - LED 52 on the bar graph
  D11   - PB3   - LED 53 on the bar graph
  D12   - PB4   - n/c
  D13   - PB5   - n/c
   =======================Analogues
  A0    - PC0   - SW1 Config Header
  A1    - PC1   - SW2 Config Header
  A2    - PC2   - SW3 Config Header
  A3    - PC3   - SW4 Config Header
  A4    - PC4   - I2C
  A5    - PC5   - I2C
 
 
Meter Range Settings **************************************************************************************************
 
*/
 
// Include required libraries
#include <Wire.h>
#include <Adafruit_INA260.h>
#include <Arduino.h>
#include <TM1637TinyDisplay.h>
#include <elapsedMillis.h>
#include <TM1637TinyDisplay6.h>

double minimumValue = 0;           // set what range the meter starts at 
double maximumValue = 4;          // set what range the meter ends at 
double numberOfLEDs = 53;          // there are 53 LEDs
 
double workingRange = maximumValue - minimumValue;
double threshold = workingRange / numberOfLEDs;
 
// Set up the timer for the animations. 
elapsedMillis animateTimer = 0;
 
elapsedMillis refreshDisplay = 0;


// Definitions
Adafruit_INA260 ina260 = Adafruit_INA260();
 
// Setup for TM1637 
#define CLK_DISP 3
#define DIO_DISP 4
#define TEST_DELAY   1000
 

#define CLK_BAR 7
#define DIO_BAR 8

 
 
// LED Definitions 
#define LED1  0
#define LED2  1
#define LED3  2
#define LED4  5
#define LED5  6
#define LED6  7
#define LED7  8
#define LED8  9
#define LED9  10
#define LED10 12
#define LED11 13
#define LED12 11
#define LED13 A0
#define LED14 A1
#define LED15 A2
#define LED16 A3
 
 
int analogPin = A0;                     
double val = 0;  
double valcalc = 0;
double actualvalcalc = 0;
 
 
 
//************************************************************************************
// set up the defines for the PWM pins
#define LED01 6
#define LED02 5
#define LED51 9
#define LED52 10
#define LED53 11
 
//************************************************************************************
// TM1637 driver IC setup for BAR
 
 
TM1637TinyDisplay6 displayBAR(CLK_BAR, DIO_BAR);

TM1637TinyDisplay displayDISP(CLK_DISP, DIO_DISP); // 4 digit

 
// Segment bits
#define SEG_A 0b00000001
#define SEG_B 0b00000010
#define SEG_C 0b00000100
#define SEG_D 0b00001000
#define SEG_E 0b00010000
#define SEG_F 0b00100000
#define SEG_G 0b01000000
#define SEG_DP 0b10000000
 
// Custom segment data for 6 digits
uint8_t customSegments[6] = {0, 0, 0, 0, 0, 0};
 
 
float previous_current_A = 0;  // Variable to store the previous current reading
bool dir = true;  // Flag to determine the direction of the loop

void setup(void) 
{
 
  Serial.begin(115200);
 
 
  displayBAR.begin();


  displayDISP.begin();

  

  //testpattern();
 
   // Wait until serial port is opened
  while (!Serial) { delay(10); }
 
  Serial.println("Adafruit INA260 Test");
 
  if (!ina260.begin()) {
    Serial.println("Couldn't find INA260 chip");
    while (1);
  }
  Serial.println("Found INA260 chip");
 
 
  pinMode(LED01, OUTPUT);
digitalWrite(LED01, HIGH);
  pinMode(LED02, OUTPUT);
digitalWrite(LED02, HIGH);
  pinMode(LED51, OUTPUT);
digitalWrite(LED51, HIGH);
  pinMode(LED52, OUTPUT);
digitalWrite(LED52, HIGH);
  pinMode(LED53, OUTPUT);
digitalWrite(LED53, HIGH);

Serial.println("And into the loop");

}

void loop(void) 
{
  int k;
  uint8_t data[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };          // Test Pattern - All
  uint8_t blank[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };         // Test Pattern - Blank
  displayBAR.setBrightness(BRIGHT_HIGH);
 
  //////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Read current from sensor and populate values with it 
  float current_mA = 0;
  float current_A = 0;
 
  current_mA = ina260.readCurrent();
  current_A = (abs(current_mA/1000));
  //current_A =12.34;
  




  if (refreshDisplay >= 100)
  {

refreshDisplay = 0;
  displayDISP.showNumber(current_A);

  }

 //////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Do some serial bits.  
  
  Serial.println(current_A);




  // Decide the direction of the loop based on current reading
  dir = (current_A >= previous_current_A);
  previous_current_A = current_A;  // Update the previous current reading
//dir=1; //force loop up

 //////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Decide if LEDs should be on or not.  
  
  // assume all the TM1637 will be OFF so we can turn off if LED 1 or 2 are OFF
  memset(customSegments, 0, sizeof(customSegments));

  for(int f = dir? 1: numberOfLEDs;
      dir? (f <= numberOfLEDs) : (f >= 1);
      dir? f++ : f--) {
    double ledThreshold = threshold * f;

    // first two LEDs are DIO not TM1637
    if (f == 1) {
      analogWrite(LED01, (current_A > ledThreshold)? 250 : 255);
    }
    else if (f == 2) {
      analogWrite(LED02, (current_A > ledThreshold)? 250 : 255);
    }
    else if (f < 51) {
      uint8_t digit;
      if (f < 11) {
        digit = 2;
      }
      else if (f < 19) {
        digit = 1;
      }
      else if (f < 27) {
        digit = 0;
      }
      else if (f < 35) {
        digit = 5;
      }
      else if (f < 43) {
        digit = 4;
      }
      else {
        digit = 3;
        
      }
      uint8_t bit_num = f - 3; // LED 3 has bit number 0, LED 50 has bit number 47
      uint8_t segment = 1 << (bit_num %8); // segment bits are in order 0..7 for all bit numbers. Well planned.
      
      if (current_A > ledThreshold) {
        customSegments[digit] |= segment;
      }
      if (((f == 50) && dir) 
        ||((f == 3) && !dir)) {
        // going up: write the TM1637 LEDs before the last 3
        // going down: write the TM1637 LEDs before the first 2
        displayBAR.setSegments(customSegments);
      }
    }
    else if (f == 51) {
      analogWrite(LED51, (current_A > ledThreshold)? 250 : 255);
    }
    else if (f == 52) {
      analogWrite(LED52, (current_A > ledThreshold)? 250 : 255);
    }
    else if (f == 53) {
      analogWrite(LED53, (current_A > ledThreshold)? 250 : 255);
    }
  }

}
