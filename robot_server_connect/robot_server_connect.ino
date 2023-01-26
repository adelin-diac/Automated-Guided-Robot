
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
String serverResponse;

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

}

void loop() {        
    String serverResponse = fetchNextPosition(currentPosition);
    if(!stopConnection){
      nextPosition = serverResponse.toInt();
    }
    // compare current position & next position to see route to go on then
    // set currentPosition
    
   currentPosition = nextPosition;  // This will be done by communicating to other MSP
  }
      
      Serial.print("Next Position = ");
      Serial.println(String(nextPosition));
      Serial.println(res);
//      Serial.println(body);
//      Serial.println(nextPosition);
      delay(1000);
  }
 

  if (!client.connected()) {
    // do nothing forevermore:
    while (stopConnection);
    Serial.println("Not connected to server - i will print forever");
    delay(800);
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
        return body;
  }
    // stop the client:
    client.stop();
  return "err";
}
