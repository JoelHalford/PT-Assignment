#include <ZumoMotors.h>
#include <Pushbutton.h>
#include <QTRSensors.h>
#include <ZumoReflectanceSensorArray.h>

#define LED 13

//thresholds for reflectance sensors
#define MIDDLE_THRESHOLD 400
#define LR_THRESHOLD 1500

//pins for ultrasonic sensor
#define trigPin 2
#define echoPin 6

#define NUM_SENSORS 6             //the number of sensors

//motor sensors
#define TURN_BASE_SPEED  150      //base speed of turns
#define TURN_DELAY       700      //90 degree turn
#define FINE_TURN_DELAY  180      //slight turn
#define REVERSE_SPEED    100      //reverse speed
#define REVERSE_DURATION 300      //duration of reverse
#define RIGHT_SPEED      150      //right speed
#define LEFT_SPEED       170      //left speed fast to align

unsigned int sensor_values[NUM_SENSORS];

//booleans for rooms, corridors and zumo being automated
bool automatic = false;
bool leftRoom = false;
bool rightRoom = false;
bool leftCorridor = false;
bool rightCorridor = false;

ZumoMotors motors;
ZumoReflectanceSensorArray sensors(QTR_NO_EMITTER_PIN);
Pushbutton button(ZUMO_BUTTON); // pushbutton on pin 12

//string for incoming string, sent through port over processing
String incomingString;
int incomingChar;

void setup() {
  //initialize serial communications
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  digitalWrite(13, HIGH);
  button.waitForButton();
  digitalWrite(13, LOW);
  delay(1000);
}


void loop() 
{// put your main code here, to run repeatedly:
  sensors.read(sensor_values);
 
  if (Serial.available() > 0) 
  {//if serial is available, run this
    
      Serial.flush();
      incomingChar = Serial.read();    
    
    if (incomingChar == 'w') 
    {
      roomOrCorridor();

      automatic = true;

      Serial.println("Moving forwards.");
      while (automatic == true)
      {//code will always run unless told to stop
        incomingChar = Serial.read(); //reads serial input and places into a String variable
        
        if (incomingChar == 'x')
        {//if stop is pressed, set automatic to false, stop motors and break from the loop
          automatic = false;
          motors.setSpeeds(0,0);
          delay(100);
          break;
        }
        
        motors.setSpeeds(LEFT_SPEED, RIGHT_SPEED); //move forward
        moveAndCheckForLines();    //keep Zumo within track borders
      }
    }
    else if (incomingChar == 'c') 
    {
      roomOrCorridor();

      automatic = true;

      Serial.println("Moving forwards.");
      while (automatic == true)
      {//code will always run unless told to stop
        incomingChar = Serial.read(); //reads serial input and places into a String variable
        
        if (incomingChar == 'x')
        {//if stop is pressed, set automatic to false, stop motors and break from the loop
          automatic = false;
          motors.setSpeeds(0,0);
          delay(100);
          break;
        }
        
        motors.setSpeeds(LEFT_SPEED, RIGHT_SPEED); //move forward
        moveAndCheckForLines();    //keep Zumo within track borders
      }
    }
    else if (incomingChar == 'a') {
      Serial.println("Turned left.");
      motors.setSpeeds(-TURN_BASE_SPEED, TURN_BASE_SPEED); //move left
      delay(TURN_DELAY);                                   //delay for set amount
      motors.setSpeeds(0,0);                               //stop motors
    }
    else if (incomingChar == 'A') {
      motors.setSpeeds(-TURN_BASE_SPEED, TURN_BASE_SPEED); //move left
      delay(FINE_TURN_DELAY);
      motors.setSpeeds(0,0);
    }
    else if (incomingChar == 's') {
      motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);    //move backwards
    }
    else if (incomingChar == 'd') {
      Serial.println("Turned right.");
      motors.setSpeeds(TURN_BASE_SPEED, -TURN_BASE_SPEED); //move left
      delay(TURN_DELAY);
      motors.setSpeeds(0,0);
    }
    else if (incomingChar == 'D') {
      motors.setSpeeds(TURN_BASE_SPEED, -TURN_BASE_SPEED); //move left
      delay(FINE_TURN_DELAY);
      motors.setSpeeds(0,0);
    }
    else if (incomingChar == 'x')
    {
      automatic = false;
      motors.setSpeeds(0,0);
      delay(100);
    }
    else if (incomingChar == 'r')
    {
      incomingChar = Serial.read();
      if (incomingChar == 'R')
      {
        Serial.println("rRo");
        Serial.println("Room to the right.");
        rightRoom = true;
      }
      if (incomingChar == 'C')
      {
        Serial.println("rCo");
        Serial.println("Corridor to the right.");
        rightCorridor = true;
      }
    }
    else if (incomingChar == 'l')
    {
      incomingChar = Serial.read();
      if (incomingChar == 'R')
      {
      Serial.println("lRo");   
      Serial.println("Room to the left.");
      leftRoom = true;
      }
      if (incomingChar == 'C')
      {
        Serial.println("lCo");
        Serial.println("Corridor to the left.");
        leftCorridor = true;
      }
    }
    else if (incomingChar == 'R')
    {//checks first character for entering/leaving 
      incomingChar = Serial.read();
      if (incomingChar == 'E')
      {
        enterRoom();
      }
      else if (incomingChar == 'L')
      {
        exitRoom();
      }
    }
  }
}

void moveAndCheckForLines() 
{
  sensors.read(sensor_values);
  
  if ((sensor_values[1] > MIDDLE_THRESHOLD) || (sensor_values[2] > MIDDLE_THRESHOLD) || (sensor_values[3] > MIDDLE_THRESHOLD) 
  || (sensor_values[4] > MIDDLE_THRESHOLD) || ((sensor_values[0] > MIDDLE_THRESHOLD) && (sensor_values[5] > MIDDLE_THRESHOLD)))
  {//if 2nd, 3rd, 4th, 5th, 1st and 6th sensor over threshold, stop motors
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(REVERSE_DURATION);
    automatic = false;
    motors.setSpeeds(0,0);
    Serial.println("Wall detected, stopping.");
  }
  
  else if (sensor_values[0] > LR_THRESHOLD)
  {//if first sensor is above threshold, reverse and adjust then continue forwards
      motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
      delay(REVERSE_DURATION);
      motors.setSpeeds(TURN_BASE_SPEED, -TURN_BASE_SPEED);
      delay(100);
      motors.setSpeeds(LEFT_SPEED, RIGHT_SPEED);
  }
  else if (sensor_values[5] > LR_THRESHOLD)
  {//if last sensor is above threshold, reverse and adjust then continue forwards
      motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
      delay(REVERSE_DURATION);
      motors.setSpeeds(-TURN_BASE_SPEED, TURN_BASE_SPEED);
      delay(100);
      motors.setSpeeds(LEFT_SPEED, RIGHT_SPEED);
  }
  else 
  {
    motors.setSpeeds(LEFT_SPEED, RIGHT_SPEED);
  }
}

bool roomOrCorridor() 
{//checks if room/corridor is true then sends relevant message
  if (leftRoom == true) {
    Serial.print("Entering left room.\n");
    leftRoom = false;
  }
  else if (rightRoom == true) {
    Serial.print("Entering right room.\n");
    rightRoom = false;
  }
  else if (leftCorridor == true) {
    Serial.print("Entering right corridor.\n");
    leftCorridor = false;
    
  }
  else if (rightCorridor == true) {
    Serial.print("Entering right corridor.\n");
    rightCorridor = false;
  }
}

void enterRoom() 
{//enters room slightly, turns 90 degrees and proceeds to scan room 
  motors.setSpeeds(LEFT_SPEED, RIGHT_SPEED);
  delay(200);
  motors.setSpeeds(0,0);  
  delay(100);
  motors.setSpeeds(-TURN_BASE_SPEED, TURN_BASE_SPEED);
  delay(TURN_DELAY);
  
  for (int x = 0; x < 9; x++) 
  {//scans 9 times in 180 degree motion, stops when detected an object
    motors.setSpeeds(TURN_BASE_SPEED, -TURN_BASE_SPEED);
    delay(TURN_DELAY * 2 / 9);
    long duration, distance;
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = (duration/2) / 29.1;
  
    if (distance >= 18 || distance <= 0)
    {//if object out of bounds or doesn't exist, ignore
    }
    else 
    {//else object has been found
      Serial.println("objectFound");
      motors.setSpeeds(0,0);
      break;
    }
  }
  motors.setSpeeds(0,0);
  
}
void exitRoom() 
{//reverses slightly, exiting the room. Sets leftRoom/rightRoom to false
  if (leftRoom)
  {
    motors.setSpeeds(-100, -100);
    delay(200);
    motors.setSpeeds(0,0);
    Serial.print("Exited left room.\n");
    leftRoom = false;
  }
  else if (rightRoom)
  {
    motors.setSpeeds(-100, -100);
    delay(200);
    motors.setSpeeds(0,0);
    Serial.print("Exited right room.\n");
    rightRoom = false;
  }
}
