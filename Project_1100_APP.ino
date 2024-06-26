/*  
 
Project 1100/1101
Sketch to measure current. 
Charles Hoile
 
Project Notes **************************************************************************************************
 
  Version 1.0  - 19/06/2024 - Initial Release
 
 
Hardware Pinouts **************************************************************************************************
   =======================Digitals
  D0    - PD0   - n/c (Reserved for Serial Comms)
  D1    - PD1   - n/c (Reserved for Serial Comms)
  D2    - PD2   - n/c
  D3    - PD3   - TM1637 Controller for the 4 digit 7 segment display
  D4    - PD4   - TM1637 Controller for the 4 digit 7 segment display
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
 
double minimumValue = 0;           // set what range the meter starts at 
double maximumValue = 3;          // set what range the meter ends at 
double numberOfLEDs = 53;          // there are 53 LEDs
 
double workingRange = maximumValue - minimumValue;
double threshold = workingRange / numberOfLEDs;
 
// setup thresholds for the LED bar graph. 
double threshold1 = threshold;
double threshold2 = threshold * 2;
double threshold3 = threshold * 3;
double threshold4 = threshold * 4;
double threshold5 = threshold * 5;
double threshold6 = threshold * 6;
double threshold7 = threshold * 7;
double threshold8 = threshold * 8;
double threshold9 = threshold * 9;
double threshold10 = threshold * 10;
double threshold11 = threshold * 11;
double threshold12 = threshold * 12;
double threshold13 = threshold * 13;
double threshold14 = threshold * 14;
double threshold15 = threshold * 15;
double threshold16 = threshold * 16;
double threshold17 = threshold * 17;
double threshold18 = threshold * 18;
double threshold19 = threshold * 19;
double threshold20 = threshold * 20;
double threshold21 = threshold * 21;
double threshold22 = threshold * 22;
double threshold23 = threshold * 23;
double threshold24 = threshold * 24;
double threshold25 = threshold * 25;
double threshold26 = threshold * 26;
double threshold27 = threshold * 27;
double threshold28 = threshold * 28;
double threshold29 = threshold * 29;
double threshold30 = threshold * 30;
double threshold31 = threshold * 31;
double threshold32 = threshold * 32;
double threshold33 = threshold * 33;
double threshold34 = threshold * 34;
double threshold35 = threshold * 35;
double threshold36 = threshold * 36;
double threshold37 = threshold * 37;
double threshold38 = threshold * 38;
double threshold39 = threshold * 39;
double threshold40 = threshold * 40;
double threshold41 = threshold * 41;
double threshold42 = threshold * 42;
double threshold43 = threshold * 43;
double threshold44 = threshold * 44;
double threshold45 = threshold * 45;
double threshold46 = threshold * 46;
double threshold47 = threshold * 47;
double threshold48 = threshold * 48;
double threshold49 = threshold * 49;
double threshold50 = threshold * 50;
double threshold51 = threshold * 51;
double threshold52 = threshold * 52;
double threshold53 = threshold * 53;
// I am **sure** there is a better way to do this, but ChatGPT did this for me, so i don't care so much for the moment. . 
 
 
 
// Include required libraries
#include <Wire.h>
#include <Adafruit_INA260.h>
#include <Arduino.h>
//#include <TM1637TinyDisplay.h>
#include <elapsedMillis.h>
#include <TM1637TinyDisplay6.h>
 
// Set up the timer for the animations. 
elapsedMillis animateTimer = 0;
 
 
// Definitions
Adafruit_INA260 ina260 = Adafruit_INA260();
 
// Setup for TM1637 
#define CLK 7
#define DIO 8
#define TEST_DELAY   1000
 
 
 
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
 
 
TM1637TinyDisplay6 display(CLK, DIO);
 
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
 
 
 
void setup(void) 
{
 
  Serial.begin(115200);
 
 
  display.begin();
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
 
 
 
 
}
 
void loop(void) 
{
 
 
 
  int k;
  uint8_t data[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };          // Test Pattern - All
  uint8_t blank[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };         // Test Pattern - Blank
  display.setBrightness(BRIGHT_HIGH);
 
  //////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Read current from sensor and populate values with it 
 
  float current_mA = 0;
  float current_A = 0;
 
  current_mA = ina260.readCurrent();
  current_A = (abs(current_mA/1000));
  //delay(70);
  //display.showNumber(current_A, 2);
 
 
  Serial.println(current_A);
 
 //////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Decide if LEDs should be on or not.  
 
  if(current_A > threshold1){
  analogWrite(LED01, 250);
  }
  else{
  analogWrite(LED01, 255);
  }
 
 
  if(current_A > threshold2){
  analogWrite(LED02, 250);
  }
  else{
  analogWrite(LED02, 255);
  }
 
 
 if(current_A > threshold3){
  turnOnSegment(2, SEG_A);
  }
  else{
  turnOffSegment(2, SEG_A);
  }
 
 
 if(current_A > threshold4){
  turnOnSegment(2, SEG_B);
  }
  else{
  turnOffSegment(2, SEG_B);
  }
 
 
 if(current_A > threshold5){
  turnOnSegment(2, SEG_C);
  }
  else{
  turnOffSegment(2, SEG_C);
  }
 
 
 if(current_A > threshold6){
  turnOnSegment(2, SEG_D);
  }
  else{
  turnOffSegment(2, SEG_D);
  }
 
 if(current_A > threshold7){
  turnOnSegment(2, SEG_E);
  }
  else{
  turnOffSegment(2, SEG_E);
  }
 
 
 if(current_A > threshold8){
  turnOnSegment(2, SEG_F);
  }
  else{
  turnOffSegment(2, SEG_F);
  }
 
 
 if(current_A > threshold9){
  turnOnSegment(2, SEG_G);
  }
  else{
  turnOffSegment(2, SEG_G);
  }
 
  if(current_A > threshold10){
  turnOnSegment(2, SEG_DP);
  }
  else{
  turnOffSegment(2, SEG_DP);
  }
 
 if(current_A > threshold11){
  turnOnSegment(1, SEG_A);
  }
  else{
  turnOffSegment(1, SEG_A);
  }
 
 
 if(current_A > threshold12){
  turnOnSegment(1, SEG_B);
  }
  else{
  turnOffSegment(1, SEG_B);
  }
 
 
 if(current_A > threshold13){
  turnOnSegment(1, SEG_C);
  }
  else{
  turnOffSegment(1, SEG_C);
  }
 
 
 if(current_A > threshold14){
  turnOnSegment(1, SEG_D);
  }
  else{
  turnOffSegment(1, SEG_D);
  }
 
 if(current_A > threshold15){
  turnOnSegment(1, SEG_E);
  }
  else{
  turnOffSegment(1, SEG_E);
  }
 
 
 if(current_A > threshold16){
  turnOnSegment(1, SEG_F);
  }
  else{
  turnOffSegment(1, SEG_F);
  }
 
 
 if(current_A > threshold17){
  turnOnSegment(1, SEG_G);
  }
  else{
  turnOffSegment(1, SEG_G);
  }
 
  if(current_A > threshold18){
  turnOnSegment(1, SEG_DP);
  }
  else{
  turnOffSegment(1, SEG_DP);
  }
 
 if(current_A > threshold19){
  turnOnSegment(0, SEG_A);
  }
  else{
  turnOffSegment(0, SEG_A);
  }
 
 
 if(current_A > threshold20){
  turnOnSegment(0, SEG_B);
  }
  else{
  turnOffSegment(0, SEG_B);
  }
 
 
 if(current_A > threshold21){
  turnOnSegment(0, SEG_C);
  }
  else{
  turnOffSegment(0, SEG_C);
  }
 
 
 if(current_A > threshold22){
  turnOnSegment(0, SEG_D);
  }
  else{
  turnOffSegment(0, SEG_D);
  }
 
 if(current_A > threshold23){
  turnOnSegment(0, SEG_E);
  }
  else{
  turnOffSegment(0, SEG_E);
  }
 
 
 if(current_A > threshold24){
  turnOnSegment(0, SEG_F);
  }
  else{
  turnOffSegment(0, SEG_F);
  }
 
 
 if(current_A > threshold25){
  turnOnSegment(0, SEG_G);
  }
  else{
  turnOffSegment(0, SEG_G);
  }
 
  if(current_A > threshold26){
  turnOnSegment(0, SEG_DP);
  }
  else{
  turnOffSegment(0, SEG_DP);
  }
 if(current_A > threshold27){
  turnOnSegment(5, SEG_A);
  }
  else{
  turnOffSegment(5, SEG_A);
  }
 
 
 if(current_A > threshold28){
  turnOnSegment(5, SEG_B);
  }
  else{
  turnOffSegment(5, SEG_B);
  }
 
 
 if(current_A > threshold29){
  turnOnSegment(5, SEG_C);
  }
  else{
  turnOffSegment(5, SEG_C);
  }
 
 
 if(current_A > threshold30){
  turnOnSegment(5, SEG_D);
  }
  else{
  turnOffSegment(5, SEG_D);
  }
 
 if(current_A > threshold31){
  turnOnSegment(5, SEG_E);
  }
  else{
  turnOffSegment(5, SEG_E);
  }
 
 
 if(current_A > threshold32){
  turnOnSegment(5, SEG_F);
  }
  else{
  turnOffSegment(5, SEG_F);
  }
 
 
 if(current_A > threshold33){
  turnOnSegment(5, SEG_G);
  }
  else{
  turnOffSegment(5, SEG_G);
  }
 
  if(current_A > threshold34){
  turnOnSegment(5, SEG_DP);
  }
  else{
  turnOffSegment(5, SEG_DP);
  }
 
 if(current_A > threshold35){
  turnOnSegment(4, SEG_A);
  }
  else{
  turnOffSegment(4, SEG_A);
  }
 
 
 if(current_A > threshold36){
  turnOnSegment(4, SEG_B);
  }
  else{
  turnOffSegment(4, SEG_B);
  }
 
 
 if(current_A > threshold37){
  turnOnSegment(4, SEG_C);
  }
  else{
  turnOffSegment(4, SEG_C);
  }
 
 
 if(current_A > threshold38){
  turnOnSegment(4, SEG_D);
  }
  else{
  turnOffSegment(4, SEG_D);
  }
 
 if(current_A > threshold39){
  turnOnSegment(4, SEG_E);
  }
  else{
  turnOffSegment(4, SEG_E);
  }
 
 
 if(current_A > threshold40){
  turnOnSegment(4, SEG_F);
  }
  else{
  turnOffSegment(4, SEG_F);
  }
 
 
 if(current_A > threshold41){
  turnOnSegment(4, SEG_G);
  }
  else{
  turnOffSegment(4, SEG_G);
  }
 
  if(current_A > threshold42){
  turnOnSegment(4, SEG_DP);
  }
  else{
  turnOffSegment(4, SEG_DP);
  }
 
 
 
 if(current_A > threshold43){
  turnOnSegment(3, SEG_A);
  }
  else{
  turnOffSegment(3, SEG_A);
  }
 
 
 if(current_A > threshold44){
  turnOnSegment(3, SEG_B);
  }
  else{
  turnOffSegment(3, SEG_B);
  }
 
 
 if(current_A > threshold45){
  turnOnSegment(3, SEG_C);
  }
  else{
  turnOffSegment(3, SEG_C);
  }
 
 
 if(current_A > threshold46){
  turnOnSegment(3, SEG_D);
  }
  else{
  turnOffSegment(3, SEG_D);
  }
 
 if(current_A > threshold47){
  turnOnSegment(3, SEG_E);
  }
  else{
  turnOffSegment(3, SEG_E);
  }
 
 
 if(current_A > threshold48){
  turnOnSegment(3, SEG_F);
  }
  else{
  turnOffSegment(3, SEG_F);
  }
 
 
 if(current_A > threshold49){
  turnOnSegment(3, SEG_G);
  }
  else{
  turnOffSegment(3, SEG_G);
  }
 
  if(current_A > threshold50){
  turnOnSegment(3, SEG_DP);
  }
  else{
  turnOffSegment(3, SEG_DP);
  }
 
////////////////////////////////////////////////////////////////////
  if(current_A > threshold51){
  analogWrite(LED51, 250);
  }
  else{
  analogWrite(LED51, 255);
  }
////////////////////////////////////////////////////////////////////
  if(current_A > threshold52){
  analogWrite(LED52, 250);
  }
  else{
  analogWrite(LED52, 255);
  }
////////////////////////////////////////////////////////////////////
  if(current_A > threshold53){
  analogWrite(LED53, 250);
  }
  else{
  analogWrite(LED53, 255);
  }
 
}
 
void turnOnSegment(uint8_t digit, uint8_t segment) {
  if (digit < 6) {
    customSegments[digit] |= segment;
  display.setSegments(customSegments);
  }
}
 
void turnOffSegment(uint8_t digit, uint8_t segment) {
  if (digit < 6) {
    customSegments[digit] &= ~segment;
  display.setSegments(customSegments);
  }
}