/*
Toy barcode scanner for the World of Wonder Childrens museum in Lafayette, CO
 
 This code is functional, but a work in progress! (make it smaller, better, faster and more reliable)
 
 Theory of operation: 
 A Sharp IR proximity sensor is looking up through a window checking for any objects above it. If it finds something, we blink a nice big red LED and make a beep sound.
 The brightness of the LED is controllable from full off to full on.
 The sensitivity fo the proximity sensor is adjustable to compensate for ambient light and to prevent false triggers.
 The volume of the beeper is controlable, but not in software. Volume is set via a pot and a LM386 amplifier. (full off to full on)
 
 Hardware: base proto board with an Arduino Pro on top. A proto sheild on top has the audio amp  and headers for the pot board
 
 TO DO: 
 Add code for a status LED that indicates the main loop is running. I thought I'd use mills and a timer to slowly flash an LED so we know if the software has stopped running.
 FUTURE possibility of using I2C for an external "price" display? Might be overkill for little kids.
 Revise the hardware sometime in the future to use a printed circiut board rather than proto board. (more professional and less likely to have mysterious glitches)
 I've had some issues with the reliability of stacking headers. It seems if they are not perfectly straight, they may have continuity issues. Change machined pin headers?
 Still need to create an up to date schematic in Eagle and make some board files. Switch off serial debugging once things are running smoothly. (speed up the code)
 
 More TODO:
 Add watch dog timer (250ms) and reset overall system every 4 hours
 */

int ledPin = 6;                         // LED on pin 6 (PWM)
int beeperPin = 7;                      // Beeper on pin 7
int senseIRPin = A4;                    // Sensor on analog pin 4
int sensePotPin = A1;                   // Sensitivity pot on analog pin 1
int brightPotPin = A0;                  // Brightness pot on analog pin 0

void setup()
{
  pinMode(ledPin, OUTPUT);              // Makes LED pin an output
  pinMode(beeperPin, OUTPUT);           // Makes beeper pin an output
  pinMode(senseIRPin, INPUT);           // Makea IR sensor pin an input
  pinMode(sensePotPin, INPUT);          // Makes sensitivity pot pin an input
  pinMode(brightPotPin, INPUT);         // Makes brightness pot pin an input
  Serial.begin(9600);                   // Start up serial port

  tone(beeperPin,1000,125);              //Fancy startup sound, just like a real scanner!
  delay(125);
  tone(beeperPin,1500,125);
  delay(125);
  tone(beeperPin,2000,125);
  delay(125);
}

void loop()
{
  int brightValue = analogRead(brightPotPin);      // Get the brightness from the brightness pot
  brightValue = map(brightValue,0,1023,0,255);     // Convert this 10 bit value to an 8 bit value

  int threshold = analogRead(sensePotPin);         // Get the scan distance from the threshold pot
  threshold = map(threshold,0,1023,0,255);         // Conver this 10 bit value to an 8 bit value

  int sensorValue;                                 // Small loop to grab 10 readings and get the average value
  sensorValue = 0;
  for(int x = 0 ; x < 10 ; x++){
    sensorValue += analogRead(senseIRPin);
  }
  sensorValue /= 10;
  sensorValue = map(sensorValue,0,1023,0,255);      // Conver the sensor value to an 8 bit value

  Serial.print(" Brightness  ");                   // Print some debug information so we can tweak the pots
  Serial.print(brightValue);

  Serial.print(" Threshold  ");
  Serial.print(threshold);

  Serial.print(" Sensor  ");
  Serial.print(sensorValue);

  // check if sensor is greater than threshold, if it is, blink the light and beep
  if (threshold < sensorValue)
  {
    Serial.print(" Valid scan!");                  // Print some debug information
    tone(beeperPin,2000,125);                      // Make a beep sound

    //   analogWrite(ledPin, brightValue);              // Turn on valid scan LED
    digitalWrite(ledPin, HIGH);
    while(threshold < sensorValue) {               // Spin our wheels until user removes the object
      sensorValue = 0;
      for(int x = 0 ; x < 4 ; x++){                // 
        sensorValue += analogRead(senseIRPin);     //
      }
      sensorValue /= 4;                            // reduce multiple beeps for the same object
      sensorValue = map(sensorValue,0,1023,0,255);  // End spin our wheels until user removes the object
    }
    //   analogWrite(ledPin, 0);                        //Turn off Status LED
    digitalWrite(ledPin, LOW);
    delay(1000);                                   //Wait a second to prevent rapid beeping
  }
  else
  {
    //do nothing
    Serial.print(" Not valid scan");
  }
  Serial.println();                                // Start a new line in the debug terminal
}






