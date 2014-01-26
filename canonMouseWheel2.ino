// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the GPIO interface pins
// I/O Pins
// 0 - Rx serial
// 1 - Tx serial
// 2 - Enter button (middle mouse) (int0)
// 3 - Full press/focus button (left mouse)  (int 1)
// 4 - Half press/focus button (right mouse)
// 5 - RS
// 6 - !lcd light On
// 7 - Phase A (mouse wheel)
// 8 - Phase B (mouse wheel)
// 9 - LCD D4
// 10 - LCD D5
// 11 - LCD D6
// 12 - LCD D7
// 13 - Arduino LED



////////////////////////////////////////
// Define the LCD library
// RS, E, D4, D5, D6, D7 r/w-GND
LiquidCrystal lcd(5, 9, 10, 11, 12, 13);



////////////////////////////////////////
// Define Pins

const int enterButton   = 2; // the number of the ENTER button pin
const int shutterButton = 3; // the number of the SHUTTER button pin
const int focusButton   = 4; // the number of the FOCUS button pin
const int wheelPhaseA   = 7; // the number of the pushbutton pin
const int wheelPhaseB   = 8; // the number of the pushbutton pin
const int lcdLight      = 6; // the light for the LCD display (active low)
const int arduinoLED    = 13;// the light for the LCD display (active low)

const int DEBOUNCE_TIME_MS    = 50;
const int BACKLIGHT_TIME_MS   = 5000;
const int MODE_CHANGE_TIME_MS = 750;


////////////////////////////////////////
// Define variables

////////////////////
// buttons
int buttonStateEnter = 0;       // variable for reading the pushbutton status
int buttonStateShutter = 0;     // variable for reading the pushbutton status
int buttonStateFocus = 0;       // variable for reading the pushbutton status

// for our button state machine
enum buttonStates {PRESSED, NOT_PRESSED, DEBOUNCING};
enum buttonStates currentStateEnter = NOT_PRESSED;
enum buttonStates currentStateShutter = NOT_PRESSED;
enum buttonStates currentStateFocus = NOT_PRESSED;



////////////////////
// the phases of the mouse wheel
int prevWheelState = 0;    // a mixed variable that has previous bits phaseA & phaseB
int wheelState = 0;        // a mixed variable that has both bits phaseA & phaseB
char dir = 'd';             // variable for direction "d"own and "u"p 

////////////////////
// Wheel info :: These just count for interests sake
int totalWheelCount = 0;
int upWheelCount = 0;
int downWheelCount = 0;

////////////////////
// 0123456789012345 <- 16x2(1602) display
// LCD cursor (x) position
int cursor_x = 0;
int cursor_x_min = 0;
int cursor_x_max = 15;

////////////////////
// STATE info
// 1 if we are changing the mode of the remote, 0 for normal operation
int modeChange = 0;

// mode 0=simple 1=Interval 2=Bulb
int currentMode = 0;


// Pseudo scheduler allow for 8 Schedules
unsigned long scheduledNextTime1 = 0;  // Enable
unsigned long scheduledNextTime2 = 0;  // Shutter
unsigned long scheduledNextTime3 = 0;  // Focus
unsigned long scheduledNextTime4 = 0;  // LCD backlight
unsigned long scheduledNextTime5 = 0;  // Mode switch
unsigned long scheduledNextTime6 = 0;
unsigned long scheduledNextTime7 = 0;
unsigned long scheduledNextTime8 = 0;





////////////////////////////////////////////////////////////////////////////////
void setup() {

  // initialize pins as OUTPUT:
  pinMode(lcdLight,   OUTPUT);      
  pinMode(arduinoLED, OUTPUT);      

  // initialize the pins as INPUT:
  pinMode(wheelPhaseA,   INPUT_PULLUP);     
  pinMode(wheelPhaseB,   INPUT_PULLUP);
  pinMode(enterButton,   INPUT_PULLUP);     
  pinMode(shutterButton, INPUT_PULLUP);     
  pinMode(focusButton,   INPUT_PULLUP);     

  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);

 // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);  
  lcd.setCursor(0,1);
  lcdBacklightOn();
  lcd.print("Hello, Compadre!");

  delay(1000);
  mainScreen();  
  
  // Read the button state just once so that our gray code reader wont get confused
  prevWheelState = ((digitalRead(wheelPhaseB) << 1 ) + digitalRead(wheelPhaseA));
}



////////////////////////////////////////////////////////////////////////////////
void loop() {
  readButtonStates();
  
  // print out the state of the button:
  //  Serial.println(prevWheelState);
  //  Serial.println(wheelState);
  Serial.print("U");
  Serial.print(upWheelCount);
  Serial.print(" D");
  Serial.print(downWheelCount);
  Serial.print(" T");
  Serial.print(totalWheelCount);
  Serial.print(" dir:");
  Serial.print(dir);
  Serial.print(" E");
  Serial.print((buttonStateEnter^1));
  Serial.print(" F");
  Serial.print(buttonStateShutter^1);
  Serial.print(" H");
  Serial.print(buttonStateFocus^1);
  Serial.print(" t1");
  Serial.print(scheduledNextTime1^1);
  Serial.print(" t2");
  Serial.print(scheduledNextTime2^1);
  Serial.print(" t3");
  Serial.print(scheduledNextTime3^1);
  Serial.print(" bE");
  Serial.print(currentStateEnter^1);
  Serial.print(" bS");
  Serial.print(currentStateShutter^1);
  Serial.print(" bF");
  Serial.println(currentStateFocus^1);

  prevWheelState = wheelState;


  ////////////////////////////////////////////////
  // if in use mode
  if (modeChange == 0) {

    //////////////////////////////////////
    // mode 0  = SIMPLE
    if (currentMode == 0) {
      
      /*      //////////////////////////////////////////
      // ENABLE Press
      // see if user want to go into Mode set (eg MODE_CHANGE_TIME_MS delay)
      
      if (buttonStateEnter == LOW) {
       delay(750);
        // check again
        buttonStateEnter = digitalRead(enterButton);
        if (buttonStateEnter == LOW) {
          // we want to change mode

	  
          delay(750);
        }
      }  
      */
    }
    
    
    //////////////////////////////////////
    // mode 1  = INTERVAL
    if (currentMode == 1) {

    }

    //////////////////////////////////////
    // mode 2  = BULB
    if (currentMode == 2) {
    }


  } 
  else { // modeChange==1

    currentMode = nextModeScreen();
    
    // see if we want to save it
    if (buttonStateEnter == LOW) {
      delay(500);

      // check again
      buttonStateEnter = digitalRead(enterButton);
      if (buttonStateEnter == LOW) {  
        lcd.clear();
        lcd.setCursor(0,0);
        modeChange = 0;
  
        if(currentMode == 0){
          lcd.print("Simple");
        }
        
        if(currentMode == 1){
          lcd.print("Interval");
	  //          setIntervalTime();
        }
        
        if(currentMode == 2){
          lcd.print("Bulb");
          setBulbTime();
        }
      }
    } 
  }
  
  // see if its time to turn off the lcdBacklight
  lcdBacklightOff();  
}



void readButtonStates(){
  ////////////////////////////////////////
  // read the state of ENTER button
  buttonStateEnter = digitalRead(enterButton);

  // if its low then check a bunch stuff... otherwise 
  if (buttonStateEnter == LOW){
  lcdBacklightOn();

    if (currentStateEnter == NOT_PRESSED){
      // set the future compare time and change state to DEBOUNCING
      scheduledNextTime1 = millis() + DEBOUNCE_TIME_MS;
      scheduledNextTime5 = millis() + MODE_CHANGE_TIME_MS;

      currentStateEnter = DEBOUNCING;
    }
    
    // are we in the midst of DEBOUNCING?
    if (currentStateEnter == DEBOUNCING){
      
      // have we passed the debouncing time?
      if(millis() >= scheduledNextTime1){
	
	// is the button still pressed?
	if (buttonStateEnter == LOW){
	  
	  currentStateEnter = PRESSED;
	}
	else {
	  currentStateEnter = NOT_PRESSED;
	}
      }
      //else {} continue waiting
    }
    
    
    if (currentStateEnter == PRESSED){
      // do something

      if(millis() >= scheduledNextTime5 ){
	// goto mode change menu

	modeChangeMenu();
      }


    }
  }
  else {
    // button is high
    if (currentStateEnter != DEBOUNCING){
      // we might be in a bounce
      currentStateEnter = NOT_PRESSED;
    }
  }
  


  ////////////////////////////////////////
  // read the state of SHUTTER button
  buttonStateShutter = digitalRead(shutterButton);

  // if its low then check a bunch stuff... otherwise 
  if (buttonStateShutter == LOW){
  lcdBacklightOn();

    
    if (currentStateShutter == NOT_PRESSED){
      // set the future compare time and change state to DEBOUNCING
      scheduledNextTime2 = millis() + DEBOUNCE_TIME_MS;
      currentStateShutter = DEBOUNCING;
    }
    
    
    // are we in the midst of DEBOUNCING?
    if (currentStateShutter == DEBOUNCING){
    
      // have we passed the debouncing time?
      if(millis() >= scheduledNextTime2){
	
	// is the button still pressed?
	if (buttonStateShutter == LOW){
	  
	  currentStateShutter = PRESSED;
	}
	else {
	  currentStateShutter = NOT_PRESSED;
	}
      }
      //else {} continue waiting
    }
  
    
    if (currentStateShutter == PRESSED){
      shutter();
    }
    
  }
  else {
    // button is high
    if (currentStateShutter != DEBOUNCING){
      // we might be in a bounce
      currentStateShutter = NOT_PRESSED;
    }
  }





  ////////////////////////////////////////
  // read the state of Focus
  buttonStateFocus = digitalRead(focusButton);

  // if its low then check a bunch stuff... otherwise 
  if (buttonStateFocus == LOW){
  lcdBacklightOn();
    
    if (currentStateFocus == NOT_PRESSED){
      // set the future compare time and change state to DEBOUNCING
      scheduledNextTime3 = millis() + DEBOUNCE_TIME_MS;
      currentStateFocus = DEBOUNCING;
    }
    
    
    // are we in the midst of DEBOUNCING?
    if (currentStateFocus == DEBOUNCING){
      
      // have we passed the debouncing time?
      if(millis() >= scheduledNextTime3){

	// is the button still pressed?
	if (buttonStateFocus == LOW){
	  
	  currentStateFocus = PRESSED;
	}
	else {
	  currentStateFocus = NOT_PRESSED;
	}
      }
      //else {} continue waiting
    }
    
    
    if (currentStateFocus == PRESSED){
      focus();
    }
  }
  else {
    // button is high
    if (currentStateFocus != DEBOUNCING){
      // we might be in a bounce
      currentStateFocus = NOT_PRESSED;
    }
  }
  

  ////////////////////
  // read the state of the rotary wheel
  wheelState = readWheel();
  return;
}



////////////////////////////////////////
// print the main menu on the screen
void mainScreen() {
  // Print a message to the LCD.
  lcd.clear();
  lcdBacklightOn();

  if (currentMode == 0){
    lcd.setCursor(2,0);
    lcd.print("Mode: Simple");
  }
  lcd.setCursor(0,1);
  lcd.print("Mode change: B2");
  return;
}



////////////////////////////////////////
//
void modeChangeMenu(){
    lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("Choose mode:");
  
  lcd.setCursor(0,1);
  lcd.print(">  Simple  < *..");
  lcdBacklightOn();
  
  modeChange = 1;
  return;
}


////////////////////////////////////////
// show the mode changes
int nextModeScreen() {
  if (dir == 'u'){
    lcdBacklightOn();

    if (currentMode == 2){
       currentMode = 0;
    }
    else {
      currentMode++;
    }    
  }

  if (dir == 'd'){
    lcdBacklightOn();
    
    if (currentMode == 0){
       currentMode = 2;
    }
    else {
      currentMode--;
    }    
  }


  if (currentMode == 0){
    lcd.setCursor(0,1);
    lcd.print(">  Simple  < *..");
  }
  if (currentMode == 1){
    lcd.setCursor(0,1);
    lcd.print("> Interval < .*.");
  }
  if (currentMode == 2){
    lcd.setCursor(0,1);
    lcd.print(">   Bulb   < ..*");
  }
  
  return currentMode;
}




////////////////////////////////////////////////////////////////////
// read the value of the wheel
int readWheel() {
  wheelState = ((digitalRead(wheelPhaseB) << 1) + digitalRead(wheelPhaseA));

  // The mouse wheel uses GRAY CODE!!  :)

  // no change
  if ((wheelState == 0) && (prevWheelState == 0)) {
    dir = 'n';  
  }
  if ((wheelState == 1) && (prevWheelState == 1)) {
    dir = 'n';  
  }
  if ((wheelState == 3) && (prevWheelState == 3)) {
    dir = 'n';  
  }
  if ((wheelState == 2) && (prevWheelState == 2)) {
    dir = 'n';  
  }

 
  // DOWN one click  
  if ((wheelState == 1) && (prevWheelState == 0)) {
    // commented out to make a psedo divide by 2 (this is because the mouse wheel moves 2 for each "click")
    // downWheelCount++;
    // totalWheelCount--;
    dir = 'd';
  }
  if ((wheelState == 3) && (prevWheelState == 1)) {
    downWheelCount++;
    totalWheelCount--;
    dir = 'd';
  }
  if ((wheelState == 2) && (prevWheelState == 3)) {
    // commented out to make a psedo divide by 2 (this is because the mouse wheel moves 2 for each "click")
    // downWheelCount++;
    // totalWheelCount--;
    dir = 'd';
  }
  if ((wheelState == 0) && (prevWheelState == 2)) {
    downWheelCount++;
    totalWheelCount--;
    dir = 'd';
  }


  // UP one click  
  if ((wheelState == 2) && (prevWheelState == 0)) {
    // commented out to make a psedo divide by 2 (this is because the mouse wheel moves 2 for each "click")
    // upWheelCount++;
    // totalWheelCount++;
    dir = 'u';
  }
  if ((wheelState == 3) && (prevWheelState == 2)) {
    upWheelCount++;
    totalWheelCount++;
    dir = 'u';
  }
  if ((wheelState == 1) && (prevWheelState == 3)) {
    // commented out to make a psedo divide by 2 (this is because the mouse wheel moves 2 for each "click")
    // upWheelCount++;
    // totalWheelCount++;
    dir = 'u';
  }
  if ((wheelState == 0) && (prevWheelState == 1)) {
    upWheelCount++;
    totalWheelCount++;
    dir = 'u';
  }

  return wheelState;
}


////////////////////////////////////////
//
void setIntervalTime() {
  
 return;
}



////////////////////////////////////////
// 
void setBulbTime() {
  
  
 return;
}



/////////////////////////////////////////////////////////////////
// depress the SHUTTER fully
void shutter() {
  // Print a message to the LCD.
  lcd.clear();
  lcd.setCursor(2,0);
  lcdBacklightOn();
  lcd.print("** Click **");

  while (digitalRead(shutterButton) == LOW){
    // just stall here and let the camera do its job
  }

  mainScreen();  
  return;
}



////////////////////////////////////////
// depress the shutter halfway to FOCUS
void focus() {
  lcd.clear();
  lcdBacklightOn();
  lcd.setCursor(2,0);
  lcd.print("** Focus **");

  while (digitalRead(focusButton) == LOW){
    // just stall here and let the camera do its job
  }

  mainScreen();
  return;
}



////////////////////////////////////////
// 
void startIntervalCamera(){

  return;
}



////////////////////////////////////////
// turn on the backlight , or keep it on
void lcdBacklightOn() {
  // set the time to turn light off
  scheduledNextTime4 = millis() + BACKLIGHT_TIME_MS;
  
  // activate the light
  digitalWrite(lcdLight, LOW);
  
  return; 
}


////////////////////////////////////////
// turn off the backlight if the timer expires.
void lcdBacklightOff(){
  if (millis() >= scheduledNextTime4){
    digitalWrite(lcdLight, HIGH);
  }
  
  return;
}
