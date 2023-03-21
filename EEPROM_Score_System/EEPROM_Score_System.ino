#include <Keypad.h>
#include <EEPROM.h>

// Switch membrane module setup
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {5, 4, 3, 2}; //connect to the column pinouts of the keypad

//Create an object of keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
int gameScore = 43; //FOR TESTING PURPOSES
// 

bool isScoringState = false;
bool isInitialState = true;

void setup(){
  Serial.begin(9600);

/* //Do only once on a microController for initial EEPROM setup: initializes all EEPROM addresses to 0
  for (int i = 0; i < EEPROM.length(); i++) {
    EEPROM.write(i, 0);
  }
*/
}

void loop(){

  //*********FOR TESTING PURPOSES*********
  Serial.println("Current Score Board:");
  Serial.println("Rank   Name   Score");
  int tempScore = readScoreEEPROM(1);
  if(tempScore != -1)
  {
    Serial.print("1st    ");
    Serial.print(readNameEEPROM(1));
    Serial.print("   ");
    Serial.println(tempScore);

  }
  tempScore = readScoreEEPROM(2);
  if(tempScore != -1)
  {
    Serial.print("2nd    ");
    Serial.print(readNameEEPROM(2));
    Serial.print("   ");
    Serial.println(tempScore);
  }
  tempScore = readScoreEEPROM(3);
  if(tempScore != -1)
  {
    Serial.print("3rd    ");
    Serial.print(readNameEEPROM(3));
    Serial.print("   ");
    Serial.println(tempScore);
  }
  Serial.println("Enter score:");
  while (Serial.available() == 0) {} // Wait for the user to enter a value
  gameScore = Serial.parseInt();
  isScoringState = true;

  
  //**************************************
 

  if (isScoringState) 
  {
    // TODO: Display player score on LCD

    //verify player score, if it does not make scoreboard, return to initial state
    int playerRank = getPlayerRank(gameScore);
    if (playerRank > 0) 
    {
      // TODO: Prompt user to enter name on LCD

      Serial.println("Enter name: "); // FOR TESTING PURPOSES      

      // get name input, loop until player has confirmed name by pressing "*". Name should be between 1 and 3 characters long
      char keyPress; //store character from switch membrane module
      String name = ""; //empty string to add characters to form player name
      while ((name.length() == 0) || (keyPress != '*' && name.length() < 3))
      {
        keyPress = keypad.getKey();

        // add character to name only if not '*' (indicates name submission)
        if(keyPress && keyPress !='*') 
        {
          Serial.println(keyPress); // FOR TESTING PURPOSES
          name += keyPress;
        }
      }
      // add name based on rank
      writeNameEEPROM(name, playerRank);
      // add score based on rank
      writeScoreEEPROM(gameScore, playerRank);
      // return to initial state of game

      isScoringState = false;
      isInitialState = true;
    }
    else
    {
      isScoringState = false;
      isInitialState = true;
      Serial.println("Scoring state is false"); //FOR TESTING PURPOSES
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
