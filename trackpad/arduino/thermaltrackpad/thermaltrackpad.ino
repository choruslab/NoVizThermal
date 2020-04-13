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

#include <Wire.h>
int tmp102Address = 0x48;   // Breakout sensor

#include <VSync.h>
ValueReceiver<1> receiver;  // Receiver Object
ValueSender<1> sender;      // Sender Object

int output;
int input;
int yellow = 9;
int green = 10;
int red = 11;
int fetPin = 3;
const int pinUP = 225;
const int pinDOWN = 0;
const int DANGER = 24;

void setup() {
  Serial.begin(9600);

  //set up PINS
  Wire.begin();
  pinMode(green, OUTPUT); 
  pinMode(yellow, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(fetPin, OUTPUT); 

  //set up comms
  receiver.observe(output);
  sender.observe(input);
}

void loop() {
  receiver.sync();  //get output from processing

  if (output == 1){
    analogWrite(fetPin, pinUP); //turn heater on
    digitalWrite(yellow, HIGH); //turn indicator on
    input = 1;
    delay(30000);
    analogWrite(fetPin, pinDOWN); //turn heater off
  } else {
    analogWrite(fetPin, pinDOWN); //turn heater off
    digitalWrite(yellow, LOW); //turn indicator off
    input = 2;
  }

  sender.sync();
  
  //NOTE: the getTemperature is not accurate
  float celsius = getTemperature();
  Serial.print("Celsius: ");
  Serial.print(celsius);
  Serial.print("\n");

  if(celsius >= DANGER){
    //turn red on
    digitalWrite(green, LOW);
    digitalWrite(red, HIGH);
  }
  else{
    //turn green on
    digitalWrite(green, HIGH);
    digitalWrite(red, LOW);
  }
}

float getTemperature(){
  Wire.requestFrom(tmp102Address, 2);
  byte MSB = Wire.read();
  byte LSB = Wire.read();
  
  //it's a 12bit int, using two's compliment for negative
  int TemperatureSum = ((MSB << 8) | LSB) >> 4;
  float celsius = TemperatureSum*0.0625;
  return celsius;
}
