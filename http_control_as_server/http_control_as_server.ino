#define rightPin1 34 // Right Wheel pin1 - 2.3
#define rightPin2 38 // Right Wheel pin2 - 2.4

#define leftPin1 39 // Left Wheel pin1 - 2.6
#define leftPin2 40 // Left Wheel pin2 - 2.7

#define RIGHT_WHEEL_SPEED_MAX 220
#define LEFT_WHEEL_SPEED_MAX 255

#define statusLED 30 // 5.5 

#ifndef __CC3200R1M1RGC__
// Do not include SPI for CC3200 LaunchPad
#include <SPI.h>
#endif

#include <WiFi.h>
#define BUFSIZE 512

// your network name also called SSID
char ssid[] = "NETGEAR63";
// your network password
char password[] = "littlecello367";

WiFiClient node_red;

void moveForward(int msDelay = 0, bool slowly = false);
void moveBackward(int msDelay);

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

  pinMode(rightPin1, OUTPUT);
  pinMode(rightPin2, OUTPUT);
  
  pinMode(leftPin1, OUTPUT);
  pinMode(leftPin2, OUTPUT);

  pinMode(statusLED, OUTPUT);
  digitalWrite(statusLED, HIGH);

}

char myLaptopIP[] = "192.168.1.150";
int controlVal = 0;
int direction = 0;

void loop() {
  while(controlVal == 0){
    // read from node-red
//    delay(1000);
    if(node_red.connect(myLaptopIP, 1880)){
      Serial.println("Connected to node-red");
      node_red.println("GET /get-control HTTP/1.1");
      node_red.println();
      delay(1000);
      if(node_red.available()){
        Serial.println("getting res");
        char buffer2[512];
        memset(buffer2,0,512);
        node_red.readBytes(buffer2,512);
        String response(buffer2);
//        Serial.println(response);
        int split = response.indexOf("\r\n\r\n");
        String body = response.substring(split+4, response.length());
        body.trim();
        controlVal = body.toInt();
        Serial.println(controlVal);
      }
    }
    node_red.stop();  
  }
  while(controlVal == 1){
    if(node_red.connect(myLaptopIP, 1880)){
      node_red.println("GET /get-dir HTTP/1.1");
      node_red.println();
//      Serial.println("requested for direction");
//      if(node_red.available()){
//        Serial.println("getting dir");
        char buffer2[512];
        memset(buffer2,0,512);
        node_red.readBytes(buffer2,512);
        String response(buffer2);
//        Serial.println(response);
        int split = response.indexOf("\r\n\r\n");
        String body = response.substring(split+4, response.length());
        body.trim();
        direction = body.toInt();
//      }
    node_red.stop();
    }

    switch(direction){
      case 0:
        // Right Wheel
        analogWrite(rightPin1, 0);
        analogWrite(rightPin2, 0);
      
        // Left Wheel
        analogWrite(leftPin1, 0);
        analogWrite(leftPin2, 0);
        direction = 0;
        break;
      case 1:
        moveForward(0, false);
        direction = 0;
        break;
      case 2:
        moveBackward(0);
        direction = 0;
        break;
      case 3: // turn right
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
        break;
      case 4: // turn left
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
        break;
      default:
        // Right Wheel
        analogWrite(rightPin1, 0);
        analogWrite(rightPin2, 0);
      
        // Left Wheel
        analogWrite(leftPin1, 0);
        analogWrite(leftPin2, 0);
        break;
    }
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

  // Right Wheel
  analogWrite(rightPin1, 0);
  analogWrite(rightPin2, RIGHT_WHEEL_SPEED_MAX);

  // Left Wheel
  analogWrite(leftPin1, 0);
  analogWrite(leftPin2, LEFT_WHEEL_SPEED_MAX);
}
