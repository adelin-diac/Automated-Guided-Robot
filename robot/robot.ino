#define rightPin1 34 // Right Wheel pin1 - 2.3
#define rightPin2 38 // Right Wheel pin2 - 2.4

#define leftPin1 39 // Left Wheel pin1 - 2.6
#define leftPin2 40 // Left Wheel pin2 - 2.7

#define RIGHT_WHEEL_SPEED_MAX 220
#define LEFT_WHEEL_SPEED_MAX 235

#ifndef __CC3200R1M1RGC__
// Do not include SPI for CC3200 LaunchPad
#include <SPI.h>
#endif

#include <WiFi.h>
#define BUFSIZE 512


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

// your network name also called SSID
char ssid[] = "NETGEAR63";
// your network password
char password[] = "littlecello367";

//IPAddress server("54.78.246.30");  
char server[] = "54.78.246.30";    

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;

int currentPosition = -1;
int nextPosition = 0;
bool facingEast = true;

bool stopConnection = false;
String serverResponse;

byte sensorCombined;
byte sensorVals[5];
const int maxSensorVal = 600; // Value for breakpoint of wether sensor is activated

bool moveRobot = true;

// Functions declarations
  // MOVEMENT
void moveForward(int msDelay = 0, bool slowly = false);
void moveBackward(int msDelay);
void stopRobot();
void moveRobotFromPos();
//void turnAround(bool directionToTurnIsLeft);
void turnAround();

  // INTERNET STUFF
String readResponse();
String getResponseBody(String& response);
int getStatusCode(String& response);
String fetchNextPosition(int currentPos);
void sendAndReceiveServerResponse();
void routing();

void setup() {
    //Initialize serial and wait for port to open:
     Serial.begin(9600);

  // attempt to connect to Wifi network:
    Serial.print("Attempting to connect to Network named: ");
  // print the network name (SSID);
  Serial.println(ssid); 
  // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    WiFi.begin(ssid, password);
    while ( WiFi.status() != WL_CONNECTED) {
    // print dots while we wait to connect
       Serial.print(".");
       delay(300);
     }
  
  Serial.println("\nYou're connected to the network");
  Serial.println("Waiting for an ip address");
  
     while (WiFi.localIP() == INADDR_NONE) {
    // print dots while we wait for an ip addresss
     Serial.print(".");
     delay(300);
     }

  Serial.println("\nIP Address obtained");
  printWifiStatus();

  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);
  pinMode(sensor3, INPUT);
  pinMode(sensor4, INPUT);
  pinMode(sensor5, INPUT);
  
  pinMode(rightPin1, OUTPUT);
  pinMode(rightPin2, OUTPUT);
  
  pinMode(leftPin1, OUTPUT);
  pinMode(leftPin2, OUTPUT);

}

void loop() {
  sensorCombined = 0;
  
  sensorVals[0] = digitalRead(sensor1); //left left
  sensorVals[1] = digitalRead(sensor2); //left
  sensorVals[2] = digitalRead(sensor3); //middle
  sensorVals[3] = digitalRead(sensor4); //right
  sensorVals[4] = digitalRead(sensor5); //right right
  
  for(int i=0; i<5; i++){
   sensorCombined = sensorVals[i] << (4-i) | sensorCombined;
  }
//  Serial.println(sensorCombined);

  switch(sensorCombined){
    case(15): // 01111 = 15
      turnLeft();
      break;
    case(7): // 00111 = 7
      turnLeft();
      break;
    case(23): // 10111 = 23
      turnLeft();
      break;
    //////////////////////
    case(30): // 11110 = 30
      turnRight();
      break;
    case(28): // 11100 = 28
      turnRight();
      break;
    case(29): // 11101 = 29
      turnRight();
      break;
    /////////////////////////
    case(27): // 11011 = 27
      moveForward();
      break;
    ///////////////////////////
    case(0): //00000
      sendAndReceiveServerResponse();
      break;
    case(16): //10000
      sendAndReceiveServerResponse();
      break;
    case(1): //00001
      sendAndReceiveServerResponse();
      break;
    ///////////////////////////
    default:
      moveForward(0, true);
      break;
  }
} 

void turnLeft(){
  analogWrite(leftPin1, 0);
  analogWrite(leftPin2, 60);
  
  analogWrite(rightPin1, 255);
  analogWrite(rightPin2, 0);
}
void turnRight(){
  analogWrite(rightPin1, 0);
  analogWrite(rightPin2, 60);
  
  analogWrite(leftPin1, 255);
  analogWrite(leftPin2, 0);
}

void rotateRobotRight(){
  // Right Wheel
  analogWrite(rightPin1, 0);
  analogWrite(rightPin2, 170);

  // Left Wheel
  analogWrite(leftPin1, 170);
  analogWrite(leftPin2, 0);
  bool keepTurning = true;
  delay(150);
  
  while(keepTurning){
    sensorCombined = 0;
  
    sensorVals[0] = digitalRead(sensor1); //left left
    sensorVals[1] = digitalRead(sensor2); //left
    sensorVals[2] = digitalRead(sensor3); //middle
    sensorVals[3] = digitalRead(sensor4); //right
    sensorVals[4] = digitalRead(sensor5); //right right
    
    for(int i=0; i<5; i++){
     sensorCombined = sensorVals[i] << (4-i) | sensorCombined;
    }
    switch(sensorCombined){
      case(19):
        moveForward(0, true);
        return;
      case(27):
        moveForward(0, true);
        return;
      case(25):
        moveForward(0, true);
        return;
      default:
        keepTurning = true;
        break;
    }
  }
  
  
}
void rotateRobotLeft(){
  // Right Wheel
  analogWrite(rightPin1, 150);
  analogWrite(rightPin2, 0);

  // Left Wheel
  analogWrite(leftPin1, 0);
  analogWrite(leftPin2, 150);
  bool keepTurning = true;
  delay(150);

  while(keepTurning){
    sensorCombined = 0;
  
    sensorVals[0] = digitalRead(sensor1); //left left
    sensorVals[1] = digitalRead(sensor2); //left
    sensorVals[2] = digitalRead(sensor3); //middle
    sensorVals[3] = digitalRead(sensor4); //right
    sensorVals[4] = digitalRead(sensor5); //right right
    
    for(int i=0; i<5; i++){
     sensorCombined = sensorVals[i] << (4-i) | sensorCombined;
    }
    switch(sensorCombined){
      case(19):
        moveForward(0, true);
        return;
      case(27):
        moveForward(0, true);
        return;
      case(25):
        moveForward(0, true);
        return;
      default:
        keepTurning = true;
        break;
    }
  }
  
  
}
void moveForward(int msDelay, bool slowly){
//  Serial.println("Moving Forward");
  if(slowly){
    analogWrite(rightPin1, 100);
    analogWrite(rightPin2, 0);

    // Left Wheel
    analogWrite(leftPin1, 120);
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
}

void stopRobot(){
  // Right Wheel
  analogWrite(rightPin1, 0);
  analogWrite(rightPin2, RIGHT_WHEEL_SPEED_MAX);

  // Left Wheel
  analogWrite(leftPin1, 0);
  analogWrite(leftPin2, LEFT_WHEEL_SPEED_MAX);

  delay(20);
  
  // Right Wheel
  analogWrite(rightPin1, 0);
  analogWrite(rightPin2, 0);

  // Left Wheel
  analogWrite(leftPin1, 0);
  analogWrite(leftPin2, 0);
}

void turnAround(){
  // Turn right
//  if(!directionToTurnIsLeft){
//    // Right Wheel
//    analogWrite(rightPin1, 0);
//    analogWrite(rightPin2, 200);
//  
//    // Left Wheel
//    analogWrite(leftPin1, 200);
//    analogWrite(leftPin2, 0);
//  }
//  // Turn left
//  else{
    // Right Wheel
    analogWrite(rightPin2, 0);
    analogWrite(rightPin1, 220);
  
    // Left Wheel
    analogWrite(leftPin2, 220);
    analogWrite(leftPin1, 0);
//  }
  delay(600);
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

// Read Response from Server
String readResponse(){
    char buffer[BUFSIZE];
    memset(buffer,0,BUFSIZE);
    client.readBytes(buffer,BUFSIZE);
    String response(buffer);
    return response;
}

// Get next position from server
String getResponseBody(String& response){
    int split = response.indexOf("\r\n\r\n");
    String body = response.substring(split+4, response.length());
    body.trim();
    return body;
}

// Get status code of server fetch
int getStatusCode(String& response){
  String code = response.substring(9,12);
  return code.toInt();
}

// Make HTTP request to server
String fetchNextPosition(int currentPos){
  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(server, 8081)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("POST /api/arrived/e6295a5e HTTP/1.1");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: 10");
    client.println();
    client.println("position=" + String(currentPosition));
    client.println();
    
    String res = readResponse();
    if(getStatusCode(res) == 200){
        String body = getResponseBody(res);
        if(body == "undefined"){
            stopConnection = true;
        }
        // stop the client:
        Serial.println("Stopping Connection");
        client.stop();
        return body;
  }
  // stop the client:
  Serial.println("Stopping Connection");
  client.stop();
  return "err";
  }
}
void sendAndReceiveServerResponse(){
      // don't send server request if not at next position
      if(currentPosition != nextPosition) {routing(); return;}
      
      stopRobot();
      serverResponse = fetchNextPosition(currentPosition);

      if(!stopConnection){
        nextPosition = serverResponse.toInt();
      }
          
      // Get path to next position
      
      // If disconnected from server & stopconnection is true -> stop forever at intersection
      if (!client.connected()) {   
      // do nothing forevermore:
        while (stopConnection){
          Serial.println("I have completed my objective");
          delay(800);
      } 
    }
    routing();
}

void routing(){
  //take the current position and the next position and tries to get the route for it.
  //start with the position 0 and the next position as 1.
  switch(nextPosition){
    case(0):
      switch(currentPosition){
        case(-1):
          currentPosition = 0;
          break;
        case(1):
          if(!facingEast){
              turnAround();
          }
          moveRobotFromPos();
          stopRobot();
          rotateRobotRight();
          facingEast = false;
          currentPosition = 0;
          break;
        case(2):
          if(!facingEast){
            turnAround();
          }
          moveRobotFromPos();
          moveRobotFromPos();
          facingEast = false;
          currentPosition = 0;
          break;
        case(3):
          if(!facingEast){
            turnAround();
          }
          moveRobotFromPos();
          moveRobotFromPos();
          moveRobotFromPos();
          facingEast= false;
          currentPosition = 0;
          break;
        case(4):
          if(!facingEast){
            turnAround();
          }
          moveRobotFromPos();
          facingEast = true;
          currentPosition = 0;
          break;
      }
      
      break;
    // If next position is 1
    case(1):
      switch(currentPosition){
        case(0):
        if(!facingEast){
          turnAround();
        }
          moveRobotFromPos();
          stopRobot();
          rotateRobotLeft();
          facingEast = false;
          currentPosition = 1;
          break;
        case(2):
          if(!facingEast){
             turnAround();
          }
          moveRobotFromPos();
          stopRobot();
          rotateRobotRight();
          facingEast = false;
          currentPosition = 1;
          break;
        case(3):
          if(!facingEast){
            turnAround();
          }
          moveRobotFromPos();
          moveRobotFromPos();
          stopRobot();
          rotateRobotRight();
          facingEast = false;
          currentPosition = 1;
          break;
        case(4):
          if(!facingEast){
            turnAround();
          }
          moveRobotFromPos();
          delay(150);
          moveRobotFromPos();
          stopRobot();
          rotateRobotLeft();
          facingEast = false;
          currentPosition = 1;
      }
      
      break;
      // If next position is 2
    case(2):
      switch(currentPosition){
        case(0):
          if(!facingEast){
            turnAround();
          }
          moveRobotFromPos();
          delay(150);
          moveRobotFromPos();
          facingEast = false;
        break;
        case(1):
          if(!facingEast){
            turnAround();
          }
          moveRobotFromPos();
          stopRobot();
          rotateRobotLeft();
          facingEast = false;
          break;
        case(3):
          if(!facingEast){
            turnAround();
          }
          moveRobotFromPos();
          facingEast = true;
          break;
        case(4):
          if(!facingEast){
            turnAround();
          }
          moveRobotFromPos();
          delay(150);
          moveRobotFromPos();
          delay(150);
          moveRobotFromPos();
          facingEast = false;
        break;
      }
      currentPosition = 2;
      break;
      // If next position is 3
    case(3):
      switch(currentPosition){
        case(0):
          if(!facingEast){
            turnAround();
          }
          moveRobotFromPos();
          delay(150);
          moveRobotFromPos();
          delay(150);
          moveRobotFromPos();
          facingEast = false;
          break;
        case(1):
          if(facingEast){
            turnAround();
          }
          moveRobotFromPos();
          rotateRobotRight();
          facingEast = true;
          break;
        case(2):
          if(facingEast){
            turnAround();
          }
          moveRobotFromPos();
          facingEast = false;
          break;
        case(4):
          if(facingEast){
            turnAround();
          }
          moveRobotFromPos();
          moveRobotFromPos();
          facingEast = true;
          break;
      }
      currentPosition = 3;
      break;
      // If next position is 4
    case(4):
      switch(currentPosition){
        case(0):
          if(facingEast){
            turnAround();
          }
          moveRobotFromPos();
          facingEast = false;
          break;
        case(1):
          if(!facingEast){
            turnAround();
          }
          moveRobotFromPos();
          stopRobot();
          rotateRobotRight();
          moveRobotFromPos();
          facingEast = false;
        break;
        case(2):
          if(!facingEast){
            turnAround();
          }
          moveRobotFromPos();
          moveRobotFromPos();
          moveRobotFromPos();
          facingEast = false;
        break;
        case(3):
          if(facingEast){
            turnAround();
          }
          moveRobotFromPos();
          delay(100);
          moveRobotFromPos();
          facingEast = true;
          break;
      }
      currentPosition = 4;
      break;
      // If next position is 5
    case(5):
      switch(currentPosition){
        // This will make it go to a certain point and then stop near wall
      }
      currentPosition = 5;
      break;
  } 
}

void moveRobotFromPos(){
  bool moveIt = true;
  while(moveIt){
    sensorCombined = 0;
  
    sensorVals[0] = digitalRead(sensor1); //left left
    sensorVals[1] = digitalRead(sensor2); //left
    sensorVals[2] = digitalRead(sensor3); //middle
    sensorVals[3] = digitalRead(sensor4); //right
    sensorVals[4] = digitalRead(sensor5); //right right
    
    for(int i=0; i<5; i++){
     sensorCombined = sensorVals[i] << (4-i) | sensorCombined;
    }
  
    switch(sensorCombined){
      case(15): // 01111 = 15
        turnLeft();
        break;
      case(7): // 00111 = 7
        turnLeft();
        break;
      case(23): // 10111 = 23
        turnLeft();
        break;
      //////////////////////
      case(30): // 11110 = 30
        turnRight();
        break;
      case(28): // 11100 = 28
        turnRight();
        break;
      case(29): // 11101 = 29
        turnRight();
        break;
      /////////////////////////
      case(27): // 11011 = 27
        moveForward();
        break;
      ///////////////////////////
      case(0): //00000
        moveIt = false;
        break;
      case(16): //10000
        moveIt = false;
        break;
      case(1): //00001
        moveIt = false;
        break;
      ///////////////////////////
      default:
        moveForward(0, true);
        break;
    }
  }
  
}
