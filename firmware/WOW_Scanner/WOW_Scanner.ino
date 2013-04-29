/*
 Toy barcode scanner for the World of Wonder Childrens museum in Lafayette, CO
 By Chris Francis
 
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
 
 4-28-2013 (NES): 
 Many hardware problems. The main board's voltage regulator overheats when 9Vs is applied. Arduino Pro replaced.
 IDC cable to three trimpots was faulty. Removed cable and the three trim pots. Vol is now directly controlled on amp board.
 Removed LM386 based amplifier. There was some very noisy feedback. Installed the TPA2005D1: https://www.sparkfun.com/products/11044
 P-Channel MOSFET was installed incorrectly on low-side of LED power circuit. Replaced with 2N3904 BJT. LED now works analog/correctly.
 Why is barrel jack mounted on bottom of board? No idea. Can be changed in the future.
 
 */

int ledPin = 6;                         // LED on pin 6 (PWM)
int beeperPin = 7;                      // Beeper on pin 7
int senseIRPin = A4;                    // Sensor on analog pin 4
//int sensePotPin = A1;                   // Sensitivity pot on analog pin 1
int statLED = 13; //On board stat LED

const int threshold = 400; //If the IR reads more than this value, make some noise
long lastCheck = 0; //This keeps track of the millis for blinking the status LED

//These two functions allow the LED to be really bright when on, and 
//just barely on when the scanner is in idle mode
#define LEDON() analogWrite(ledPin, 255)
#define LEDOFF() analogWrite(ledPin, 10)

void setup()
{
  Serial.begin(57600);                   // Start up serial port

  pinMode(ledPin, OUTPUT);              // Makes LED pin an output
  LEDOFF(); //Turn off LED
  
  pinMode(beeperPin, OUTPUT);           // Makes beeper pin an output
  pinMode(senseIRPin, INPUT);           // Makea IR sensor pin an input
  //pinMode(sensePotPin, INPUT);          // Makes sensitivity pot pin an input

  //Fancy startup sound, just like a real scanner!
  LEDON();
  tone(beeperPin, 1000, 125);
  delay(125);
  tone(beeperPin, 1500, 125);
  delay(125);
  tone(beeperPin, 2000, 125);
  delay(125);
  LEDOFF();
}

void loop()
{
  //int threshold = analogRead(sensePotPin);         // Get the scan distance from the threshold pot
  //threshold = map(threshold,0,1023,0,255);         // Conver this 10 bit value to an 8 bit value

  int sensorVal = readSensor(); // Small loop to grab 16 readings and get the average value

  Serial.print("Sensor:");
  Serial.print(sensorVal);
  
  Serial.print(" Threshold:");
  Serial.println(threshold);

  //If the sensor value is greater than the threshold, make some noise!
  if (sensorVal > threshold)
  {
    tone(beeperPin, 2000, 125); // Make a beep sound

    LEDON(); // Turn on valid scan LED

    while(sensorVal > threshold) // Spin our wheels until user removes the object
    {
      sensorVal = readSensor(); // Small loop to grab 16 readings and get the average value

      Serial.print("Scanning - Sensor:");
      Serial.print(sensorVal);
      
      Serial.print(" Threshold:");
      Serial.println(threshold);
    }

    LEDOFF(); //Turn off Status LED
    delay(1000); //Wait a second to prevent rapid beeping
  }

  //Blink the status LED every four seconds
  if(millis() - lastCheck > 4000)
  {
    lastCheck = millis(); 
    
    digitalWrite(statLED, HIGH);
    delay(50);
    digitalWrite(statLED, LOW);
  }

  delay(10);
}

//Takes a series of readings from the analog sensor
//Reports a value from 0 to 1023
int readSensor() 
{
  #define NUMBER_OF_SAMPLES 16
  
  int sensorValue = 0;

  for(int x = 0 ; x < NUMBER_OF_SAMPLES ; x++)
  {
    sensorValue += analogRead(senseIRPin);
    delay(1);
  }

  sensorValue /= NUMBER_OF_SAMPLES;

  //sensorValue = map(sensorValue, 0, 1023, 0, 255); //Map to an 8-bit value
  
  return(sensorValue);  
}
