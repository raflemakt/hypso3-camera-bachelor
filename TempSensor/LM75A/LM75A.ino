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
  Wire.requestFrom(tempAddress, 2);

  //Leser av begge bytene, lsb kan bare være 0 eller 1
  while(Wire.available() >= 2) {
    byte msb = Wire.read();
    byte lsb = Wire.read();

    tempData = msb;
    //Gjør data om til floats til senere
    tempData_f = float(tempData);

    //Legger til 0.5 hvis lsb er lik 1
    if (lsb | "1000" == 1){
      tempData_f += 0.5;
    } else {
      tempData_f = msb;
    }
    //printer ut all dataen
    Serial.print("Temperature data: ");
    Serial.print(tempData_f, 1);
    Serial.println(" °C");
    delay(1000);
  }
}
