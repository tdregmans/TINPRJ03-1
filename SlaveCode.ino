#include <Wire.h>
#include <string.h>
const int shortPressTime = 1000;
int lastState = LOW;
int currentState;
unsigned long pressedTime = 0;
unsigned long releasedTime = 0;





// code voor slaves
// versie = 0.5
// versie 0.5 SlaveComms5 hoort bij 0.5 MasterComms5
// constanten voor 7-segment Display
const byte getallen[] = {
  B11111100, //0
  B01100000, //1
  B11011010, //2
  B11110010, //3
  B01100110, //4
  B10110110, //5
  B11111101, //6
};
const int MIJNVERDIEPING = 5;
// voor elke slave anders !!!
int huidigeVerdieping;
// alle pins
#define sevenSegmentData 4
#define sevenSegmentLatch 3
#define sevenSegmentClock 2
#define redLed 6
#define redButton 7
#define whiteLed 8
#define whiteButton 9
#define ledOne 10
#define ledTwo 11
#define IRsensor 12
bool roodAan;
bool witAan;
// functie die wordt aangeroepen als Master iets zendt
void recieveEvent() {
  huidigeVerdieping = Wire.read();
  sevenSegment(huidigeVerdieping);
}
// functie die wordt aangeroepen als Master vraagt om info
void requestEvent() {
  bool antwoord[3];
  antwoord[0] = !digitalRead(IRsensor);
  antwoord[1] = roodAan;
  antwoord[2] = witAan;
  for (int index = 0; index < 3; index++) {
    //Serial.println(antwoord[index]);
    Wire.write(antwoord[index]);
  }
  //Wire.write();
  Serial.println("antwoorden verstuurd");
  // Format: Wire.write("xyz");
  // x = R als rood is ingedrukt
  // x = 0 als rood niet is ingedrukt
  // y = W als wit is ingedrukt
  // y = 0 als wit is niet ingedrukt
  // z = 1 als IR iets detects
  // z = 0 als IR niet iets detects
}
void setup() {
  Serial.begin(9600);
  // code I2C
  Wire.begin(MIJNVERDIEPING);
  Wire.onRequest(requestEvent);
  Wire.onReceive(recieveEvent);
  // pins voor aansturing 7-segment Display
  pinMode(sevenSegmentData, OUTPUT);
  pinMode(sevenSegmentLatch, OUTPUT);
  pinMode(sevenSegmentClock, OUTPUT);
  // pins voor knoppen
  pinMode(redLed, OUTPUT);
  pinMode(redButton, INPUT);
  pinMode(whiteLed, OUTPUT);
  pinMode(whiteButton, INPUT);
  // pins voor leds boven de lift
  pinMode(ledOne, OUTPUT);
  // ledOne is de rode led bovenaan
  pinMode(ledTwo, OUTPUT);
  // ledTwo is de groene led bovenaan
  // pins voor IR-sensor
  pinMode(IRsensor, INPUT);
}
void sevenSegment(int getal) {
  if (getal >= 0 and getal <= 5) {
    // functie voor aansturen van 7-segment display
    // variabele 'getal' gebruiken als input
    // voorbeeld code voor verdieping 5: sevenSegment(5);
    digitalWrite(sevenSegmentLatch, LOW);
    shiftOut(sevenSegmentData, sevenSegmentClock, LSBFIRST, getallen[getal]);
    digitalWrite(sevenSegmentLatch, HIGH);
  }
  Serial.println("SevenSegment geupdate");
}
void loop() {
  sevenSegment(huidigeVerdieping);
  Serial.println(huidigeVerdieping);
  // Dectecteer cabine met IR-sensor
  bool detectedLift = !digitalRead(IRsensor);
  currentState = detectedLift;
  if (lastState == HIGH && currentState == LOW) {
    pressedTime = millis();
  }
  else if (lastState == LOW && currentState == HIGH) {
    releasedTime = millis();
    long pressDuration = releasedTime - pressedTime;
    if (pressDuration > shortPressTime) {
      digitalWrite(redLed, LOW);
      digitalWrite(whiteLed, LOW);
      roodAan = false;
      witAan = false;
      digitalWrite(ledOne, LOW);
      digitalWrite(ledTwo, HIGH);
    }
  }
  //if(MIJNVERDIEPING == huidigeVerdieping && (roodAan || witAan)) {
  if (detectedLift) {
    // Rode led boven lift moet uit en groen moet aan
    digitalWrite(ledOne, LOW);
    digitalWrite(ledTwo, HIGH);
  }
  else {
    // Rode led boven lift moet aan en groen moet uit
    digitalWrite(ledOne, HIGH);
    digitalWrite(ledTwo, LOW);
  }
  // hoe moeten de knoppen gelezen worden:
  if (digitalRead(redButton) == HIGH) {
    // rood is ingedrukt
    digitalWrite(redLed, HIGH);
    roodAan = true;
  }
  if (digitalRead(whiteButton) == HIGH) {
    // wit is ingedrukt
    digitalWrite(whiteLed, HIGH);
    witAan = true;
  }
}
