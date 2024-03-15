#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <Stepper.h>

Adafruit_ADS1115 ads(0x48);

const int stepsPerRevolution = 200;
const int motorSpeed = 500;

const int LEFT_BUTTON = 33;
const int RIGHT_BUTTON = 32;
const int MOTOR_PIN_1 = 14;
const int MOTOR_PIN_2 = 27;
const int MOTOR_PIN_3 = 26;
const int MOTOR_PIN_4 = 25;

Stepper stepper(stepsPerRevolution, MOTOR_PIN_1, MOTOR_PIN_2, MOTOR_PIN_3, MOTOR_PIN_4);

bool lastLeftButtonState = HIGH;
bool lastRightButtonState = HIGH;
bool actionTakenLeft = false;
bool actionTakenRight = false;

const unsigned long debounceDelay = 50; 
unsigned long lastDebounceTimeLeft = 0; 
unsigned long lastDebounceTimeRight = 0;

void setup() {
  Serial.begin(115200);
  ads.begin();
  pinMode(LEFT_BUTTON, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON, INPUT_PULLUP);
  stepper.setSpeed(motorSpeed);
}

void disableMotor() {
  digitalWrite(MOTOR_PIN_1, LOW);
  digitalWrite(MOTOR_PIN_2, LOW);
  digitalWrite(MOTOR_PIN_3, LOW);
  digitalWrite(MOTOR_PIN_4, LOW);
}

void loop() {
  static int targetFrontBrakePercentage = 0;
  static int targetRearBrakePercentage = 0;
  static bool targetReached = false;
  
  bool currentLeftButtonState = digitalRead(LEFT_BUTTON);
  bool currentRightButtonState = digitalRead(RIGHT_BUTTON);
  unsigned long currentTime = millis();

  int16_t pot1 = ads.readADC_SingleEnded(0);
  int16_t pot2 = ads.readADC_SingleEnded(1);
  int total = pot1 + pot2;
  int currentFrontBrakePercentage = (pot1 * 100) / total;
  int currentRearBrakePercentage = 100 - currentFrontBrakePercentage;

  if (currentLeftButtonState != lastLeftButtonState) {
    lastDebounceTimeLeft = currentTime;
  }
  if (!actionTakenLeft && (currentTime - lastDebounceTimeLeft) > debounceDelay && currentLeftButtonState == LOW && !targetReached) {
    targetFrontBrakePercentage = currentFrontBrakePercentage + 10;
    targetRearBrakePercentage = currentRearBrakePercentage - 10;
    Serial.print("Frana fata: ");
    Serial.print(currentFrontBrakePercentage);
    Serial.print("%, Frana spate: ");
    Serial.print(currentRearBrakePercentage);
    Serial.print("%, Target frana fata: ");
    Serial.print(targetFrontBrakePercentage);
    Serial.print("%, Target frana spate: ");
    Serial.println(targetRearBrakePercentage);
    while (currentFrontBrakePercentage < targetFrontBrakePercentage) {
      stepper.step(1);
      pot1 = ads.readADC_SingleEnded(0);
      pot2 = ads.readADC_SingleEnded(1);
      total = pot1 + pot2;
      currentFrontBrakePercentage = (pot1 * 100) / total;
      delay(1);
    }
    Serial.println("Target reached: " + String(currentFrontBrakePercentage) + "% front brake");
    targetReached = true;
    actionTakenLeft = true;
    disableMotor();
  }

  if (currentRightButtonState != lastRightButtonState) {
    lastDebounceTimeRight = currentTime;
  }
  if (!actionTakenRight && (currentTime - lastDebounceTimeRight) > debounceDelay && currentRightButtonState == LOW && !targetReached) {
    targetFrontBrakePercentage = currentFrontBrakePercentage - 10;
    targetRearBrakePercentage = currentRearBrakePercentage + 10;
    Serial.print("Frana fata: ");
    Serial.print(currentFrontBrakePercentage);
    Serial.print("%, Frana spate: ");
    Serial.print(currentRearBrakePercentage);
    Serial.print("%, Target frana fata: ");
    Serial.print(targetFrontBrakePercentage);
    Serial.print("%, Target frana spate: ");
    Serial.println(targetRearBrakePercentage);
    while (currentFrontBrakePercentage > targetFrontBrakePercentage) {
      stepper.step(-1);
      pot1 = ads.readADC_SingleEnded(0);
      pot2 = ads.readADC_SingleEnded(1);
      total = pot1 + pot2;
      currentFrontBrakePercentage = (pot1 * 100) / total;
      delay(1);
    }
    Serial.println("Target reached: " + String(currentFrontBrakePercentage) + "% front brake");
    targetReached = true;
    actionTakenRight = true;
    disableMotor();
  }

  if (currentLeftButtonState == HIGH) {
    actionTakenLeft = false;
  }
  if (currentRightButtonState == HIGH) {
    actionTakenRight = false;
  }

  if (currentLeftButtonState == HIGH && currentRightButtonState == HIGH) {
    targetReached = false;
  }

  lastLeftButtonState = currentLeftButtonState;
  lastRightButtonState = currentRightButtonState;
}
