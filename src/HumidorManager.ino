/*******************************************************************************
 * Project  : HumidorManager
 * Compiler : Particle Photon
 * Verion   : 0.8.0-rc.10
 * Date     : 12.10.2018
 * Author   : Tim Hornikel
 * License  : GNU General Public License v3+
*******************************************************************************/

#include "application.h"

// Includings of 3rd party libraries
#include "PietteTech_DHT.h"

 // system defines
#define DHTTYPE  AM2302              // Sensor type DHT11/21/22/AM2301/AM2302
#define DHTPIN   D3           	    // Digital pin for communications
#define DHT_SAMPLE_INTERVAL   5000  // Sample every 5 seconds

/*
 * NOTE: Use of callback_wrapper has been deprecated but left in this example
 *       to confirm backwards compabibility.  Look at DHT_2sensor for how
 *       to write code without the callback_wrapper
 */
 //declaration
void dht_wrapper(); // must be declared before the lib initialization

// Lib instantiate
PietteTech_DHT DHT(DHTPIN, DHTTYPE, dht_wrapper);
// ref: https://www.sparkfun.com/datasheets/Sensors/Temperature/DHT22.pdf

// Select external Antenna
STARTUP(WiFi.selectAntenna(ANT_EXTERNAL));

// globals
unsigned int DHTnextSampleTime;	    // Next time we want to start sample
bool bDHTstarted;		    // flag to indicate we started acquisition
int n;                              // counter
String sensorStatus = "Initialization";
// Version MAJOR.MINOR.PATCH
String firmwareVersion = "v1.0-rc1";
double dHumidity = 0.0;
double dTemperature = 0.0;

void setup() {

  Serial.printlnf("System version: %s", System.version().c_str());

  Particle.variable("Status", sensorStatus);
  Particle.variable("Temperature", &dTemperature, DOUBLE);
  Particle.variable("Humidity", &dHumidity, DOUBLE);
  Particle.variable("Firmware", firmwareVersion);

  Serial.begin(9600);
  while (!Serial.available() && millis() < 30000) {
    Serial.println("Press any key to start.");
    Particle.process();
    delay(1000);
  }

  Serial.println("DHT Example program using DHT.acquire and DHT.aquiring");
  Serial.print("LIB version: ");
  Serial.println(DHTLIB_VERSION);
  Serial.println("---------------");

  DHTnextSampleTime = 0;  // Start the first sample immediately

}


/*
 * NOTE:  Use of callback_wrapper has been deprecated but left in this example
 * to confirm backwards compatibility.
 */
 // This wrapper is in charge of calling
 // must be defined like this for the lib work
void dht_wrapper() {

  DHT.isrCallback();

}

void loop() {

  // Check if we need to start the next sample
  if (millis() > DHTnextSampleTime) {
    if (!bDHTstarted) {		// start the sample
      Serial.print("\n\n");
      Serial.print(n);
      Serial.print(": Retrieving information from sensor: ");
      DHT.acquire();
      bDHTstarted = true;
    }

    if (!DHT.acquiring()) {		// has sample completed?

        // get DHT status
      int result = DHT.getStatus();

      Serial.print("Read sensor: ");
      switch (result) {
      case DHTLIB_OK:
        Serial.println("OK");
        sensorStatus = "OK";
        break;
      case DHTLIB_ERROR_CHECKSUM:
        Serial.println("Error\n\r\tChecksum error");
        sensorStatus = "Error\n\r\tChecksum error";
        break;
      case DHTLIB_ERROR_ISR_TIMEOUT:
        Serial.println("Error\n\r\tISR time out error");
        sensorStatus = "Error\n\r\tISR time out error";
        break;
      case DHTLIB_ERROR_RESPONSE_TIMEOUT:
        Serial.println("Error\n\r\tResponse time out error");
        sensorStatus = "Error\n\r\tResponse time out error";
        break;
      case DHTLIB_ERROR_DATA_TIMEOUT:
        Serial.println("Error\n\r\tData time out error");
        sensorStatus = "Error\n\r\tData time out error";
        break;
      case DHTLIB_ERROR_ACQUIRING:
        Serial.println("Error\n\r\tAcquiring");
        sensorStatus = "Error\n\r\tAcquiring";
        break;
      case DHTLIB_ERROR_DELTA:
        Serial.println("Error\n\r\tDelta time to small");
        sensorStatus = "Error\n\r\tDelta time to small";
        break;
      case DHTLIB_ERROR_NOTSTARTED:
        Serial.println("Error\n\r\tNot started");
        sensorStatus = "Error\n\r\tNot started";
        break;
      default:
        Serial.println("Unknown error");
        sensorStatus = "Unknown error";
        break;
      }

      Serial.print("Humidity (%): ");
      Serial.println(DHT.getHumidity(), 2);
      Particle.publish("Humidity (%)", String(DHT.getHumidity(), 2));
      dHumidity = (double) DHT.getHumidity();

      Serial.print("Temperature (°C): ");
      Serial.println(DHT.getCelsius(), 2);
      Particle.publish("Temperature (°C)", String(DHT.getCelsius(), 2));
      dTemperature = (double) DHT.getCelsius();

      //Serial.print("Temperature (°F): ");
      //Serial.println(DHT.getFahrenheit(), 2);

      //Serial.print("Temperature (K): ");
      //Serial.println(DHT.getKelvin(), 2);

      Serial.print("Dew Point (°C): ");
      Serial.println(DHT.getDewPoint());

      Serial.print("Dew Point Slow (°C): ");
      Serial.println(DHT.getDewPointSlow());

      n++;  // increment counter
      bDHTstarted = false;  // reset the sample flag so we can take another
      DHTnextSampleTime = millis() + DHT_SAMPLE_INTERVAL;  // set the time for next sample

    }
  }
}
