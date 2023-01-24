#define rightPin1 34 // Right Wheel pin1 - 2.3
#define rightPin2 38 // Right Wheel pin2 - 2.4

#define leftPin1 39 // Left Wheel pin1 - 2.6
#define leftPin2 40 // Left Wheel pin2 - 2.7

#define RIGHT_WHEEL_SPEED_MAX 175
#define LEFT_WHEEL_SPEED_MAX 200

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

String readResponse();
String getResponseBody(String& response);
int getStatusCode(String& response);

int currentPosition = 0;
int nextPosition;
bool stopConnection = false;


const int maxSensorVal = 750; // Value for breakpoint of wether sensor is activated

bool moveRobot = true;
int sensorVals[5];

void moveForward(int msDelay, bool slowly);
void moveBackward(int msDelay);
void stopRobot();

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



//  // at start take a reading from the sensors to figure out what is white and what is black
//  sensorVals[0] = analogRead(sensor1); //left left
//  sensorVals[1] = analogRead(sensor2); //left
//  sensorVals[2] = analogRead(sensor3); //middle
//  sensorVals[3] = analogRead(sensor4); //right
//  sensorVals[4] = analogRead(sensor5); //right right

}

void loop() {
  moveForward(0, false);

  sensorVals[0] = analogRead(sensor1); //left left
  sensorVals[1] = analogRead(sensor2); //left
  sensorVals[2] = analogRead(sensor3); //middle
  sensorVals[3] = analogRead(sensor4); //right
  sensorVals[4] = analogRead(sensor5); //right right

//  if(sensorVals[0] < 750 && sensorVals[4] < 750){ // Call API
//    Serial.println("stop");
//    stopRobot();
//  }
if(sensorVals[1] < 500 && sensorVals[2] < 500 && sensorVals[3] < 500 || sensorVals[0] < 500 && sensorVals[1] < 500 && sensorVals[2] < 500 || sensorVals[2] < 500 && sensorVals[3] < 500 && sensorVals[4] < 500){
    stopRobot();
   
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
        
        nextPosition = body.toInt();
      // Send nextPosition to the other MSP
      // Other MSP makes robot go to next position
      // Once there, send a response to this MSP with the current position
        
       currentPosition = nextPosition;  // This will be done by communicating to other MSP
      
      
      Serial.print("Next Position = ");
      Serial.println(String(nextPosition));
      Serial.println(res);
//      Serial.println(body);
//      Serial.println(nextPosition);
    delay(1000);
//    } 
  }
  
  // if there are incoming bytes available
  // from the server, read them and print them:
//  while (client.available()) {
//    char c = client.read();
//    Serial.write(c);
//  }

  client.stop();
  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting from server.");
    

    // do nothing forevermore:
    while (stopConnection);
  }

}
  if(sensorVals[2] < 400 && sensorVals[1] > maxSensorVal && sensorVals[3] > maxSensorVal ){
    moveForward(0, false);
    return;
  }
  if(sensorVals[2] > maxSensorVal && sensorVals[1] > maxSensorVal && sensorVals[3] > maxSensorVal&& sensorVals[0] > maxSensorVal && sensorVals[4] > maxSensorVal){
    stopRobot();
    moveBackward(0);
    return;
  }
  if(sensorVals[1] < maxSensorVal && sensorVals[2] < maxSensorVal){
    // NEED TO TURN RIGHT HERE - DECREASE SPEED OF LEFT SLIGHTLY
    analogWrite(rightPin1, 100);
    analogWrite(rightPin2, 0);

    analogWrite(leftPin1, 75);
    analogWrite(leftPin2, 0);
    return;
  }
  if(sensorVals[3] < maxSensorVal && sensorVals[2] < maxSensorVal){
    // NEED TO TURN LEFT HERE - DECREASE SPEED OF RIGHT SLIGHTLY
    analogWrite(leftPin1, 100);
    analogWrite(leftPin2, 0);
    
    analogWrite(rightPin1, 75);
    analogWrite(rightPin2, 0);
    return;
  }

   if(sensorVals[1] < maxSensorVal){
    // NEED TO TURN RIGHT HERE - DECREASE SPEED OF LEFT SLIGHTLY
    analogWrite(rightPin1, 100);
    analogWrite(rightPin2, 0);

    analogWrite(leftPin1, 25);
    analogWrite(leftPin2, 0);
    return;
  }
  if(sensorVals[3] < maxSensorVal){
    // NEED TO TURN LEFT HERE - DECREASE SPEED OF RIGHT SLIGHTLY
    analogWrite(leftPin1, 100);
    analogWrite(leftPin2, 0);
    
    analogWrite(rightPin1, 25);
    analogWrite(rightPin2, 0);
    return;
  }

  if(sensorVals[0] < maxSensorVal){ // TURN RIGHT QUICKLY
    analogWrite(leftPin1, 0);
    analogWrite(leftPin2, 100);
    
    analogWrite(rightPin1, 100);
    analogWrite(rightPin2, 0); 
    return;
  }
  if(sensorVals[4] < maxSensorVal){ // TURN RIGHT QUICKLY
    analogWrite(rightPin1, 0);
    analogWrite(rightPin2, 100); 
    
    analogWrite(leftPin1, 100);
    analogWrite(leftPin2, 0);
    return;
  }

  
 
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
////  Serial.println("");
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
  analogWrite(rightPin2, 0);

  // Left Wheel
  analogWrite(leftPin1, 0);
  analogWrite(leftPin2, 0);
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

int getStatusCode(String& response){
  String code = response.substring(9,12);
  return code.toInt();
}
