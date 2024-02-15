#include <Wire.h>

//Temperatursensorens adresse
const int tempAddress = 0b1001000;
uint16_t tempData;
float tempData_f;

void setup() {
  //Starter I2C-kommunikasjonen og serial-monitor
  Wire.begin();
  Serial.begin(9600);

  //Sjekker om arduinoen finner temperatursensoren
  Wire.beginTransmission(tempAddress);
  while (Wire.endTransmission() != 0) {
    Serial.println("Device not found, check connections");
    delay(5000);
  }
  Serial.println("Device found :D");
}

void loop() {
   //spør om x antall bytes (2 i dette tilfellet)
  Wire.requestFrom(tempAddress, (uint8_t)2);

  //Leser av begge bytene, gjør resultatet om til en float
  while(Wire.available() >= 2){
    uint8_t msb = Wire.read();
    uint8_t lsb = Wire.read();

    uint16_t tempData = msb << 8 | lsb;
    float tempData_f = float(int16_t(tempData)) / 256.0f;
  
    //printer ut all dataen
    Serial.print("Temperature data: ");
    Serial.print(tempData_f, 1);
    Serial.println(" °C");
    delay(1000);
  }
}
