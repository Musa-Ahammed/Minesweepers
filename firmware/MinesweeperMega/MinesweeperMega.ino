/*
 * Minesweeper Robot | V2
 * Arduino Mega 2560
 * RC differential drive, metal detector, 3 ultrasonic sensors
 * 16x2 I2C LCD, Bluetooth telemetry, LED and buzzer
 * Manual RC control with signal-loss motor stop
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// 1. Pin definitions and settings

LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte RC_T = 2;
const byte RC_S = 3;

const byte LPWM = 5;
const byte RPWM = 6;
const byte LDIR = 22;
const byte RDIR = 23;

const byte TRIG[3] = {30, 32, 34};
const byte ECHO[3] = {31, 33, 35};

const byte LEDP = 40;
const byte BUZZ = 41;
const byte DET = A0;

const int DEADBAND = 30;
const int DET_DELTA = 80;
const int MAX_SPEED = 255;

const unsigned long RC_TIMEOUT = 30000;
const unsigned long SENSOR_INTERVAL = 60;
const unsigned long DISPLAY_INTERVAL = 200;
const unsigned long TELEMETRY_INTERVAL = 200;

int detectorBaseline = 0;
int detectorValue = 0;

int throttle = 0;
int steering = 0;

bool metalDetected = false;

long distanceCM[3] = {-1, -1, -1};

unsigned long lastSensor = 0;
unsigned long lastDisplay = 0;
unsigned long lastTelemetry = 0;

byte sensorIndex = 0;


// 2. RC receiver signal reading

int readRC(byte pin) {

  unsigned long pulse = pulseIn(pin, HIGH, RC_TIMEOUT);

  if (pulse < 900 || pulse > 2100) {
    return 9999;
  }

  int value = map((long)pulse, 1000, 2000, -255, 255);

  value = constrain(value, -255, 255);

  if (abs(value) < DEADBAND) {
    value = 0;
  }

  return value;
}


// 3. Motor speed and direction control

void motor(byte pwmPin, byte dirPin, int speedValue) {

  speedValue = constrain(speedValue, -MAX_SPEED, MAX_SPEED);

  if (speedValue == 0) {
    analogWrite(pwmPin, 0);
    return;
  }

  digitalWrite(dirPin, speedValue > 0 ? HIGH : LOW);
  analogWrite(pwmPin, abs(speedValue));
}


// 4. Motor stop function

void stopMotors() {

  analogWrite(LPWM, 0);
  analogWrite(RPWM, 0);
}


// 5. Ultrasonic distance measurement

long readDistance(byte index) {

  digitalWrite(TRIG[index], LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG[index], HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG[index], LOW);

  unsigned long duration = pulseIn(ECHO[index], HIGH, 25000);

  if (duration == 0) {
    return -1;
  }

  return duration / 58;
}


// 6. Metal detector calibration

void calibrateDetector() {

  long total = 0;

  for (int i = 0; i < 100; i++) {

    total += analogRead(DET);
    delay(5);
  }

  detectorBaseline = total / 100;
}


// 7. Metal detection and alarm

void updateDetector() {

  detectorValue = analogRead(DET);

  metalDetected =
    abs(detectorValue - detectorBaseline) >= DET_DELTA;

  digitalWrite(LEDP, metalDetected ? HIGH : LOW);
  digitalWrite(BUZZ, metalDetected ? HIGH : LOW);
}


// 8. LCD display

void updateLCD() {

  lcd.setCursor(0, 0);

  if (metalDetected) {
    lcd.print("METAL DETECTED  ");
  } else {
    lcd.print("Scanning...     ");
  }

  lcd.setCursor(0, 1);
  lcd.print("ADC:");
  lcd.print(detectorValue);
  lcd.print("        ");
}


// 9. Bluetooth telemetry

void sendTelemetry() {

  Serial1.print("ADC=");
  Serial1.print(detectorValue);

  Serial1.print(",metal=");
  Serial1.print(metalDetected ? 1 : 0);

  Serial1.print(",front_cm=");
  Serial1.print(distanceCM[0]);

  Serial1.print(",left_cm=");
  Serial1.print(distanceCM[1]);

  Serial1.print(",right_cm=");
  Serial1.println(distanceCM[2]);
}


// 10. System initialization

void setup() {

  pinMode(RC_T, INPUT);
  pinMode(RC_S, INPUT);

  pinMode(LPWM, OUTPUT);
  pinMode(RPWM, OUTPUT);

  pinMode(LDIR, OUTPUT);
  pinMode(RDIR, OUTPUT);

  pinMode(LEDP, OUTPUT);
  pinMode(BUZZ, OUTPUT);

  stopMotors();

  digitalWrite(LEDP, LOW);
  digitalWrite(BUZZ, LOW);

  for (byte i = 0; i < 3; i++) {

    pinMode(TRIG[i], OUTPUT);
    pinMode(ECHO[i], INPUT);

    digitalWrite(TRIG[i], LOW);
  }

  Serial.begin(9600);
  Serial1.begin(9600);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Mine Finder");

  lcd.setCursor(0, 1);
  lcd.print("Calibrating...");

  calibrateDetector();

  delay(500);
  lcd.clear();
}


// 11. RC movement control

void updateDrive() {

  throttle = readRC(RC_T);
  steering = readRC(RC_S);

  if (throttle == 9999 || steering == 9999) {

    stopMotors();
    return;
  }

  int leftSpeed = constrain(
    throttle + steering, -MAX_SPEED, MAX_SPEED
  );

  int rightSpeed = constrain(
    throttle - steering, -MAX_SPEED, MAX_SPEED
  );

  motor(LPWM, LDIR, leftSpeed);
  motor(RPWM, RDIR, rightSpeed);
}


// 12. Main program loop

void loop() {

  updateDrive();
  updateDetector();

  unsigned long currentTime = millis();

  if (currentTime - lastSensor >= SENSOR_INTERVAL) {

    distanceCM[sensorIndex] = readDistance(sensorIndex);

    sensorIndex++;

    if (sensorIndex >= 3) {
      sensorIndex = 0;
    }

    lastSensor = millis();
  }

  currentTime = millis();

  if (currentTime - lastDisplay >= DISPLAY_INTERVAL) {

    updateLCD();
    lastDisplay = currentTime;
  }

  if (currentTime - lastTelemetry >= TELEMETRY_INTERVAL) {

    sendTelemetry();
    lastTelemetry = currentTime;
  }
}