#include <Stepper.h>

const int STEPS_PER_REV = 200;
const int LEFT_BUTTON = 33;
const int RIGHT_BUTTON = 32;
const int MOTOR_PIN_1 = 14;
const int MOTOR_PIN_2 = 27;
const int MOTOR_PIN_3 = 26;
const int MOTOR_PIN_4 = 25;
bool lastLeftButtonState = HIGH;
bool lastRightButtonState = HIGH;

bool actionTakenLeft = false;
bool actionTakenRight = false;

const unsigned long debounceDelay = 50; 
unsigned long lastDebounceTimeLeft = 0; 
unsigned long lastDebounceTimeRight = 0; 

Stepper stepper_NEMA17(STEPS_PER_REV, MOTOR_PIN_1, MOTOR_PIN_2, MOTOR_PIN_3, MOTOR_PIN_4);

void disableMotor() {
  digitalWrite(MOTOR_PIN_1, LOW);
  digitalWrite(MOTOR_PIN_2, LOW);
  digitalWrite(MOTOR_PIN_3, LOW);
  digitalWrite(MOTOR_PIN_4, LOW);
}

void setup() {
  pinMode(LEFT_BUTTON, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON, INPUT_PULLUP);
  pinMode(MOTOR_PIN_1, OUTPUT);
  pinMode(MOTOR_PIN_2, OUTPUT);
  pinMode(MOTOR_PIN_3, OUTPUT);
  pinMode(MOTOR_PIN_4, OUTPUT);
}

void loop() {
  bool currentLeftButtonState = digitalRead(LEFT_BUTTON);
  bool currentRightButtonState = digitalRead(RIGHT_BUTTON);
  unsigned long currentTime = millis();

  if (currentLeftButtonState != lastLeftButtonState) {
    lastDebounceTimeLeft = currentTime;
    actionTakenLeft = false;
  }
  if (!actionTakenLeft && (currentTime - lastDebounceTimeLeft) > debounceDelay) {
    if (currentLeftButtonState == LOW) {
      stepper_NEMA17.setSpeed(300);
      stepper_NEMA17.step(25);
      Serial.println("Left Button Pressed - Moving Right");
      actionTakenLeft = true;
      delay(100);
      disableMotor();
    }
  }


  if (currentRightButtonState != lastRightButtonState) {
    lastDebounceTimeRight = currentTime;
    actionTakenRight = false;
  }
  if (!actionTakenRight && (currentTime - lastDebounceTimeRight) > debounceDelay) {
    if (currentRightButtonState == LOW) {
      stepper_NEMA17.setSpeed(300);
      stepper_NEMA17.step(-25);
      Serial.println("Right Button Pressed - Moving Left");
      actionTakenRight = true;
      delay(100);
      disableMotor();
    }
  }

  lastLeftButtonState = currentLeftButtonState;
  lastRightButtonState = currentRightButtonState;
}
