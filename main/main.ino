#include <ZumoMotors.h>
#include <Pushbutton.h>
#include <QTRSensors.h>
#include <ZumoReflectanceSensorArray.h>

#define LED 13

//thresholds for reflectance sensors
#define MIDDLE_THRESHOLD 400
#define LR_THRESHOLD 1500

//pins for ultra sonic sensor
#define trigPin 2
#define echoPin 6

//number of sensors
#define NUM_SENSORS 6

//motor sensors
#define TURN_BASE_SPEED  100
#define TURN_DELAY       1600      //90 degree turn
#define FINE_TURN_DELAY  1600 / 9  //10 degree fine turn
#define REVERSE_SPEED    100       
#define REVERSE_DURATION 300
#define FORWARD_SPEED    200
#define LEFT_SPEED       170

unsigned int sensor_values[NUM_SENSORS];
unsigned long interval = 1000;
unsigned long previousMillis = 0;

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


void loop() {
  // put your main code here, to run repeatedly:
  sensors.read(sensor_values);
 
  if (Serial.available() > 0) {


      incomingChar = Serial.read();

    
    
    if (incomingChar == 'w' || incomingChar == 'c') 
    {
      if (incomingChar == 'c') 
      {
        Serial.println("turnC");
        Serial.println("Turn complete.");        
      }
      
      roomOrCorridor();

      automatic = true;
      
      Serial.println("Going forwards.");
      while (automatic == true) //code will always run unless told to stop
      {
        incomingChar = Serial.read(); //reads serial input and places into a String variable
        
        if (incomingChar == 'x')
        {
          automatic = false;
          motors.setSpeeds(0,0);
          delay(100);
          break;
        }
        
        motors.setSpeeds(LEFT_SPEED, FORWARD_SPEED); //move forward
        moveAndCheckForLines();    //keep Zumo within track borders
      }
    }
    else if (incomingChar == 'a') {
      Serial.println("leftTurn");
      Serial.println("Turning left 90 degrees.");
      motors.setSpeeds(-TURN_BASE_SPEED, TURN_BASE_SPEED); //move left
      delay(TURN_DELAY);
      motors.setSpeeds(0,0);
    }
    else if (incomingChar == 'A') {
      Serial.println("Turning left 10 degrees.");
      motors.setSpeeds(-TURN_BASE_SPEED, TURN_BASE_SPEED); //move left
      delay(FINE_TURN_DELAY);
      motors.setSpeeds(0,0);
    }
    else if (incomingChar == 's') {
      Serial.println("Reversing.");
      motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED); //move backwards
    }
    else if (incomingChar == 'd') {
      Serial.println("rightTurn");
      Serial.println("Turning right 90 degrees.");
      motors.setSpeeds(TURN_BASE_SPEED, -TURN_BASE_SPEED); //move left
      delay(TURN_DELAY);
      motors.setSpeeds(0,0);
    }
    else if (incomingChar == 'D') {
      Serial.println("Turning right 10 degrees.");
      motors.setSpeeds(TURN_BASE_SPEED, -TURN_BASE_SPEED); //move left
      delay(FINE_TURN_DELAY);
      motors.setSpeeds(0,0);
    }
    else if (incomingChar == 'x') {
      Serial.println("Stopping.");
      automatic = false;
      motors.setSpeeds(0, 0); //stop
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
    {
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
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(REVERSE_DURATION);
    motors.setSpeeds(TURN_BASE_SPEED, -TURN_BASE_SPEED);
    delay(100);
    motors.setSpeeds(LEFT_SPEED, FORWARD_SPEED);
}
else if (sensor_values[5] > LR_THRESHOLD)
{
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(REVERSE_DURATION);
    motors.setSpeeds(-TURN_BASE_SPEED, TURN_BASE_SPEED);
    delay(100);
    motors.setSpeeds(LEFT_SPEED, FORWARD_SPEED);
}
else 
{
  motors.setSpeeds(LEFT_SPEED, FORWARD_SPEED);
}
}


bool overLine(int sensorPin) {
  return sensorPin > MIDDLE_THRESHOLD;
}

bool roomOrCorridor() {
  if (leftRoom == true) {
    Serial.print("Entering left room.\n");
  }
  else if (rightRoom == true) {
    Serial.print("Entering right room.\n");
  }
  else if (leftCorridor == true) {
    Serial.print("Entering right corridor.\n");
  }
  else if (rightCorridor == true) {
    Serial.print("Entering right room.\n");
  }
}

void enterRoom() {
  
  motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  delay(200);
  motors.setSpeeds(0,0);  
  delay(100);
  motors.setSpeeds(-TURN_BASE_SPEED, TURN_BASE_SPEED);
  delay(TURN_DELAY);
  
  for (int x = 0; x < 9; x++) {
    motors.setSpeeds(TURN_BASE_SPEED, -TURN_BASE_SPEED);
    delay(TURN_DELAY * 4 / 9);
    long duration, distance;
    digitalWrite(trigPin, LOW);  // Added this line
    delayMicroseconds(2); // Added this line
    digitalWrite(trigPin, HIGH);
  //  delayMicroseconds(1000); - Removed this line
    delayMicroseconds(10); // Added this line
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = (duration/2) / 29.1;
  
    if (distance >= 18 || distance <= 0){
      Serial.println("Not found.");
    }
    else {
      Serial.println("Object found.");
      motors.setSpeeds(0,0);
    }
  }
  motors.setSpeeds(0,0);
  
}
void exitRoom() {

  if (leftRoom)
  {
    motors.setSpeeds(-100, -100);
    delay(200);
    Serial.print("Left left room.\n");
    motors.setSpeeds(TURN_BASE_SPEED, -TURN_BASE_SPEED); //move left
    delay(TURN_DELAY);
    motors.setSpeeds(0,0);
    leftRoom = false;
  }
  else if (rightRoom)
  {
    motors.setSpeeds(-100, -100);
    delay(200);
    Serial.print("Left right room.\n");
    motors.setSpeeds(-TURN_BASE_SPEED, TURN_BASE_SPEED); //move left
    delay(TURN_DELAY);
    motors.setSpeeds(0,0);
    rightRoom = false;
  }
}

