#include <FastLED.h>
#include <LiquidCrystal.h>
#include <avr/io.h> 
#include <avr/interrupt.h>


#define pin4  4 //LED Data Pin
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
unsigned long gameTimeStart = 0;  //Used for keeping track of when the player started a game


char topScores[3][10] = { "BLU: 20", "RRR: 18", "OYY: 17"}; //USED FOR TESTING PURPOSES, Will be replaced by what is returned after a game ends
char playerName[10];                                        //Save a player's initals

const int RS = 12, EP = 11, D4 = 8, D5 = 7, D6 = 6, D7 = 5, V0 = 9; //Initalized variables for LCD Display
LiquidCrystal lcd(RS, EP, D4, D5, D6, D7);             
int Contrast = 0;                              //Need to set contrast. Else LCD looks blank

unsigned long startTime, currentTime = 0;   //Start time for when UNO is turned on. Used to cycle through top 3 scores on the LCD
int currentScoreIndex = 0;                  //Used to cycle through 3 top scores

void setup() {
  Serial.begin(9600);                        //Used for testing
  startTime = millis();                    //Save to start tracking when UNO was turned on and cycle through top 3 scores every 1 second
  FastLED.addLeds<NEOPIXEL, pin4>(leds, NumberOfLEDs);       //SetUp LED ring

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
}

void loop() {
  
  if(!gameStarted){ // Inital State, Waiting for a player to hit the start button
    Serial.println( "Game Not started");
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
    delay(delayTime);                //delay by 100 ms initally, But will be set based on what the last player had it set too
    pointsDifficulty = 3;              //Used for testing purposes only            

    //Print top 3 scorers on LCD
    lcd.setCursor(1, 0);
    lcd.print("Top 3 Scores");
    currentTime = millis();
    if(currentTime - startTime >= 1000){
      int stringLength = strlen(topScores[currentScoreIndex]); //Will need to be changed if topscores is no longer an array of strings
      lcd.setCursor((16 - stringLength) / 2, 1);               //Sets the 'PlayerInitals: Score' centered on the second line of the display
      lcd.print(topScores[currentScoreIndex]);
      currentScoreIndex++;                         //Update score index to show next score
      if(currentScoreIndex == 3){                  //Reset back to 0 if you already showed all 3 scores
        currentScoreIndex = 0;
      }      
        startTime = millis();                     //Reset the start time
     }
    //for(int i = 0; i < 3; i++){
    //  int stringLength = strlen(topScores[i]);
    //   lcd.setCursor(1, 0);
    //   lcd.print("Top 3 Scores");
    //   lcd.setCursor((16 - stringLength) / 2, 1);
    //  lcd.print(topScores[i]);
     // delay(2000);
    //  lcd.clear();
    //}  
 } else if(gameStarted){
   
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
  if(!gameStarted){
    //!False = True, player hit button to start the game
    gameStarted = true;
    playerScore = 0; //set the player score to 0
        //Check the difficulty
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
  else{
    //the player hit the button while playing, Button is to be used as a 'Hit'
    int distance = targetLED - (cyclingLED-1);
    Serial.print("Distance: ");    //Used for testing
    Serial.println( distance);      //Used for testing
    Serial.print("targetLED: ");   //Used for testing
    Serial.print( targetLED);      //Used for testing
    Serial.print("Cycling LED: ");   //Used for testing
    Serial.println( cyclingLED);     //Used for testing
    if( distance == 0){
      //'HIT'
      playerScore = playerScore + pointsDifficulty;
    }        
    Serial.print("Total Points: ");   //Used for testing
    Serial.println( playerScore);   //Used for testing
  }
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
        Serial.println(timeLeft);   //Used for testing
    Serial.print(timeLeft);   //Used for testing

    //Check if 45 secods have past  
    currentTime = millis();
    Serial.print("Difference: ");   //Used for testing
    Serial.println(currentTime - gameTimeStart);   //Used for testing
    if(currentTime - gameTimeStart >= 45000){ //If it has been 45 seconds or more since since the start time of the game
      Serial.println("End Of Game");
      gameStarted = false;
      startingLED = 0;
      cyclingLED = 0;
      reverseStart = 59;
      playerScore = 0;
      startTime = millis();  //Reset start time to be used for cycling through top 3 scores
      lcd.clear();      
    }
  }
}
