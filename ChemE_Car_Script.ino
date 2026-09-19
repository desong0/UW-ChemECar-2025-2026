#include <Servo.h>

Servo servoKI;
Servo servoHCl;

// Component pins
#define Photosensor_PIN A3
#define Relay_PIN 15
#define Rear_Button_PIN 2
#define Servo1_PIN 7
#define Servo2_PIN 9

//global variables
volatile bool buttonFlag = false;
unsigned long lastMillis = 0;
unsigned long lastMillisPhoto = 0;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 8000;
bool relayON = false;
bool isRunning = false;
double Voltage = 0;

void setup() {
  Serial.begin(9600);

  //reset servo motors to 0 postion
  servoKI.attach(Servo1_PIN);
  servoKI.write(0);
  servoHCl.attach(Servo2_PIN);
  servoHCl.write(0);
  delay(1000);

  //initialize pins
  pinMode(Photosensor_PIN, INPUT);
  pinMode(Relay_PIN, OUTPUT);
  pinMode(Rear_Button_PIN, INPUT_PULLUP); //Set button as input 

  attachInterrupt(digitalPinToInterrupt(Rear_Button_PIN), buttonPressed, FALLING);
  digitalWrite(Relay_PIN, LOW);
}

void loop() {
  if (buttonFlag) {
    buttonFlag = false;

    if (!isRunning && millis() - lastDebounceTime > debounceDelay) { //prevents multiple runs due to contact bounce
      lastDebounceTime = millis();
      isRunning = true;
      Serial.println("Button pressed");
      digitalWrite(Relay_PIN, LOW); //Ensure relay is off prior to start
      moveServos();
      digitalWrite(Relay_PIN, HIGH); //Turn relay on to flow power to motor
      isRunning = false;
    }
  }

  //Get photosensor voltage mesurement every second
  if (millis() - lastMillisPhoto >= 1000) { 
    lastMillisPhoto = millis();
    Voltage = getVoltage();

    Serial.print("Voltage: ");
    Serial.println(Voltage);

    if(limitReached()) { //If limit reached then turn off relay power
      digitalWrite(Relay_PIN, LOW);
    }
  }

}

void buttonPressed() {
  buttonFlag = true;
}

bool limitReached() {
  float activationVoltage = 0.1; //Volts
  if (getVoltage() <= activationVoltage) {
    Serial.println("Limit breached");
    return true;
  }
  else {
    return false;
  }
}

void normalizeVoltage() {
  // Make method to normalize voltage to ambient conditions. Will serve as baseline for determing if reaction has occurred.
}

double getVoltage() {
  int rawValue = analogRead(Photosensor_PIN);
  float voltage = rawValue * (5.0 / 1023.0);
  return voltage;
}

void moveServos() {
  servoKI.attach(Servo1_PIN);
  servoHCl.attach(Servo2_PIN);
      
  servoHCl.write(175);
  delay(1300);
  servoHCl.write(0);
  delay(1000);
  servoKI.write(175);
  delay(1300);
  servoKI.write(0);
  delay(500);

  servoKI.detach();
  servoHCl.detach();

}

void relayONOFFTime(int interval){
  if (millis() - lastMillis >= 5000) {
    lastMillis = millis();
    relayON = !relayON;
    digitalWrite(Relay_PIN, relayON ? HIGH : LOW);
  }
}