#include <Servo.h>

// --- PIN DEFINITIONEN ---
#define ENA 5
#define ENB 6
#define IN1 7
#define IN2 8
#define IN3 9
#define IN4 11
#define SERVO_PIN 3
#define TRIG A5
#define ECHO A4

// Line Tracking Sensoren
#define LT_L !digitalRead(2)
#define LT_M !digitalRead(4)
#define LT_R !digitalRead(10)

Servo head;
int carSpeed = 230; // Wird jetzt live durch App-Balken (D2) überschrieben
String inputString = ""; 

enum Mode { MANUAL, LINE, OBSTACLE };
Mode currentMode = MANUAL;

void setup() {
  Serial.begin(9600);
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);
  pinMode(2, INPUT); pinMode(4, INPUT); pinMode(10, INPUT);
  pinMode(TRIG, OUTPUT); pinMode(ECHO, INPUT);
  
  head.attach(SERVO_PIN);
  head.write(90); 
  stopRobot();
}

void loop() {
  // 1. BLUETOOTH DATEN VERARBEITEN
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    
    // Not-Aus bei 's'
    if (inChar == 's') { 
      currentMode = MANUAL; 
      stopRobot(); 
      inputString = ""; 
      return; 
    }
    
    // Modus-Wahl (falls App '1' oder '2' separat schickt)
    if (inChar == '1' && inputString == "") currentMode = LINE;
    if (inChar == '2' && inputString == "") currentMode = OBSTACLE;

    inputString += inChar;
    if (inChar == '}') {
      parseJoystick(inputString);
      inputString = "";
    }
  }

  // 2. AKTIVEN MODUS AUSFÜHREN
  if (currentMode == LINE) {
    runLineTracking();
  } else if (currentMode == OBSTACLE) {
    runObstacleAvoidance();
  }
}

// --- PARSER FÜR RICHTUNG (D1) UND GESCHWINDIGKEIT (D2) ---
void parseJoystick(String data) {
  // D1 auslesen (Richtung)
  int d1Index = data.indexOf("\"D1\"");
  if (d1Index != -1) {
    int colonIndex = data.indexOf(":", d1Index);
    if (colonIndex != -1) {
      int i = colonIndex + 1;
      while (i < data.length() && !isDigit(data[i])) i++;
      if (i < data.length()) executeMovement(data[i]);
    }
  }

  // D2 auslesen (Speed-Balken) - NEU!
  int d2Index = data.indexOf("\"D2\"");
  if (d2Index != -1) {
    int colonIndex = data.indexOf(":", d2Index);
    if (colonIndex != -1) {
      String speedVal = "";
      for (int i = colonIndex + 1; i < data.length(); i++) {
        if (isDigit(data[i])) speedVal += data[i];
        else if (data[i] == ',' || data[i] == '}' || data[i] == '\n') break;
      }
      if (speedVal.length() > 0) {
        carSpeed = speedVal.toInt(); // Übernimmt 0-255 von der App
      }
    }
  }
}

void executeMovement(char dir) {
  currentMode = MANUAL; 
  switch (dir) {
    case '3': forward(); break;         // OBEN
    case '4': back();    break;         // UNTEN
    case '6': forwardLeft(); break;     // OBEN LINKS
    case '8': forwardRight(); break;    // OBEN RECHTS
    case '9': backRight(); break;       // UNTEN RECHTS
    case '7': backLeft(); break;        // UNTEN LINKS
    case '1': spinLeft(); break;        // LINKS DREHEN
    case '2': spinRight(); break;       // RECHTS DREHEN
    case '0': stopRobot(); break;
  }
}

// --- MOTOR FUNKTIONEN ---
void forward() {
  analogWrite(ENA, carSpeed); analogWrite(ENB, carSpeed);
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
}
void back() {
  analogWrite(ENA, carSpeed); analogWrite(ENB, carSpeed);
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}
void forwardLeft() {
  analogWrite(ENA, carSpeed/3); analogWrite(ENB, carSpeed);
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
}
void forwardRight() {
  analogWrite(ENA, carSpeed); analogWrite(ENB, carSpeed/3);
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
}
void backLeft() {
  analogWrite(ENA, carSpeed/3); analogWrite(ENB, carSpeed);
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}
void backRight() {
  analogWrite(ENA, carSpeed); analogWrite(ENB, carSpeed/3);
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}
void spinLeft() {
  analogWrite(ENA, carSpeed); analogWrite(ENB, carSpeed);
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
}
void spinRight() {
  analogWrite(ENA, carSpeed); analogWrite(ENB, carSpeed);
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}
void stopRobot() {
  digitalWrite(ENA, LOW); digitalWrite(ENB, LOW);
}

// --- SMOOTHE LINIENVERFOLGUNG ---
void runLineTracking() {
  int speedH = carSpeed; 
  int speedL = carSpeed / 4; // Dynamisches Lenkverhältnis

  if (LT_M && !LT_L && !LT_R) forward(); 
  else if (LT_M && LT_R) steer(speedH, speedL); 
  else if (LT_M && LT_L) steer(speedL, speedH); 
  else if (LT_R) spinRight(); 
  else if (LT_L) spinLeft();
  else stopRobot();
}

void steer(int sL, int sR) {
  analogWrite(ENA, sL); analogWrite(ENB, sR);
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
}

// --- HINDERNISVERMEIDUNG ---
void runObstacleAvoidance() {
  head.write(90);
  digitalWrite(TRIG, LOW); delayMicroseconds(2);
  digitalWrite(TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  long duration = pulseIn(ECHO, HIGH, 30000);
  int dist = duration * 0.034 / 2;
  
  if (dist > 0 && dist < 25) {
    stopRobot();
    head.write(160); delay(400); 
    head.write(20); delay(400);
    head.write(90);
    spinRight(); delay(600); stopRobot();
  } else {
    forward();
  }
}
