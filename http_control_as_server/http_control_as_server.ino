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
char ssid[] = "wifissid";
// your network password
char password[] = "wifipassword";

WiFiServer server(80);

void moveForward(int msDelay = 0, bool slowly = false);
void moveBackward(int msDelay);

void setup() {
  Serial.begin(115200);      // initialize serial communication
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
  
  // you're connected now, so print out the status  
  printWifiStatus();

  Serial.println("Starting webserver on port 80");
  server.begin();                           // start the web server on port 80
  Serial.println("Webserver started!");

  pinMode(rightPin1, OUTPUT);
  pinMode(rightPin2, OUTPUT);
  
  pinMode(leftPin1, OUTPUT);
  pinMode(leftPin2, OUTPUT);

  pinMode(statusLED, OUTPUT);
  digitalWrite(statusLED, HIGH);
}

byte controlVal = 1;
bool moving = false;
bool directionRight = false;
bool directionLeft = false;

void loop() {
  
  int i = 0;
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    char buffer[150] = {0};                 // make a buffer to hold incoming data
    while (client.connected()) {            // loop while the client's connected
//      if(controlVal == 0){return;}
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (strlen(buffer) == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            break;
          }
          else {      // if you got a newline, then clear the buffer:
            memset(buffer, 0, 150);
            i = 0;
          }
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          buffer[i++] = c;      // add it to the end of the currentLine
        }
        
        if(endsWith(buffer, "GET /ctrl-rem")){
          controlVal = 0;
        }
        else if (endsWith(buffer, "GET /stop")) {
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
        else if (endsWith(buffer, "GET /fwd")) {
          moveForward(0, false);  
          moving = true;
          directionRight = false;
          directionLeft = false;         
        }
        if(endsWith(buffer, "GET /back")){
          moveBackward(0);
          moving = true;
          directionRight = false;
          directionLeft = false;
        }
        else if(endsWith(buffer, "GET /r")){
          
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
        else if(endsWith(buffer, "GET /l")){
          
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
    client.stop();
    Serial.println("client disonnected");
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

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
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
