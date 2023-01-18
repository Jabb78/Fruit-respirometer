


#include <Wire.h>
#include <Time.h>
#define K33 0x68
#define K30 0x68
#define MultiplexerADR 0x70

void tcaselect(uint8_t i) {
  if (i > 7) return;
 
  Wire.beginTransmission(MultiplexerADR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}


void setup() {
 Serial.begin(9600);    
 Wire.begin ();
 pinMode(13, OUTPUT);  // We will use this pin as a read?indicator
}

///////////////////////////////////////////////////////////////////
// Function : void wakeSensor()
// Executes : Sends wakeup commands to K33 sensors.
// Note     : THIS COMMAND MUST BE MODIFIED FOR THE SPECIFIC AVR YOU ARE USING
//            THE REGISTERS ARE HARD?CODED
/////////////////////////////////////////////////////////////////

void wakeSensor() {
 // This command serves as a wakeup to the CO2 sensor, for K33?ELG/BLG Sensors Only
 
 // You'll have the look up the registers for your specific device, but the idea here is simple:
 // 1. Disabled the I2C engine on the AVR
 // 2. Set the Data Direction register to output on the SDA line
 // 3. Toggle the line low for ~1ms to wake the micro up. Enable I2C Engine
 // 4. Wake a millisecond.
 
 TWCR &= ~(1<<2); // Disable I2C Engine
 DDRC |= (1<<4); // Set pin to output mode
 PORTC &= ~(1<<4); // Pull pin low
 delay(1);
 PORTC |= (1<<4); // Pull pin high again
 TWCR |= (1<<2); // I2C is now enabled
 delay(1);    
}

///////////////////////////////////////////////////////////////////
// Function : void initPoll()
// Executes : Tells sensor to take a measurement.
// Notes    : A fuller implementation would read the register back and  
//            ensure the flag was set, but in our case we ensure the poll
//            period is >25s and life is generally good.
///////////////////////////////////////////////////////////////////
void initPoll() {
  tcaselect (5);
 Wire.beginTransmission(K33);
 Wire.write(0x11);
 Wire.write(0x00);
 Wire.write(0x60);
 Wire.write(0x35);
 Wire.write(0xA6);
 
 Wire.endTransmission();
 delay(20);  
 Wire.requestFrom(K33, 2);
   
 byte i = 0;
 byte buffer[2] = {0, 0};

 while(Wire.available()) {
     buffer[i] = Wire.read();
     i++;
 }  
 
}

///////////////////////////////////////////////////////////////////
// Function : double readRH()
// Returns  : The current RH Value, ?1 if error has occured
///////////////////////////////////////////////////////////////////

double readRH() {
 int RH_value = 0;   // We will store the RH value inside this variable.  
 digitalWrite(13, HIGH);                
 
 //////////////////////////
 /* Begin Write Sequence */
 //////////////////////////
 tcaselect (5);
 Wire.beginTransmission(K33);
 Wire.write(0x22); //Command number 2 (Read Ram, 2 bytes)
 Wire.write(0x00); //Sensor address in ?? EEPROM ??
 Wire.write(0x14); //Two bytes starting from 0x14 (high byte) and 0x15 (low byte)
 Wire.write(0x36); //Checksum
 
 Wire.endTransmission();
 
 delay(20);
 
 /////////////////////////  
 /* Begin Read Sequence */
 /////////////////////////  
   
 Wire.requestFrom(K33, 4);
   
 byte i = 0;
 byte buffer[4] = {0, 0, 0, 0};
 
while(Wire.available()) {
     buffer[i] = Wire.read();
     i++;
 }  
   
 RH_value = 0;
 RH_value |= buffer[1] & 0xFF;
 RH_value = RH_value << 8;
 RH_value |= buffer[2] & 0xFF;

 byte sum = 0;                              //Checksum Byte
 sum = buffer[0] + buffer[1] + buffer[2];   //Byte addition utilizes overflow
 
 if(sum == buffer[3]) {
     // Success!
     digitalWrite(13, LOW);
     delay(10);
     return ((double)RH_value / (double) 100);
 }  
 else {
  // Failure!  
  digitalWrite(13, LOW);
  delay(10);
  return ((double) -1);
 }  
}

///////////////////////////////////////////////////////////////////
// Function : double readTemp()
// Returns  : The current Temperature Value, ?1 if error has occured
///////////////////////////////////////////////////////////////////

double readTemp() {
 int Temp_value = 0;   // We will store the temperature value inside this variable.  
 digitalWrite(13, HIGH);                
 
 //////////////////////////
 /* Begin Write Sequence */
 //////////////////////////
 tcaselect (5);
 Wire.beginTransmission(K33); //int K33 == 0x68
 Wire.write(0x22); //Command number 2 (Read Ram, 2 bytes)
 Wire.write(0x00); //Sensor address in ?? EEPROM ??
 Wire.write(0x12); //Two bytes starting from 0x12 (high byte) and 0x13 (low byte)
 Wire.write(0x34); //Checksum
 
 Wire.endTransmission();
 
 delay(20);
 
 /////////////////////////  
 /* Begin Read Sequence */
 /////////////////////////  
   
 Wire.requestFrom(K33, 4);
   
 byte i = 0;
 byte buffer[4] = {0, 0, 0, 0};
 
while(Wire.available()) {
     buffer[i] = Wire.read();
     i++;
 }  
   
 Temp_value = 0;
 Temp_value |= buffer[1] & 0xFF;
 Temp_value = Temp_value << 8;
 Temp_value |= buffer[2] & 0xFF;
 
 
 byte sum = 0;                              //Checksum Byte
 sum = buffer[0] + buffer[1] + buffer[2];   //Byte addition utilizes overflow
 
 if(sum == buffer[3]) {
     // Success!
     digitalWrite(13, LOW);
     delay(10);
     return ((double)Temp_value / (double) 100);
 }  
 else {
  // Failure!  
  digitalWrite(13, LOW);
  delay(10);
  return ((double) -1);
 }  
}

///////////////////////////////////////////////////////////////////
// Function : double readCO2_K33()
// Returns  : The current CO2 Value, ?1 if error has occured
///////////////////////////////////////////////////////////////////

double readCO2_K33() {
 int CO2_value_K33 = 0;   // We will store the temperature value inside this variable.  
 digitalWrite(13, HIGH);                
 
 //////////////////////////
 /* Begin Write Sequence */
 //////////////////////////
 tcaselect (5);
 Wire.beginTransmission(K33); //int K33 == 0x68
 Wire.write(0x22); //Command number 2 (Read Ram, 2 bytes)
 Wire.write(0x00); //Sensor address in ?? EEPROM ??
 Wire.write(0x08); //Two bytes starting from 0x08 (high byte) and 0x09 (low byte). They contain the CO2 data
 Wire.write(0x2A); //Checksum
 
 Wire.endTransmission();
 
 delay(50);
 
 /////////////////////////  
 /* Begin Read Sequence */
 /////////////////////////  
   
 Wire.requestFrom(K33, 4);
   
 byte i = 0;
 byte buffer[4] = {0, 0, 0, 0};
 
while(Wire.available()) {
     buffer[i] = Wire.read();
     i++;
 }  
   
 CO2_value_K33 = 0;
 CO2_value_K33 |= buffer[1] & 0xFF;
 CO2_value_K33 = CO2_value_K33 << 8;
 CO2_value_K33 |= buffer[2] & 0xFF;
 
 
 byte sum = 0;                              //Checksum Byte
 sum = buffer[0] + buffer[1] + buffer[2];   //Byte addition utilizes overflow
 
 if(sum == buffer[3]) {
     // Success!
     digitalWrite(13, LOW);
     delay(10);
     return ((double)CO2_value_K33 * (double) 1);
 }  
 else {
  // Failure!  
  digitalWrite(13, LOW);
  delay(10);
  return ((double) -1);
 }  
}
///////////////////////////////////////////////////////////////////
// Function : double readCO2_K30()
// Returns  : The current CO2 Value, ?1 if error has occured
///////////////////////////////////////////////////////////////////

double readCO2_K30() {
 int CO2_value_K30 = 0;   // We will store the temperature value inside this variable.  
 digitalWrite(13, HIGH);                
 
 //////////////////////////
 /* Begin Write Sequence */
 //////////////////////////
 tcaselect (2);
 Wire.beginTransmission(K30); //int K33 == 0x68
 Wire.write(0x22); //Command number 2 (Read Ram, 2 bytes)
 Wire.write(0x00); //Sensor address in ?? EEPROM ??
 Wire.write(0x08); //Two bytes starting from 0x08 (high byte) and 0x09 (low byte). They contain the CO2 data
 Wire.write(0x2A); //Checksum
 
 Wire.endTransmission();
 
 delay(50);
 
 /////////////////////////  
 /* Begin Read Sequence */
 /////////////////////////  
   
 Wire.requestFrom(K30, 4);
   
 byte i = 0;
 byte buffer[5] = {0, 0, 0, 0};
 
while(Wire.available()) {
     buffer[i] = Wire.read();
     i++;
 }  
   
 CO2_value_K30 = 0;
 CO2_value_K30 |= buffer[1] & 0xFF;
 CO2_value_K30 = CO2_value_K30 << 8;
 CO2_value_K30 |= buffer[2] & 0xFF;

 
 byte sum = 0;                              //Checksum Byte
 sum = buffer[0] + buffer[1] + buffer[2];   //Byte addition utilizes overflow
 
 if(sum == buffer[3]) {
     // Success!
     digitalWrite(13, LOW);
     delay(10);
     return ((double)CO2_value_K30 * (double) 1);
 }  
 else {
  // Failure!  
  digitalWrite(13, LOW);
  delay(10);
  return ((double) -1);
 }  
}
///////////////////////////////////////////////////////////////////
// Function : void loop()
// Returns  : Read RH and then temperature
///////////////////////////////////////////////////////////////////

void loop() {
 // We keep the sample period >25s or so, else the sensor will start ignoring sample requests.
 wakeSensor();
 initPoll();
 
 delay(19000);
 wakeSensor();
 double RHValue = readRH();  

 delay(20);
 wakeSensor();
 double TempValue = readTemp();

 delay(50);
 wakeSensor();
 double CO2Value_K33 = readCO2_K33();  

 delay(50);
 double CO2Value_K30 = readCO2_K30();  
 
 if(RHValue >= -50) {
       Serial.print (millis()/1000);
       Serial.print(" s RH: ");
       Serial.print(RHValue);
       Serial.print(" % Temp: ");
       Serial.print(TempValue);
       Serial.print(" C CO2a: ");
       Serial.print(CO2Value_K33, 0);
       Serial.print(" ppm CO2b: ");
       Serial.print(CO2Value_K30, 0);
       Serial.println(" ppm");     
 }    
 else {
       Serial.println(" | Checksum failed / Communication failure");
 }    
 delay(1000);
}

