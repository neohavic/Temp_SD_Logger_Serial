/*
  DS18B20 24Hr Temp logger proof of concept
  Ver 0.9B
  Austin Everman

  Uses code from SD datalogger example and from http://forum.bildr.org/viewtopic.php?t=476
*/

#include <SPI.h>
#include <OneWire.h>

int DS18S20_Pin = 2; //DS18S20 Signal pin on digital 2

//Temperature chip i/o
OneWire ds(DS18S20_Pin); // on digital pin 2


void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);   // Blnk the green LED to confirm setup
  delay(5000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(5000);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(5000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(5000);

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
}

void loop()
{
  Serial.println(getTemp());
  delay(5000);
}


float getTemp()
{
  //returns the temperature from one DS18S20 in DEG Celsius

  byte data[12];
  byte addr[8];

  if ( !ds.search(addr))
  {
    //no more sensors on chain, reset search
    ds.reset_search();
    return -1000;
  }

  if ( OneWire::crc8( addr, 7) != addr[7])
  {
    Serial.print("CRC is not valid!\n");
    return -1000;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28)
  {
    Serial.print("Device is not recognized");
    return -1000;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1); // start conversion, with parasite power on at the end

  byte present = ds.reset();
  ds.select(addr);
  ds.write(0xBE); // Read Scratchpad


  for (int i = 0; i < 9; i++)// we need 9 bytes
  {
    data[i] = ds.read();
  }

  ds.reset_search();

  byte MSB = data[1];
  byte LSB = data[0];
  float CONT_REMAIN = data[6];
  float CONT_PER_C = data[7];

  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 2 - 0.25 + (CONT_PER_C - CONT_REMAIN) / CONT_PER_C;

  return TemperatureSum;
}







