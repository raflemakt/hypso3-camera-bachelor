#include <Wire.h>

const int tempAddress = 0b1001000;
int16_t tempData;

void setup() {
  Wire.begin();
  Serial.begin(9600);

  Wire.beginTransmission(tempAddress);
  while (Wire.endTransmission() != 0) {
    Serial.println("Device not found, check connections");
    delay(5000);
  }
  Serial.println("Device found :D");
}

void loop() {

  Wire.requestFrom(tempAddress, 2);

  if (Wire.available() >= 2) {
    byte msb = Wire.read();
    byte lsb = Wire.read();

    tempData = (msb << 8) | lsb;


    Serial.print("Temperature data: ");
    Serial.print(tempData, BIN);
    Serial.println(" °C");
  }
  delay(1000);
}
