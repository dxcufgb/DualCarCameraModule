#include <EasyButton.h>

#define REVERSE_SIGNAL_IN A1
#define CAMERA_SWITCH A0
#define REVERSE_SIGNAL_OUT A2
#define INPUT_BUTTON_1 A3
#define INPUT_BUTTON_2 A4
#define REVERSE_LED A5
#define BUTTON1_LED 3
#define BUTTON2_LED 2

EasyButton button1(INPUT_BUTTON_1);
EasyButton button2(INPUT_BUTTON_2);

int button1_state;
int button2_state;
int reverse_signal;
int cameraSwitch;

unsigned long pressTime;
int duration;
bool onUntilPressed = false;
//0 = off, 1 = backward, 2 = forward
int direction = 0;
int longPressDuration = 1500;
int viewDuration = 5;
int backwardOverride = 0;
unsigned long backwardReleaseTime;
int backwardReleaseDelay = 3000;
int buttonDebounce = 50;
unsigned long lastButtonRead;

void setup() {
  pinMode(REVERSE_SIGNAL_IN, INPUT);
  pinMode(CAMERA_SWITCH, OUTPUT);
  pinMode(REVERSE_SIGNAL_OUT, OUTPUT);
  pinMode(REVERSE_LED, OUTPUT);
  pinMode(BUTTON1_LED, OUTPUT);
  pinMode(BUTTON2_LED, OUTPUT);
  digitalWrite(CAMERA_SWITCH, LOW);
  digitalWrite(REVERSE_SIGNAL_OUT, LOW);
  digitalWrite(REVERSE_LED, LOW);
  digitalWrite(BUTTON1_LED, LOW);
  digitalWrite(BUTTON2_LED, LOW);
  button1.begin();
  button2.begin();
  button1.onPressed(lookForwardTimed);
  button2.onPressed(lookBackwardTimed);
  button1.onPressedFor(longPressDuration, lookForwardUntilPressed);
  button2.onPressedFor(longPressDuration, lookBackwardUntilPressed);
}

void loop() {
  // put your main code here, to run repeatedly:
  button1_state = digitalRead(INPUT_BUTTON_1);
  button2_state = digitalRead(INPUT_BUTTON_2);
  reverse_signal = digitalRead(REVERSE_SIGNAL_IN);
  if (button1_state == LOW) {
    digitalWrite(BUTTON1_LED, HIGH);
  } else {
    digitalWrite(BUTTON1_LED, LOW);
  }
  if (button2_state == LOW) {
    digitalWrite(BUTTON2_LED, HIGH);
  } else {
    digitalWrite(BUTTON2_LED, LOW);
  }
  if (reverse_signal == LOW) {
      direction = 1;
      digitalWrite(CAMERA_SWITCH, LOW);
      digitalWrite(REVERSE_SIGNAL_OUT, HIGH);
      digitalWrite(REVERSE_LED, HIGH);
      backwardOverride = 1;
  }
  if (backwardOverride == 1) {
    if (reverse_signal == HIGH && direction == 1) {
      backwardReleaseTime = millis();
      direction = 0;
    } else if (reverse_signal == HIGH && (millis() - backwardReleaseTime) >= backwardReleaseDelay && direction == 0 && backwardOverride == 1) {
      digitalWrite(REVERSE_SIGNAL_OUT, LOW);
      digitalWrite(REVERSE_LED, LOW);
      backwardReleaseTime = 0;
      backwardOverride = 0;
    }
  } else {
    if ((millis() - lastButtonRead) >= buttonDebounce) {
      button1.read();
      button2.read();
      lastButtonRead = millis();
    }
    if (!onUntilPressed && (millis() - pressTime) >= duration && direction != 0) {
      direction = 0;
      digitalWrite(CAMERA_SWITCH, LOW);
      digitalWrite(REVERSE_SIGNAL_OUT, LOW);
      digitalWrite(REVERSE_LED, LOW);
    } else {
      if (button1_state == LOW && direction != 2 && !onUntilPressed) {
        digitalWrite(REVERSE_SIGNAL_OUT, HIGH);
        digitalWrite(CAMERA_SWITCH, HIGH);
        digitalWrite(REVERSE_LED, HIGH);
        //direction = 2;
      }      
      if (button2_state == LOW && direction != 1 && !onUntilPressed) {
        digitalWrite(REVERSE_SIGNAL_OUT, HIGH);
        digitalWrite(CAMERA_SWITCH, LOW);
        digitalWrite(REVERSE_LED, HIGH);
        //direction = 1;
      }
    }
  }
}

void lookForward(int lookTime) {
  if (lookTime == -2) {
    digitalWrite(CAMERA_SWITCH, LOW);
    digitalWrite(REVERSE_SIGNAL_OUT, LOW);
    digitalWrite(REVERSE_LED, LOW);
    onUntilPressed = false;
    direction = 0;
  } else {
    direction = 2;
    digitalWrite(CAMERA_SWITCH, HIGH);
    digitalWrite(REVERSE_SIGNAL_OUT, HIGH);
    digitalWrite(REVERSE_LED, HIGH);
    if (lookTime == -1) {
      onUntilPressed = true;
      pressTime = millis();
      duration = 0;
    } else {
      onUntilPressed = false;
      pressTime = millis();
      duration = lookTime * 1000;
    }
  }
}

void lookBackward(int lookTime) {
  if (lookTime == -2) {
    digitalWrite(CAMERA_SWITCH, LOW);
    digitalWrite(REVERSE_SIGNAL_OUT, LOW);
    digitalWrite(REVERSE_LED, LOW);
    onUntilPressed = false;
    direction = 0;
  } else {
    direction = 1;
    digitalWrite(CAMERA_SWITCH, LOW);
    digitalWrite(REVERSE_SIGNAL_OUT, HIGH);
    digitalWrite(REVERSE_LED, HIGH);
    if (lookTime == -1) {
      onUntilPressed = true;
      pressTime = millis();
      duration = 0;
    } else {
      onUntilPressed = false;
      pressTime = millis();
      duration = lookTime * 1000;
    }
  }
}

void lookForwardTimed() {
  if (!onUntilPressed || direction != 2) {
    lookForward(viewDuration);
  } else {
    lookForward(-2);
  }
}

void lookBackwardTimed() {
  if (!onUntilPressed || direction != 1) {
    lookBackward(viewDuration);
  } else {
    lookBackward(-2);
  }
}

void lookForwardUntilPressed() {
  lookForward(-1);
}

void lookBackwardUntilPressed() {
  lookBackward(-1);
}
