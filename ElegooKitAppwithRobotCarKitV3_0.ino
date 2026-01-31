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
int carSpeed = 200;  // Speichert DAUERHAFT deine App-Balken Einstellung
String inputString = ""; 

enum Mode { MANUAL, LINE, OBSTACLE };
Mode currentMode = MANUAL;

// --- SCAN VARIABLEN ---
int scanAngle = 90;
int scanDirection = 5; 
unsigned long lastScanTime = 0;

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
  // 1. BLUETOOTH PARSER (Verändert carSpeed nur bei Balken-Bewegung)
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == 's') { currentMode = MANUAL; stopRobot(); inputString = ""; return; }
    if (inChar == '1' && inputString == "") currentMode = LINE;
    if (inChar == '2' && inputString == "") { currentMode = OBSTACLE; head.write(90); }

    inputString += inChar;
    if (inChar == '}') { parseJoystick(inputString); inputString = ""; }
  }

  // 2. MODI AUSFÜHREN
  if (currentMode == LINE) {
    runLineTracking();
  } else if (currentMode == OBSTACLE) {
    runSmoothObstacleAvoidance();
  }
}

// --- OPTIMIERTE SMOOTH OBSTACLE AVOIDANCE (Drosselung nur hier!) ---
void runSmoothObstacleAvoidance() {
  // Wir begrenzen die Geschwindigkeit nur INTERN in dieser Funktion auf max. 160
  // Deine carSpeed am Balken bleibt davon unberührt.
  int safeBaseSpeed = min(carSpeed, 160); 

  // Servo-Scan
  if (millis() - lastScanTime > 40) {
    scanAngle += scanDirection;
    if (scanAngle >= 140 || scanAngle <= 40) scanDirection = -scanDirection;
    head.write(scanAngle);
    lastScanTime = millis();
  }

  int dist = getDistance();

  if (dist > 0 && dist < 20) {
    stopRobot();
    delay(100);
    // Rückwärts mit dem eingestellten carSpeed
    back(); delay(400);
    if (scanAngle > 90) spinRight(); else spinLeft();
    delay(400);
    stopRobot();
  } 
  else if (dist > 0 && dist < 60) {
    // Dynamische Geschwindigkeitsanpassung basierend auf dem sicheren Limit
    int curveSpeed = map(dist, 20, 60, 70, safeBaseSpeed);
    
    if (scanAngle > 105) steer(safeBaseSpeed, curveSpeed);      // Ausweichen rechts
    else if (scanAngle < 75) steer(curveSpeed, safeBaseSpeed);  // Ausweichen links
    else steer(safeBaseSpeed / 2, safeBaseSpeed / 2);           // Bremsen
  } 
  else {
    steer(safeBaseSpeed, safeBaseSpeed); // Fahrt mit sicherem Tempo
  }
}

// --- MOTOR & STEER LOGIK ---
void steer(int sL, int sR) {
  analogWrite(ENA, sL); analogWrite(ENB, sR);
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
}

void forward() { steer(carSpeed, carSpeed); }

void back() {
  analogWrite(ENA, carSpeed); analogWrite(ENB, carSpeed);
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void stopRobot() { digitalWrite(ENA, LOW); digitalWrite(ENB, LOW); }

// --- LINIENVERFOLGUNG (Nutzt vollen carSpeed vom Balken) ---
void runLineTracking() {
  int speedH = carSpeed; 
  int speedL = carSpeed / 4; 
  if (LT_M && !LT_L && !LT_R) forward(); 
  else if (LT_M && LT_R) steer(speedH, speedL); 
  else if (LT_M && LT_L) steer(speedL, speedH); 
  else if (LT_R) spinRight(); 
  else if (LT_L) spinLeft();
  else stopRobot();
}

// --- JOYSTICK PARSER (Aktualisiert carSpeed permanent) ---
void parseJoystick(String data) {
  int d1Index = data.indexOf("\"D1\"");
  if (d1Index != -1) {
    int colonIndex = data.indexOf(":", d1Index);
    if (colonIndex != -1) {
      int i = colonIndex + 1;
      while (i < data.length() && !isDigit(data[i])) i++;
      if (i < data.length()) executeMovement(data[i]);
    }
  }
  int d2Index = data.indexOf("\"D2\"");
  if (d2Index != -1) {
    int colonIndex = data.indexOf(":", d2Index);
    if (colonIndex != -1) {
      String speedVal = "";
      for (int i = colonIndex + 1; i < data.length(); i++) {
        if (isDigit(data[i])) speedVal += data[i];
        else if (data[i] == ',' || data[i] == '}' || data[i] == '\n') break;
      }
      if (speedVal.length() > 0) carSpeed = speedVal.toInt();
    }
  }
}

void executeMovement(char dir) {
  currentMode = MANUAL; 
  switch (dir) {
    case '3': forward(); break;
    case '4': back(); break;
    case '6': steer(carSpeed/3, carSpeed); break;
    case '8': steer(carSpeed, carSpeed/3); break;
    case '9': backRight(); break;
    case '7': backLeft(); break;
    case '1': spinLeft(); break;
    case '2': spinRight(); break;
    case '0': stopRobot(); break;
  }
}

void backLeft() {
  analogWrite(ENA, carSpeed/3); analogWrite(ENB, carSpeed);
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void backRight() {
  analogWrite(ENA, carSpeed); analogWrite(ENB, carSpeed/3);
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void spinLeft() {
  analogWrite(ENA, carSpeed); analogWrite(ENB, carSpeed);
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
}

void spinRight() {
  analogWrite(ENA, carSpeed); analogWrite(ENB, carSpeed);
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

int getDistance() {
  digitalWrite(TRIG, LOW); delayMicroseconds(2);
  digitalWrite(TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  long duration = pulseIn(ECHO, HIGH, 25000);
  if (duration == 0) return 400; 
  return duration * 0.034 / 2;
}

