#include <ZumoMotors.h>
#include <Pushbutton.h>
#include <QTRSensors.h>
#include <ZumoReflectanceSensorArray.h>

#define LED 13
#define MIDDLE_THRESHOLD 300
#define LR_THRESHOLD 1500


#define NUM_SENSORS 6

#define TURN_BASE_SPEED  100
#define TURN_DELAY       1600
#define REVERSE_SPEED    100
#define REVERSE_DURATION 300
#define FORWARD_SPEED    200
#define LEFT_SPEED       170

unsigned int sensor_values[NUM_SENSORS];
unsigned long startTime = millis();

bool automatic = false;
bool leftRoom = false;
bool rightRoom = false;
bool leftCorridor = false;
bool rightCorridor = false;

ZumoMotors motors;
ZumoReflectanceSensorArray sensors(QTR_NO_EMITTER_PIN);
Pushbutton button(ZUMO_BUTTON); // pushbutton on pin 12

//int incomingByte;
String incomingString;

void setup() {
  //initialize serial communications
  Serial.begin(9600);
  
  digitalWrite(13, HIGH);
  button.waitForButton();
  digitalWrite(13, LOW);
  delay(1000);
}


void loop() {
  // put your main code here, to run repeatedly:
  sensors.read(sensor_values);
 
  if (Serial.available() > 0) {
    incomingString = Serial.readString();

    if (incomingString == "w" || incomingString == "c") {

      if (incomingString == "c") {
        Serial.println("turnC");
        Serial.println("Turn complete.");        
      }
      
      roomOrCorridor();
      
      automatic = true;
      Serial.println("Going forwards.");
      while (automatic == true) //code will always run unless told to stop
      {
        incomingString = Serial.read(); //reads serial input and places into a String variable
        
        motors.setSpeeds(LEFT_SPEED, FORWARD_SPEED); //move forward
        moveAndCheckForLines();    //keep Zumo within track borders
      }
    }
    else if (incomingString == "a") {
      Serial.println("Turning left.");
      motors.setSpeeds(-TURN_BASE_SPEED, TURN_BASE_SPEED); //move left
      delay(TURN_DELAY);
      motors.setSpeeds(0,0);
    }
    else if (incomingString == "s") {
      Serial.println("Reversing.");
      motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED); //move backwards
    }
    else if (incomingString == "d") {
      Serial.println("Turning right.");
      motors.setSpeeds(TURN_BASE_SPEED, -TURN_BASE_SPEED); //move left
      delay(TURN_DELAY);
      motors.setSpeeds(0,0);
    }
    else if (incomingString == "x") {
      Serial.println("Stopping.");
      motors.setSpeeds(0, 0); //stop
    }
    else if (incomingString == "rRo")
    {
      Serial.println("rRo");
      Serial.println("Room to the right.");
      rightRoom = true;
      
    }
    else if (incomingString == "lRo")
    {      
      Serial.println("Room to the left.");
      leftRoom = true;
    }
    else if (incomingString == "rCo")
    {
      Serial.println("Corridor to the right.");
      rightCorridor = true;
    }
    else if (incomingString == "lCo")
    {
      Serial.println("Corridor to the left.");
      leftCorridor = true;
    }
  }
}

void moveAndCheckForLines() {

sensors.read(sensor_values);

if ((sensor_values[1] > MIDDLE_THRESHOLD) || (sensor_values[2] > MIDDLE_THRESHOLD) || (sensor_values[3] > MIDDLE_THRESHOLD) 
|| (sensor_values[4] > MIDDLE_THRESHOLD) || ((sensor_values[0] > MIDDLE_THRESHOLD) && (sensor_values[5] > MIDDLE_THRESHOLD)))
{
  Serial.print("Front: ");
  motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
  delay(REVERSE_DURATION);
  automatic = false;
  motors.setSpeeds(0,0);
  Serial.println("Wall detected, stopping.");
}

else if (sensor_values[0] > LR_THRESHOLD)
{   
//  if (wallHit == 0)
//  {
//    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
//    delay(REVERSE_DURATION);
//    automatic = false;
//    motors.setSpeeds(0,0);
//    Serial.println("L Wall detected, stopping.");
//  }
//  else
//  {
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(REVERSE_DURATION);
    motors.setSpeeds(TURN_BASE_SPEED + 30, -TURN_BASE_SPEED);
    delay(100);
    motors.setSpeeds(LEFT_SPEED, FORWARD_SPEED);
//    wallHit = 1;
//  }
}
else if (sensor_values[5] > LR_THRESHOLD)
{
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(REVERSE_DURATION);
    motors.setSpeeds(-TURN_BASE_SPEED, TURN_BASE_SPEED);
    delay(100);
    motors.setSpeeds(LEFT_SPEED, FORWARD_SPEED);
//    wallHit = 0;
//  }
}
else 
{
  motors.setSpeeds(LEFT_SPEED, FORWARD_SPEED);
}
}
//  if (isAtDeadEnd())
//  {
//    Serial.println("F Wall detected, stopping.");
//    motors.setSpeeds(-SPEED, -SPEED); //reverse
//    delay(200); //for 200ms
//    motors.setSpeeds(0, 0);
//    delay(1000); //for 200ms
//    incomingString = "x";
//  }


bool overLine(int sensorPin) {
  return sensorPin > MIDDLE_THRESHOLD;
}

bool roomOrCorridor() {
  if (leftRoom == true) {
    Serial.print("Entering left room.\n");
    leftRoom == false;
  }
  else if (rightRoom == true) {
    Serial.print("Entering right room.\n");
    rightRoom == false;
  }
  else if (leftCorridor == true) {
    Serial.print("Entering right corridor.\n");
    leftCorridor == false;
  }
  else if (rightCorridor == true) {
    Serial.print("Entering right room.\n");
    rightCorridor == false;
  }
}
