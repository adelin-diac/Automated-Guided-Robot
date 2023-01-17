#define rightPin1 34 // Right Wheel pin1 - 2.3
#define rightPin2 38 // Right Wheel pin2 - 2.4

#define leftPin1 39 // Left Wheel pin1 - 2.6
#define leftPin2 40 // Left Wheel pin2 - 2.7

#define RIGHT_WHEEL_SPEED_MAX 150
#define LEFT_WHEEL_SPEED_MAX 185

// Sensor1 is leftmost sensor
// Sensor5 is rightmost sensor
/* 
       AGV LAYOUT
       
    [S1, S2, S3, S4, S5]
[][ ////// BODY ///////  ][]
  [                      ]
  
*/
#define sensor1 28 // Burgundy - 4.7
#define sensor2 26 // Red - 4.4
#define sensor3 25 // Orange - 4.2
#define sensor4 24 // Yellow - 4.0
#define sensor5 23 // Green - 6.1

const int maxSensorVal = 750;

bool moveRobot = true;
int sensorVals[5];

void moveForward(int msDelay, bool slowly);
void moveBackward(int msDelay);
void stopRobot();

void setup() {
  Serial.begin(9600);
  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);
  pinMode(sensor3, INPUT);
  pinMode(sensor4, INPUT);
  pinMode(sensor5, INPUT);
  
  pinMode(rightPin1, OUTPUT);
  pinMode(rightPin2, OUTPUT);
  
  pinMode(leftPin1, OUTPUT);
  pinMode(leftPin2, OUTPUT);
  moveForward(0, false);
}

void loop() {
  moveForward(0, true);
  sensorVals[0] = analogRead(sensor1); 
  sensorVals[1] = analogRead(sensor2); 
  sensorVals[2] = analogRead(sensor3);
  sensorVals[3] = analogRead(sensor4);
  sensorVals[4] = analogRead(sensor5);

//  if(sensorVals[0] < 750 && sensorVals[4] < 750){ // Call API
//    Serial.println("stop");
//    stopRobot();
//  }

  if(sensorVals[2] < 400 && sensorVals[1] > maxSensorVal && sensorVals[3] > maxSensorVal ){
    moveForward(0, true);
  }
  if(sensorVals[2] > maxSensorVal && sensorVals[1] > maxSensorVal && sensorVals[3] > maxSensorVal){
    stopRobot();
    moveBackward(0);
  }
  if(sensorVals[1] < maxSensorVal){
    // NEED TO TURN RIGHT HERE - DECREASE SPEED OF LEFT SLIGHTLY
    analogWrite(rightPin1, 100);
    analogWrite(rightPin2, 0);

    analogWrite(leftPin1, 50);
    analogWrite(leftPin2, 0);
  }
  if(sensorVals[3] < maxSensorVal){
    // NEED TO TURN LEFT HERE - DECREASE SPEED OF RIGHT SLIGHTLY
    analogWrite(leftPin1, 100);
    analogWrite(leftPin2, 0);
    
    analogWrite(rightPin1, 50);
    analogWrite(rightPin2, 0);
  }
  if(sensorVals[0] < maxSensorVal ){ // TURN LEFT QUICKLY
    analogWrite(leftPin1, 10);
    analogWrite(leftPin2, 0);
    
    analogWrite(rightPin1, 100);
    analogWrite(rightPin2, 0); 
  }
  if(sensorVals[4] < maxSensorVal){ // TURN RIGHT QUICKLY
    analogWrite(rightPin1, 25);
    analogWrite(rightPin2, 0); 
    
    analogWrite(leftPin1, 100);
    analogWrite(leftPin2, 0);
  }
//  else{
//    moveForward(0);
//  }

//  Serial.print(sensorVals[0]);
//  Serial.print("\t");
//  Serial.print(sensorVals[1]);
//  Serial.print("\t");
//  Serial.print(sensorVals[2]);
//  Serial.print("\t");
//  Serial.print(sensorVals[3]);
//  Serial.print("\t");
//  Serial.print(sensorVals[4]);
//  Serial.println("");
}

void moveForward(int msDelay, bool slowly){
  Serial.println("Moving Forward");
  if(slowly){
    analogWrite(rightPin1, 125);
    analogWrite(rightPin2, 0);

    // Left Wheel
    analogWrite(leftPin1, 105);
    analogWrite(leftPin2, 0);
    return;
  }
  // Right Wheel
  analogWrite(rightPin1, RIGHT_WHEEL_SPEED_MAX);
  analogWrite(rightPin2, 0);

  // Left Wheel
  analogWrite(leftPin1, LEFT_WHEEL_SPEED_MAX);
  analogWrite(leftPin2, 0);
  delay(msDelay);
}

void moveBackward(int msDelay){
  Serial.println("Moving Backward");

  // Right Wheel
  analogWrite(rightPin1, 0);
  analogWrite(rightPin2, RIGHT_WHEEL_SPEED_MAX);

  // Left Wheel
  analogWrite(leftPin1, 0);
  analogWrite(leftPin2, LEFT_WHEEL_SPEED_MAX);
  delay(msDelay);
}

void stopRobot(){
  // Right Wheel
  analogWrite(rightPin1, 0);
  analogWrite(rightPin2, 0);

  // Left Wheel
  analogWrite(leftPin1, 0);
  analogWrite(leftPin2, 0);
}
