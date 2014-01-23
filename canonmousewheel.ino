// include the library code:
#include <LiquidCrystal.h>


// initialize the library with the numbers of the GPIO interface pins
// RS, E, D4, D5, D6, D7 r/w-GND
LiquidCrystal lcd(3, 9, 10, 11, 12, 13);


// constants won't change. They're used here to set pin numbers:
const int buttonPinE = 6;   // the number of the pushbutton pin
const int buttonPinF = 5;   // the number of the pushbutton pin
const int buttonPinH = 4;   // the number of the pushbutton pin
const int buttonPinA = 7;   // the number of the pushbutton pin
const int buttonPinB = 8;   // the number of the pushbutton pin
const int ledPin =  13;     // the number of the LED pin


// variables will change:
//int buttonStateA = 0;     // variable for reading the pushbutton status
//int buttonStateB = 0;     // variable for reading the pushbutton status
// Enter buttons
int buttonStateE = 0;       // variable for reading the pushbutton status
// Full button 
int buttonStateF = 0;       // variable for reading the pushbutton status
// Half button
int buttonStateH = 0;       // variable for reading the pushbutton status

int prevButtonState = 0;    // a mixed variable that has previous bits A & B
int buttonState = 0;        // a mixed variable that has both bits A & B

// status variables
char dir = 'd';             // variable for direction "d"own and "u"p 

// set pointer to dir
//dir_ptr = &dir;

// These just count for interests sake
int totalWheelCount = 0;
int upWheelCount = 0;
int downWheelCount = 0;

// cursor position
int cursor_x = 0;
int cursor_x_min = 0;
int cursor_x_max = 19;


// 1 if we are changing the mode of the remote, 0 for normal operation
int modeChange = 0;

// mode 0=simple 1=Interval 2=Bulb
int currentMode = 0;

// characters for the interval timer
char hour10 = '0';
char hour1 = '0';
char min10 = '0';
char min1 = '0';
char sec10 = '0';
char sec1 = '0';




void setup() {
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);      
  // initialize the phase A of the wheel as an input:
  pinMode(buttonPinA, INPUT_PULLUP);     
  // initialize the phase B of the wheel as an input:
  pinMode(buttonPinB, INPUT_PULLUP);
  
  // initialize the pushbutton pin as an input:
  pinMode(buttonPinE, INPUT_PULLUP);     
  // initialize the pushbutton pin as an input:
  pinMode(buttonPinF, INPUT_PULLUP);     
  // initialize the pushbutton pin as an input:
  pinMode(buttonPinH, INPUT_PULLUP);     

  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);


 // set up the LCD's number of columns and rows: worksewi
  lcd.begin(20, 2);  
  lcd.setCursor(4,1);
  // Print a message to the LCD.
  lcd.print("Hello, Compadre!");
  delay(1500);
  lcd.clear();

  mainScreen();  



  // Read the button state just once so that our gray code reader wont get confused
  // read the state of the pushbutton value:
  prevButtonState = ((digitalRead(buttonPinB) << 1 ) + digitalRead(buttonPinA));
}



////////////////////////////////////////////////////////////////////////////////
void loop() {

  readAllStates();
  
  // print out the state of the button:
  //  Serial.println(prevButtonState);
  //  Serial.println(buttonState);
  Serial.print("U");
  Serial.print(upWheelCount);
  Serial.print(" D");
  Serial.print(downWheelCount);
  Serial.print(" T");
  Serial.print(totalWheelCount);
  Serial.print(" dir:");
  Serial.print(dir);
  Serial.print(" E");
  Serial.print((buttonStateE^1));
  Serial.print(" F");
  Serial.print(buttonStateF^1);
  Serial.print(" H");
  Serial.println(buttonStateH^1);

  prevButtonState = buttonState;


  ////////////////////////////////////////////////
  // if in use mode
  if (modeChange == 0) {

    //////////////////////////////////////
    // mode 0  = SIMPLE
    if (currentMode == 0) {
      
      ////////////////////////////////////////////////
      // FULL PRESS mode
      // see if user wants take picture
      if (buttonStateF == LOW) {
        delay(25); // debounce the itchy trigger

        // check again
        buttonStateF = digitalRead(buttonPinF);
        if (buttonStateF == LOW) {
          fullShutter();
        }
      }
    
    
    
      ////////////////////////////////////////////////
      // HALF PRESS
      // if not, see if user wants to focus
      if (buttonStateH == LOW) {
        delay(50); // less responsive

        // check again
        buttonStateH = digitalRead(buttonPinH);
        if (buttonStateH == LOW) {
          halfShutter();
        }
      }


      //////////////////////////////////////////
      // ENABLE Press
      // see if user want to go into Mode set (eg .75 sec delay)
      if (buttonStateE == LOW) {
        delay(750);
        // check again
        buttonStateE = digitalRead(buttonPinE);
        if (buttonStateE == LOW) {
          // we want to change mode
          lcd.clear();

          lcd.setCursor(0,0);
          lcd.print("Choose mode:");

          lcd.setCursor(0,1);
          lcd.print("[Simple] Interval  Bulb ");

          modeChange = 1;
          delay(750);
        }
      }  

    }
    
    
    //////////////////////////////////////
    // mode 1  = INTERVAL
    if (currentMode == 1) {

      ////////////////////////////////////////////////
      // FULL PRESS mode
      // see if user wants take picture
      if (buttonStateF == LOW) {
        delay(25); // debounce the itchy trigger

        // check again
        buttonStateF = digitalRead(buttonPinF);
        if (buttonStateF == LOW) {
          startIntervalCamera();
        }
      }
    
    
    
      ////////////////////////////////////////////////
      // HALF PRESS
      // if not, see if user wants to focus
      if (buttonStateH == LOW) {
        delay(50); // less responsive

        // check again
        buttonStateH = digitalRead(buttonPinH);
        if (buttonStateH == LOW) {
          halfShutter();
        }
      }

      
    } 

    //////////////////////////////////////
    // mode 2  = BULB
    if (currentMode == 2) {
    }


  } 
  else {   // modeChange==1
    currentMode = nextModeScreen();
    
    // see if we want to save it
    if (buttonStateE == LOW) {
      delay(500);

      // check again
      buttonStateE = digitalRead(buttonPinE);
      if (buttonStateE == LOW) {  
        lcd.clear();
        lcd.setCursor(0,0);
        modeChange = 0;
  
        if(currentMode == 0){
          lcd.print("Simple");
        }
        
        if(currentMode == 1){
          lcd.print("Interval");
          setIntervalTime();
        }
        
        if(currentMode == 2){
          lcd.print("Bulb");
          setBulbTime();
        }
        
      }
    } 
  }
}



void readAllStates(){
  // read the state of ENTER/MODE
  buttonStateE = digitalRead(buttonPinE);

  // read the state of FULL Press
  buttonStateF = digitalRead(buttonPinF);

  // read the state of HALF press
  buttonStateH = digitalRead(buttonPinH);

  // read the state of the rotary wheel
  buttonState = readWheel();
  return;
}



///////////////////////////////////////////////////////////////////
// print the main menu on the screen
void mainScreen() {
  // Print a message to the LCD.
  lcd.setCursor(0,0);
  lcd.print("Mode: Simple Remote");

  // Print a message to the LCD.
  lcd.setCursor(0,1);
  lcd.print("Hold B2 to change mode");
 return;
}



///////////////////////////////////////////////////////////////////
// show the mode changes
int nextModeScreen() {
  if (dir == 'u'){
    if (currentMode == 2){
       currentMode = 0;
    }
    else {
      currentMode++;
    }    
  }

  if (dir == 'd'){
    if (currentMode == 0){
       currentMode = 2;
    }
    else {
      currentMode--;
    }    
  }


  if (currentMode == 0){
    lcd.setCursor(0,1);
    lcd.print("[Simple] Interval  Bulb ");
  }
  if (currentMode == 1){
    lcd.setCursor(0,1);
    lcd.print(" Simple [Interval] Bulb ");
  }
  if (currentMode == 2){
    lcd.setCursor(0,1);
    lcd.print(" Simple  Interval [Bulb]");
  }
  
  return currentMode;
}




////////////////////////////////////////////////////////////////////
// read the value of the wheel
int readWheel() {
  buttonState = ((digitalRead(buttonPinB) << 1) + digitalRead(buttonPinA));

  // The mouse wheel uses GRAY CODE!!  :)

  // no change
  if ((buttonState == 0) && (prevButtonState == 0)) {
    dir = 'n';  
  }
  if ((buttonState == 1) && (prevButtonState == 1)) {
    dir = 'n';  
  }
  if ((buttonState == 3) && (prevButtonState == 3)) {
    dir = 'n';  
  }
  if ((buttonState == 2) && (prevButtonState == 2)) {
    dir = 'n';  
  }

 
  // DOWN one click  
  if ((buttonState == 1) && (prevButtonState == 0)) {
    // commented out to make a psedo divide by 2 (this is because the mouse wheel moves 2 for each "click")
    // downWheelCount++;
    // totalWheelCount--;
    dir = 'd';
  }
  if ((buttonState == 3) && (prevButtonState == 1)) {
    downWheelCount++;
    totalWheelCount--;
    dir = 'd';
  }
  if ((buttonState == 2) && (prevButtonState == 3)) {
    // commented out to make a psedo divide by 2 (this is because the mouse wheel moves 2 for each "click")
    // downWheelCount++;
    // totalWheelCount--;
    dir = 'd';
  }
  if ((buttonState == 0) && (prevButtonState == 2)) {
    downWheelCount++;
    totalWheelCount--;
    dir = 'd';
  }


  // UP one click  
  if ((buttonState == 2) && (prevButtonState == 0)) {
    // commented out to make a psedo divide by 2 (this is because the mouse wheel moves 2 for each "click")
    // upWheelCount++;
    // totalWheelCount++;
    dir = 'u';
  }
  if ((buttonState == 3) && (prevButtonState == 2)) {
    upWheelCount++;
    totalWheelCount++;
    dir = 'u';
  }
  if ((buttonState == 1) && (prevButtonState == 3)) {
    // commented out to make a psedo divide by 2 (this is because the mouse wheel moves 2 for each "click")
    // upWheelCount++;
    // totalWheelCount++;
    dir = 'u';
  }
  if ((buttonState == 0) && (prevButtonState == 1)) {
    upWheelCount++;
    totalWheelCount++;
    dir = 'u';
  }

  return buttonState;
}



////////////////////////////////////////////////////////////////////
// read the value of the wheel
int readWheelDir() {
  buttonState = ((digitalRead(buttonPinB) << 1) + digitalRead(buttonPinA));

  // The mouse wheel uses GRAY CODE!!  :)
  // no change
  if (((buttonState == 0) && (prevButtonState == 0)) || ((buttonState == 1) && (prevButtonState == 1)) ||
    ((buttonState == 3) && (prevButtonState == 3)) || ((buttonState == 2) && (prevButtonState == 2))) {
    dir = 'n';  
  }

   // DOWN one click  
  if (((buttonState == 1) && (prevButtonState == 0)) || ((buttonState == 3) && (prevButtonState == 1)) ||
    ((buttonState == 2) && (prevButtonState == 3)) || ((buttonState == 0) && (prevButtonState == 2))) {
    dir = 'd';
  }

  // UP one click  
  if (((buttonState == 2) && (prevButtonState == 0)) || ((buttonState == 3) && (prevButtonState == 2)) ||
    ((buttonState == 1) && (prevButtonState == 3)) || ((buttonState == 0) && (prevButtonState == 1))) {
    dir = 'u';
  }
  return buttonState;
}



/////////////////////////////////////////////////////////////////
void setIntervalTime() {
  lcd.setCursor(0,1);
//  lcd.print("00:00:00");
  lcd.print("%s%s:%s%s:%s%s",hour10,hour1,min10,min1,sec10,sec1);
  
  // valid locations for digits
  // 0,1 tens of hours
  // 1,1 minutes
  // 3,1 tens of mins
  // 4,1 mins
  // 6,1 tens of secs
  // 7,1 secs
  
  lcd.setCursor(0,1);
  lcd.cursor();  
  lcd.blink();
  

  // check the button state.  Rotate wheel the next/previous digit
  // quick tap sets change digit mode
  buttonStateE = digitalRead(buttonPinE);

  if (buttonStateE == HIGH) {          
    buttonState = readWheel();
    if (dir == 'u'){
      if (cursor_x < 7){
        cursor_x++;
      }
    }

    if (dir == 'd'){
      if (cursor_x > 0){
        cursor_x--;
      }
    }

    lcd.setCursor(cursor_x,1);
    delay(100);
  }
  else { // button is low
    // test button again
    delay(250);
    buttonStateE = digitalRead(buttonPinE);
    
    if (buttonStateE == LOW) {
      // change digit
      buttonState = readWheel();
      if (dir == 'u'){
        if (cursor_x == 7){
           if (sec1 == '9'){
              sec1 = '0';
           }
           else {
             sec1++;
           }
        }
        if (cursor_x == 6){
           if (sec10 == '9'){
              sec10 = '0';
           }
           else {
             sec10++;
           }
        }
        if (cursor_x == 4){
           if (min1 == '9'){
              min1 = '0';
           }
           else {
             min1++;
           }
        }
        if (cursor_x == 3){
           if (min10 == '9'){
              min10 = '0';
           }
           else {
             min10++;
           }
        }
        if (cursor_x == 1){
           if (hour1 == '9'){
              hour1 = '0';
           }
           else {
             hour1++;
           }
        }
        if (cursor_x == 0){
           if (hour10 == '9'){
              hour10 = '0';
           }
           else {
             hour10++;
           }
        }
      }


      if (dir == 'd'){
        if (cursor_x == 7){
           if (sec1 == '0'){
              sec1 = '9';
           }
           else {
             sec1--;
           }
        }
        if (cursor_x == 6){
           if (sec10 == '0'){
              sec10 = '9';
           }
           else {
             sec10--;
           }
        }
        if (cursor_x == 4){
           if (min1 == '0'){
              min1 = '9';
           }
           else {
             min1--;
           }
        }
        if (cursor_x == 3){
           if (min10 == '0'){
              min10 = '9';
           }
           else {
             min10--;
           }
        }
        if (cursor_x == 1){
           if (hour1 == '0'){
              hour1 = '9';
           }
           else {
             hour1--;
           }
        }
        if (cursor_x == 0){
           if (hour10 == '0'){
              hour10 = '9';
           }
           else {
             hour10--;
           }
        }
      }

      delay(100);        
      lcd.setCursor(0,1);
      lcd.print("%s%s:%s%s:%s%s",hour10,hour1,min10,min1,sec10,sec1);
      lcd.setCursor(cursor_x);
    }             
  }
  
 return;
}


/////////////////////////////////////////////////////////////////
// 
void setBulbTime() {
  
  
 return;
}



/////////////////////////////////////////////////////////////////
// depress the shutter fully and take a picture
void fullShutter() {
  // Print a message to the LCD.
  lcd.clear();
  lcd.setCursor(6,0);
  lcd.print("** Click **");
  delay(200);
  lcd.clear();
  mainScreen();  
  return;
}



/////////////////////////////////////////////////////////////////
// depress the shutter halfway to focus
void halfShutter() {
  lcd.clear();
  lcd.setCursor(6,0);
  lcd.print("** Focus **");
  delay(300);
  lcd.clear();
  mainScreen();  
  return;
}



void startIntervalCamera(){
  return;
}

