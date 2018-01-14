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
#define TURN_BASE_SPEED  150
#define TURN_DELAY       700      //90 degree turn
#define FINE_TURN_DELAY  178  //10 degree fine turn
#define REVERSE_SPEED    100       
#define REVERSE_DURATION 300
#define FORWARD_SPEED    150
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

      Serial.flush();
      incomingChar = Serial.read();    
    
    if (incomingChar == 'w') 
    {
      roomOrCorridor();

      automatic = true;

      Serial.println("forwards");
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
    if (incomingChar == 'c') 
    {
      Serial.println("turnC");
      Serial.println("Turn complete.");        
      
      roomOrCorridor();

      automatic = true;

      while (automatic == true) //code will always run unless told to stop
      {
        incomingChar = Serial.read(); //reads serial input and places into a String variable
        
        
        motors.setSpeeds(LEFT_SPEED, FORWARD_SPEED); //move forward
        moveAndCheckForLines();    //keep Zumo within track borders
      }
    }
    else if (incomingChar == 'a') {
      Serial.println("leftTurn");
      motors.setSpeeds(-TURN_BASE_SPEED, TURN_BASE_SPEED); //move left
      delay(TURN_DELAY);
      motors.setSpeeds(0,0);
    }
    else if (incomingChar == 'A') {
      motors.setSpeeds(-TURN_BASE_SPEED, TURN_BASE_SPEED); //move left
      delay(FINE_TURN_DELAY);
      motors.setSpeeds(0,0);
    }
    else if (incomingChar == 's') {
      motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED); //move backwards
    }
    else if (incomingChar == 'd') {
      Serial.println("rightTurn");
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


bool overLine(int sensorPin) 
{
  return sensorPin > MIDDLE_THRESHOLD;
}

bool roomOrCorridor() 
{
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
{  
  motors.setSpeeds(LEFT_SPEED, FORWARD_SPEED);
  delay(200);
  motors.setSpeeds(0,0);  
  delay(100);
  motors.setSpeeds(-TURN_BASE_SPEED, TURN_BASE_SPEED);
  delay(TURN_DELAY);
  
  for (int x = 0; x < 9; x++) {
    motors.setSpeeds(TURN_BASE_SPEED, -TURN_BASE_SPEED);
    delay(TURN_DELAY * 4 / 9);
    long duration, distance;
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = (duration/2) / 29.1;
  
    if (distance >= 18 || distance <= 0){
      
    }
    else {
      Serial.println("Object found.");
      Serial.println("objectFound");
      motors.setSpeeds(0,0);
      break;
    }
  }
  motors.setSpeeds(0,0);
  
}
void exitRoom() {

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

