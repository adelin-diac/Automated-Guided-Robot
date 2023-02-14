#define distSens 33
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(distSens, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly: 
  int d = analogRead(distSens);
  Serial.println(d);
  delay(100);
}
