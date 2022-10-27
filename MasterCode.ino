#include <Keypad.h>
#include <CheapStepper.h>
#include <Wire.h>
#include <arduinio.h>



// code voor master
// projectgroep 15


CheapStepper stepper (28, 30, 32, 34);
// constanten voor motor
const boolean omhoog = true;
const boolean omlaag = false;


// Keypad constanten
const byte ROWS = 4;
const byte COLS = 4;
byte rowPins[ROWS] = {38, 40, 42, 44};
byte colPins[COLS] = {46, 48, 50, 52};


char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', '7'},
  {'4', '5', '7', '7'},
  {'7', '7', '7', '7'},
  {'7', '0', '7', '7'}
};
// alle niet gebruikte toetsen geven output '7'


Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);


const int etages = 6;
int huidigeVerdieping;
int targetVerdieping = 0;
bool antwoord[etages][3];


// functie voor het opvragen van IR sensor input van een bepaalde verdieping
bool checkIr(int verdieping) {
  Wire.beginTransmission(verdieping);
  Wire.requestFrom(verdieping, 3, true);
  for (int id = 0; id < 3; id++) {
    antwoord[verdieping][id] = Wire.read();
  }
  Wire.endTransmission();
  if (antwoord[verdieping][0] == 1) {
    return true;
  }
  else {
    return false;
  }
}


// functie voor het schrijven van de verdieping naar alle slaves
void sendToAll(int huidigeVerdieping) {
  for (int etagePointer = 0; etagePointer < etages; etagePointer++) {
    Wire.beginTransmission(etagePointer);
    Wire.write(huidigeVerdieping);
    Wire.endTransmission();
  }
}


void setup() {
  Serial.begin(9600);
  Wire.begin();
  stepper.setRpm(15);


  // ga naar etage 0
  while (!checkIr(0)) {
    stepper.moveDegrees(omlaag, 10);
  }
  huidigeVerdieping = 0;
  targetVerdieping = 0;
  sendToAll(huidigeVerdieping);


}


int keyPadOutput = 0;


void loop() {
  sendToAll(huidigeVerdieping);
  // lees Keypad input
  char customKey = customKeypad.getKey();
  if (customKey) {
    if (customKey == '7') {
      Serial.println("Error: er is een keypad input die niet verwerkt kan worden");
    }
    else {

      Serial.println(customKey);
      if (customKey == '0') {
        Serial.print("onderweg naar ");
        keyPadOutput = 0;
      }
      else if (customKey == '1') {
        Serial.print("onderweg naar ");
        keyPadOutput = 1;
      }
      else if (customKey == '2') {
        Serial.print("onderweg naar ");
        keyPadOutput = 2;
      }
      else if (customKey == '3') {
        Serial.print("onderweg naar ");
        keyPadOutput = 3;
      }
      else if (customKey == '4') {
        Serial.print("onderweg naar ");
        keyPadOutput = 4;
      }
      else if (customKey == '5') {
        Serial.print("onderweg naar ");
        keyPadOutput = 5;
      }
      targetVerdieping = keyPadOutput;
      Serial.println(keyPadOutput);
    }
  }
  Serial.println(targetVerdieping);
  // vraag en ontvang huidige etages én stuur die naar alle 7-segments
  for (int etagePointer = 0; etagePointer < etages; etagePointer++) {
    Wire.beginTransmission(etagePointer);
    Wire.requestFrom(etagePointer, 3, true);
    for (int id = 0; id < 3; id++) {
      antwoord[etagePointer][id] = Wire.read();
    }
    if (antwoord[etagePointer][0] == true) {
      huidigeVerdieping = etagePointer;
      //Serial.print("verdieping is: ");
      //Serial.println(huidigeVerdieping);
    }
  }
  // resultaten te vinden in antwoorden[etage][x/y/z]


  // ga na welke etages op de knop is gedrukt
  for (int etagePointer = 0; etagePointer < etages; etagePointer++) {
    if (antwoord[etagePointer][1] || antwoord[etagePointer][2]) {
      targetVerdieping = etagePointer;
    }
  }


  // als we er nog niet zijn, dan bewegen
  if (!checkIr(targetVerdieping)) {
    sendToAll(huidigeVerdieping);
    if (targetVerdieping > huidigeVerdieping) {
      stepper.moveDegrees(omhoog, 10);

    }
    else if (targetVerdieping < huidigeVerdieping) {
      stepper.moveDegrees(omlaag, 10);
    }
  }
}
