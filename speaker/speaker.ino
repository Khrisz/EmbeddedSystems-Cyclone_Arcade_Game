#include "pitches.h"
#include <Volume.h>

#define joyX A0
#define joyY A1
#define BUZZER_PIN 5

int melodies[2][100] = {

  {
    NOTE_D4, NOTE_G4, NOTE_FS4, NOTE_A4,
    NOTE_G4, NOTE_C5, NOTE_AS4, NOTE_A4,                   
    NOTE_FS4, NOTE_G4, NOTE_A4, NOTE_FS4, NOTE_DS4, NOTE_D4,
    NOTE_C4, NOTE_D4,0,                                 

    NOTE_D4, NOTE_G4, NOTE_FS4, NOTE_A4,
    NOTE_G4, NOTE_C5, NOTE_D5, NOTE_C5, NOTE_AS4, NOTE_C5, NOTE_AS4, NOTE_A4,      //29               //8
    NOTE_FS4, NOTE_G4, NOTE_A4, NOTE_FS4, NOTE_DS4, NOTE_D4,
    NOTE_C4, NOTE_D4,0,                                       

    NOTE_D4, NOTE_FS4, NOTE_G4, NOTE_A4, NOTE_DS5, NOTE_D5,
    NOTE_C5, NOTE_AS4, NOTE_A4, NOTE_C5,
    NOTE_C4, NOTE_D4, NOTE_DS4, NOTE_FS4, NOTE_D5, NOTE_C5,
    NOTE_AS4, NOTE_A4, NOTE_C5, NOTE_AS4,             //58

    NOTE_D4, NOTE_FS4, NOTE_G4, NOTE_A4, NOTE_DS5, NOTE_D5,
    NOTE_C5, NOTE_D5, NOTE_C5, NOTE_AS4, NOTE_C5, NOTE_AS4, NOTE_A4, NOTE_C5, NOTE_G4,
    NOTE_A4, 0, NOTE_AS4, NOTE_A4, 0, NOTE_G4,
    NOTE_G4, NOTE_A4, NOTE_G4, NOTE_FS4, 0,

    NOTE_C4, NOTE_D4, NOTE_G4, NOTE_FS4, NOTE_DS4,
    NOTE_C4, NOTE_D4, 0,
    NOTE_C4, NOTE_D4, NOTE_G4, NOTE_FS4, NOTE_DS4,
    NOTE_C4, NOTE_D4, END
  },

  {
    NOTE_E7, NOTE_E7, 0, NOTE_E7,
    0, NOTE_C7, NOTE_E7, 0,
    NOTE_G7, 0, 0,  0,
    NOTE_G6, 0, 0, 0,

    NOTE_C7, 0, 0, NOTE_G6,
    0, 0, NOTE_E6, 0,
    0, NOTE_A6, 0, NOTE_B6,
    0, NOTE_AS6, NOTE_A6, 0,

    NOTE_G6, NOTE_E7, NOTE_G7,
    NOTE_A7, 0, NOTE_F7, NOTE_G7,
    0, NOTE_E7, 0, NOTE_C7,
    NOTE_D7, NOTE_B6, 0, 0,

    NOTE_C7, 0, 0, NOTE_G6,
    0, 0, NOTE_E6, 0,
    0, NOTE_A6, 0, NOTE_B6,
    0, NOTE_AS6, NOTE_A6, 0,

    NOTE_G6, NOTE_E7, NOTE_G7,
    NOTE_A7, 0, NOTE_F7, NOTE_G7,
    0, NOTE_E7, 0, NOTE_C7,
    NOTE_D7, NOTE_B6, 0, 0, END
    }
  };

int durations[3][100] = 
  {
    {
      8,4,8,4,
      4,4,4,12,
      4,4,4,4,4,4,
      4,16,4,

      8,4,8,4,
      4,2,1,1,2,1,1,12,
      4,4,4,4,4,4,
      4,16,4,

      4,4,4,4,4,4,
      4,4,4,12,
      4,4,4,4,4,4,
      4,4,4,12,

      4,4,4,4,4,4,
      2,1,1,2,1,1,4,8,4,
      2,6,4,2,6,4,
      2,1,1,16,4,

      4,8,4,4,4,
      4,16,4,
      4,8,4,4,4,
      4,20,
    },
    {
    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,

    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,

    9, 9, 9,
    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,

    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,

    9, 9, 9,
    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,
    }
  };


  int melody[] = {
    NOTE_D4, NOTE_G4, NOTE_FS4, NOTE_A4,
    NOTE_G4, NOTE_C5, NOTE_AS4, NOTE_A4,                   
    NOTE_FS4, NOTE_G4, NOTE_A4, NOTE_FS4, NOTE_DS4, NOTE_D4,
    NOTE_C4, NOTE_D4,0,                                 

    NOTE_D4, NOTE_G4, NOTE_FS4, NOTE_A4,
    NOTE_G4, NOTE_C5, NOTE_D5, NOTE_C5, NOTE_AS4, NOTE_C5, NOTE_AS4, NOTE_A4,      //29               //8
    NOTE_FS4, NOTE_G4, NOTE_A4, NOTE_FS4, NOTE_DS4, NOTE_D4,
    NOTE_C4, NOTE_D4,0,                                       

    NOTE_D4, NOTE_FS4, NOTE_G4, NOTE_A4, NOTE_DS5, NOTE_D5,
    NOTE_C5, NOTE_AS4, NOTE_A4, NOTE_C5,
    NOTE_C4, NOTE_D4, NOTE_DS4, NOTE_FS4, NOTE_D5, NOTE_C5,
    NOTE_AS4, NOTE_A4, NOTE_C5, NOTE_AS4,             //58

    NOTE_D4, NOTE_FS4, NOTE_G4, NOTE_A4, NOTE_DS5, NOTE_D5,
    NOTE_C5, NOTE_D5, NOTE_C5, NOTE_AS4, NOTE_C5, NOTE_AS4, NOTE_A4, NOTE_C5, NOTE_G4,
    NOTE_A4, 0, NOTE_AS4, NOTE_A4, 0, NOTE_G4,
    NOTE_G4, NOTE_A4, NOTE_G4, NOTE_FS4, 0,

    NOTE_C4, NOTE_D4, NOTE_G4, NOTE_FS4, NOTE_DS4,
    NOTE_C4, NOTE_D4, 0,
    NOTE_C4, NOTE_D4, NOTE_G4, NOTE_FS4, NOTE_DS4,
    NOTE_C4, NOTE_D4, END

};

int noteDurations[] = {       //duration of the notes
  8,4,8,4,
  4,4,4,12,
  4,4,4,4,4,4,
  4,16,4,

  8,4,8,4,
  4,2,1,1,2,1,1,12,
  4,4,4,4,4,4,
  4,16,4,

  4,4,4,4,4,4,
  4,4,4,12,
  4,4,4,4,4,4,
  4,4,4,12,

  4,4,4,4,4,4,
  2,1,1,2,1,1,4,8,4,
  2,6,4,2,6,4,
  2,1,1,16,4,

  4,8,4,4,4,
  4,16,4,
  4,8,4,4,4,
  4,20,
};

int melody2[] = {
  NOTE_E7, NOTE_E7, 0, NOTE_E7,
  0, NOTE_C7, NOTE_E7, 0,
  NOTE_G7, 0, 0,  0,
  NOTE_G6, 0, 0, 0,

  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,

  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0,

  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,

  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0, END
};
//Mario main them tempo
int noteDurations2[] = {
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
};

Volume vol;
int volume = 30;
int speed = 50;
int trackIndex = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  vol.begin();
  
}

void loop() {
  // put your main code here, to run repeatedly:

  
  for (int i = 0; melodies[trackIndex][i] != -1; i++) {
    int noteDuration = speed * durations[trackIndex][i];

    vol.tone(melodies[trackIndex][i], volume);
    Serial.println("Tone");
    vol.delay(noteDuration * .95);
    vol.noTone();
    Serial.println("No Tone");

    // check joystick position
    int xValue = analogRead(joyX);
    int yValue = analogRead(joyY);

    // change volume if joystick has been shifted left or right
    if (xValue == 1023 && volume < 250) {
      volume += 10;
    }
    else if (xValue == 0 && volume > 0) {
      volume -= 10;
    }

    // change track if joystick has been shifted up or down     
    if (yValue == 1023) {
      trackIndex = (trackIndex + 1) % 2;
      i = 0;
    }

  }

  delay(3000);
  

 
}
