/*
  DigitalReadSerial

  Sets digital output pin 4 as high to feed an optoisolator 
  Reads a digital input on pin 3, prints the result to the Serial Monitor

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/DigitalReadSerial

This code was modified from its original version as an example from the Arduino IDE
*/

// this message will be sent to the MQTT board to indicate that the chime has been activated. 
byte doorChime [] = {0x02, 0x43, 0x03}; // [stx] c [etx] to represent a state change for the door chime
// digital pin 5 is connected to the security panel output
int panelInput = 3;
int constantOut = 4;
// need a boolean to keep the board from reporting constantly. 

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600); // this goes to the Huzzah board
  // make the pushbutton's pin an input:
  pinMode(panelInput, INPUT); // from the converter
  pinMode(constantOut, OUTPUT); // to the converter
  digitalWrite(constantOut, HIGH);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input pin:
  int buttonState = digitalRead(panelInput);
  // print out the state of the button:
  if (buttonState == LOW)
  {
     Serial.write(doorChime,3);
     delay(3000);
  }
  delay(1);        // delay in between reads for stability
}
