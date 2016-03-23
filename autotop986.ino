/*
  986 Auto-Top by Adam Robertson <oldskool73@gmail.com>

  Based on : 986 Auto-Top by Alex Roy

  This auto-presses the open or close buttons for the convertible top on a Porsche 986
  for the specified open/close seconds automatically if you quickly tap the button.

  If the button tap is longer (eg it is held down), the button goes into manual mode and reacts like normal,
  i.e. is only auto-pressed for the same amount of time as the user is pressing.
  Once engaged, manual mode remains active until no input has been received for a defined amount of time.

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

// time to lock in manual mode once engaged
#define manualModeLockTime 1000

// operation modes
#define modeWait 0
#define modeAutoOpen 1
#define modeAutoClose 2
#define modeManualOpen 3
#define modeManualClose 4
#define modeHalt 5

unsigned long ms, motorStartTime, manualModeReleaseTime;
bool outputRequired = false;
bool manualModeLocked = false;
bool openTimeComplete = false;
bool closeTimeComplete = false;

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
  updateTimers();
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

/** Timer Functions */

void updateTimers() {
  manualModeLocked = (ms - manualModeReleaseTime < manualModeLockTime);
  openTimeComplete = (ms - motorStartTime > openTime);
  closeTimeComplete = (ms - motorStartTime > closeTime);
}

/** Mode Update Functions */

void updateMode() {

  previousMode = currentMode;

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

  outputRequired = currentMode != previousMode;
}

void updateModeWait() {
  log("updateModeWait");
  if (manualModeLocked) {
    //react to buttons locked in manual mode
    if (openButtonIn.on()) {
      currentMode = modeManualOpen;
    } else if (closeButtonIn.on()) {
      currentMode = modeManualClose;
    }
  } else {
    //react to buttons in auto mode
    if (openButtonIn.on()) {
      currentMode = modeAutoOpen;
    } else if (closeButtonIn.on()) {
      currentMode = modeAutoClose;
    }
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
  if (openTimeComplete) {
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
  if (closeTimeComplete) {
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
    manualModeReleaseTime = millis();
  }
}

void updateModeManualClose() {
  log("updateModeManualClose");
  //wait on button release
  if (!closeButtonIn.on()) {
    currentMode = modeWait;
    manualModeReleaseTime = millis();
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
  if (outputRequired) {
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
}

void sendOpen() {
  log("send : open");
  digitalWrite(pinOpenBtnOut, HIGH);
  digitalWrite(pinCloseBtnOut, LOW);
  digitalWrite(pinParkingBrakeOut, HIGH);
  motorStartTime = millis();
}

void sendClose() {
  log("send : close");
  digitalWrite(pinOpenBtnOut, LOW);
  digitalWrite(pinCloseBtnOut, HIGH);
  digitalWrite(pinParkingBrakeOut, HIGH);
  motorStartTime = millis();
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
