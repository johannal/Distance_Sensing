#include <Servo.h>

// create servo objects
Servo leftMotor;
Servo rightMotor;

// constants 
const int buttonPin = 2;
const int ledPin = 7;

const int serialPeriod = 250;       // only print to the serial console every 1/4 second
unsigned long timeSerialDelay = 0;

const int loopPeriod = 20;          // a period of 20ms = a frequency of 50Hz
unsigned long timeLoopDelay   = 0;

// specify the trig & echo pins used for the ultrasonic sensors
const int ultrasonicTrigPin = 8;
const int ultrasonicEchoPin = 9;

int ultrasonicDistance;
int ultrasonicDuration;

// define the states
#define DRIVE_FORWARD     0
#define TURN_LEFT         1

int state = DRIVE_FORWARD; // 0 = drive forward (DEFAULT), 1 = turn left

int buttonState = 0;

void setup()
{
    Serial.begin(9600);
    // Pins
    pinMode(buttonPin, INPUT);
    pinMode(ledPin, OUTPUT);
    
    // ultrasonic sensor pin configurations
    pinMode(ultrasonicTrigPin, OUTPUT);
    pinMode(ultrasonicEchoPin, INPUT);
    
    // servos
    leftMotor.attach(13);
    rightMotor.attach(12);
}


void loop()
{
    showDistance(); // prints debugging messages to the serial monitor
    
    killSwitch(); //check kill switch
    
    if(millis() - timeLoopDelay >= loopPeriod)
    {
        readUltrasonicSensors(); // read and store the measured distances
        
        driveAround();
        
        timeLoopDelay = millis();
    }
}

void killSwitch()
{
  //    if(digitalRead(2) == HIGH) // if the push button switch was pressed
  //    {
  //        while(1)
  //        {
  //            leftMotor.write(90);
  //            rightMotor.write(90);
  //        }
  //    }
  
  // read the state of the button
  buttonState = digitalRead(buttonPin);
 
  Serial.print("buttonState:");
  Serial.print(buttonState);
  Serial.println();
 
  // if the state of the button is HIGH it is pressed
  if(buttonState == HIGH)
  {
    endProgram();
  }
  else if(buttonState == LOW)
  {
    return;
  }
}

void endProgram()
{
  rightMotor.write(90);
  leftMotor.write(90);
  Serial.println("Robot stopped by kill switch");
  while(1) { }
}

void driveAround()
{
    if(state == DRIVE_FORWARD) // no obstacles detected
    {
        if(ultrasonicDistance > 6 || ultrasonicDistance < 0) // if there's nothing in front of us (note: ultrasonicDistance will be negative for some ultrasonics if there's nothing in range)
        {
            // drive forward
            rightMotor.write(180);
            leftMotor.write(0);
            digitalWrite(ledPin, LOW); // turn off LED
            Serial.println("No obstacle detected: move forward");
        }
        else // there's an object in front of us
        {
            state = TURN_LEFT;
            digitalWrite(ledPin, HIGH); // turn ON LED
            Serial.println("Obstacle detected: turn left");
        }
    }
    else if(state == TURN_LEFT) // obstacle detected -- turn left
    {
        unsigned long timeToTurnLeft = 500; // it takes around .5 seconds to turn 90 degrees
        
        unsigned long turnStartTime = millis(); // save the time that we started turning

        while((millis()-turnStartTime) < timeToTurnLeft) // stay in this loop until timeToTurnLeft (.5 seconds) has elapsed
        {
            // turn left
            rightMotor.write(180);
            leftMotor.write(180);
        }
        
        state = DRIVE_FORWARD;
    }
}

void readUltrasonicSensors()
{
    // ultrasonic distance 
    digitalWrite(ultrasonicTrigPin, HIGH);
    delayMicroseconds(10);                  // must keep the trig pin high for at least 10µs
    digitalWrite(ultrasonicTrigPin, LOW);
    
    // To get a distance measurement, measure the amount of time the sensor sends a HIGH signal (object in front)
    // divide by 2 to get the time it took the ping to get to the object (half the round trip time)
    // then divide by the speed of sound, at 20 Celsius is 343 metres/sec, ~29 µs/cm 
    ultrasonicDuration = pulseIn(ultrasonicEchoPin, HIGH);
    ultrasonicDistance = (ultrasonicDuration/2)/29;
}


void showDistance()
{
  // output the distance in cm to the serial monitor
    if((millis() - timeSerialDelay) > serialPeriod)
    {
        Serial.print("ultrasonicDuration: ");
        Serial.print(ultrasonicDuration);
        Serial.println();
        Serial.print("ultrasonicDistance: ");
        Serial.print(ultrasonicDistance);
        Serial.print("cm");
        Serial.println();
        
        timeSerialDelay = millis();
    }
}
