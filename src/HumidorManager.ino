/*******************************************************************************
 * Project  : HumidorManager
 * Compiler : Particle Photon
 * Verion   : 0.7.0
 * Date     : 07.10.2018
 * Author   : Tim Hornikel
 * License  : GNU General Public License v3+
*******************************************************************************/

// Includings of 3rd party libraries
#include "Adafruit_Si7021.h"

Adafruit_Si7021 sensor = Adafruit_Si7021();

void setup() {
  Serial.begin(115200);
  Serial.println("Si7021 test");
  sensor.begin();
}

void loop() {
  Serial.print("Humidity:    "); Serial.print(sensor.readHumidity(), 2);
  Serial.print("\tTemperature: "); Serial.println(sensor.readTemperature(), 2);
  delay(100);
}
