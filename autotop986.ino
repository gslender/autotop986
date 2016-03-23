/*
  986 Auto-Top by Adam Robertson <oldskool73@gmail.com>

  Based on : 986 Auto-Top by Alex Roy

  This auto-presses the open or close buttons for the convertible top on a Porsche 986
  for the specified open/close seconds automatically if you quickly tap the button.

  If the button tap is longer (eg it is held down), the button reacts like normal
  and is only auto-pressed for the same amount of time as the user is pressing.

  If the top is already automatically opening or closing, a second press of either
  button cancels the auto-open proceedure.

*/

#include "Switch.h"
#include "SimpleTimer.h"

// pin definitions
#define pinOpenBtnOut       2
#define pinCloseBtnOut      3
#define pinParkingBrakeOut  4
#define pinOpenBtnIn        8
#define pinCloseBtnIn       9

#define loopInterval 50

// time taken to open top
#define openTime 15600
// time taken to close top
#define closeTime 16400

// operation modes
#define modeWait 0
#define modeAutoOpen 1
#define modeAutoClose 2
#define modeManualOpen 3
#define modeManualClose 4
#define modeHalt 5

unsigned long ms, motorStartTime;

int currentMode = modeWait;
int previousMode = modeWait;

String lastLog = "null";

Switch openButtonIn = Switch(pinOpenBtnIn);//,INPUT, HIGH);
Switch closeButtonIn = Switch(pinCloseBtnIn);//,INPUT, HIGH);

void setup() {
  Serial.begin(9600);
  Serial.println("setup");

  initInputPins();
  initOutputPins();
  sendStop();
}

void loop() {
  ms = millis();

  pollInput();
  updateMode();
  sendOutput();

  //delay(loopInterval);
}


/** Setup Functions **/

void initInputPins() {
//  pinMode(pinOpenBtnIn, INPUT);
//  pinMode(pinCloseBtnIn, INPUT);
}

void initOutputPins() {
  pinMode(pinOpenBtnOut, OUTPUT);
  pinMode(pinCloseBtnOut, OUTPUT);
  pinMode(pinParkingBrakeOut, OUTPUT);
}

/** Input Functions **/

void pollInput() {

  openButtonIn.poll();
  closeButtonIn.poll();

}

/** Mode Update Functions */

void updateMode() {

  switch (currentMode) {

    case modeWait:
      updateModeWait();
      break;
    case modeAutoOpen:
      updateModeAutoOpen();
      break;
    case modeAutoClose:
      updateModeAutoClose();
      break;
    case modeManualOpen:
      updateModeManualOpen();
      break;
    case modeManualClose:
      updateModeManualClose();
      break;
    case modeHalt:
      updateModeHalt();
      break;
  }

}

void updateModeWait() {
  log("updateModeWait");
  if (openButtonIn.on()) {
    currentMode = modeAutoOpen;
    motorStartTime = millis();
  }
  if (closeButtonIn.on()) {
    currentMode = modeAutoClose;
    motorStartTime = millis();
  }
}

void updateModeAutoOpen() {
  log("updateModeAutoOpen");
  //halt on button push
  if (openButtonIn.pushed() || closeButtonIn.on()) {
    currentMode = modeHalt;
    return;
  }
  //wait on timer
  if (ms - motorStartTime > openTime) {
    currentMode = modeWait;
    return;
  }
  //switch to manual mode on long press
  if (openButtonIn.longPress()) {
    currentMode = modeManualOpen;
    return;
  }
}

void updateModeAutoClose() {
  log("updateModeAutoClose");
  //halt on button push
  if (openButtonIn.on() || closeButtonIn.pushed()) {
    currentMode = modeHalt;
  }
  //wait on timer
  if (ms - motorStartTime > closeTime) {
    currentMode = modeWait;
    return;
  }
  //switch to manual mode on long press
  if (closeButtonIn.longPress()) {
    currentMode = modeManualClose;
    return;
  }
}

void updateModeManualOpen() {
  log("updateModeManualOpen");
  //wait on button release
  if (!openButtonIn.on()) {
    currentMode = modeWait;
  }
}

void updateModeManualClose() {
  log("updateModeManualClose");
  //wait on button release
  if (!closeButtonIn.on()) {
    currentMode = modeWait;
  }
}

void updateModeHalt() {
  log("updateModeHalt");
  //only release halt when both buttons are released
  if (!closeButtonIn.on() && !openButtonIn.on()) {
    currentMode = modeWait;
  }
}

/** Output Functions */

void sendOutput() {
  if (currentMode != previousMode) {
    log("mode changed");
    Serial.println(currentMode);

    if (currentMode == modeAutoOpen || currentMode == modeManualOpen) {
      sendOpen();
    } else if (currentMode == modeAutoClose || currentMode == modeManualClose) {
      sendClose();
    } else {
      sendStop();
    }
  }

  previousMode = currentMode;
}

void sendOpen() {
  log("send : open");
  digitalWrite(pinOpenBtnOut, HIGH);
  digitalWrite(pinCloseBtnOut, LOW);
  digitalWrite(pinParkingBrakeOut, HIGH);
}

void sendClose() {
  log("send : close");
  digitalWrite(pinOpenBtnOut, LOW);
  digitalWrite(pinCloseBtnOut, HIGH);
  digitalWrite(pinParkingBrakeOut, HIGH);
}


void sendStop() {
  log("send : stop!");
  digitalWrite(pinOpenBtnOut, LOW);
  digitalWrite(pinCloseBtnOut, LOW);
  digitalWrite(pinParkingBrakeOut, LOW);
}

void log(String log) {
  if (log != lastLog) {
    Serial.println(log);
    lastLog = log;
  }
}
