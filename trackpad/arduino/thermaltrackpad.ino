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

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
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

  test_function();

  pinMode(GREEN, OUTPUT);
  pinMode(PELTIER, OUTPUT); 

  configure_temperature_sensor();

  configure_wifi_connection();
}


void loop() {
  if (is_peltier_active) {
    handle_peltier_temperature();
  }

  print_temperature();
  
  WiFiClient client = server.available();
  if (client) {
    handle_requests(client);

    // close the connection:
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
    Serial.println(ssid);

    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);

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

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

void handle_requests(WiFiClient client) {
  String request_data = "";
    while (client.connected()) {
    if (client.available()) {             // if there's bytes to read from the client,
      char c = client.read();
      Serial.write(c);
      if (c == '\n') {

        // if the current line is blank, you got two newline characters in a row.
        // that's the end of the client HTTP request
        if (request_data.length() == 0) {
          send_HTML_response(client);
          break;

        } else {    // if you got a newline, then clear request_data:
          request_data = "";
        }

      } else if (c != '\r') {  // if you got anything else but a carriage return character,
        request_data += c;
      }

      if (request_data.endsWith("GET /H")) {
        // matrix.renderBitmap(heart_frame, 8, 12);
        Serial.println("GET /H");
        digitalWrite(GREEN, HIGH);
        is_peltier_active = true;
        analogWrite(PELTIER, ON);
      }

      if (request_data.endsWith("GET /L")) {
        // matrix.renderBitmap(empty_frame, 8, 12);
        Serial.println("GET /L");
        digitalWrite(GREEN, LOW);
        is_peltier_active = false;
        analogWrite(PELTIER, OFF);
      }
    }
    
  }
}

void send_HTML_response(WiFiClient client) {
  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();

  // the content of the HTTP response follows the header:
  client.print("<p style=\"font-size:7vw;\">Click <a href=\"/H\">here</a> turn the LED on<br></p>");
  client.print("<p style=\"font-size:7vw;\">Click <a href=\"/L\">here</a> turn the LED off<br></p>");
  client.print("<h1> TEMPERATURE: </h1>");
  client.print("<p>" + String(sensor0.readTempC()) + "</p>");
  
  // The HTTP response ends with another blank line:
  client.println();
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
  // add some static text
  // will only show "UNO" (not enough space on the display)
  const String text = "   " + String(sensor0.readTempC());
  matrix.textFont(Font_5x7);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.println(text);
  matrix.endText(SCROLL_LEFT);
  matrix.endDraw();
}