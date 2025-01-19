/*  
 
Project 1100/1101
Sketch to measure current. 
Charles Hoile.
With some help from Rob Williams.
 
Project Notes **************************************************************************************************
 
  Version 1.0  - 19/06/2024 - Initial Release.
  Version 2.0  - 24/06/2024 - Some improvements after Rob has helped me. It's very smooth now. 
  Version 3.0  - 25/06/2024 - Rob continues to be a godsend.
  Version 4.0  - 25/06/2024 - Added in some sensible debug functions.
  Version 5.0  - 26/06/2024 - Added in modes for displaying Voltage and Power.
  Version 6.0  - 29/06/2024 - Adding in more mature methods of doing the above.
  Version 7.0  - 29/06/2024 - Cleaning up some messy code. 
 
 
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
  D12   - PB4   - SW4 Config Header
  D13   - PB5   - n/c
   =======================Analogues
  A0    - PC0   - SW0 Config Header
  A1    - PC1   - SW1 Config Header
  A2    - PC2   - SW2 Config Header
  A3    - PC3   - SW3 Config Header
  A4    - PC4   - I2C
  A5    - PC5   - I2C
 
SW Config Header Pins Usage **************************************************************************************************

Putting a header on any of these switches will "configure" the meter do display a certain thing. 

None    - Current Mode    (Will display current)
SW0     - Voltage Mode    (Will display Voltage)
SW1     - Power Mode      (Will display Power)
SW2     - Scale           (Will change the bar display from 0-15 max to 0-50 max)
SW3     - Animation       (Will enable animations on the meter)
SW4     - Spare           Spare

 


Meter Range Settings **************************************************************************************************
 
*/
 
// Include required libraries
#include <Wire.h>
#include <Adafruit_INA260.h>
#include <Arduino.h>
#include <TM1637TinyDisplay.h>
#include <elapsedMillis.h>
#include <TM1637TinyDisplay6.h>


//************************************************************************************
// set up the min and max range of the meter

double minimumValue = 0;           // set what range the meter starts at 
double maximumValue = 15;          // set what range the meter ends at 
double numberOfLEDs = 53;          // there are 53 LEDs (it's unlikely this will change)
double workingRange = maximumValue - minimumValue;
double threshold = workingRange / numberOfLEDs;
 
// Set up the timer for the animations. 
elapsedMillis animateTimer = 0;
elapsedMillis refreshDisplay = 0;
elapsedMillis serialDebugRefresh = 0;
elapsedMillis generalTimer = 0;


// Definitions
Adafruit_INA260 ina260 = Adafruit_INA260();
 
// Setup for TM1637 for the Display
#define CLK_DISP 3
#define DIO_DISP 4
 
// Setup for TM1637 for the Bar Graph
#define CLK_BAR 7
#define DIO_BAR 8

 
//************************************************************************************
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
// set up the defines for mode select pins

#define SW0 A0
#define SW1 A1
#define SW2 A2
#define SW3 A3
#define SW4 12

 
//************************************************************************************
// TM1637 driver IC setup for Display and Bar Graph LEDs
 
 
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
 
 
float previous_valueToDisplay = 0;  // Variable to store the previous current reading
bool dir = true;  // Flag to determine the direction of the loop

void setup(void) 
{
 
  //////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Start Serial 
  Serial.begin(115200);
 
  //////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Print Banner on Serial 

  Serial.println(" ");
  Serial.println(" ");
  Serial.println("----- Project 1101  -  S/W Version 7.0 ------------------------------");
  Serial.println("----- 53Meter Combined Current/Voltage/Power Meter ------------------");
  Serial.println(" ");
  Serial.println("System Startup");
  Serial.println(" ");  


  //////////////////////////////////////////////////////////////////////////////////////////////////////////
  // "begin" the "displays" 
 
  displayBAR.begin();
  displayDISP.begin();

  

  //testpattern();
 
   // Wait until serial port is opened
  while (!Serial) 
  { 
    delay(10); 
  }
 
  //////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Get the INA260 sensor up and running. 

  Serial.println("Check for INA260 presence;");
 
  if (!ina260.begin()) 
  {
    Serial.println("Couldn't find INA260 chip");
    while (1);
  }
  Serial.println("Found INA260 chip");
 

  //////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Set up the LEDs. 

 
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


//************************************************************************************
// Set up the inputs for the mode pins

  pinMode(SW0, INPUT_PULLUP);
  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  pinMode(SW3, INPUT_PULLUP);
  pinMode(SW4, INPUT_PULLUP);


//************************************************************************************
// Setup the scale


  if(digitalRead(SW2)==LOW)         // so if this is LOW, then we want to modify the scale 
  {
    Serial.println("Extended Scale Selected (0-50)");
    minimumValue = 0;           
    maximumValue = 50;
    workingRange = maximumValue - minimumValue;
    threshold = workingRange / numberOfLEDs;
    // all this stuff is already defined above in lines 70ish, but easier to re-define here. 
  }
  else
  {
    Serial.println("Normal Scale Selected (0-15)");
  }

//************************************************************************************
// Last thing we do before we get into void loop()
  Serial.println("Startup Complete");
  Serial.println(" ");
  Serial.println(" ");
  Serial.println(" ");

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
  float valueToDisplay = 0;


  //////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Decide how to setup and show the meter
  
  if(digitalRead(SW0)==LOW)
  {
    // If the IF is true, then we display Voltage on the meter
    float voltage_mA;                           // define the float
    voltage_mA = ina260.readBusVoltage();       // read mV from the INA
    voltage_mA = abs(voltage_mA);               // Absolute it
    valueToDisplay = voltage_mA/1000;           // make it V not mV
  }

  else if(digitalRead(SW1)==LOW)
  {
    // If the IF is true, then we display Power on the meter
    float power_mW;                             // define the float
    power_mW = ina260.readPower();              // read mW from the INA
    power_mW = abs(power_mW);                   // Absolute it
    valueToDisplay = power_mW/1000;             // make it W not mW
  }

  else
  {
    // Feeling like just viewing current today?
    float current_mA;                           // define the float
    current_mA = ina260.readCurrent();          // read mA from the INA
    current_mA = abs(current_mA);               // Absolute it
    valueToDisplay = current_mA/1000;           // make it A not mA
  }
  

  //////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Refresh rate of the Display (Set to 100ms)

  if (refreshDisplay >= 100)
  {
    refreshDisplay = 0;
    displayDISP.showNumber(valueToDisplay);
  }


 //////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Do some serial bits.  
    
    
if (serialDebugRefresh >= 250)        // So that is to say, is it time to print details on serial debug?
  {
    serialDebugRefresh = 0;           // reset the timer

    //Serial.print("Serial Output");
    //Serial.print(" -- ");

    Serial.print("Timestamp ");
    Serial.print(1000000 + (generalTimer/100));
    Serial.print(" -- ");
    
    Serial.print("Current: ");
    Serial.print((abs(ina260.readCurrent()/1000)),3);
    Serial.print("A");
    Serial.print(" -- ");

    Serial.print("Voltage: ");
    Serial.print((ina260.readBusVoltage()/1000),3);
    Serial.print("V");
    Serial.print(" -- ");

    Serial.print("Power: ");
    Serial.print(ina260.readPower()/1000);
    Serial.print("W");
    

    Serial.println("");

  }


  // Decide the direction of the loop based on current reading
  dir = (valueToDisplay >= previous_valueToDisplay);
  previous_valueToDisplay = valueToDisplay;  // Update the previous current reading
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
      analogWrite(LED01, (valueToDisplay > ledThreshold)? 250 : 255);
    }
    else if (f == 2) {
      analogWrite(LED02, (valueToDisplay > ledThreshold)? 250 : 255);
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
      
      if (valueToDisplay > ledThreshold) {
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
      analogWrite(LED52, (valueToDisplay > ledThreshold)? 250 : 255);
    }
    else if (f == 52) {
      analogWrite(LED51, (valueToDisplay > ledThreshold)? 250 : 255);
    }
    else if (f == 53) {
      analogWrite(LED53, (valueToDisplay > ledThreshold)? 250 : 255);
    }
  }

}
