#define rightPin1 34 // Right Wheel pin1 - 2.3
#define rightPin2 38 // Right Wheel pin2 - 2.4

#define leftPin1 39 // Left Wheel pin1 - 2.6
#define leftPin2 40 // Left Wheel pin2 - 2.7

#define distSens 12 // Distance sensor - 5.1

#define RIGHT_WHEEL_SPEED_MAX 255
#define LEFT_WHEEL_SPEED_MAX 255

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
#define statusLED 30 // 5.5 

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
WiFiClient node_red;
WiFiServer controlServer(80);

int currentPosition = -1;
int nextPosition = 0;
bool facingEast = true;
int controlVal = 0;

bool stopConnection = false;
String serverResponse;

byte sensorCombined;
byte sensorVals[5];
const int maxSensorVal = 600; // Value for breakpoint of wether sensor is activated

bool moveRobot = true;

// Functions declarations
  // MOVEMENT
void moveForward(int msDelay = 0, bool slowly = false);
void moveBackward(int msDelay = 0);
void stopRobot();
void moveRobotFromPos();
void robotRotation(int motorLeft, int motorRight);
void centerRobotOnLine();

void turnAround();
int distance();
void leaveLine();

// others
boolean endsWith(char* inString, const char* compString);

  // INTERNET STUFF
String readResponse();
String getResponseBody(String& response);
int getStatusCode(String& response);
String fetchNextPosition(int currentPos);
void sendAndReceiveServerResponse();
void routing();

void checkControlVal();
void remoteControl();

void setup() {
    //Initialize serial and wait for port to open:
     Serial.begin(19200);

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

  pinMode(distSens, INPUT);
  pinMode(statusLED, OUTPUT);
  bool vals[] = {HIGH, LOW};
  
  for(int i = 0; i<11; i++){
    digitalWrite(statusLED, vals[i % 2]); 
    delay(100);
  } 
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
  analogWrite(rightPin2, 190);

  // Left Wheel
  analogWrite(leftPin1, 190);
  analogWrite(leftPin2, 0);
  bool keepTurning = true;
  delay(170);
  
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
  analogWrite(rightPin1, 190);
  analogWrite(rightPin2, 0);

  // Left Wheel
  analogWrite(leftPin1, 0);
  analogWrite(leftPin2, 190);
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
    analogWrite(rightPin1, 120);
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
//  Serial.println("Moving Backward");

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
    // Right Wheel
    analogWrite(rightPin2, 0);
    analogWrite(rightPin1, 220);
  
    // Left Wheel
    analogWrite(leftPin2, 220);
    analogWrite(leftPin1, 0);
  delay(600);
}
int distance() {
  float volts = analogRead(distSens) * 0.0048828125; // value from sensor * (5/1024)
  int distance = 13 * pow(volts, -1);
  return distance;
  
}
void leaveLine(){
  int distanceVal = distance();
  moveForward(0, false);
  
  while(true){
    Serial.println(distanceVal);
    if(distanceVal < 4){
      stopRobot();
      
      Serial.println("Starting webserver on port 80");
      controlServer.begin();                           // start the web server on port 80
      Serial.println("Webserver started!");
      
      while(true){
        remoteControl();
      }
    }
    distanceVal = distance();
  }
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
void centerRobotOnLine(){
//  Serial.println("centering robot");
    bool rotate = true;
    while(rotate){
      sensorCombined = 0;
    
      sensorVals[0] = digitalRead(sensor1); //left left
      sensorVals[1] = digitalRead(sensor2); //left
      sensorVals[2] = digitalRead(sensor3); //middle
      sensorVals[3] = digitalRead(sensor4); //right
      sensorVals[4] = digitalRead(sensor5); //right right
  
      for(int i=0; i<5; i++){
       sensorCombined = sensorVals[i] << (4-i) | sensorCombined;
      }
//    Serial.println(sensorCombined);
      switch(sensorCombined){
        case 27: // 11011
//          moveBackward(100);
          robotRotation(0,0);
          rotate = false;
          break;
        case 23: // 10111
          robotRotation(0, 100);
          break;
        case 29: //11101
          robotRotation(100, 0);
          break;
        case 15: //01111
          robotRotation(0, 100);
          break;
        case 30: //11110
          robotRotation(100, 0);
          break;
        case 7: //00111
          robotRotation(0, 100);
          break;
        case 28: //11100
          robotRotation(100, 0);
          break;
        default:
           // Right Wheel
          analogWrite(rightPin1, 60);
          analogWrite(rightPin2, 0);
          
          // Left Wheel
          analogWrite(leftPin1, 60);
          analogWrite(leftPin2, 0);
          break;
      }
    }
}

void robotRotation(int motorLeft, int motorRight){
  if(motorLeft > 0 && motorRight > 0) return;
  // Right Wheel
  analogWrite(rightPin1, motorRight);
  analogWrite(rightPin2, motorLeft);

  // Left Wheel
  analogWrite(leftPin1, motorLeft);
  analogWrite(leftPin2, motorRight);
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

char myLaptopIP[] = "192.168.1.150";

void checkControlVal(){
  if(node_red.connect(myLaptopIP, 1880)){
      Serial.println("Connected to node-red");
      node_red.println("GET /get-control HTTP/1.1");
      node_red.println();
//      if(node_red.available()){
      Serial.println("getting res");
      char buffer2[512];
      memset(buffer2,0,512);
      node_red.readBytes(buffer2,512);
      String response(buffer2);
      int split = response.indexOf("\r\n\r\n");
      String body = response.substring(split+4, response.length());
      body.trim();
      controlVal = body.toInt();
//      }
    }
    node_red.stop();
}

bool moving = false;
bool directionRight = false;
bool directionLeft = false;

void remoteControl(){
  checkControlVal();
//  Serial.println(controlVal);
  
  while(controlVal == 1){
//    centerRobotOnLine();
    int i = 0;
    WiFiClient clientConnected = controlServer.available();   // listen for incoming clients

    if (clientConnected) {                             // if you get a client,
      char serverBuffer[150] = {0};                 // make a buffer to hold incoming data
      while (clientConnected.connected()) {            // loop while the client's connected
  //      if(controlVal == 0){return;}
        if (clientConnected.available()) {             // if there's bytes to read from the client,
          char c = clientConnected.read();             // read a byte, then
          Serial.write(c);                    // print it out the serial monitor
          if (c == '\n') {                    // if the byte is a newline character
  
            // if the current line is blank, you got two newline characters in a row.
            // that's the end of the client HTTP request, so send a response:
            if (strlen(serverBuffer) == 0) {
              // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
              // and a content-type so the client knows what's coming, then a blank line:
              clientConnected.println("HTTP/1.1 200 OK");
              clientConnected.println("Content-type:text/html");
              clientConnected.println();
  
              break;
            }
            else {      // if you got a newline, then clear the buffer:
              memset(serverBuffer, 0, 150);
              i = 0;
            }
          }
          else if (c != '\r') {    // if you got anything else but a carriage return character,
            serverBuffer[i++] = c;      // add it to the end of the currentLine
          }
          
          if(endsWith(serverBuffer, "GET /ctrl-rem")){
            controlVal = 0;
          }
          else if (endsWith(serverBuffer, "GET /stop")) {
            // Right Wheel
            analogWrite(rightPin1, 0);
            analogWrite(rightPin2, 0);
        
            // Left Wheel
            analogWrite(leftPin1, 0);
            analogWrite(leftPin2, 0); 
            moving = false;
            directionRight = false;
            directionLeft = false;
          }
          else if (endsWith(serverBuffer, "GET /fwd")) {
            moveForward(0, false);  
            moving = true;
            directionRight = false;
            directionLeft = false;         
          }
          if(endsWith(serverBuffer, "GET /back")){
            moveBackward(0);
            moving = true;
            directionRight = false;
            directionLeft = false;
          }
          else if(endsWith(serverBuffer, "GET /r")){
            
            if(moving){
              if(directionLeft){
                // Left Wheel
                analogWrite(leftPin1, 255);
                analogWrite(leftPin2, 0);
              }
              // Right Wheel
              analogWrite(rightPin1, 100);
              analogWrite(rightPin2, 0);
            }else{
               // Right Wheel
              analogWrite(rightPin1, 0);
              analogWrite(rightPin2, 0);
            
              // Left Wheel
              analogWrite(leftPin1, 0);
              analogWrite(leftPin2, 0);
              // Right Wheel
              analogWrite(rightPin1, 0);
              analogWrite(rightPin2, 255);
            
              // Left Wheel
              analogWrite(leftPin1, 255);
              analogWrite(leftPin2, 0);
              delay(200);
              // Right Wheel
              analogWrite(rightPin1, 0);
              analogWrite(rightPin2, 0);
            
              // Left Wheel
              analogWrite(leftPin1, 0);
              analogWrite(leftPin2, 0);
   
              directionLeft = false;
              directionRight = true;
            } 
           
          }
          else if(endsWith(serverBuffer, "GET /l")){
            
            if(moving){
              if(directionRight){
                // Right Wheel
                analogWrite(rightPin1, 255);
                analogWrite(rightPin2, 0);
              }
              // Left Wheel
              analogWrite(leftPin1, 100);
              analogWrite(leftPin2, 0);
            }else{
              // Right Wheel
              analogWrite(rightPin1, 0);
              analogWrite(rightPin2, 0);
            
              // Left Wheel
              analogWrite(leftPin1, 0);
              analogWrite(leftPin2, 0);
              // Right Wheel
              analogWrite(rightPin1, 255);
              analogWrite(rightPin2, 0);
            
              // Left Wheel
              analogWrite(leftPin1, 0);
              analogWrite(leftPin2, 255);
              delay(200);
              // Right Wheel
              analogWrite(rightPin1, 0);
              analogWrite(rightPin2, 0);
            
              // Left Wheel
              analogWrite(leftPin1, 0);
              analogWrite(leftPin2, 0);
  
              directionLeft = true;
              directionRight = false;
            }
          }
        }
      }
      // close the connection:
      clientConnected.stop();
      Serial.println("client disonnected");
    }
  }
}

//a way to check if one array ends with another array
boolean endsWith(char* inString, const char* compString) {
  int compLength = strlen(compString);
  int strLength = strlen(inString);
  
  //compare the last "compLength" values of the inString
  int i;
  for (i = 0; i < compLength; i++) {
    char a = inString[(strLength - 1) - i];
    char b = compString[(compLength - 1) - i];
    if (a != b) {
      return false;
    }
  }
  return true;
}

void routing(){
  //take the current position and the next position and tries to get the route for it.
  //start with the position 0 and the next position as 1.
  switch(nextPosition){
    case(0): //next position = 0
      switch(currentPosition){
        case(-1):
          currentPosition = 0;
          break;
        case(1):
          if(!facingEast){
              turnAround();
          }
          moveRobotFromPos();
          delay(150);
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
          delay(150);
          facingEast = false;
          currentPosition = 0;
          break;
        case(3):
          if(!facingEast){
            turnAround();
          }
          moveRobotFromPos();
          delay(150);
          moveRobotFromPos();
          delay(150);
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
          delay(150);
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
          delay(150);
          stopRobot();
          rotateRobotLeft();
          facingEast = false;
          currentPosition = 1;
          break;
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
          facingEast = false;
          currentPosition = 2;
        break;
        case(1):
          if(!facingEast){
            turnAround();
          }
          moveRobotFromPos();
          stopRobot();
          rotateRobotLeft();
          facingEast = false;
          currentPosition = 2;
          break;
        case(3):
          if(!facingEast){
            turnAround();
          }
          facingEast = true;
          currentPosition = 2;
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
          currentPosition = 2;
        break;
      }
      
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
          facingEast = false;
          currentPosition = 3;
          break;
        case(1):
          if(facingEast){
            turnAround();
          }
          moveRobotFromPos();
          delay(150);
          stopRobot();
          rotateRobotRight();
          facingEast = true;
          currentPosition = 3;
          break;
        case(2):
          if(facingEast){
            turnAround();
          }
          facingEast = false;
          currentPosition = 3;
          break;
        case(4):
          if(facingEast){
            turnAround();
          }
          moveRobotFromPos();
          delay(150);
          facingEast = true;
          currentPosition = 3;
          break;
      }
      break;
      // If next position is 4
    case(4):
      switch(currentPosition){
        case(0):
          if(facingEast){
            turnAround();
          }
          facingEast = false;
          currentPosition = 4;
          break;
        case(1):
          if(!facingEast){
            turnAround();
          }
          moveRobotFromPos();
          delay(150);
          stopRobot();
          rotateRobotRight();
          moveRobotFromPos();
          delay(150);
          facingEast = false;
          currentPosition = 4;
          break;
        case(2):
          if(!facingEast){
            turnAround();
          }
          moveRobotFromPos();
          delay(150);
          moveRobotFromPos();
          delay(150);
          moveRobotFromPos();
          facingEast = false;
          currentPosition = 4;
          break;
        case(3):
          if(facingEast){
            turnAround();
          }
          moveRobotFromPos();
          delay(100);
          moveRobotFromPos();
          facingEast = true;
          currentPosition = 4;
          break;
      }
      break;
      // If next position is 5
    case(5):
      switch(currentPosition){
        // This will make it go to a certain point and then stop near wall
        case(0):
          if(!facingEast){
            turnAround();
          }
          moveRobotFromPos();
          delay(150);
          rotateRobotLeft();
          moveRobotFromPos();
          delay(150);
          moveRobotFromPos();
          delay(150);
          centerRobotOnLine();
          currentPosition = 5;
          leaveLine();
          break;
        case(1):
          if(facingEast){
            turnAround();
          }
          delay(150);
          moveRobotFromPos();
          delay(150);
          centerRobotOnLine();
          currentPosition = 5;
          leaveLine();
          break;
        case(2):
          if(!facingEast){
            turnAround();
          }
          moveRobotFromPos();
          delay(150);
          rotateRobotRight();
          moveRobotFromPos();
          delay(150);
          moveRobotFromPos();
          delay(150);
          centerRobotOnLine();
          currentPosition = 5;
          leaveLine();
          break;
        case(3):
          if(facingEast){
            turnAround();
          }
          moveRobotFromPos();
          delay(150);
          stopRobot();
          delay(150);
          rotateRobotRight();
          delay(100);
          centerRobotOnLine();
          currentPosition = 5;
          leaveLine();
        break;
        case(4):
          if(facingEast){
            turnAround();
          }
          moveRobotFromPos();
          delay(150);
          stopRobot();
          delay(150);
          rotateRobotLeft();
          delay(100);
          centerRobotOnLine();
          currentPosition = 5;
          leaveLine();
          break;
      }
      break;
  } 
}
