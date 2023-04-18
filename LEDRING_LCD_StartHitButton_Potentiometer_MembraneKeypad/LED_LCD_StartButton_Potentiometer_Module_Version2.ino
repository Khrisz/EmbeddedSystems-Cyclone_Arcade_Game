//Code for LED Ring, LCD, Start/Hit Button,Potentiometer, and MODULE

#include <FastLED.h>
#include <LiquidCrystal.h>
#include <avr/io.h> 
#include <avr/interrupt.h>

//////Module Code
#include <Keypad.h>
#include <EEPROM.h>

// Switch membrane module setup
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns

int currentState1;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {13, 12, 11, 10}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {A1, A2, A4, A5}; //connect to the column pinouts of the keypad

//Create an object of keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
int gameScore = 0;
// 

bool isScoringState = false; 
//bool isInitialState = true;
//////Module Code


#define pin4  4 //LED Data Pin
#define potentiometerPin A0
#define NumberOfLEDs  60
#define targetLED 30 //LED that will be green and a players target
#define Easy 340   //Difficulty Easy: Pedomiter will have a range from 0 to 340 
#define Medium 682 //Difficulty Medium: Pedomiter will have a range from 341 to 682
#define Hard 1023 //Difficulty Hard: Pedomiter will have a range from 683 to 1023


CRGB leds[NumberOfLEDs];   //array  of leds to set or clear the data

int startingLED = 0;  //Starting LED for when a game starts
int cyclingLED = 0;   //The LED that will be on at a current loo[]
int currentDifficulty = 300; //USED FOR TESTING PURPOSES, Will be replaced by the pedomiter reading
int pointsDifficulty = 3;    //USED FOR TESTING PURPOSES, Will be replaced based on the pedomiter reading
int delayTime = 100;         //USED FOR TESTING PURPOSES, Will be replaced based on the pedomiter reading
bool gameStarted  = false;   //Flag for checking if the user started a game
int reverseStart = 59;       //Starting led for when a player is not playing the game. Lights go in a counter clock cycle
int startHitPin = 2;          //Source for INT0, Used for starting a game and then as a 'Hit'         
int playerScore = 0;          //Tracks User's score
int currentLevel = 1;
bool readyToCheck = true;
unsigned long gameTimeStart, newTime, timePressed = 0;  //Used for keeping track of when the player started a game


char topScores[3][10] = { "BLU: 20", "RRR: 18", "OYY: 17"}; //USED FOR TESTING PURPOSES, Will be replaced by what is returned after a game ends
char playerName[10];                                        //Save a player's initals

//const int RS = 12, E = 11, D4 = 8, D5 = 7, D6 = 6, D7 = 5, V0 = 9; //Initalized variables for LCD Display
//Change pins to free up rserved SPI pins 13,12,11,10
const int RS = 9, E = 8, D4 = 7, D5 = 6, D6 = 5, D7 = 3, V0 = A3; //Initalized variables for LCD Display

LiquidCrystal lcd(RS, E, D4, D5, D6, D7);             
int Contrast = 0;                              //Need to set contrast. Else LCD looks blank

unsigned long startTime, currentTime = 0;   //Start time for when UNO is turned on. Used to cycle through top 3 scores on the LCD
int currentScoreIndex = 1;                  //Used to cycle through 3 top scores
int lastState1;

void setup() {
  Serial.begin(9600);                        //Used for testing
  startTime = millis();                    //Save to start tracking when UNO was turned on and cycle through top 3 scores every 1 second
  FastLED.addLeds<NEOPIXEL, pin4>(leds, NumberOfLEDs);       //SetUp LED ring
  currentState1 = digitalRead(startHitPin);
  //Internal Timer Interrupt Setup
  cli(); // mask all interupts
  TCCR1B = 0; //set timer control register to 0
  TCCR1A = 0;
  //Enable timer1  overflow interrupt
  TIMSK1 = (1 << TOIE1);  //status reg bit 0 will have 1, 
  TCCR1B |= (1 << CS12);   //set CS12 bit to 1 for clock/256
  //1 * 16000000 / 65535 + 1  = 245.144 rounded
  //closest prescaler value is 256    
  sei(); //set interrupt

//External Setup for starting and Hitting
  pinMode(startHitPin,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(startHitPin), startHitISR, RISING);

  //LCD Setup
  analogWrite(V0, Contrast);
  lcd.begin(16, 2);   //columns, rows

/////////Module Code
  /* //Do only once on a microController for initial EEPROM setup: initializes all EEPROM addresses to 0*/
  for (int i = 0; i < EEPROM.length(); i++) {
    EEPROM.write(i, 0);
  }

/////////Module Code
}

void loop() {
  if (isScoringState) 
  {
    // TODO: Display player score on LCD
    for (int i = 0; i < NumberOfLEDs; i++)
      {
        leds[i] = CRGB::Red;  //Turns off all the leds
      }
    //verify player score, if it does not make scoreboard, return to initial state
    int playerRank = getPlayerRank(gameScore);
    if (playerRank > 0) 
    {
      // TODO: Prompt user to enter name on LCD
      lcd.clear();  
      lcd.setCursor(1, 0);
      lcd.print("Enter Name: ");
      //Serial.println("Enter name: "); // FOR TESTING PURPOSES      

      // get name input, loop until player has confirmed name by pressing "*". Name should be between 1 and 3 characters long
      char keyPress; //store character from switch membrane module
      String name = ""; //empty string to add characters to form player name
      while ((name.length() == 0) || (keyPress != '*' && name.length() < 3))
      {
        keyPress = keypad.getKey();

        // add character to name only if not '*' (indicates name submission)
        if(keyPress && keyPress !='*') 
        {
         // Serial.println(keyPress); // FOR TESTING PURPOSES
          name += keyPress;
                lcd.clear();  
      lcd.setCursor(1, 0);
      lcd.print("Enter Name: ");
          lcd.setCursor(0, 1);
          lcd.print(name);
        }
      }
      // add name based on rank
      writeNameEEPROM(name, playerRank);
      // add score based on rank
      writeScoreEEPROM(gameScore, playerRank);
      // return to initial state of game

      isScoringState = false;
      //isInitialState = true;
    }
    else
    {
      isScoringState = false;
      //isInitialState = true;
    }
    for (int i = 0; i < NumberOfLEDs; i++)
    {
      leds[i] = CRGB::Black;  //Turns off all the leds
    }
    gameScore = 0;
    lcd.clear();  
  }
  if(!gameStarted){ // Inital State, Waiting for a player to hit the start button
  //Serial.println( "Game Not started");
    for (int i = NumberOfLEDs-1; i >= 0; i--)
      {
        leds[i] = CRGB::Black;  //Turns off all the leds
      }
      
    leds[targetLED] = CRGB::Green;  //Turn on target LED and set to green
    leds[reverseStart] = CRGB::Red;  //Turn on current cycled LED and set to red, Initally will be led 59, then 58 etc
    FastLED.show(); //Initializes light cycle
    reverseStart = reverseStart - 1; //Update the cycling LED to next position
    if(reverseStart < startingLED){  // -1 < 0
      reverseStart = 59;             //Reset Reverse start to 59
    }
    currentDifficulty = analogRead(potentiometerPin); //Set current difficulty to a speed based on the range that is being read
    if(currentDifficulty <= Easy){
      delayTime = 550; 
      pointsDifficulty = 1;
    } else if (currentDifficulty <= Medium){
      delayTime = 100;    //delay by  100ms
      pointsDifficulty = 3;
    } else{
      delayTime = 25; //delay by 25ms
      pointsDifficulty = 5;
    }
    delay(delayTime);                //delay by 100 ms initally, But will be set based on what the last player had it set too    

    //Print top 3 scorers on LCD
    currentTime = millis();
    if(currentTime - startTime >= 1000){
      String scorerName = readNameEEPROM(currentScoreIndex);
      int scorerPoints = readScoreEEPROM(currentScoreIndex);
      if(scorerPoints > 0){
        scorerName = scorerName + ": " + String(scorerPoints);
        //int stringLength = strlen(scorerName); //Will need to be changed if topscores is no longer an array of strings
        int stringLength = sizeof(scorerName)/sizeof(scorerName[0]); //Will need to be changed if topscores is no longer an array of strings
        lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Top 3 Scores");
        lcd.setCursor((16 - stringLength) / 2, 1);               //Sets the 'PlayerInitals: Score' centered on the second line of the display
        lcd.print(scorerName);
      } else{
        scorerName = "";
        //int stringLength = strlen(scorerName); //Will need to be changed if topscores is no longer an array of strings
        int stringLength = sizeof(scorerName)/sizeof(scorerName[0]); //Will need to be changed if topscores is no longer an array of strings
        lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Top 3 Scores");
        lcd.setCursor((16 - stringLength) / 2, 1);               //Sets the 'PlayerInitals: Score' centered on the second line of the display
        lcd.print(scorerName);
      }
      currentScoreIndex++;                         //Update score index to show next score
      if(currentScoreIndex == 4){                  //Reset back to 0 if you already showed all 3 scores
        currentScoreIndex = 0;
      }      
        startTime = millis();                     //Reset the start time
     }
  } else if(gameStarted){
   newTime = millis();
    //Below for actual playing mode
    for (int i = 0; i < NumberOfLEDs; i++)
      {
        leds[i] = CRGB::Black;  //Turns off all the leds
      }
    leds[targetLED] = CRGB::Green;  //Turn on target LED and set to green
    leds[cyclingLED] = CRGB::Red;  //Turn on current cycled LED and set to red, initally will be 0, then 1, then 2, etc
    FastLED.show(); //Initializes light cycle
    cyclingLED = cyclingLED + 1; //Update the cycling LED to next position
    if(cyclingLED == NumberOfLEDs){
      cyclingLED = startingLED;          //Reset cycling LED back to 0
    }
    delay(delayTime);             //Based on the pedomiter reading
  }
}

void startHitISR() {
  timePressed = millis();
if(newTime - timePressed > 50) { 
  //readyToCheck = false;
  //timePressed = millis();
  if(!gameStarted && isScoringState == false){
    //!False = True, player hit button to start the game
    gameStarted = true;
    playerScore = 0; //set the player score to 0
        //Check the difficulty
    if(currentDifficulty <= Easy){
      delayTime = 350; 
      pointsDifficulty = 1;
      currentLevel = 1;
    } else if (currentDifficulty <= Medium){
      delayTime = 100;    //delay by  100ms
      pointsDifficulty = 3;
      currentLevel = 2;
    } else{
      delayTime = 50; //delay by 25ms
      pointsDifficulty = 5;
      currentLevel = 3;
    }
    /*
    //Set the compare interrupt to go off in 45 seconds
    cli(); // mask all interupts
    TCCR1B = 0; //set timer control register to 0
    TCCR1A = 0;
    //Enable timer1 compare interrupt
    OCR1A = 12100;  //Compare value
    TIMSK1 = (1 << OCIE1A);  // 
    TCCR1B |= (1 << WGM12);  // clear after compare
    //45 * 16000000 / 700,000 + 1  = 1029.57 rounded prescale value  
    //closest prescaler value is 1024 
    //Interrupt time = 0.0000625 * 1024 * 700,000 = 44800 ms= 44.8s
    TCCR1B |= (1 << CS12);   
    TCCR1B |= (1 << CS10);   
    sei(); //set interrupt
      */
    lcd.clear();  
    gameTimeStart = millis(); //Start the game timer, Will be checked every second by the internal timer interrupt
  }
  else if(gameStarted && isScoringState == false){
    //the player hit the button while playing, Button is to be used as a 'Hit'
    int distance = targetLED - (cyclingLED-1);
    if(currentLevel == 2){
      distance = distance + 1;
    } else if(currentLevel == 3){
        distance = distance + 2;
    }     
    if( distance == 0){
      //'HIT'
      playerScore = playerScore + pointsDifficulty;
    }        
    //Send 1 to Slave Arduino
    Serial.write(1);       //send '1', turn on
  }
}
//readyToCheck = true;
}

//Check if Game time is over. If so, check players score and add to array if it made top 3, Reset everything else to inital conditions
ISR(TIMER1_OVF_vect){  //Vector Timer1 overflow
  //What is in here will execute when there is a timer interrupt
  //Time is up for the player
  if(gameStarted){  //Check if there is a player playing

  //Display time left and current points
    currentTime = millis();
    int timeElapsed = (currentTime - gameTimeStart) / 1000;
    int timeLeft = 45 - timeElapsed;
    lcd.clear();  
    lcd.setCursor(1, 0);
    lcd.print(String("Time Left:  ") + String(timeLeft));
    lcd.setCursor( 0 , 1);     
    lcd.print(String("Score:  ") + String(playerScore));

    //Check if 45 secods have past  
    currentTime = millis();
    if(currentTime - gameTimeStart >= 45000){ //If it has been 45 seconds or more since since the start time of the game
    //  Serial.println("End Of Game");
      gameStarted = false;
      startingLED = 0;
      cyclingLED = 0;
      reverseStart = 59;
      gameScore  = playerScore;
      playerScore = 0;
      startTime = millis();  //Reset start time to be used for cycling through top 3 scores
      lcd.clear();      

      ////Update scores code      
      isScoringState = true;
      ////

      //Send 0 to slave arduino
      Serial.write(0);
    }
  }
}


// returns rank (1, 2, or 3) if score makes it into scoreBoard, otherwise, return -1.
// If score == 0, return -1
int getPlayerRank(int playerScore) {

  // if 3rd player scores exists, check if greater than
  int savedScore = EEPROM.read(2);
  if (savedScore > 0) 
  {
    if (playerScore <= savedScore) {return -1;}
  } 

  // if 2nd player scores exists, check if greater than
  savedScore = EEPROM.read(1);
  if (savedScore > 0) 
  {
    if (playerScore <= savedScore) {return 3;}
  }

  // if 1st player scores exists, check if greater than
  savedScore = EEPROM.read(0);
  if (savedScore > 0) 
  {
    if (playerScore <= savedScore) {return 2;}
  } 

  if(playerScore == 0) {return -1;}

  return 1;
}

// returns string of specified length starting from a specified address
String readStringEEPROM(int address, int stringLength) 
{
  String string = "";

  for(int i = 0; i < stringLength; i++)
  {
    string += (char) EEPROM.read(address + i);
  }

  return string;
  
}

// writes string of specified length at specified address
// returns false if string does not fit in selected EEPROM memory range
bool writeStringEEPROM(int address, String string)
{
  if ((address + string.length()) > EEPROM.length()) {return false;}

  for(int i = 0; i < string.length(); i++) 
  {
    EEPROM.update(address + i, string[i]);
  }  
  
}

// writes player name in address corresponding to player rank (rank * 3)
// name length is stored in address (11 + rank)
// returns false if name.length() is greater than 3 or less than 1
bool writeNameEEPROM(String name, int rank) 
{
  if (name.length() > 3 || name.length() < 1) {return false;}

  String tempName1st;
  String tempName2nd;

  switch(rank) {
    case 3:
      //replace 3rd place name with new name
      writeStringEEPROM(9, name);
      EEPROM.update(14, name.length()); //store name length

    break;
    case 2:
      tempName2nd = readStringEEPROM(6, EEPROM.read(13)); // store current 2nd place name in temporary location

      //move 2nd place name to 3rd place
      writeStringEEPROM(9, tempName2nd);
      EEPROM.update(14, tempName2nd.length());     

      //insert new name into 2nd place
      writeStringEEPROM(6, name); // store new name for 2nd place
      EEPROM.update(13, name.length()); //store name length    

    break;
    case 1:
      tempName1st = readStringEEPROM(3, EEPROM.read(12)); // store current 1st place name in temporary location
      tempName2nd = readStringEEPROM(6, EEPROM.read(13)); // store current 2nd place name in temporary location

      //move 1st place name to 2nd place
      writeStringEEPROM(6, tempName1st);
      EEPROM.update(13, tempName1st.length());

      //move 2nd place name to 3rd place
      writeStringEEPROM(9, tempName2nd);
      EEPROM.update(14, tempName2nd.length());

      //insert new name into 1st place
      writeStringEEPROM(3, name); // store new name for 1st place
      EEPROM.update(12, name.length()); //store name length 

    break;
  }

  return true;
}

// returns name associated with given rank at address (rank * 3)
String readNameEEPROM(int rank) {
  String name = "";
  char c;
  for (int i = 0; i < EEPROM.read(11 + rank); i++) 
  {
    c = (char) EEPROM.read(i + (rank * 3));
    name += c;

  }
  return name;
}

// returns false if score is greater than 255
bool writeScoreEEPROM(int score, int rank) 
{
  int tempScore1;
  int tempScore2;

  if(score > 255) {return false;}

  switch(rank) {
    case 3:
      EEPROM.update(2, score); // update only 3rd place score
    break;
    case 2:
      tempScore2 = EEPROM.read(1);
      EEPROM.update(1, score); // update 2nd place score
      EEPROM.update(2, tempScore2); // move previous 2nd place score to 3rd
    break;
    case 1:
      tempScore1 = EEPROM.read(0);
      tempScore2 = EEPROM.read(1);
      EEPROM.update(0, score);
      EEPROM.update(1, tempScore1); // move previous 1st place score to 2nd
      EEPROM.update(2, tempScore2); // move previous 2nd place score to 3rd
    break;
  }

}

// returns -1 if rank has no player score set
int readScoreEEPROM(int rank) {
  if (EEPROM.read(rank - 1) == 0) {return -1;}

  return EEPROM.read(rank - 1);
}

// resets EEPROM values 0 - 14
void resetScoresEEPROM() {

  for (int i = 0; i < 15; i++) 
  {
    EEPROM.write(i, 0);
  }
}
