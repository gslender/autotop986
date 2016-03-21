/*
  986 Auto-Top by Alex Roy
  This auto-presses the open or close buttons for the convertible top on a Porsche 986
  for the specified open/close seconds automatically if you quickly tap the button.
  If the button tap is longer (eg it is held down), the button is only autp-pressed 
  for the same amount of time as the user is pressing.
  
  If the top is already automatically opening or closing, a second press of either 
  button cancels the auto-open proceedure.
  
  My 2001 Boxster S open/close timing:
  13.6sec to open
  14.4sec to close
 */
 
 #define openBtnOut     2
 #define closeBtnOut    3
 #define pBrakeOut      4
 #define openBtnIn      8
 #define closeBtnIn     9
 
 int movementCount = 0;
 int openBtnCount = 0;
 int closeBtnCount = 0;
 int openMode = 0; //0=no auto movement, 1=auto open, 2 = auto close, 3 = manual open, 4 = manual close
 bool btnReleased = false;
 bool ignoreBtn = false;
 
 #define shortPressInterval 250
 #define loopInterval 50
 
 #define openTime 15600
 #define closeTime 16400
 
 void setup() {
   pinMode(openBtnOut, OUTPUT);
   pinMode(closeBtnOut, OUTPUT);
   pinMode(pBrakeOut, OUTPUT);
   
   pinMode(openBtnIn, INPUT);
   pinMode(closeBtnIn, INPUT);
   
   allStop();
 }
 
 void loop() {
   //check buttons
   if(digitalRead(openBtnIn) == HIGH) {
     if(btnReleased){
       allStop();
       ignoreBtn = true;
     } else if(ignoreBtn == false) {
       if(openBtnCount < 32000) {
         openBtnCount += loopInterval;
       }
       closeBtnCount = 0;
       digitalWrite(closeBtnOut, LOW);
     }
   } else if(digitalRead(closeBtnIn) == HIGH) {
     if(btnReleased){
       allStop();
       ignoreBtn = true;
     } else if(ignoreBtn == false) {
       if(closeBtnCount < 32000) {
         closeBtnCount += loopInterval;
       }
       openBtnCount = 0;
       digitalWrite(openBtnOut, LOW);
     }
   } else if (openMode != 1 && openMode != 2) {
     allStop();
     ignoreBtn = false;
   } else {
     btnReleased = true;
     ignoreBtn = false;
   }
   
   switch(openMode) { ////0=no auto movement, 1=auto open, 2 = auto close, 3 = manual open, 4 = manual close
     case 1: //currently auto-opening
       movementCount -= loopInterval;
       if(openBtnCount > shortPressInterval) { //if the interval is greater, transition to manual open mode
         openMode = 3;
       } else { //continue with auto-open
         if(movementCount <= 0 || closeBtnCount > 0) { //stop auto-open function
           allStop();
         } else { //continue with auto-open function
           //continue with current pin state
         }
       }
       break;
       
     case 2: //currently auto-closing
       movementCount -= loopInterval;
       if(closeBtnCount > shortPressInterval) { //if the interval is greater, transition to manual close mode
         openMode = 4;
       } else { //continue with auto-close
         if(movementCount <= 0 || openBtnCount > 0) { //stop auto-open function
           allStop();
         } else { //continue with auto-close function
           //continue with current pin state
         }
       }
       break;
       
     case 3: //currently manually opening
       movementCount = 0;
       if(openBtnCount > 0) {
         //continue with current pin state
       } else {
         allStop();
       }
       break;
       
     case 4: //currently manually closing
       movementCount = 0;
       if(closeBtnCount > 0) {
         //continue with current pin state
       } else {
         allStop();
       }
       break;
       
     default: //no movement currently
       if(openBtnCount > 0) {
         openMode = 1;
         movementCount = openTime;
         digitalWrite(pBrakeOut, HIGH);
         digitalWrite(closeBtnOut, LOW);
         digitalWrite(openBtnOut, HIGH);
       } else if(closeBtnCount > 0) {
         openMode = 2;
         movementCount = closeTime;
         digitalWrite(pBrakeOut, HIGH);
         digitalWrite(openBtnOut, LOW);
         digitalWrite(closeBtnOut, HIGH);
       } else {
         allStop();
       }
       
       break;
   }
   
   delay(loopInterval);
 }
 
 void allStop() {
   digitalWrite(openBtnOut, LOW);
   digitalWrite(closeBtnOut, LOW);
   digitalWrite(pBrakeOut, LOW);
   openMode = 0;
   movementCount = 0;
   openBtnCount = 0;
   closeBtnCount = 0;
   btnReleased = false;
 }
