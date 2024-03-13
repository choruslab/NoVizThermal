/*
  OSC BASE CODE SOURCE: https://maker.pro/arduino/projects/learn-how-to-enable-communication-between-an-arduino-and-web-browser
  ARDUINO CODE SOURCE: https://astronomersanonymous.wordpress.com/2016/04/02/controlling-heating-pads-with-arduino-uno/
  
  COMPONENTS: 
  1 Electric Heating Pad (such as: https://www.sparkfun.com/products/11289)
  1 N-Channel MOSFET (https://www.sparkfun.com/products/10213 )
  1 1N4001 Diode or Schottky Diode ( https://www.adafruit.com/products/755 )
  1 10k ohm resistor
  1 Arduino Uno or equivalent
  Solder & soldering tools
  TMP102 digital temperature sensor ( https://www.sparkfun.com/products/11931 )
  Several LEDS (for testing circuit only) and 330 ohm resistors
  wire, alligator clips, breadboard
*/

#include <WiFiS3.h>
#include "arduino_secrets.h" 

int status = WL_IDLE_STATUS;
WiFiServer server(80);

#include <Wire.h>
#include <SparkFunTMP102.h>

const int ALERT_PIN = A3;
TMP102 sensor0;

#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;

const int GREEN = 2;
const int PELTIER = 11;
const int ON = 225;
const int OFF = 0;
const int MAX_TEMP = 30;
bool is_peltier_active = false;


void setup() {
  Serial.begin(9600);
  matrix.begin();

  pinMode(GREEN, OUTPUT);
  pinMode(PELTIER, OUTPUT); 

  configure_temperature_sensor();

  configure_wifi_connection();
}


void loop() {
  if (is_peltier_active) {
    handle_peltier_temperature();
  }

  // print_temperature();
  // Serial.println(sensor0.readTempC());
  
  WiFiClient client = server.available();
  if (client) {
    handle_requests(client);

    // Respond and close the connection:
    client.println("HTTP/1.1 204 No Content\r\n\r\n");
    client.flush();
    client.stop();
    Serial.println("client disconnected");
  }
}


void configure_wifi_connection() {
  if (WiFi.firmwareVersion() < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(SECRET_SSID);

    // Connect to WPA/WPA2 network
    status = WiFi.begin(SECRET_SSID, SECRET_PASS);

    // wait 10 seconds for connection:
    delay(10000);
  }
  
  server.begin();
  printWifiStatus();
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
 
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

/*
 * HTTP REQUEST FORMAT:
 * GET /resource HTTP/1.1\r\n
 * Host: 10.101.101.10\r\n
 * ...
 * \r\n
 */
void handle_requests(WiFiClient client) {
  String request_data = "";
  Serial.println("Client is here!");
  while (client.connected()) {
    if (client.available()) {
      String request_line = client.readStringUntil('\n');
      Serial.println(request_line);

      if (request_line.startsWith("PUT /H")) {
        Serial.println("PUT /H");
        digitalWrite(GREEN, HIGH);
        is_peltier_active = true;
        analogWrite(PELTIER, ON);

        break;
      }

      if (request_line.startsWith("PUT /L")) {
        Serial.println("PUT /L");
        digitalWrite(GREEN, LOW);
        is_peltier_active = false;
        analogWrite(PELTIER, OFF);

        break;
      }

      if (request_line.startsWith("\r")) {
        break;
      }
    }
  
  }
}


void configure_temperature_sensor() {
  Wire.begin();

  if(!sensor0.begin()) {
    Serial.println("Cannot connect to TMP102.");
    while(1);
  }
}

void handle_peltier_temperature() {
  Serial.println("Handling Temperature...");
  float temperature = sensor0.readTempC();

  if (temperature < MAX_TEMP) {
    Serial.println("All good");
    analogWrite(PELTIER, ON);
  } else {
    Serial.println("Too hot!!");
    analogWrite(PELTIER, OFF);
  }
}

void print_temperature() {
  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(80);
  const String text = "   " + String(sensor0.readTempC());
  matrix.textFont(Font_5x7);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.println(text);
  matrix.endText(SCROLL_LEFT);
  matrix.endDraw();
}